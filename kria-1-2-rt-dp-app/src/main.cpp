
/*******************************************************************************
Author: FEE CTU
Purpose: Host program
Comment: Refactoring for HLS
*******************************************************************************/
/*----------------------------------------------------------------------------*/
/*-------------------------------- INCLUDES ---------------------------------*/
#include "allocator.h"
#include <iostream>
#include <stdlib.h>
#include <string>
#include <fstream>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <poll.h>
#include "header/MotorModel.h"
#include "header/CurVelModel.h"
#include "header/regulator.h"
// #include "header/svmCore.h" // already in invertor
#include "header/transformation.h"
#include "header/invertor.h"

#include <stdio.h>
#include <chrono>
#include <thread>
#include <mutex>
#include <future>

#define MAP_SIZE 0x10000
#define SPI_SPICR 0x60
#define SPI_SPISR 0x64
#define SPI_DTR 0x68
#define SPI_DRR 0x6C
#define SPI_SPISSR 0x70
#define SPI_TRANSMIT_FIFO_OCUPANCY 0x74
#define SPI_RECEIVE_FIFO_OCUPANCY 0x78
#define SPI_DGIER 0x1C
#define SPI_IPISR 0x20
#define SPI_IPIER 0x28

#define TIMER_MAP_SIZE 0x10000
#define TIMER_0_CTR_REG 0x08

