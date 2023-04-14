

#define OCL_CHECK(error, call)                                                                   \
    call;                                                                                        \
    if (error != CL_SUCCESS) {                                                                   \
        printf("%s:%d Error calling " #call ", error code is: %d\n", __FILE__, __LINE__, error); \
        exit(EXIT_FAILURE);                                                                      \
    }

#include "vadd.h"
#include <fstream>
#include <iostream>
#include <stdlib.h>
#include <unistd.h>

#include <sys/mman.h>
#include <fcntl.h>
#include <poll.h>
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
void sendSPIdata(void *ptr, off_t slaveSelect, off_t data)
{
    *((unsigned *)(ptr + SPI_SPICR)) = 0x1E6;
    *((unsigned *)(ptr + SPI_SPISSR)) = slaveSelect; // 0x01 - slave select 1 to high, when slave no. 2 - it would be 0x02
    *((unsigned *)(ptr + SPI_DTR)) = data;
    *((unsigned *)(ptr + SPI_SPISSR)) = 0x0;
    *((unsigned *)(ptr + SPI_SPICR)) = 0x86;
    std::this_thread::sleep_for(std::chrono::nanoseconds(1)); // here should be waiting for an interrrupt
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
}

/*
* @name     initializeLEDmatrix
* @brief    Initialize LED matrix with MAX7219.
* @todo     Implement interrupt and delete sleep_for...
*/
void initializeLEDmatrix(void *ptr, off_t slaveSelect)
{
    off_t initilSeq[5] = {0x0900, 0x0a01, 0x0b07, 0x0c01, 0x0f00};

    for(int i = 0;i<5;i++)
    {
        *((unsigned *)(ptr + SPI_SPICR)) = 0x1E6;
        *((unsigned *)(ptr + SPI_SPISSR)) = slaveSelect; // 0x01 - slave select 1 to high, when slave no. 2 - it would be 0x02
        *((unsigned *)(ptr + SPI_DTR)) = initilSeq[i];
        *((unsigned *)(ptr + SPI_SPISSR)) = 0x0;
        *((unsigned *)(ptr + SPI_SPICR)) = 0x86;
        std::this_thread::sleep_for(std::chrono::nanoseconds(1)); // here should be waiting for an interrrupt
        *((unsigned *)(ptr + SPI_SPISSR)) = slaveSelect;
    }
   
}

/*
* @name     clearLEDmatrix
* @brief    Clear LED matrix (set all LEDs to 0).
* @todo     Implement interrupt and delete sleep_for...
*/
void clearLEDmatrix(void *ptr, off_t slaveSelect)
{

    off_t clearSeq[8] = {0x100, 0x200, 0x300, 0x400, 0x500, 0x600, 0x700, 0x800};
    for(int i = 0;i<8;i++)
    {
        *((unsigned *)(ptr + SPI_SPICR)) = 0x1E6;
        *((unsigned *)(ptr + SPI_SPISSR)) = slaveSelect; // 0x01 - slave select 1 to high, when slave no. 2 - it would be 0x02
        *((unsigned *)(ptr + SPI_DTR)) = clearSeq[i];
        *((unsigned *)(ptr + SPI_SPISSR)) = 0x0;
        *((unsigned *)(ptr + SPI_SPICR)) = 0x86;
        std::this_thread::sleep_for(std::chrono::nanoseconds(1)); // here should be waiting for an interrrupt
        *((unsigned *)(ptr + SPI_SPISSR)) = slaveSelect;
    }
}

/*
* @name     printLetterOnLEDMatrix
* @brief    Print desired letter on a LED matrix.
* @todo     Implement interrupt and delete sleep_for...
*/
void printLetterOnLEDMatrix(void *ptr, off_t slaveSelect, off_t *pismeno)
{
        for(int i = 0;i<8;i++)
    {
        *((unsigned *)(ptr + SPI_SPICR)) = 0x1E6;
        *((unsigned *)(ptr + SPI_SPISSR)) = slaveSelect; // 0x01 - slave select 1 to high, when slave no. 2 - it would be 0x02
        *((unsigned *)(ptr + SPI_DTR)) = pismeno[i];
        *((unsigned *)(ptr + SPI_SPISSR)) = 0x0;
        *((unsigned *)(ptr + SPI_SPICR)) = 0x86;
        std::this_thread::sleep_for(std::chrono::nanoseconds(1)); // here should be waiting for an interrrupt
        *((unsigned *)(ptr + SPI_SPISSR)) = slaveSelect;
    }
}

/*
* @name     Thread for SPI communication.
* @brief    SPI communication demonstration. Without Interrupts of knowing if the transfer has finished. Need to work on that.
* @todo     Handle and test interrupts that transfer has finished.
*/
void threadLoopSPI()
{
    void *ptr; // pointer to a virtual memory filled by mmap
    int fd; // file descriptor
    char *uiod; // name what device to open
    uiod = "/dev/mem";
    off_t memoryBase = 0x0080040000; // base memory (in mmap() it is an offset)
    off_t dataToSend; // data to send to SPI manually
    off_t slaveSelect; // slave selection (chip select CS)
    int initializeMatrix; // variable for if statement to initializeMatrix with initialization sequence
    int clearMatrix; // variable for if statement to clearMatrix (set all LEDs to 0)
    int modeSelection; // mode selction to write data manually or to write a letter

    // letter variable
    off_t pismeno[8] = { 0b000110000001,
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
    ptr = mmap(NULL, MAP_SIZE, PROT_READ|PROT_WRITE, MAP_SHARED, fd, memoryBase);


    // Slave selection
    std::cout<<"Input SS (CS):\n";
    scanf("%lx", &slaveSelect);
    printf("Selected SPI slave: 0x%lx\n", slaveSelect);


    // Mode selection
    std::cout<<"Select mode: [0] - dataTransfer, [1] - printLetter\n";
    scanf("%i", &modeSelection);

    
    // SPI communication initialization
    std::cout<<"Initializing SPI...\n";
    initializeSPI(ptr, 0x1);
    std::cout<<"SPI successfully initialized...\n";


    // LED matrix initialization
    std::cout<<"Initialize matrix? [1 = yes / 0 = no]\n";
    scanf("%i", &initializeMatrix);
    (initializeMatrix == 1) ? initializeLEDmatrix(ptr, slaveSelect) : (void)0;
    printf("initializeMatrix is set to: %i\n", initializeMatrix);


    // Clearing matrix
    std::cout<<"Clear matrix? [1 = yes / 0 = no]\n";
    scanf("%i", &clearMatrix);
    (clearMatrix == 1) ? clearLEDmatrix(ptr, slaveSelect) : (void)0 ;
    printf("clearMatrix is set to: %i\n", clearMatrix);

    
    // Main semaphore
    switch(modeSelection)
    {
        case 0:
        {
            std::cout<<"Input data to transfer:\n";
            scanf("%lx", &dataToSend);
            printf("Value to send via SPI: 0x%lx\n", dataToSend);
            sendSPIdata(ptr, slaveSelect, dataToSend);
            break;
        }

        case 1:
        {
            printLetterOnLEDMatrix(ptr, slaveSelect, pismeno);
            break;
        }
    }

    // Unmaping memory
    munmap(ptr, MAP_SIZE);

    // Closing fd
    close(fd);
}




static const int DATA_SIZE = 4096;

static const std::string error_message =
    "Error: Result mismatch:\n"
    "i = %d CPU result = %d Device result = %d\n";

int main(int argc, char* argv[]) {
    // TARGET_DEVICE macro needs to be passed from gcc command line
    if (argc != 2) {
        std::cout << "Usage: " << argv[0] << " <xclbin>" << std::endl;
        return EXIT_FAILURE;
    }

    std::string xclbinFilename = argv[1];

    // Compute the size of array in bytes
    size_t size_in_bytes = DATA_SIZE * sizeof(int);

    // Creates a vector of DATA_SIZE elements with an initial value of 10 and 32
    // using customized allocator for getting buffer alignment to 4k boundary

    std::vector<cl::Device> devices;
    cl_int err;
    cl::Context context;
    cl::CommandQueue q;
    cl::Kernel krnl_vector_add;
    cl::Program program;
    std::vector<cl::Platform> platforms;
    bool found_device = false;

    // traversing all Platforms To find Xilinx Platform and targeted
    // Device in Xilinx Platform
    cl::Platform::get(&platforms);
    for (size_t i = 0; (i < platforms.size()) & (found_device == false); i++) {
        cl::Platform platform = platforms[i];
        std::string platformName = platform.getInfo<CL_PLATFORM_NAME>();
        if (platformName == "Xilinx") {
            devices.clear();
            platform.getDevices(CL_DEVICE_TYPE_ACCELERATOR, &devices);
            if (devices.size()) {
                found_device = true;
                break;
            }
        }
    }
    if (found_device == false) {
        std::cout << "Error: Unable to find Target Device " << std::endl;
        return EXIT_FAILURE;
    }

    std::cout << "INFO: Reading " << xclbinFilename << std::endl;
    FILE* fp;
    if ((fp = fopen(xclbinFilename.c_str(), "r")) == nullptr) {
        printf("ERROR: %s xclbin not available please build\n", xclbinFilename.c_str());
        exit(EXIT_FAILURE);
    }
    // Load xclbin
    std::cout << "Loading: '" << xclbinFilename << "'\n";
    std::ifstream bin_file(xclbinFilename, std::ifstream::binary);
    bin_file.seekg(0, bin_file.end);
    unsigned nb = bin_file.tellg();
    bin_file.seekg(0, bin_file.beg);
    char* buf = new char[nb];
    bin_file.read(buf, nb);

    // Creating Program from Binary File
    cl::Program::Binaries bins;
    bins.push_back({buf, nb});
    bool valid_device = false;
    for (unsigned int i = 0; i < devices.size(); i++) {
        auto device = devices[i];
        // Creating Context and Command Queue for selected Device
        OCL_CHECK(err, context = cl::Context(device, nullptr, nullptr, nullptr, &err));
        OCL_CHECK(err, q = cl::CommandQueue(context, device, CL_QUEUE_PROFILING_ENABLE, &err));
        std::cout << "Trying to program device[" << i << "]: " << device.getInfo<CL_DEVICE_NAME>() << std::endl;
        cl::Program program(context, {device}, bins, nullptr, &err);
        if (err != CL_SUCCESS) {
            std::cout << "Failed to program device[" << i << "] with xclbin file!\n";
        } else {
            std::cout << "Device[" << i << "]: program successful!\n";
            OCL_CHECK(err, krnl_vector_add = cl::Kernel(program, "krnl_vadd", &err));
            valid_device = true;
            break; // we break because we found a valid device
        }
    }
    if (!valid_device) {
        std::cout << "Failed to program any device found, exit!\n";
        exit(EXIT_FAILURE);
    }

    // These commands will allocate memory on the Device. The cl::Buffer objects can
    // be used to reference the memory locations on the device.
    OCL_CHECK(err, cl::Buffer buffer_a(context, CL_MEM_READ_ONLY, size_in_bytes, NULL, &err));
    OCL_CHECK(err, cl::Buffer buffer_b(context, CL_MEM_READ_ONLY, size_in_bytes, NULL, &err));
    OCL_CHECK(err, cl::Buffer buffer_result(context, CL_MEM_WRITE_ONLY, size_in_bytes, NULL, &err));

    // set the kernel Arguments
    int narg = 0;
    OCL_CHECK(err, err = krnl_vector_add.setArg(narg++, buffer_a));
    OCL_CHECK(err, err = krnl_vector_add.setArg(narg++, buffer_b));
    OCL_CHECK(err, err = krnl_vector_add.setArg(narg++, buffer_result));
    OCL_CHECK(err, err = krnl_vector_add.setArg(narg++, DATA_SIZE));

    // We then need to map our OpenCL buffers to get the pointers
    int* ptr_a;
    int* ptr_b;
    int* ptr_result;
    OCL_CHECK(err,
              ptr_a = (int*)q.enqueueMapBuffer(buffer_a, CL_TRUE, CL_MAP_WRITE, 0, size_in_bytes, NULL, NULL, &err));
    OCL_CHECK(err,
              ptr_b = (int*)q.enqueueMapBuffer(buffer_b, CL_TRUE, CL_MAP_WRITE, 0, size_in_bytes, NULL, NULL, &err));
    OCL_CHECK(err, ptr_result = (int*)q.enqueueMapBuffer(buffer_result, CL_TRUE, CL_MAP_READ, 0, size_in_bytes, NULL,
                                                         NULL, &err));

    // Initialize the vectors used in the test
    for (int i = 0; i < DATA_SIZE; i++) {
        ptr_a[i] = rand() % DATA_SIZE;
        ptr_b[i] = rand() % DATA_SIZE;
    }

    // Data will be migrated to kernel space
    OCL_CHECK(err, err = q.enqueueMigrateMemObjects({buffer_a, buffer_b}, 0 /* 0 means from host*/));

    // Launch the Kernel
    OCL_CHECK(err, err = q.enqueueTask(krnl_vector_add));

    // The result of the previous kernel execution will need to be retrieved in
    // order to view the results. This call will transfer the data from FPGA to
    // source_results vector
    OCL_CHECK(err, q.enqueueMigrateMemObjects({buffer_result}, CL_MIGRATE_MEM_OBJECT_HOST));

    OCL_CHECK(err, q.finish());

    // Verify the result
    int match = 0;
    for (int i = 0; i < DATA_SIZE; i++) {
        int host_result = ptr_a[i] + ptr_b[i];
        if (ptr_result[i] != host_result) {
            printf(error_message.c_str(), i, host_result, ptr_result[i]);
            match = 1;
            break;
        }
    }

    OCL_CHECK(err, err = q.enqueueUnmapMemObject(buffer_a, ptr_a));
    OCL_CHECK(err, err = q.enqueueUnmapMemObject(buffer_b, ptr_b));
    OCL_CHECK(err, err = q.enqueueUnmapMemObject(buffer_result, ptr_result));
    OCL_CHECK(err, err = q.finish());

    std::cout << "TEST " << (match ? "FAILED" : "PASSED") << std::endl;



    std::cout << "-------------------\n";
    std::cout << "testing spi\n";
    threadLoopSPI();
    return (0);
}
