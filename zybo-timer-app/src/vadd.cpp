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

/*------------------------*/
/* MULTITHREADING*/
#include <future> //FUTURES CAN return value

/*----------------------*/
#define IN 0
#define OUT 1

#define TIMER_MAP_SIZE 0x10000
#define TIMER_0_CTR_REG 0x08


static const int DATA_SIZE = 4096;


int main(int argc, char* argv[]) {

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
    cl::Kernel krnl_main;
    // cl::Kernel krnl_GenerateVoltageAlphaBeta;
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

    if (found_device == false) {
        std::cout << "Error: Unable to find Target Device " << std::endl;
        return EXIT_FAILURE;
    }
    /*-------------------------------------------------------------------------------------*/


    /*-------------------------------------------------------------------------------------*/
    /*--------------------- LOADING AND READING XCLBIN FILE TO FPGA ----------------------*/
    // Check for availability of PL program
    std::cout << "INFO: Reading " << xclbinFilename << std::endl;
    FILE* fp;
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
    char* buf = new char[nb]; // Creating buffer/array with the length of the stream => bin_file

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
            OCL_CHECK(err, krnl_main = cl::Kernel(program, "krnl_vadd", &err)); // assign to krnl_compute the new PL kernel, which is found in kernel_main and called kernel_main

            valid_device = true;

           

            break; // break because found a valid device

            /*------------------------------------------------------------------------------------------------------*/
        }
    }
    if (!valid_device) {
        std::cout << "Failed to program any device found, exit!\n";
        exit(EXIT_FAILURE);
    }

    /*-------------------------------- END OF SETTING THE ENVIRONMENT ---------------------------------*/
    /*-||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||-*/



    std::cout << "interrupt testing part\n";
    /*----------------------------------------------------*/

    char *uiod;
    void *ptr;
    int value = 0;
    uiod = "/dev/uio1";
    int fd;
    int irq_on = 1;
    int breaking = 0;
    uint32_t info = 1; /* unmask */
    /* Open the UIO device file */
    // fd = open(uiod, O_RDWR | O_NONBLOCK); // when using read()
    fd = open(uiod, O_RDWR | O_NONBLOCK); // when using poll()

    
        struct pollfd fds = {
            .fd = fd,
            .events = POLLIN | POLLOUT,
        };


    if (fd < 1) {
        perror("open\n");
        printf("Invalid UIO device file:%s.\n", uiod);
        return -1;
    }

     /* mmap the UIO device */
    ptr = mmap(NULL, TIMER_MAP_SIZE, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
    value = *((unsigned *) (ptr + TIMER_0_CTR_REG));
    printf("timer counter register: %08x\n", value);
    std::cout << "sizeof(ptr)= " << sizeof(ptr) << "\n";
    std::cout << "sizeof(fd)= " << sizeof(fd) << "\n";
    
    // unsigned data = 0xC0;
   
 //first type kernel
    OCL_CHECK(err, cl::Buffer buffer_memory(context, CL_MEM_USE_HOST_PTR, sizeof((unsigned *) ptr),ptr,&err));
   

    int narg = 0;
    OCL_CHECK(err, err = krnl_main.setArg(narg++, buffer_memory));
    

    OCL_CHECK(err, err = q.enqueueMigrateMemObjects({buffer_memory}, 0 /* 0 means from host*/));
    
    OCL_CHECK(err, err = q.enqueueTask(krnl_main));
    OCL_CHECK(err, q.enqueueMigrateMemObjects({buffer_memory}, CL_MIGRATE_MEM_OBJECT_HOST));

    // OCL_CHECK(err, q.finish());
    printf("Kernel finished, check value.\n");
    value = *((unsigned *) (ptr));
    printf("timer counter register: %08x\n", value);
    std::cout << "addres prt: " << ptr << "\n";
    /* Enable All Interrupts */
    printf("Enable Interrupt and Start for the first time!\n");

    printf("Second run of a kernel!\n");
   
    

    for(int i = 0;i<500;i++)
    {
    OCL_CHECK(err, err = q.enqueueMigrateMemObjects({buffer_memory}, 0 /* 0 means from host*/));
    OCL_CHECK(err, err = q.enqueueTask(krnl_main));
    
    OCL_CHECK(err, q.enqueueMigrateMemObjects({buffer_memory}, CL_MIGRATE_MEM_OBJECT_HOST));
    
    }
    OCL_CHECK(err, q.finish());

     
     *((unsigned *)(ptr)) = 0XC0;
  

    printf("before reading\n");
    /* Wait for interrupt */
      
        printf("right after reading\n");
        while(1)
        {


        // when used non blocking read O_NONBLOCK
        // ssize_t nb = read(fd, &info, sizeof(info));
        // value = *((unsigned *) (ptr + TIMER_0_CTR_REG));
        // printf("timer counter register: %08x\n", value);
        // if (nb == (ssize_t)sizeof(info))
        // {
        //     /* Do something in response to the interrupt. */
        //     printf("Interrupt #%u!\n", info);
        //     break;
        // } 

        
         int ret = poll(&fds, 1, -1);

         value = *((unsigned *) (ptr + TIMER_0_CTR_REG));
            printf("timer counter register: %08x\n", value);
        if (ret >= 1) {
            ssize_t nb = read(fd, &info, sizeof(info));
            

            if (nb == (ssize_t)sizeof(info)) {
                /* Do something in response to the interrupt. */
               printf("Interrupt #%u!\n", info);
            break;
            }


        }

        }
       
       
    *((unsigned *)(ptr)) = 0X1C0;
    write(fd, &irq_on, sizeof(irq_on));
    *((unsigned *)(ptr + 0x4)) = 0XAFFFFFFF;
    *((unsigned *)(ptr)) = 0XE0;
  
    *((unsigned *)(ptr + 0x8)) = 0X0;
    munmap(ptr, TIMER_MAP_SIZE);
    close(fd);
    return (0);
}