// #include "experimental/xrt_profile.h" // for profiling host program
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
/*-------------------- INFO FUNCTION FOR PRINTING ERRORS --------------------*/
#define OCL_CHECK(error, call)                                                                   \
    call;                                                                                        \
    if (error != CL_SUCCESS)                                                                     \
    {                                                                                            \
        printf("%s:%d Error calling " #call ", error code is: %d\n", __FILE__, __LINE__, error); \
        exit(EXIT_FAILURE);                                                                      \
    }
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/*--------------------------- CONSTANT DEFINITION ---------------------------*/
#define PI 3.141592
/*----------------------------------------------------------------------------*/

InvertorSwitchType invertorSwitchGlobal;

void stopInvertor()
{
    invertorSwitchGlobal.sw1 = 0;
    invertorSwitchGlobal.sw2 = 0;
    invertorSwitchGlobal.sw3 = 0;
    invertorSwitchGlobal.sw4 = 0;
    invertorSwitchGlobal.sw5 = 0;
    invertorSwitchGlobal.sw6 = 0;

    std::cout << "Set all invertor switches at 0!\n";
}

// Define the function to be called when ctrl-c (SIGINT) is sent to process
void signal_callback_handler(int signum)
{
    std::cout << "\nCaught signal " << signum << "\n";

    // create function for setting default values to switches and regulators
    stopInvertor();
    std::cout << "Terminating program!\n";
    // Terminate program
    exit(signum);
}

int threadLoopOutput = 0;
bool isDataFromBackgroundThreadReady = false; // zde je použito proto aby se zajistilo, že nejsou použity v main while smyšce programu na pozadí dvě stejné hodnoty - když nedošlo vlastně ještě k zápisu, jinak kdyby se mohli použít, tak se nebude vůbec tato proměnná v if statements používat
std::mutex gLock;

/*
 * @name     acknowledgeSPIInterrupt
 * @brief    Basic function to acknowledge an interrupt.
 * @todo     Make API from SPI and Interrupt solving. Solve how it works in Linux.
 */

void acknowledgeSPIInterrupt(int fd, void *ptr)
{
    struct pollfd fds = {
        .fd = fd,
        .events = POLLIN | POLLOUT,
    };

    int irq_on = 1;    // for writing 0x1 to /dev/uioX
    uint32_t info = 1; // in read function of a interrupt checking
    off_t interruptStatus;

    while (true)
    {
        int ret = poll(&fds, 1, -1); // poll on a return value

        // there was change in ret
        if (ret >= 1)
        {
            /* Do something in response to the interrupt. */
            printf("AXI Quad SPI Interrupt!\n");
            interruptStatus = *((unsigned *)(ptr + SPI_IPISR));
            // printf("1st From acknowledge IPISR: 0x%lx\n", interruptStatus);
            std::this_thread::sleep_for(std::chrono::nanoseconds(2)); // could not resolve other way now, because reading and writing to register takes more time that one tick probably
            *((unsigned *)(ptr + SPI_IPISR)) = interruptStatus;       // reseting SPI interrupt status register
            write(fd, &irq_on, sizeof(irq_on));
            break;
        }
    }
}

/*
 * @name     SPI communication functions.
 * @brief
 * @todo     Resolve when using multiple slaves to automate slave changing. Eg. when using one slave on 1. SS, just changing 0x01 to 0x00, when using slave only on 2. SS change 0x2 to 0x0, but when using slaves on 1. and 2. SS and activating slave only on SS 1. it should probably be 0x3 (both at 1) to 0x2 (first at 0) to 0x3 (both at 1).
 */

/*
 * @name     sendSPIdata
 * @brief    Send SPI data to desired slave via mapped device.
 * @todo     Implement interrupt and delete sleep_for...
 */
void sendSPIdata(void *ptr, int fd, off_t slaveSelect, off_t data)
{
    *((unsigned *)(ptr + SPI_SPICR)) = 0x1E6;
    *((unsigned *)(ptr + SPI_SPISSR)) = slaveSelect; // 0x01 - slave select 1 to high, when slave no. 2 - it would be 0x02
    *((unsigned *)(ptr + SPI_DTR)) = data;
    *((unsigned *)(ptr + SPI_SPISSR)) = 0x0;
    *((unsigned *)(ptr + SPI_SPICR)) = 0x86;
    // std::this_thread::sleep_for(std::chrono::nanoseconds(1)); // here should be waiting for an interrrupt
    acknowledgeSPIInterrupt(fd, ptr); // acknowledging interrupt
    *((unsigned *)(ptr + SPI_SPISSR)) = slaveSelect;
}

/*
 * @name     initializeSPI
 * @brief    Initialize SPI communication in PL for desired slave and mapped device.
 * @todo     Implement interrupt and delete sleep_for...
 */
void initializeSPI(void *ptr, off_t slaveSelect)
{
    *((unsigned *)(ptr + SPI_SPICR)) = 0x1E6;

    *((unsigned *)(ptr + SPI_DTR)) = 0x06;

    *((unsigned *)(ptr + SPI_SPISSR)) = 0x00;

    *((unsigned *)(ptr + SPI_SPISSR)) = slaveSelect;

    *((unsigned *)(ptr + SPI_SPICR)) = 0x1E6;

    // Interrupt settings

    // Global Interrupt Enable
    *((unsigned *)(ptr + SPI_DGIER)) = 0x80000000;

    // Interrupt enables
    *((unsigned *)(ptr + SPI_IPIER)) = 0x3FFF;

    off_t interruptStatus;

    interruptStatus = *((unsigned *)(ptr + SPI_IPISR));
    std::this_thread::sleep_for(std::chrono::nanoseconds(1)); // could not resolve other way now, because reading and writing to register takes more time that one tick probably
    *((unsigned *)(ptr + SPI_IPISR)) = interruptStatus;       // reseting SPI interrupt status register
}

/*
 * @name     initializeLEDmatrix
 * @brief    Initialize LED matrix with MAX7219.
 * @todo     Implement interrupt and delete sleep_for...
 */
void initializeLEDmatrix(void *ptr, int fd, off_t slaveSelect)
{
    off_t initilSeq[5] = {0x0900, 0x0a01, 0x0b07, 0x0c01, 0x0f00};

    for (int i = 0; i < 5; i++)
    {
        *((unsigned *)(ptr + SPI_SPICR)) = 0x1E6;
        *((unsigned *)(ptr + SPI_SPISSR)) = slaveSelect; // 0x01 - slave select 1 to high, when slave no. 2 - it would be 0x02
        *((unsigned *)(ptr + SPI_DTR)) = initilSeq[i];
        *((unsigned *)(ptr + SPI_SPISSR)) = 0x0;
        *((unsigned *)(ptr + SPI_SPICR)) = 0x86;
        // std::this_thread::sleep_for(std::chrono::nanoseconds(1)); // here should be waiting for an interrrupt
        acknowledgeSPIInterrupt(fd, ptr); // acknowledging interrupt
        *((unsigned *)(ptr + SPI_SPISSR)) = slaveSelect;
    }
}

/*
 * @name     clearLEDmatrix
 * @brief    Clear LED matrix (set all LEDs to 0).
 * @todo     Implement interrupt and delete sleep_for...
 */
void clearLEDmatrix(void *ptr, int fd, off_t slaveSelect)
{

    off_t clearSeq[8] = {0x100, 0x200, 0x300, 0x400, 0x500, 0x600, 0x700, 0x800};
    for (int i = 0; i < 8; i++)
    {
        *((unsigned *)(ptr + SPI_SPICR)) = 0x1E6;
        *((unsigned *)(ptr + SPI_SPISSR)) = slaveSelect; // 0x01 - slave select 1 to high, when slave no. 2 - it would be 0x02
        *((unsigned *)(ptr + SPI_DTR)) = clearSeq[i];
        *((unsigned *)(ptr + SPI_SPISSR)) = 0x0;
        *((unsigned *)(ptr + SPI_SPICR)) = 0x86;
        // std::this_thread::sleep_for(std::chrono::nanoseconds(1)); // here should be waiting for an interrrupt
        acknowledgeSPIInterrupt(fd, ptr); // acknowledging interrupt
        *((unsigned *)(ptr + SPI_SPISSR)) = slaveSelect;
    }
}

/*
 * @name     printLetterOnLEDMatrix
 * @brief    Print desired letter on a LED matrix.
 * @todo     Implement interrupt and delete sleep_for...
 */
void printLetterOnLEDMatrix(void *ptr, int fd, off_t slaveSelect, off_t *pismeno)
{

    for (int i = 0; i < 8; i++)
    {
        *((unsigned *)(ptr + SPI_SPICR)) = 0x1E6;
        *((unsigned *)(ptr + SPI_SPISSR)) = slaveSelect; // 0x01 - slave select 1 to high, when slave no. 2 - it would be 0x02
        *((unsigned *)(ptr + SPI_DTR)) = pismeno[i];
        *((unsigned *)(ptr + SPI_SPISSR)) = 0x0;
        *((unsigned *)(ptr + SPI_SPICR)) = 0x86;
        // std::this_thread::sleep_for(std::chrono::nanoseconds(1)); // here should be waiting for an interrrupt
        acknowledgeSPIInterrupt(fd, ptr); // acknowledging interrupt

        *((unsigned *)(ptr + SPI_SPISSR)) = slaveSelect;
    }
}

/*
 * @name     mainSPIfunction
 * @brief    SPI communication demonstration. Without Interrupts of knowing if the transfer has finished. Need to work on that.
 * @todo     Handle and test interrupts that transfer has finished.
 */
void mainSPIfunction()
{
    void *ptr;  // pointer to a virtual memory filled by mmap
    int fd;     // file descriptor
    char *uiod; // name what device to open
    // uiod = "/dev/mem";
    uiod = "/dev/uio4";
    off_t memoryBase = 0x0080040000; // base memory (in mmap() it is an offset)
    off_t dataToSend;                // data to send to SPI manually
    off_t slaveSelect;               // slave selection (chip select CS)
    int initializeMatrix;            // variable for if statement to initializeMatrix with initialization sequence
    int clearMatrix;                 // variable for if statement to clearMatrix (set all LEDs to 0)
    int modeSelection;               // mode selction to write data manually or to write a letter

    // letter variable
    off_t pismeno[8] = {0b000110000001,
                        0b001011000001,
                        0b001110100001,
                        0b010010010001,
                        0b010110001001,
                        0b011010000101,
                        0b011110000011,
                        0b100010000001};

    fd = open(uiod, O_RDWR | O_NONBLOCK); // opening device

    // if error
    if (fd < 1)
    {
        perror("open\n");
        printf("Invalid UIO device file:%s.\n", uiod);
    }
    else
    {
        std::cout << "Successfully opened device.\n";
    }

    // map PL memory to virtual memory
    ptr = mmap(NULL, MAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    // Slave selection
    std::cout << "Input SS (CS):\n";
    scanf("%lx", &slaveSelect);
    printf("Selected SPI slave: 0x%lx\n", slaveSelect);

    // Mode selection
    std::cout << "Select mode: [0] - dataTransfer, [1] - printLetter\n";
    scanf("%i", &modeSelection);

    // SPI communication initialization
    std::cout << "Initializing SPI...\n";
    initializeSPI(ptr, slaveSelect);
    std::cout << "SPI successfully initialized...\n";

    // LED matrix initialization
    std::cout << "Initialize matrix? [1 = yes / 0 = no]\n";
    scanf("%i", &initializeMatrix);
    (initializeMatrix == 1) ? initializeLEDmatrix(ptr, fd, slaveSelect) : (void)0;
    printf("initializeMatrix is set to: %i\n", initializeMatrix);

    // Clearing matrix
    std::cout << "Clear matrix? [1 = yes / 0 = no]\n";
    scanf("%i", &clearMatrix);
    (clearMatrix == 1) ? clearLEDmatrix(ptr, fd, slaveSelect) : (void)0;
    printf("clearMatrix is set to: %i\n", clearMatrix);

    // Main semaphore
    // 0 - send data via SPI manually
    // 1 - send desired letter defined in pismeno[] array
    switch (modeSelection)
    {
    case 0:
    {
        std::cout << "Input data to transfer:\n";
        scanf("%lx", &dataToSend);
        printf("Value to send via SPI: 0x%lx\n", dataToSend);
        sendSPIdata(ptr, fd, slaveSelect, dataToSend);
        break;
    }

    case 1:
    {
        printLetterOnLEDMatrix(ptr, fd, slaveSelect, pismeno);
        break;
    }
    }

    printf("IPISR: 0x%lx\n", *((unsigned *)(ptr + SPI_IPISR)));
    // Unmaping memory
    munmap(ptr, MAP_SIZE);

    // Closing fd
    close(fd);
}

bool oddNumberOfTimer = true;

void threadSPI(void *ptr, int fd, off_t slaveSelect)
{
    // void *ptr;  // pointer to a virtual memory filled by mmap
    // int fd;     // file descriptor
    // char *uiod; // name what device to open
    //  uiod = "/dev/mem";
    // uiod = "/dev/uio4";
    //  off_t slaveSelect = 0x1;

    // letter variable
    off_t pismenoZ[8] = {0b000110000001,
                         0b001011000001,
                         0b001110100001,
                         0b010010010001,
                         0b010110001001,
                         0b011010000101,
                         0b011110000011,
                         0b100010000001};

    // letter variable
    off_t pismenoP[8] = {0b000100000000 + 0b00000000,
                         0b001000000000 + 0b00000000,
                         0b001100000000 + 0b00000000,
                         0b010000000000 + 0b00000000,
                         0b010100000000 + 0b01110000,
                         0b011000000000 + 0b10001000,
                         0b011100000000 + 0b10001000,
                         0b100000000000 + 0b11111111};

    // fd = open(uiod, O_RDWR | O_NONBLOCK); // opening device

    // if error
    /*
    if (fd < 1)
    {
        perror("open\n");
        printf("Invalid UIO device file:%s.\n", uiod);
    }
    else
    {
        std::cout << "Successfully opened device.\n";
    }*/

    // map PL memory to virtual memory
    // ptr = mmap(NULL, MAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    // initializeSPI(ptr, 0x1);
    // initializeLEDmatrix(ptr, fd, slaveSelect);

    if (oddNumberOfTimer == true)
    {
        clearLEDmatrix(ptr, fd, slaveSelect);
        printLetterOnLEDMatrix(ptr, fd, slaveSelect, pismenoZ);
        oddNumberOfTimer = false;
    }
    else
    {
        clearLEDmatrix(ptr, fd, slaveSelect);
        printLetterOnLEDMatrix(ptr, fd, slaveSelect, pismenoP);
        oddNumberOfTimer = true;
    }

    // munmap(ptr, MAP_SIZE);

    // // Closing fd
    // close(fd);
}

/*
 * @name     threadLoop
 * @brief    Threaded function for timer and data acquisition.
 * @todo     Make multiple functions and multiple data acquisitions paralel but use data only when data from all sources all valid.
 */

void threadLoop()
{
    void *timer1ptr;    // pointer to a virtual memory filled by mmap
    int timer1fd;       // file descriptor of uio to reset interrupt in /proc/interrupts
    char *uiod;         // name of the uio to reset interrupts
    uiod = "/dev/uio5"; // check when making changes in a platform
    int irq_on = 1;     // for writing 0x1 to /dev/uioX
    uint32_t info = 1;  // in read function of a interrupt checking
    off_t slaveSelect = 0x1;

    timer1fd = open(uiod, O_RDWR | O_NONBLOCK); // opening uioX device

    // if error
    if (timer1fd < 1)
    {
        perror("open\n");
        printf("Invalid UIO device file:%s.\n", uiod);
    }

    // for polling the interrupt struct
    struct pollfd fds = {
        .fd = timer1fd,
        .events = POLLIN | POLLOUT,
    };

    // mmap the timer in virtual memory
    timer1ptr = mmap(NULL, TIMER_MAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, timer1fd, 0);

    // initial values for timer
    *((unsigned *)(timer1ptr)) = 0X1C0;
    write(timer1fd, &irq_on, sizeof(irq_on));
    // *((unsigned *)(timer1ptr + 0x4)) = 0xE8287BFF;
    // *((unsigned *)(timer1ptr + 0x4)) = 0xFFFFFF37;
    // *((unsigned *)(timer1ptr + 0x4)) = 0xFFE17B7F; // 10 ms
    *((unsigned *)(timer1ptr + 0x4)) = 0xFA0A1EFF; // 0.5 s
    *((unsigned *)(timer1ptr)) = 0XE0;

    // one tick is 0.8 ns, have to wait till data is moved to counter register
    // otherwise it wont start, solve maybe later or ask about it
    std::this_thread::sleep_for(std::chrono::nanoseconds(1));

    *((unsigned *)(timer1ptr + 0x8)) = 0X0;
    *((unsigned *)(timer1ptr)) = 0XC0;

    void *ptrSPI;  // pointer to a virtual memory filled by mmap
    int fdSPI;     // file descriptor
    char *uiodSPI; // name what device to open
    uiodSPI = "/dev/uio4";

    fdSPI = open(uiodSPI, O_RDWR | O_NONBLOCK); // opening device

    // if error

    if (fdSPI < 1)
    {
        perror("open\n");
        printf("Invalid UIO SPI device file:%s.\n", uiod);
    }
    else
    {
        std::cout << "Successfully SPI opened device.\n";
    }

    ptrSPI = mmap(NULL, MAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fdSPI, 0);
    // std::cout << "Initializing SPI!\n";
    std::this_thread::sleep_for(std::chrono::nanoseconds(1));
    initializeSPI(ptrSPI, slaveSelect);
    // std::cout << "Initializing Matrix!\n";
    std::this_thread::sleep_for(std::chrono::nanoseconds(1));
    initializeLEDmatrix(ptrSPI, fdSPI, slaveSelect);
    // std::cout << "Clearing Matrix!\n";
    std::this_thread::sleep_for(std::chrono::nanoseconds(1));
    clearLEDmatrix(ptrSPI, fdSPI, slaveSelect);

    while (true)
    {
        // std::cout << "Outer loop\n";
        while (true) // polling while
        {
            // std::cout << "Inner loop\n";
            int ret = poll(&fds, 1, -1); // poll on a return value

            // there was change in ret
            if (ret >= 1)
            {

                ssize_t nb = read(timer1fd, &info, sizeof(info));
                if (nb == (ssize_t)sizeof(info))
                {
                    /* Do something in response to the interrupt. */
                    printf("Interrupt #%u!\n", info);
                    // if timer has finished (interrupt has risen)
                    // copy data / insert data to shared variable
                    if (isDataFromBackgroundThreadReady == false)
                    {
                        threadSPI(ptrSPI, fdSPI, slaveSelect);
                        gLock.lock();                            // mutex locking - any other thread can't access this variable (think it cannot write or read)
                        threadLoopOutput = threadLoopOutput + 1; // edit the shared variable
                        gLock.unlock();                          // mutex unlock
                        isDataFromBackgroundThreadReady = true;  // flag to main while loop that new data is present
                    }
                    break;
                }
            }
        }

        // resolving and starting timer again
        *((unsigned *)(timer1ptr)) = 0X1C0;
        write(timer1fd, &irq_on, sizeof(irq_on));
        // *((unsigned *)(timer1ptr + 0x4)) = 0xE8287BFF;
        // *((unsigned *)(timer1ptr + 0x4)) = 0xFFFFFF37; // 1 us
        // *((unsigned *)(timer1ptr + 0x4)) = 0xFFE17B7F; // 10 ms
        *((unsigned *)(timer1ptr + 0x4)) = 0xFA0A1EFF; // 0.5 s
        *((unsigned *)(timer1ptr)) = 0XE0;

        // one tick is 0.8 ns, have to wait till data is moved to counter register
        // otherwise it wont start, solve maybe later or ask about it
        std::this_thread::sleep_for(std::chrono::nanoseconds(1));

        *((unsigned *)(timer1ptr + 0x8)) = 0X0;
        *((unsigned *)(timer1ptr)) = 0XC0; // start
    }

    munmap(ptrSPI, MAP_SIZE);
    close(fdSPI);
    munmap(timer1ptr, TIMER_MAP_SIZE);
    close(timer1fd);
}

/*------------------------------------------------------------------------------------*/
/*-------------------------------- MAIN HOST PROGRAM ---------------------------------*/
int main(int argc, char *argv[])
{

    // Register signal and signal handler
    signal(SIGINT, signal_callback_handler);

    /*-|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||-*/
    /*-------------------------------- SETTING THE ENVIRONMENT ---------------------------------*/
    // TARGET_DEVICE macro needs to be passed from gcc command line
    if (argc != 2)
    {
        std::cout << "Usage: " << argv[0] << " <xclbin>" << std::endl;
        return EXIT_FAILURE;
    }

    std::string xclbinFilename = argv[1];

    /*-------------------------------------------------------------------------------------*/
    /*-------------------------------- OPENCL DEFINITIONS ---------------------------------*/
    std::vector<cl::Device> devices;
    cl_int err;
    cl::Context context;
    cl::CommandQueue q;
    cl::Program program;
    std::vector<cl::Platform> platforms;
    bool found_device = false;
    /*-----------------------------------------------------------------------------------------*/

    /*-------------------------------- OPENCL KERNEL CREATION ---------------------------------*/
    cl::Kernel krnl_calculateCurVelModel;
    cl::Kernel krnl_calculateInvMot;
    /*----------------------------------------------------------------------------------------*/

    /*-------------------------------------------------------------------------------------*/
    /*------------------------- PLATFORM SEARCHING AND SELECTING --------------------------*/

    // Searching through devices on a platform and connecting from CPU to device (PL)
    cl::Platform::get(&platforms);

    // Loop through all the platforms based on a vector of platforms size till the device is found (found_device==true)
    for (size_t i = 0; (i < platforms.size()) & (found_device == false); i++)
    {
        cl::Platform platform = platforms[i];
        std::string platformName = platform.getInfo<CL_PLATFORM_NAME>();

        // If the found device is Xilinx device
        if (platformName == "Xilinx")
        {
            devices.clear();
            // Gets list of devices for selected platform and pastes them on the adress of devices Device
            platform.getDevices(CL_DEVICE_TYPE_ACCELERATOR, &devices);
            if (devices.size())
            {
                found_device = true;
                break;
            }
        }
    }

    if (found_device == false)
    {
        std::cout << "Error: Unable to find Target Device " << std::endl;
        return EXIT_FAILURE;
    }
    /*-------------------------------------------------------------------------------------*/

    /*-------------------------------------------------------------------------------------*/
    /*--------------------- LOADING AND READING XCLBIN FILE TO FPGA ----------------------*/
    // Check for availability of PL program
    std::cout << "INFO: Reading " << xclbinFilename << std::endl;
    FILE *fp;
    if ((fp = fopen(xclbinFilename.c_str(), "r")) == nullptr)
    {
        printf("ERROR: %s xclbin not available please build\n", xclbinFilename.c_str());
        exit(EXIT_FAILURE);
    }

    // Load xclbin
    std::cout << "Loading: '" << xclbinFilename << "'\n";
    std::ifstream bin_file(xclbinFilename, std::ifstream::binary);

    // Sets the position of the next character to be extracted from the input stream.
    // istream& seekg (streamoff off, ios_base::seekdir way); off = offset from way - where you are offseting from
    // Basically moving pointer in the stream
    bin_file.seekg(0, bin_file.end);
    unsigned nb = bin_file.tellg();  // Returns the position of the current character in the input stream.
    bin_file.seekg(0, bin_file.beg); // Setting position of the next character to be extracted from the input stream
    char *buf = new char[nb];        // Creating buffer/array with the length of the stream => bin_file

    // buf - pointer of the array where the extracted characters are stored
    // nb – number of characters to extract.
    bin_file.read(buf, nb);

    /*-------------------------------------------------------------------------------------*/

    /*-------------------------------------------------------------------------------------*/
    /*------------------------------- PROGRAMMING THE FPGA -------------------------------*/
    // Creating Program from Binary File
    cl::Program::Binaries bins;
    // {} initialization of vector - array
    // push_back - Adds a new element at the end of the vector, after its current last element.
    bins.push_back({buf, nb});
    bool valid_device = false;

    for (unsigned int i = 0; i < devices.size(); i++)
    {
        auto device = devices[i];
        // Creating Context and Command Queue for selected Device

        OCL_CHECK(err, context = cl::Context(device, nullptr, nullptr, nullptr, &err));

        OCL_CHECK(err, q = cl::CommandQueue(context, device, CL_QUEUE_PROFILING_ENABLE, &err));

        std::cout << "Trying to program device[" << i << "]: " << device.getInfo<CL_DEVICE_NAME>() << std::endl;

        // cl:Program - Construct a program object from a list of devices and a per-device list of binaries.
        cl::Program program(context, {device}, bins, nullptr, &err);

        if (err != CL_SUCCESS)
        {
            std::cout << "Failed to program device[" << i << "] with xclbin file!\n";
        }
        else
        {
            std::cout << "Device[" << i << "]: program successful!\n";

            /*-------------------------------- SELECTING KERNEL FROM KERNEL FILE ---------------------------------*/

            // This call will get the kernel object from program. A kernel is an
            // OpenCL function that is executed on the FPGA.
            OCL_CHECK(err, krnl_calculateCurVelModel = cl::Kernel(program, "krnl_calculateCurVelModel", &err));
            OCL_CHECK(err, krnl_calculateInvMot = cl::Kernel(program, "krnl_calculateInvMot", &err));
            valid_device = true;

            break; // break because found a valid device

            /*------------------------------------------------------------------------------------------------------*/
        }
    }
    if (!valid_device)
    {
        std::cout << "Failed to program any device found, exit!\n";
        exit(EXIT_FAILURE);
    }

    /*-||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||-*/

    /****************************************************************************************************************/

    /*
     * @name The App
     * @brief Main CPU and FPGA application.
     */

    int modeSelection = 0;
    printf("Select mode:\n0 - preloaded data (disabled)\n1 - keyboard input data (partialy enabled)\n2 - CPU/FPGA model\n3 - timer thread test + SPI\n4 - SPI send data\n");
    scanf("%i", &modeSelection);
    printf("You have selected: %i\n\r", modeSelection);

    // int masterInputLength = 39;
    // int masterOutputLength = 18;

    int masterInputLength = 4096;
    int masterOutputLength = 4096;
    int masterMotorInputLength = 4096;
    int masterMotorOutputLength = 4096;

    OCL_CHECK(err, cl::Buffer buffer_masterInput(context, CL_MEM_ALLOC_HOST_PTR | CL_MEM_READ_ONLY, masterInputLength * sizeof(float), NULL, &err));
    OCL_CHECK(err, cl::Buffer buffer_masterOutput(context, CL_MEM_ALLOC_HOST_PTR | CL_MEM_WRITE_ONLY, masterOutputLength * sizeof(float), NULL, &err));

    int narg = 0;

    OCL_CHECK(err, err = krnl_calculateCurVelModel.setArg(narg++, buffer_masterInput));
    OCL_CHECK(err, err = krnl_calculateCurVelModel.setArg(narg++, buffer_masterOutput));

    float *masterInput;
    float *masterOutput;

    OCL_CHECK(err, masterInput = (float *)q.enqueueMapBuffer(buffer_masterInput, CL_TRUE, CL_MAP_WRITE, 0, masterInputLength * sizeof(float), NULL, NULL, &err));

    OCL_CHECK(err, masterOutput = (float *)q.enqueueMapBuffer(buffer_masterOutput, CL_TRUE, CL_MAP_READ, 0, masterOutputLength * sizeof(float), NULL, NULL, &err));

    OCL_CHECK(err, cl::Buffer buffer_masterMotorInput(context, CL_MEM_ALLOC_HOST_PTR | CL_MEM_READ_ONLY, masterMotorInputLength * sizeof(float), NULL, &err));
    OCL_CHECK(err, cl::Buffer buffer_masterMotorOutput(context, CL_MEM_ALLOC_HOST_PTR | CL_MEM_WRITE_ONLY, masterMotorOutputLength * sizeof(float), NULL, &err));

    narg = 0;

    OCL_CHECK(err, err = krnl_calculateInvMot.setArg(narg++, buffer_masterMotorInput));
    OCL_CHECK(err, err = krnl_calculateInvMot.setArg(narg++, buffer_masterMotorOutput));

    float *masterMotorInput;
    float *masterMotorOutput;

    OCL_CHECK(err, masterMotorInput = (float *)q.enqueueMapBuffer(buffer_masterMotorInput, CL_TRUE, CL_MAP_WRITE, 0, masterMotorInputLength * sizeof(float), NULL, NULL, &err));

    OCL_CHECK(err, masterMotorOutput = (float *)q.enqueueMapBuffer(buffer_masterMotorOutput, CL_TRUE, CL_MAP_READ, 0, masterMotorOutputLength * sizeof(float), NULL, NULL, &err));

    /*-------------------------------------------------------------*/
    /*-------------------- CLASS DEFINITIONS ---------------------*/
    MotorModelClass MotorModel;
    svmCoreClass svmCore;
    CurVelModelClass CurVelModel;
    RegulatorClass Regulator;
    TransformationClass Transformation;
    InvertorClass Invertor;
    /*-------------------------------------------------------------*/

    /*-----------------------------------------------------------*/
    /*-------------------- GLOBAL SETTINGS ---------------------*/
    // global settings which will be translated in additional parameters
    float globalCalculationStep = 0.000001;
    float globalInitialCalculationTime = 0;
    float globalFinalCalculationTime = 1;
    // float commonModeVoltage;
    // float compareLevel;
    // float trianglePoint;
    float minMaxCommonModeVoltageConstant = 287;
    float uS = 400;
    float uDC = ((3 * sqrt(2)) / PI) * uS;
    float globalSimulationTime = globalInitialCalculationTime;
    /*-------------------------------------------------------------*/

    /****************************************************************/
    /*-------------------- MEMORY ALLOCATION ---------------------*/

    /*-------------------- ASYNCHRONOUS MOTOR MODEL ---------------------*/
    MotorModel.odeCalculationSettingsAllocateMemory();

    // needed for further memory allocation
    MotorModel.setOdeCalculationSettings(globalInitialCalculationTime, globalFinalCalculationTime, globalCalculationStep);

    MotorModel.motorParametersAllocateMemory();
    MotorModel.stateSpaceCoeffAllocateMemory();
    MotorModel.modelVariablesForOnlineCalculationAllocateMemory();
    // MotorModel.voltageGeneratorDataAllocateMemory();
    /*-------------------- END OF ASYNCHRONOUS MOTOR MODEL ---------------------*/

    /*-------------------- CURRENT VELOCITY MOTOR MODEL ---------------------*/
    CurVelModel.motorParametersAllocateMemory();
    CurVelModel.motorCoeffAllocateMemory();
    CurVelModel.modelCVVariablesAllocateMemory();
    CurVelModel.odeCVCalculationSettingsAllocateMemory();
    /*---------------- END OF CURRENT VELOCITY MOTOR MODEL -----------------*/

    /*-------------------- SPACE VECTOR MODULATION CORE ---------------------*/
    // svmCore.phaseWantedVoltageAllocateMemory(); // depracated
    // svmCore.invertorSwitchAllocateMemory();
    svmCore.triangleWaveSettingsAllocateMemory();
    svmCore.coreInternalVariablesAllocateMemory();
    /*----------------------------------------------------------------------*/

    /*-------------------- REGULATORS ---------------------*/
    Regulator.regulatorAllocateMemory();
    /*---------------------------------------------------*/

    /*-------------------- INVERTOR ---------------------*/
    Invertor.reconstructedInvertorOutputVoltageAllocateMemory();

    /****************************************************************/

    /*----------------------------- SET ASYNCHRONOUS MOTOR PARAMETERS FOR MODEL ------------------------------*/
    // MotorModel.setMotorParameters(); // hardcoded values
    MotorModel.motorParameters->R1 = 0.370f;            // Ohm, stator rezistance
    MotorModel.motorParameters->R2 = 0.225f;            // Ohm, rotor rezistance
    MotorModel.motorParameters->L1s = 0.00227f;         // H, stator leakage inductance
    MotorModel.motorParameters->L2s = 0.00227f;         // H, rotor leakage inductance
    MotorModel.motorParameters->Lm = 0.0825f;           // H, main flux inductance
    MotorModel.motorParameters->L1 = 0.08477f;          // H, inductance
    MotorModel.motorParameters->L2 = 0.08477f;          // H, inductance
    MotorModel.motorParameters->sigma = 0.05283f;       // = 0.0528396032, sigma = 1 - Lm^(2)/L1L2
    MotorModel.motorParameters->nOfPolePairs = 2;       // number of pole pairs
    MotorModel.motorParameters->momentOfIntertia = 0.4; // J, moment of inertia
    MotorModel.setStateSpaceCoeff();
    MotorModel.odeCalculationSettings->numberOfIterations = MotorModel.numberOfIterations();

    // initial values of ASM model
    MotorModel.modelVariables->i1alpha = 0;
    MotorModel.modelVariables->i1beta = 0;
    MotorModel.modelVariables->psi2alpha = 0;
    MotorModel.modelVariables->psi2beta = 0;
    MotorModel.modelVariables->loadTorque = 0;
    MotorModel.modelVariables->motorMechanicalAngularVelocity = 0;
    MotorModel.modelVariables->motorTorque = 0;
    MotorModel.modelVariables->u1alpha = 0;
    MotorModel.modelVariables->u1beta = 0;

    /*------------------------------------------------------------------------------------------*/
    /*-------------------- CURRENT VELOCITY MODEL SETTINGS/ INITIAL VALUES ---------------------*/
    CurVelModel.modelCVVariables->psi2alpha = 0;
    CurVelModel.modelCVVariables->psi2beta = 0;
    CurVelModel.modelCVVariables->transformAngle = 0;
    CurVelModel.odeCVCalculationSettings->calculationStep = globalCalculationStep; // just a helper variable defined on top of this file to hava the same number of samples for ASM motor model and Current-Velocity model
    CurVelModel.motorParameters->R2 = 0.225f;                                      // Ohm, rotor rezistance
    CurVelModel.motorParameters->Lm = 0.0825f;                                     // H, main flux inductance
    CurVelModel.motorParameters->L2 = 0.08477f;                                    // H, inductance
    // CurVelModel.motorParameters->nOfPolePairs = 2; // number of pole pairs
    CurVelModel.calculateMotorCVCoeff(CurVelModel.modelCVCoeff, CurVelModel.motorParameters);
    CurVelModel.modelCVCoeff->nOfPolePairs = 2;
    /*------------------------------------------------------------------------------------------*/

    /*------------------------------------------------------------------*/
    /*-------------------- TRIANGLE WAVE SETTINGS ---------------------*/
    svmCore.triangleWaveSettings->waveAmplitude = 1;
    svmCore.triangleWaveSettings->calculationStep = globalCalculationStep;
    svmCore.triangleWaveSettings->wavePeriod = 0.0001;
    svmCore.triangleWaveSettings->calculationTime = globalInitialCalculationTime;
    /*------------------------------------------------------------------*/

    /*--------------------------------------------------------------------*/
    /*-------------------- IVNVERTOR MODEL SETTINGS ---------------------*/
    float Udcmax = 311.87;
    invertorSwitchGlobal.sw1 = 0;
    invertorSwitchGlobal.sw2 = 0;
    invertorSwitchGlobal.sw3 = 0;
    invertorSwitchGlobal.sw4 = 0;
    invertorSwitchGlobal.sw5 = 0;
    invertorSwitchGlobal.sw6 = 0;
    /*--------------------------------------------------------------------*/

    /****************************************************************/
    /*-------------------- REGULATOR SETTINGS ---------------------*/

    /*-------------------- FLUX REGULATOR ---------------------*/
    Regulator.fluxRegulator->ki = 30300; // 30300
    Regulator.fluxRegulator->kp = 11410; // 11410
    Regulator.fluxRegulator->saturationOutputMax = 16.4223;
    Regulator.fluxRegulator->saturationOutputMin = 0;
    Regulator.fluxRegulator->saturationOutput = 0;
    Regulator.fluxRegulator->clampingStatus = false;
    Regulator.fluxRegulator->saturationCheckStatus = false;
    Regulator.fluxRegulator->signCheckStatus = false;
    Regulator.fluxRegulator->measuredValue = 0;
    Regulator.fluxRegulator->iSum = 0;
    /*------------------------------------------------------*/

    /*-------------------- VELOCITY REGULATOR ---------------------*/
    Regulator.velocityRegulator->ki = 4650000; // 4650000
    Regulator.velocityRegulator->kp = 3720;    // 3720
    Regulator.velocityRegulator->saturationOutputMax = 29.1228;
    Regulator.velocityRegulator->saturationOutputMin = -29.1228;
    Regulator.velocityRegulator->saturationOutput = 0;
    Regulator.velocityRegulator->clampingStatus = false;
    Regulator.velocityRegulator->saturationCheckStatus = false;
    Regulator.velocityRegulator->signCheckStatus = false;
    Regulator.velocityRegulator->measuredValue = 0;
    Regulator.velocityRegulator->iSum = 0;
    /*------------------------------------------------------*/

    /*-------------------- ID REGULATOR ---------------------*/
    Regulator.idRegulator->ki = 2915.6;                  // 2915.6
    Regulator.idRegulator->kp = 22.3961;                 // 22.3961
    Regulator.idRegulator->saturationOutputMax = Udcmax; // (3*sqrt(2))/(pi*sqrt(3))*Us = 400(3*sqrt(2))/(3.141592*sqrt(3)) = 311.87
    Regulator.idRegulator->saturationOutputMin = 0;      // - Udcmax
    Regulator.idRegulator->clampingStatus = false;
    Regulator.idRegulator->saturationCheckStatus = false;
    Regulator.idRegulator->signCheckStatus = false;
    Regulator.idRegulator->measuredValue = 0;
    Regulator.idRegulator->iSum = 0;
    /*------------------------------------------------------*/

    /*-------------------- IQ REGULATOR ---------------------*/
    Regulator.iqRegulator->ki = 2915.6;                                                                                                                          // 2915.6
    Regulator.iqRegulator->kp = 22.3961;                                                                                                                         // 22.3961
    Regulator.iqRegulator->saturationOutputMax = sqrtf((Udcmax * Udcmax) - (Regulator.idRegulator->saturationOutput * Regulator.idRegulator->saturationOutput)); // sqrt(Udcmax^2 - u1d^2) dynamically
    Regulator.iqRegulator->saturationOutputMin = -Regulator.iqRegulator->saturationOutputMax;
    Regulator.iqRegulator->clampingStatus = false;
    Regulator.iqRegulator->saturationCheckStatus = false;
    Regulator.iqRegulator->signCheckStatus = false;
    Regulator.iqRegulator->measuredValue = 0;
    Regulator.iqRegulator->iSum = 0;
    /*------------------------------------------------------*/

    /****************************************************************/

    /*--------------------------------------------------------------*/
    /*-------------------- WANTED VALUES INPUT ---------------------*/
    // now hardcoded, change later
    Regulator.fluxRegulator->wantedValue = 1;
    Regulator.velocityRegulator->wantedValue = 0;
    Regulator.idRegulator->wantedValue = 0;
    Regulator.iqRegulator->wantedValue = 0;
    /*--------------------------------------------------------------*/

    float inputI1 = 0;
    float inputI2 = 0;
    float inputI3 = 0;

    if (modeSelection == 2)
    {

        /*------------------------------------------------------------*/
        /*-------------------- PROGRAM SETTINGS ---------------------*/
        int verboseOutput = false;
        int numberOfIterations = 0;
        std::cout << "\n\r------------------------------------------\n";
        std::cout << "Select verbose output:\n0 - disabled\n1 - enabled\n";
        std::cout << "------------------------------------------\n";
        scanf("%i", &verboseOutput);
        // std::cout << "\n\r------------------------------------------\n";
        // std::cout << "Select numberOfIterations\n";
        // std::cout << "------------------------------------------\n";
        // scanf("%i", &numberOfIterations);
        /*------------------------------------------------------------*/
        /*------------------------------------------------------------------------*/
        /*-------------------- OUTPUT CSV DATA FILE CREATION ---------------------*/
        std::ofstream globalSimulationData;
        globalSimulationData.open("outputData/globalSimulationData.csv", std::ofstream::out | std::ofstream::trunc);
        /*-------------------------------------------------------------------*/

        std::chrono::time_point<std::chrono::system_clock> startTime;
        std::chrono::time_point<std::chrono::system_clock> endTime;

        /*
         * @brief Initial values settings
         *
         * */

        masterInput[0] = globalSimulationTime;
        masterInput[1] = globalCalculationStep;
        masterInput[2] = minMaxCommonModeVoltageConstant;
        masterInput[3] = globalCalculationStep / 2;
        masterInput[4] = inputI1;
        masterInput[5] = inputI2;
        masterInput[6] = inputI3;
        masterInput[7] = MotorModel.modelVariables->motorMechanicalAngularVelocity;
        masterInput[8] = CurVelModel.modelCVCoeff->R2MLmDL2;
        masterInput[9] = CurVelModel.modelCVCoeff->R2DL2;
        masterInput[10] = CurVelModel.modelCVCoeff->nOfPolePairs;
        masterInput[11] = svmCore.triangleWaveSettings->waveAmplitude;
        masterInput[12] = svmCore.triangleWaveSettings->calculationStep;
        masterInput[13] = svmCore.triangleWaveSettings->wavePeriod;
        masterInput[14] = svmCore.triangleWaveSettings->calculationTime;
        masterInput[15] = Udcmax;
        masterInput[16] = Regulator.fluxRegulator->ki;
        masterInput[17] = Regulator.fluxRegulator->kp;
        masterInput[18] = Regulator.fluxRegulator->saturationOutputMax;
        masterInput[19] = Regulator.fluxRegulator->saturationOutputMin;
        masterInput[20] = Regulator.fluxRegulator->iSum;
        masterInput[21] = Regulator.velocityRegulator->ki;
        masterInput[22] = Regulator.velocityRegulator->kp;
        masterInput[23] = Regulator.velocityRegulator->saturationOutputMax;
        masterInput[24] = Regulator.velocityRegulator->saturationOutputMin;
        masterInput[25] = Regulator.velocityRegulator->iSum;
        masterInput[26] = Regulator.idRegulator->ki;
        masterInput[27] = Regulator.idRegulator->kp;
        masterInput[28] = Regulator.idRegulator->saturationOutput;
        masterInput[29] = Regulator.idRegulator->iSum;
        masterInput[30] = Regulator.iqRegulator->ki;
        masterInput[31] = Regulator.iqRegulator->kp;
        masterInput[32] = Regulator.iqRegulator->iSum;
        masterInput[33] = Regulator.fluxRegulator->wantedValue;
        masterInput[34] = Regulator.velocityRegulator->wantedValue;
        masterInput[35] = Regulator.idRegulator->wantedValue;
        masterInput[36] = Regulator.iqRegulator->wantedValue;
        masterInput[37] = CurVelModel.modelCVVariables->psi2alpha;
        masterInput[38] = CurVelModel.modelCVVariables->psi2beta;
        masterInput[39] = Regulator.idRegulator->saturationOutputMax;
        masterInput[40] = Regulator.idRegulator->saturationOutputMin;
        /***************************************************************/
        /*-------------------- MAIN PROGRAM LOOP ---------------------*/

        startTime = std::chrono::system_clock::now();

        for (int i = 0; i < 1000000; i++) // replace with while in production, but in this model, it is suitable to use for cycle
        {

            /*-------------------- CONSOLE OUTPUT FOR TESTING PURPOSES BASED ON A USER SETTINGS ---------------------*/
            if (verboseOutput and (i < 4))
            {
                std::cout << "iteration number: " << i << "\n";
            }
            /*-------------------------------------------------------------------------------------------------------*/
            if (i >= 600000)
            {
                Regulator.velocityRegulator->wantedValue = 10;
            }
            masterInput[0] = globalSimulationTime;
            masterInput[1] = globalCalculationStep;
            masterInput[2] = minMaxCommonModeVoltageConstant;
            masterInput[3] = globalCalculationStep / 2;
            masterInput[4] = inputI1;
            masterInput[5] = inputI2;
            masterInput[6] = inputI3;
            masterInput[7] = MotorModel.modelVariables->motorMechanicalAngularVelocity;
            masterInput[8] = CurVelModel.modelCVCoeff->R2MLmDL2;
            masterInput[9] = CurVelModel.modelCVCoeff->R2DL2;
            masterInput[10] = CurVelModel.modelCVCoeff->nOfPolePairs;
            masterInput[11] = svmCore.triangleWaveSettings->waveAmplitude;
            masterInput[12] = svmCore.triangleWaveSettings->calculationStep;
            masterInput[13] = svmCore.triangleWaveSettings->wavePeriod;
            masterInput[14] = svmCore.triangleWaveSettings->calculationTime;
            masterInput[15] = Udcmax;
            masterInput[16] = Regulator.fluxRegulator->ki;
            masterInput[17] = Regulator.fluxRegulator->kp;
            masterInput[18] = Regulator.fluxRegulator->saturationOutputMax;
            masterInput[19] = Regulator.fluxRegulator->saturationOutputMin;
            masterInput[20] = Regulator.fluxRegulator->iSum;
            masterInput[21] = Regulator.velocityRegulator->ki;
            masterInput[22] = Regulator.velocityRegulator->kp;
            masterInput[23] = Regulator.velocityRegulator->saturationOutputMax;
            masterInput[24] = Regulator.velocityRegulator->saturationOutputMin;
            masterInput[25] = Regulator.velocityRegulator->iSum;
            masterInput[26] = Regulator.idRegulator->ki;
            masterInput[27] = Regulator.idRegulator->kp;
            masterInput[28] = Regulator.idRegulator->saturationOutput;
            masterInput[29] = Regulator.idRegulator->iSum;
            masterInput[30] = Regulator.iqRegulator->ki;
            masterInput[31] = Regulator.iqRegulator->kp;
            masterInput[32] = Regulator.iqRegulator->iSum;
            masterInput[33] = Regulator.fluxRegulator->wantedValue;
            masterInput[34] = Regulator.velocityRegulator->wantedValue;
            masterInput[35] = Regulator.idRegulator->wantedValue;
            masterInput[36] = Regulator.iqRegulator->wantedValue;
            masterInput[37] = CurVelModel.modelCVVariables->psi2alpha;
            masterInput[38] = CurVelModel.modelCVVariables->psi2beta;
            masterInput[39] = Regulator.idRegulator->saturationOutputMax;
            masterInput[40] = Regulator.idRegulator->saturationOutputMin;

            OCL_CHECK(err, err = q.enqueueMigrateMemObjects({buffer_masterInput}, 0 /* 0 means from host*/));
            OCL_CHECK(err, q.finish());

            OCL_CHECK(err, err = q.enqueueTask(krnl_calculateCurVelModel));
            OCL_CHECK(err, q.finish());

            OCL_CHECK(err, q.enqueueMigrateMemObjects({buffer_masterOutput}, CL_MIGRATE_MEM_OBJECT_HOST));

            OCL_CHECK(err, q.finish());

            invertorSwitchGlobal.sw1 = masterOutput[0];
            invertorSwitchGlobal.sw2 = masterOutput[1];
            invertorSwitchGlobal.sw3 = masterOutput[2];
            invertorSwitchGlobal.sw4 = masterOutput[3];
            invertorSwitchGlobal.sw5 = masterOutput[4];
            invertorSwitchGlobal.sw6 = masterOutput[5];

            svmCore.triangleWaveSettings->calculationTime = masterOutput[6];
            Regulator.fluxRegulator->iSum = masterOutput[7];
            Regulator.velocityRegulator->iSum = masterOutput[8];
            Regulator.idRegulator->saturationOutput = masterOutput[9];
            Regulator.idRegulator->iSum = masterOutput[10];
            Regulator.iqRegulator->iSum = masterOutput[11];
            CurVelModel.modelCVVariables->psi2alpha = masterOutput[12];
            CurVelModel.modelCVVariables->psi2beta = masterOutput[13];

            // we got switches from kernel

            /*-------------------- CONSOLE OUTPUT FOR TESTING PURPOSES BASED ON A USER SETTINGS ---------------------*/
            if (verboseOutput and (i < 4))
            {
                std::cout << "sw1: " << invertorSwitchGlobal.sw1 << "\n";
                std::cout << "sw2: " << invertorSwitchGlobal.sw2 << "\n";
                std::cout << "sw3: " << invertorSwitchGlobal.sw3 << "\n";
                std::cout << "sw4: " << invertorSwitchGlobal.sw4 << "\n";
                std::cout << "sw5: " << invertorSwitchGlobal.sw5 << "\n";
                std::cout << "sw6: " << invertorSwitchGlobal.sw6 << "\n";
            }
            /*------------------------------------------------------------------------------------------------------*/

            masterMotorInput[0] = invertorSwitchGlobal.sw1;
            masterMotorInput[1] = invertorSwitchGlobal.sw2;
            masterMotorInput[2] = invertorSwitchGlobal.sw3;
            masterMotorInput[3] = invertorSwitchGlobal.sw4;
            masterMotorInput[4] = invertorSwitchGlobal.sw5;
            masterMotorInput[5] = invertorSwitchGlobal.sw6;
            masterMotorInput[6] = uDC;
            masterMotorInput[7] = MotorModel.modelVariables->psi2alpha;
            masterMotorInput[8] = MotorModel.modelVariables->psi2beta;
            masterMotorInput[9] = MotorModel.modelVariables->i1alpha;
            masterMotorInput[10] = MotorModel.modelVariables->i1beta;
            masterMotorInput[11] = MotorModel.modelVariables->loadTorque;
            masterMotorInput[12] = MotorModel.modelVariables->motorMechanicalAngularVelocity;
            masterMotorInput[13] = globalCalculationStep / 2;
            masterMotorInput[14] = globalCalculationStep;
            masterMotorInput[15] = MotorModel.motorParameters->Lm;
            masterMotorInput[16] = MotorModel.motorParameters->sigma;
            masterMotorInput[17] = MotorModel.motorParameters->L1;
            masterMotorInput[18] = MotorModel.motorParameters->L2;
            masterMotorInput[19] = MotorModel.motorParameters->nOfPolePairs;
            masterMotorInput[20] = MotorModel.motorParameters->momentOfIntertia;
            masterMotorInput[21] = MotorModel.stateSpaceCoeff->a11;
            masterMotorInput[22] = MotorModel.stateSpaceCoeff->a12;
            masterMotorInput[23] = MotorModel.stateSpaceCoeff->a13;
            masterMotorInput[24] = MotorModel.stateSpaceCoeff->a14;
            masterMotorInput[25] = MotorModel.stateSpaceCoeff->a21;
            masterMotorInput[26] = MotorModel.stateSpaceCoeff->a22;
            masterMotorInput[27] = MotorModel.stateSpaceCoeff->a23;
            masterMotorInput[28] = MotorModel.stateSpaceCoeff->a24;
            masterMotorInput[29] = MotorModel.stateSpaceCoeff->a31;
            masterMotorInput[30] = MotorModel.stateSpaceCoeff->a32;
            masterMotorInput[31] = MotorModel.stateSpaceCoeff->a33;
            masterMotorInput[32] = MotorModel.stateSpaceCoeff->a34;
            masterMotorInput[33] = MotorModel.stateSpaceCoeff->a41;
            masterMotorInput[34] = MotorModel.stateSpaceCoeff->a42;
            masterMotorInput[35] = MotorModel.stateSpaceCoeff->a43;
            masterMotorInput[36] = MotorModel.stateSpaceCoeff->a44;
            masterMotorInput[37] = MotorModel.stateSpaceCoeff->b11;
            masterMotorInput[38] = MotorModel.stateSpaceCoeff->b22;

            // kernel tasks here
            /***************************************************/

            OCL_CHECK(err, err = q.enqueueMigrateMemObjects({buffer_masterMotorInput}, 0 /* 0 means from host*/));
            OCL_CHECK(err, q.finish());

            OCL_CHECK(err, err = q.enqueueTask(krnl_calculateInvMot));
            OCL_CHECK(err, q.finish());

            OCL_CHECK(err, q.enqueueMigrateMemObjects({buffer_masterMotorOutput}, CL_MIGRATE_MEM_OBJECT_HOST));
            OCL_CHECK(err, q.finish());

            MotorModel.modelVariables->i1alpha = masterMotorOutput[0];
            MotorModel.modelVariables->i1beta = masterMotorOutput[1];
            MotorModel.modelVariables->psi2alpha = masterMotorOutput[2];
            MotorModel.modelVariables->psi2beta = masterMotorOutput[3];
            MotorModel.modelVariables->motorTorque = masterMotorOutput[4];
            MotorModel.modelVariables->motorMechanicalAngularVelocity = masterMotorOutput[5];
            inputI1 = masterMotorOutput[6];
            inputI2 = masterMotorOutput[7];
            inputI3 = masterMotorOutput[8];

            /*-------------------- SIMULATED INVERTOR FOR SIMULATION WITH 3 PHASE CONTROLLED THYRISTOR 400 V ---------------------*/
            // invertor voltage reconstruction for phase A, B, C
            // Invertor.invertorReconstructVoltages(svmCore.invertorSwitch, Invertor.reconstructedInvertorOutputVoltage, uDC);
            /*-------------------------------------------------------------------------------------------------------------------*/

            /*-------------------- CONSOLE OUTPUT FOR TESTING PURPOSES BASED ON A USER SETTINGS ---------------------*/
            if (verboseOutput and (i < 4))
            {
                std::cout << "reconstructed u1a: " << Invertor.reconstructedInvertorOutputVoltage->u1a << "\n";
                std::cout << "reconstructed u1b: " << Invertor.reconstructedInvertorOutputVoltage->u1b << "\n";
                std::cout << "reconstructed u1c: " << Invertor.reconstructedInvertorOutputVoltage->u1c << "\n";
            }
            /*------------------------------------------------------------------------------------------------------*/

            /******************************************************************************/
            /*-------------------- VIRTUAL ASYNCHRONOUS MOTOR MODEL ---------------------*/

            /*---------------------------------------------------------------------------*/
            /*-------------------- CLARKE TRANSFORM FOR ASM MODEL ---------------------*/
            // MotorModel.modelVariables->u1alpha = Transformation.clarkeTransform1(Invertor.reconstructedInvertorOutputVoltage->u1a, Invertor.reconstructedInvertorOutputVoltage->u1b, Invertor.reconstructedInvertorOutputVoltage->u1c, 0.6667);
            // MotorModel.modelVariables->u1beta = Transformation.clarkeTransform2(Invertor.reconstructedInvertorOutputVoltage->u1a, Invertor.reconstructedInvertorOutputVoltage->u1b, Invertor.reconstructedInvertorOutputVoltage->u1c, 0.6667);
            /*---------------------------------------------------------------------------*/

            /*-------------------- CONSOLE OUTPUT FOR TESTING PURPOSES BASED ON A USER SETTINGS ---------------------*/
            if (verboseOutput and (i < 4))
            {
                std::cout << "u1alpha to motor: " << MotorModel.modelVariables->u1alpha << "\n";
                std::cout << "u1beta to motor: " << MotorModel.modelVariables->u1beta << "\n";
            }
            /*-----------------------------------------------------------------------------------------------------*/

            /*-----------------------------------------------------------------------------------*/
            /*-------------------- MAIN ASM MODEL CODE WITH RK4 ODE SOLVING ---------------------*/
            // MotorModel.mathModelCalculateOnlineValue(MotorModel.odeCalculationSettings, MotorModel.modelVariables, MotorModel.stateSpaceCoeff, MotorModel.motorParameters);
            /*-----------------------------------------------------------------------------------*/
            /******************************************************************************/

            /*-------------------- CONSOLE OUTPUT FOR TESTING PURPOSES BASED ON A USER SETTINGS ---------------------*/
            if (verboseOutput and (i < 4))
            {
                // std::cout << "ASM i1alpha: " << MotorModel.modelVariables->i1alpha << "\n";
                // std::cout << "ASM i1beta: " << MotorModel.modelVariables->i1beta << "\n";
                // std::cout << "ASM motorTorque: " << MotorModel.modelVariables->motorTorque<< "\n";
                // std::cout << "ASM motorMechanicalAngularVelocity: " << MotorModel.modelVariables->motorMechanicalAngularVelocity << "\n";
                // std::cout << "ASM psi2alpha: " << MotorModel.modelVariables->psi2alpha << "\n";
                // std::cout << "ASM psi2beta: " << MotorModel.modelVariables->psi2beta << "\n";
            }
            /*-----------------------------------------------------------------------------------------------------*/

            // inputI1 = Transformation.inverseClarkeTransform1(MotorModel.modelVariables->i1alpha, MotorModel.modelVariables->i1beta);
            // inputI2 = Transformation.inverseClarkeTransform2(MotorModel.modelVariables->i1alpha, MotorModel.modelVariables->i1beta);
            // inputI3 = Transformation.inverseClarkeTransform3(MotorModel.modelVariables->i1alpha, MotorModel.modelVariables->i1beta);

            /*-------------------- CONSOLE OUTPUT FOR TESTING PURPOSES BASED ON A USER SETTINGS ---------------------*/
            if (verboseOutput and (i < 4))
            {

                std::cout << "sw1: " << masterOutput[0] << "\n";
                std::cout << "sw2: " << masterOutput[1] << "\n";
                std::cout << "sw3: " << masterOutput[2] << "\n";
                std::cout << "sw4: " << masterOutput[3] << "\n";
                std::cout << "sw5: " << masterOutput[4] << "\n";
                std::cout << "sw6: " << masterOutput[5] << "\n";
                std::cout << "triangleWaveSettings.calculationTime: " << masterOutput[6] << "\n";
                std::cout << "fluxRegulator.iSum: " << masterOutput[7] << "\n";
                std::cout << "velocityRegulator.iSum: " << masterOutput[8] << "\n";
                std::cout << "idRegulator.saturationOutput: " << masterOutput[9] << "\n";
                std::cout << "idRegulator.iSum: " << masterOutput[10] << "\n";
                std::cout << "iqRegulator.iSum: " << masterOutput[11] << "\n";
                std::cout << "psi2alpha: " << masterOutput[12] << "\n";
                std::cout << "psi2beta: " << masterOutput[13] << "\n";
                std::cout << "psi2amplitude: " << masterOutput[14] << "\n";
                std::cout << "idRegulator.measuredValue: " << masterOutput[15] << "\n";
                std::cout << "idRegulator.wantedValue: " << masterOutput[16] << "\n";
                std::cout << "transformationAngle: " << masterOutput[17] << "\n";
            }
            /*--------------------------------------------------------------------------------------------------------*/

            /*--------------------------------------------------------------------------------------------------------------------------------------------------*/

            /****************************************************************************************************************************************************/

            /*-------------------------------------------------------------------------------*/
            /*-------------------- OUTPUT CSV DATA INSERTING TO A FILE ---------------------*/
            globalSimulationData << globalSimulationTime << "," << masterOutput[14] << "," << MotorModel.modelVariables->motorMechanicalAngularVelocity << "," << masterOutput[15] << "," << masterOutput[16] << "," << Regulator.velocityRegulator->wantedValue << "," << masterOutput[18] << "," << masterOutput[19] << "," << Regulator.fluxRegulator->iSum << "\n";
            /*-------------------------------------------------------------------------------*/

            /*----------------------------------------------------------------------------------------------------------------------*/
            /*-------------------- UPDATING SIMULATION TIME - ONLY FOR SIMULATION PURPOSES TO OUTPUT DATA CSV ---------------------*/
            globalSimulationTime = globalSimulationTime + globalCalculationStep;
            /*----------------------------------------------------------------------------------------------------------------------*/

            /*-------------------- CONSOLE OUTPUT FOR TESTING PURPOSES BASED ON A USER SETTINGS ---------------------*/
            if (verboseOutput and (i < 4))
            {
                std::cout << "------------------------------------------------------"
                          << "\n";
            }
            /*-------------------------------------------------------------------------------------------------------*/

            /*********************************************************************************/
        }
        endTime = std::chrono::system_clock::now();
        /*-------------------- OUTPUT CSV DATA FILE CLOSING ---------------------*/
        globalSimulationData.close();
        /*----------------------------------------------------------------------*/
        std::chrono::duration<double> diffTime = endTime - startTime;
        std::cout << "Time for the execution is: " << diffTime.count() << "\n";
    }

    if (modeSelection == 1)
    {

        printf("Keyboard input data mode\n\r");
        printf("------------------------------------\n\r");
        printf("Insert data divided by {space symbol}\n\r");
        printf("I1 I2 I3 MechanicalAngularVelocity psi2alpha[0] psi2beta[0] \n\r");

        scanf("%f %f %f %f %f %f", &inputI1, &inputI2, &inputI3, &MotorModel.modelVariables->motorMechanicalAngularVelocity, &CurVelModel.modelCVVariables->psi2alpha, &CurVelModel.modelCVVariables->psi2beta);
        printf("------------------------------------\n\r");
        printf("You have entered:\n\r");
        printf("I1 = %f\n\rI2 = %f\n\rI3 = %f\n\rMechanicalAngularVelocity = %f\n\rpsi2alpha[0] = %f\n\rpsi2beta = %f\n\r", inputI1, inputI2, inputI3, MotorModel.modelVariables->motorMechanicalAngularVelocity, CurVelModel.modelCVVariables->psi2alpha, CurVelModel.modelCVVariables->psi2beta);
        printf("------------------------------------\n\r");

        masterInput[0] = globalSimulationTime;
        masterInput[1] = globalCalculationStep;
        masterInput[2] = minMaxCommonModeVoltageConstant;
        masterInput[3] = globalCalculationStep / 2;
        masterInput[4] = inputI1;
        masterInput[5] = inputI2;
        masterInput[6] = inputI3;
        masterInput[7] = MotorModel.modelVariables->motorMechanicalAngularVelocity;
        masterInput[8] = CurVelModel.modelCVCoeff->R2MLmDL2;
        masterInput[9] = CurVelModel.modelCVCoeff->R2DL2;
        masterInput[10] = CurVelModel.modelCVCoeff->nOfPolePairs;
        masterInput[11] = svmCore.triangleWaveSettings->waveAmplitude;
        masterInput[12] = svmCore.triangleWaveSettings->calculationStep;
        masterInput[13] = svmCore.triangleWaveSettings->wavePeriod;
        masterInput[14] = svmCore.triangleWaveSettings->calculationTime;
        masterInput[15] = Udcmax;
        masterInput[16] = Regulator.fluxRegulator->ki;
        masterInput[17] = Regulator.fluxRegulator->kp;
        masterInput[18] = Regulator.fluxRegulator->saturationOutputMax;
        masterInput[19] = Regulator.fluxRegulator->saturationOutputMin;
        masterInput[20] = Regulator.fluxRegulator->iSum;
        masterInput[21] = Regulator.velocityRegulator->ki;
        masterInput[22] = Regulator.velocityRegulator->kp;
        masterInput[23] = Regulator.velocityRegulator->saturationOutputMax;
        masterInput[24] = Regulator.velocityRegulator->saturationOutputMin;
        masterInput[25] = Regulator.velocityRegulator->iSum;
        masterInput[26] = Regulator.idRegulator->ki;
        masterInput[27] = Regulator.idRegulator->kp;
        masterInput[28] = Regulator.idRegulator->saturationOutput;
        masterInput[29] = Regulator.idRegulator->iSum;
        masterInput[30] = Regulator.iqRegulator->ki;
        masterInput[31] = Regulator.iqRegulator->kp;
        masterInput[32] = Regulator.iqRegulator->iSum;
        masterInput[33] = Regulator.fluxRegulator->wantedValue;
        masterInput[34] = Regulator.velocityRegulator->wantedValue;
        masterInput[35] = Regulator.idRegulator->wantedValue;
        masterInput[36] = Regulator.iqRegulator->wantedValue;
        masterInput[37] = CurVelModel.modelCVVariables->psi2alpha;
        masterInput[38] = CurVelModel.modelCVVariables->psi2beta;

        masterInput[0] = globalSimulationTime;
        masterInput[4] = inputI1;
        masterInput[5] = inputI2;
        masterInput[6] = inputI3;
        masterInput[7] = MotorModel.modelVariables->motorMechanicalAngularVelocity;
        masterInput[14] = svmCore.triangleWaveSettings->calculationTime;
        masterInput[20] = Regulator.fluxRegulator->iSum;
        masterInput[25] = Regulator.velocityRegulator->iSum;
        masterInput[28] = Regulator.idRegulator->saturationOutput;
        masterInput[29] = Regulator.idRegulator->iSum;
        masterInput[32] = Regulator.iqRegulator->iSum;
        masterInput[37] = CurVelModel.modelCVVariables->psi2alpha;
        masterInput[38] = CurVelModel.modelCVVariables->psi2beta;

        for (int i = 0; i < 10; i++)
        {

            std::cout << "\n\n"
                      << i << " round\n/************************************/\n";

            OCL_CHECK(err, err = q.enqueueMigrateMemObjects({buffer_masterInput}, 0 /* 0 means from host*/));

            OCL_CHECK(err, err = q.enqueueTask(krnl_calculateCurVelModel));

            OCL_CHECK(err, q.enqueueMigrateMemObjects({buffer_masterOutput}, CL_MIGRATE_MEM_OBJECT_HOST));

            OCL_CHECK(err, q.finish());

            std::cout << "sw1: " << masterOutput[0] << "\n";
            std::cout << "sw2: " << masterOutput[1] << "\n";
            std::cout << "sw3: " << masterOutput[2] << "\n";
            std::cout << "sw4: " << masterOutput[3] << "\n";
            std::cout << "sw5: " << masterOutput[4] << "\n";
            std::cout << "sw6: " << masterOutput[5] << "\n";
            std::cout << "triangleWaveSettings.calculationTime: " << masterOutput[6] << "\n";
            std::cout << "fluxRegulator.iSum: " << masterOutput[7] << "\n";
            std::cout << "velocityRegulator.iSum: " << masterOutput[8] << "\n";
            std::cout << "idRegulator.saturationOutput: " << masterOutput[9] << "\n";
            std::cout << "idRegulator.iSum: " << masterOutput[10] << "\n";
            std::cout << "iqRegulator.iSum: " << masterOutput[11] << "\n";
            std::cout << "psi2alpha: " << masterOutput[12] << "\n";
            std::cout << "psi2beta: " << masterOutput[13] << "\n";
            std::cout << "psi2amplitude: " << masterOutput[14] << "\n";
            std::cout << "idRegulator.measuredValue: " << masterOutput[15] << "\n";
            std::cout << "idRegulator.wantedValue: " << masterOutput[16] << "\n";
            std::cout << "transformationAngle: " << masterOutput[17] << "\n";

            masterInput[0] = globalSimulationTime;
            masterInput[1] = globalCalculationStep;
            masterInput[2] = minMaxCommonModeVoltageConstant;
            masterInput[3] = globalCalculationStep / 2;
            masterInput[4] = inputI1;
            masterInput[5] = inputI2;
            masterInput[6] = inputI3;
            masterInput[7] = MotorModel.modelVariables->motorMechanicalAngularVelocity;
            masterInput[8] = CurVelModel.modelCVCoeff->R2MLmDL2;
            masterInput[9] = CurVelModel.modelCVCoeff->R2DL2;
            masterInput[10] = CurVelModel.modelCVCoeff->nOfPolePairs;
            masterInput[11] = svmCore.triangleWaveSettings->waveAmplitude;
            masterInput[12] = svmCore.triangleWaveSettings->calculationStep;
            masterInput[13] = svmCore.triangleWaveSettings->wavePeriod;
            masterInput[14] = svmCore.triangleWaveSettings->calculationTime;
            masterInput[15] = Udcmax;
            masterInput[16] = Regulator.fluxRegulator->ki;
            masterInput[17] = Regulator.fluxRegulator->kp;
            masterInput[18] = Regulator.fluxRegulator->saturationOutputMax;
            masterInput[19] = Regulator.fluxRegulator->saturationOutputMin;
            masterInput[20] = Regulator.fluxRegulator->iSum;
            masterInput[21] = Regulator.velocityRegulator->ki;
            masterInput[22] = Regulator.velocityRegulator->kp;
            masterInput[23] = Regulator.velocityRegulator->saturationOutputMax;
            masterInput[24] = Regulator.velocityRegulator->saturationOutputMin;
            masterInput[25] = Regulator.velocityRegulator->iSum;
            masterInput[26] = Regulator.idRegulator->ki;
            masterInput[27] = Regulator.idRegulator->kp;
            masterInput[28] = Regulator.idRegulator->saturationOutput;
            masterInput[29] = Regulator.idRegulator->iSum;
            masterInput[30] = Regulator.iqRegulator->ki;
            masterInput[31] = Regulator.iqRegulator->kp;
            masterInput[32] = Regulator.iqRegulator->iSum;
            masterInput[33] = Regulator.fluxRegulator->wantedValue;
            masterInput[34] = Regulator.velocityRegulator->wantedValue;
            masterInput[35] = Regulator.idRegulator->wantedValue;
            masterInput[36] = Regulator.iqRegulator->wantedValue;
            masterInput[37] = CurVelModel.modelCVVariables->psi2alpha;
            masterInput[38] = CurVelModel.modelCVVariables->psi2beta;

            masterInput[0] = globalSimulationTime;
            masterInput[4] = inputI1;
            masterInput[5] = inputI2;
            masterInput[6] = inputI3;
            masterInput[7] = MotorModel.modelVariables->motorMechanicalAngularVelocity;
            masterInput[14] = svmCore.triangleWaveSettings->calculationTime;
            masterInput[20] = Regulator.fluxRegulator->iSum;
            masterInput[25] = Regulator.velocityRegulator->iSum;
            masterInput[28] = Regulator.idRegulator->saturationOutput;
            masterInput[29] = Regulator.idRegulator->iSum;
            masterInput[32] = Regulator.iqRegulator->iSum;
            masterInput[37] = CurVelModel.modelCVVariables->psi2alpha;
            masterInput[38] = CurVelModel.modelCVVariables->psi2beta;
        }
    }

    std::cout << "the end of the most useful program is here\n";

    // timer thread testing
    if (modeSelection == 3)
    {

        std::thread backgroundThread(&threadLoop); // initiate a new background Thread

        while (true) // main while loop as in low lvl embedded proc
        {
            std::cout << "Main thread is running!\n";                    // just something to see
            std::this_thread::sleep_for(std::chrono::milliseconds(500)); // simulating some tasks in the background

            // there is new data present from background Thread
            if (isDataFromBackgroundThreadReady)
            {

                // lock the shared resource so any other thread cannot acces it - does not have to be used there, because the isDataFromBackgroundThreadReady prevents thread from entering data inserting
                gLock.lock();
                std::cout << "Background thread output is: " << threadLoopOutput << "\n";
                gLock.unlock();
                isDataFromBackgroundThreadReady = false;
            }
        }

        backgroundThread.join();
    }

    if (modeSelection == 4)
    {
        mainSPIfunction();
    }

    /*-----------------------------------------------------------*/
    /*-------------------- MEMORY FREEING ---------------------*/
    free(MotorModel.odeCalculationSettings);
    free(MotorModel.stateSpaceCoeff);
    free(MotorModel.motorParameters);
    free(MotorModel.modelVariables);
    free(CurVelModel.motorParameters);
    free(CurVelModel.modelCVCoeff);
    free(CurVelModel.modelCVVariables);
    free(CurVelModel.odeCVCalculationSettings);
    free(svmCore.invertorSwitch);
    free(svmCore.triangleWaveSettings);
    free(Regulator.fluxRegulator);
    free(Regulator.velocityRegulator);
    free(Regulator.iqRegulator);
    free(Regulator.idRegulator);
    free(Invertor.reconstructedInvertorOutputVoltage);
    free(svmCore.coreInternalVariables);

    OCL_CHECK(err, err = q.enqueueUnmapMemObject(buffer_masterInput, masterInput));
    OCL_CHECK(err, err = q.enqueueUnmapMemObject(buffer_masterOutput, masterOutput));
    OCL_CHECK(err, err = q.enqueueUnmapMemObject(buffer_masterMotorInput, masterMotorInput));
    OCL_CHECK(err, err = q.enqueueUnmapMemObject(buffer_masterMotorOutput, masterMotorInput));
    OCL_CHECK(err, q.finish());

    /*-----------------------------------------------------------*/
}

// musí být chyba v algoritmu velocity regulatoru