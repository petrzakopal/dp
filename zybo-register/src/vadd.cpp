

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

/*-----------------------*/
/* FOR INTERRUPT TESTING */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <poll.h>
// #include <cstdlib>
#include <signal.h>

static const int DATA_SIZE = 4096;

static const std::string error_message =
    "Error: Result mismatch:\n"
    "i = %d CPU result = %d Device result = %d\n";


// Define the function to be called when ctrl-c (SIGINT) is sent to process
// for setting regulators to 0 and voltages to 0 when user interrupts the program
void signal_callback_handler(int signum) {
   std::cout << "\nCaught signal " << signum << "\n";
   std::cout << "Interrupted by user.\n";
   // Terminate program
   exit(signum);
}


int main(int argc, char* argv[]) {

 // Register signal and signal handler
   signal(SIGINT, signal_callback_handler);

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


    int valuefd, exportfd, directionfd;
  
    printf("GPIO test running...\n");
  
    // The GPIO has to be exported to be able to see it
    // in sysfs
  
    exportfd = open("/sys/class/gpio/export", O_WRONLY);
    if (exportfd < 0)
    {
        printf("Cannot open GPIO to export it\n");
        exit(1);
    }
  
    write(exportfd, "980", 4);
    close(exportfd);
  
    printf("GPIO exported successfully\n");
  
    // Update the direction of the GPIO to be an output
  
    directionfd = open("/sys/class/gpio/gpio980/direction", O_RDWR);
    if (directionfd < 0)
    {
        printf("Cannot open GPIO direction it\n");
        exit(1);
    }
  
    write(directionfd, "out", 4);
    close(directionfd);
  
    printf("GPIO direction set as output successfully\n");
  
    // Get the GPIO value ready to be toggled
  
    valuefd = open("/sys/class/gpio/gpio980/value", O_RDWR | O_NONBLOCK);
    if (valuefd < 0)
    {
        printf("Cannot open GPIO value\n");
        exit(1);
    }

  
    write(valuefd,"1", 2);
    sleep(3);
    write(valuefd,"0", 2);
    close(valuefd);

    directionfd = open("/sys/class/gpio/gpio980/direction", O_RDWR);
    if (directionfd < 0)
    {
        printf("Cannot open GPIO direction it\n");
        exit(1);
    }

    write(directionfd, "in", 4);
    close(directionfd);

    exportfd = open("/sys/class/gpio/unexport", O_WRONLY);
    if (exportfd < 0)
    {
        printf("Cannot open GPIO to unexport it\n");
        exit(1);
    }
  
    write(exportfd, "980", 4);
    close(exportfd);
  
    printf("GPIO unexported successfully\n");
    printf("--------------------------------\npart of mmem\n");
    std::cout << "sleeping for 5\n";
    sleep(5);

     int fd = open("/dev/mem", O_RDWR | O_SYNC);
    if (fd < 0) {
        return 1;
    }
     void* gpio_addr = mmap(NULL, 0x1000, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0x41250000);
    volatile uint32_t* gpio_data = (volatile uint32_t*)((char*)gpio_addr);

    uint32_t data_val = *gpio_data; // read from data register

    std::cout << "data: " << data_val << "\n";
    *gpio_data = 0x00000001;
    std::cout << "data: " << data_val << "\n";
    std::cout << "sleeping for 3\n";
    sleep(3);
    *gpio_data = 0x00000000;
    std::cout << "data: " << data_val << "\n";

    std::cout << "kernel part\n";

    OCL_CHECK(err, cl::Buffer buffer_register(context, CL_MEM_USE_HOST_PTR, sizeof(uint32_t),&gpio_addr,&err));

     int narg = 0;
    OCL_CHECK(err, err = krnl_vector_add.setArg(narg++, buffer_register));
    OCL_CHECK(err, err = q.enqueueMigrateMemObjects({buffer_register}, 0 /* 0 means from host*/));
    OCL_CHECK(err, err = q.enqueueTask(krnl_vector_add));
    
    OCL_CHECK(err, q.enqueueMigrateMemObjects({buffer_register}, CL_MIGRATE_MEM_OBJECT_HOST));
    OCL_CHECK(err, q.finish());
     std::cout << "data: " << data_val << "\n";

   

    return (0);
}
