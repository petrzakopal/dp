
/*******************************************************************************
Author: Petr Zakopal, FEE CTU
Purpose: Host program
Comment: Thesis Export Version of Legacy APP PL Processing of I-n model loading from file
*******************************************************************************/

/********SOME PARTS OF THIS APP ARE CREATED USING EXAMPLES FROM XILINX ********/
/*******************************************************************************
Vendor: Xilinx
Associated Filename: vadd.cpp
Purpose: VITIS vector addition

*******************************************************************************
Copyright (C) 2019 XILINX, Inc.

This file contains confidential and proprietary information of Xilinx, Inc. and
is protected under U.S. and international copyright and other intellectual
property laws.

DISCLAIMER
This disclaimer is not a license and does not grant any rights to the materials
distributed herewith. Except as otherwise provided in a valid license issued to
you by Xilinx, and to the maximum extent permitted by applicable law:
(1) THESE MATERIALS ARE MADE AVAILABLE "AS IS" AND WITH ALL FAULTS, AND XILINX
HEREBY DISCLAIMS ALL WARRANTIES AND CONDITIONS, EXPRESS, IMPLIED, OR STATUTORY,
INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY, NON-INFRINGEMENT, OR
FITNESS FOR ANY PARTICULAR PURPOSE; and (2) Xilinx shall not be liable (whether
in contract or tort, including negligence, or under any other theory of
liability) for any loss or damage of any kind or nature related to, arising under
or in connection with these materials, including for any direct, or any indirect,
special, incidental, or consequential loss or damage (including loss of data,
profits, goodwill, or any type of loss or damage suffered as a result of any
action brought by a third party) even if such damage or loss was reasonably
foreseeable or Xilinx had been advised of the possibility of the same.

CRITICAL APPLICATIONS
Xilinx products are not designed or intended to be fail-safe, or for use in any
application requiring fail-safe performance, such as life-support or safety
devices or systems, Class III medical devices, nuclear facilities, applications
related to the deployment of airbags, or any other applications that could lead
to death, personal injury, or severe property or environmental damage
(individually and collectively, "Critical Applications"). Customer assumes the
sole risk and liability of any use of Xilinx products in Critical Applications,
subject only to applicable laws and regulations governing limitations on product
liability.

THIS COPYRIGHT NOTICE AND DISCLAIMER MUST BE RETAINED AS PART OF THIS FILE AT
ALL TIMES.

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

    // stoping invertor of global variables
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
            std::this_thread::sleep_for(std::chrono::nanoseconds(1)); // could not resolve other way now, because reading and writing to register takes more time that one tick probably
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
    *((unsigned *)(timer1ptr + 0x4)) = 0xFFFFF82F; // 10 us
    // *((unsigned *)(timer1ptr + 0x4)) = 0xFA0A1EFF; // 0.5 s
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
                    threadSPI(ptrSPI, fdSPI, slaveSelect);
                    // if timer has finished (interrupt has risen)
                    // copy data / insert data to shared variable
                    if (isDataFromBackgroundThreadReady == false)
                    {

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
        *((unsigned *)(timer1ptr + 0x4)) = 0xFFFFF82F; // 10 us
        // *((unsigned *)(timer1ptr + 0x4)) = 0xFA0A1EFF; // 0.5 s
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
    cl::Kernel krnl_CurVelLoadLegacy;
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
            OCL_CHECK(err, krnl_CurVelLoadLegacy = cl::Kernel(program, "krnl_CurVelLoadLegacy", &err));
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
    printf("Select mode:\n0 - preloaded data (enabled)\n1 - keyboard input data (disabled)\n2 - CPU/FPGA model (disabled)\n3 - timer thread test + SPI\n4 - SPI send data\n");
    scanf("%i", &modeSelection);
    printf("You have selected: %i\n\r", modeSelection);

    if (modeSelection == 0)
    {

        float *odeCVCalculationSettingsArray;

        posix_memalign((void **)&odeCVCalculationSettingsArray, 4096, 5 * sizeof(float));

        odeCVCalculationSettingsArray[0] = 0;        // initialCalculationTime
        odeCVCalculationSettingsArray[1] = 1;        // finalCalculationTime
        odeCVCalculationSettingsArray[2] = 0.000001; // calculationStep
        odeCVCalculationSettingsArray[3] = 0;        // calculationTime
        odeCVCalculationSettingsArray[4] = (int)ceil(((odeCVCalculationSettingsArray[1] - odeCVCalculationSettingsArray[0]) / odeCVCalculationSettingsArray[2]));

        float *motorParametersArray;
        posix_memalign((void **)&motorParametersArray, 4096, 4 * sizeof(float));
        motorParametersArray[0] = 0.225f;   // R2
        motorParametersArray[1] = 0.0825f;  // Lm
        motorParametersArray[2] = 0.08477f; // L2
        motorParametersArray[3] = 2;        // nOfPolePairs

        float *modelCVCoeffArray;
        posix_memalign((void **)&modelCVCoeffArray, 4096, 3 * sizeof(float));
        modelCVCoeffArray[0] = motorParametersArray[0] / motorParametersArray[2];                             // R2DL2
        modelCVCoeffArray[1] = (motorParametersArray[0] * motorParametersArray[1]) / motorParametersArray[2]; // R2MLmDL2
        modelCVCoeffArray[2] = 2;
        // float *odeCVCalculationSettingsArray;

        float *psi2AmplitudeOut;
        float *transformAngleOut;
        float *inputTime;
        float *inputI1;
        float *inputI2;
        float *inputI3;
        float *inputMotorMechanicalAngularVelocity;
        float *psi2alpha;
        float *psi2beta;

        posix_memalign((void **)&inputTime, 4096, odeCVCalculationSettingsArray[4] * sizeof(float));
        posix_memalign((void **)&inputI1, 4096, odeCVCalculationSettingsArray[4] * sizeof(float));
        posix_memalign((void **)&inputI2, 4096, odeCVCalculationSettingsArray[4] * sizeof(float));
        posix_memalign((void **)&inputI3, 4096, odeCVCalculationSettingsArray[4] * sizeof(float));
        posix_memalign((void **)&inputMotorMechanicalAngularVelocity, 4096, odeCVCalculationSettingsArray[4] * sizeof(float));

        posix_memalign((void **)&psi2alpha, 4096, odeCVCalculationSettingsArray[4] * sizeof(float));
        posix_memalign((void **)&psi2beta, 4096, odeCVCalculationSettingsArray[4] * sizeof(float));
        posix_memalign((void **)&psi2AmplitudeOut, 4096, odeCVCalculationSettingsArray[4] * sizeof(float));
        posix_memalign((void **)&transformAngleOut, 4096, odeCVCalculationSettingsArray[4] * sizeof(float));

        psi2alpha[0] = 0;
        psi2beta[0] = 0;

        /*------------------------------ LOADING DATA STREAM ------------------------------*/
        printf("------------------------------------\n\r");
        printf("Preloaded data mode\n");
        printf("------------------------------------\n\r");
        std::ifstream inputData("outputData.csv");

        int switchReadingIndex = 0;
        int timeIndex = 0;
        int i1Aindex = 0;
        int i1Bindex = 0;
        int i1Cindex = 0;
        int motorMechanicalAngularVelocityIndex = 0;

        for (std::string line; std::getline(inputData, line, ',');)
        {

            // data structure is important, defined in a lightweight export file like {time,i1,i2,i3,motorMechanicalAngularVelocity}
            switch (switchReadingIndex)
            {
            case 0:

                inputTime[timeIndex] = std::stof(line, NULL);
                timeIndex++;
                switchReadingIndex++;
                break;

            case 1:

                inputI1[i1Aindex] = std::stof(line, NULL);
                i1Aindex++;
                switchReadingIndex++;
                break;

            case 2:

                inputI2[i1Bindex] = std::stof(line, NULL);
                i1Bindex++;
                switchReadingIndex++;
                break;

            case 3:

                inputI3[i1Cindex] = std::stof(line, NULL);
                i1Cindex++;
                switchReadingIndex++;
                break;

            case 4:

                inputMotorMechanicalAngularVelocity[motorMechanicalAngularVelocityIndex] = std::stof(line, NULL);
                motorMechanicalAngularVelocityIndex++;
                switchReadingIndex = 0;
                break;
            }
        }
        inputData.close(); // close that file

        // first type kernel
        OCL_CHECK(err, cl::Buffer buffer_odeCVCalculationSettings(context, CL_MEM_USE_HOST_PTR, 4 * sizeof(float), odeCVCalculationSettingsArray, &err));

        OCL_CHECK(err, cl::Buffer buffer_modelCVCoeff(context, CL_MEM_USE_HOST_PTR, 3 * sizeof(float), modelCVCoeffArray, &err));

        OCL_CHECK(err, cl::Buffer buffer_inputI1(context, CL_MEM_USE_HOST_PTR, odeCVCalculationSettingsArray[4] * sizeof(float), inputI1, &err));

        OCL_CHECK(err, cl::Buffer buffer_inputI2(context, CL_MEM_USE_HOST_PTR, odeCVCalculationSettingsArray[4] * sizeof(float), inputI2, &err));

        OCL_CHECK(err, cl::Buffer buffer_inputI3(context, CL_MEM_USE_HOST_PTR, odeCVCalculationSettingsArray[4] * sizeof(float), inputI3, &err));

        OCL_CHECK(err, cl::Buffer buffer_inputMotorMechanicalAngularVelocity(context, CL_MEM_USE_HOST_PTR, odeCVCalculationSettingsArray[4] * sizeof(float), inputMotorMechanicalAngularVelocity, &err));

        OCL_CHECK(err, cl::Buffer buffer_psi2alpha(context, CL_MEM_USE_HOST_PTR, odeCVCalculationSettingsArray[4] * sizeof(float), psi2alpha, &err));

        OCL_CHECK(err, cl::Buffer buffer_psi2beta(context, CL_MEM_USE_HOST_PTR, odeCVCalculationSettingsArray[4] * sizeof(float), psi2beta, &err));

        int narg = 0;
        OCL_CHECK(err, err = krnl_CurVelLoadLegacy.setArg(narg++, buffer_odeCVCalculationSettings));
        OCL_CHECK(err, err = krnl_CurVelLoadLegacy.setArg(narg++, buffer_modelCVCoeff));
        OCL_CHECK(err, err = krnl_CurVelLoadLegacy.setArg(narg++, buffer_inputI1));
        OCL_CHECK(err, err = krnl_CurVelLoadLegacy.setArg(narg++, buffer_inputI2));
        OCL_CHECK(err, err = krnl_CurVelLoadLegacy.setArg(narg++, buffer_inputI3));
        OCL_CHECK(err, err = krnl_CurVelLoadLegacy.setArg(narg++, buffer_inputMotorMechanicalAngularVelocity));

        OCL_CHECK(err, err = krnl_CurVelLoadLegacy.setArg(narg++, buffer_psi2alpha));
        OCL_CHECK(err, err = krnl_CurVelLoadLegacy.setArg(narg++, buffer_psi2beta));

        // Data will be migrated to kernel space
        OCL_CHECK(err, err = q.enqueueMigrateMemObjects({buffer_odeCVCalculationSettings, buffer_modelCVCoeff, buffer_inputI1, buffer_inputI2, buffer_inputI3, buffer_inputMotorMechanicalAngularVelocity, buffer_psi2alpha, buffer_psi2beta}, 0 /* 0 means from host*/));

        /*------------------------------------------------------------------------------------------------------------*/

        // Launch the Kernel
        OCL_CHECK(err, err = q.enqueueTask(krnl_CurVelLoadLegacy));

        OCL_CHECK(err, q.enqueueMigrateMemObjects({buffer_psi2alpha, buffer_psi2beta}, CL_MIGRATE_MEM_OBJECT_HOST));
        OCL_CHECK(err, q.finish());

        // data output
        std::ofstream modelCVOutputDataFile2;
        modelCVOutputDataFile2.open("outputCurVel2.csv", std::ofstream::out | std::ofstream::trunc);
        modelCVOutputDataFile2 << "time,|psi2|,transformAngle\n";
        float psi2Amplitude;
        float transformAngle;
        float timeCV = odeCVCalculationSettingsArray[0];

        for (int i = 0; i < odeCVCalculationSettingsArray[4]; i++)
        {
            timeCV = timeCV + odeCVCalculationSettingsArray[2];
            psi2Amplitude = sqrtf(psi2alpha[i] * psi2alpha[i] + psi2beta[i] * psi2beta[i]);
            transformAngle = atan2f(psi2beta[i], psi2alpha[i]);
            std::cout << "psi2Amplitude index " << i << " : " << psi2Amplitude << "\n";
            std::cout << "transformAngle[" << i << "]: " << transformAngle << "\n";

            modelCVOutputDataFile2 << timeCV << "," << psi2Amplitude << "," << transformAngle << "\n";
        }

        modelCVOutputDataFile2.close();

        // free the memory
        free(motorParametersArray);
        free(modelCVCoeffArray);
        free(inputTime);
        free(inputI1);
        free(inputI2);
        free(inputI3);
        free(psi2alpha);
        free(psi2beta);
        free(inputMotorMechanicalAngularVelocity);
        free(odeCVCalculationSettingsArray);
    }

    std::cout << "the end of the preloaded data application\n";

    // timer thread testing
    if (modeSelection == 3)
    {

        std::thread backgroundThread(&threadLoop); // initiate a new background Thread

        while (true) // main while loop as in low lvl embedded proc
        {
            std::cout << "Main thread is running!\n";                     // just something to see
            std::this_thread::sleep_for(std::chrono::microseconds(1000)); // simulating some tasks in the background

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

    OCL_CHECK(err, q.finish());
}
