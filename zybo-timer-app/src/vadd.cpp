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

/*----------------------*/
#define IN 0
#define OUT 1

#define TIMER_MAP_SIZE 0x10000
#define TIMER_0_CTR_REG 0x08


static const int DATA_SIZE = 4096;

static const std::string error_message =
    "Error: Result mismatch:\n"
    "i = %d CPU result = %d Device result = %d\n";

int main(int argc, char* argv[]) {


    std::cout << "interrupt testing part\n";
    /*----------------------------------------------------*/

    char *uiod;
    void *ptr;
    int value = 0;
    uiod = "/dev/uio1";
    int fd;
    int irq_on = 1;
    // uint32_t info = 1; /* unmask */
    /* Open the UIO device file */
    fd = open(uiod, O_RDWR);
    if (fd < 1) {
        perror("open\n");
        printf("Invalid UIO device file:%s.\n", uiod);
        return -1;
    }

     /* mmap the UIO device */
    ptr = mmap(NULL, TIMER_MAP_SIZE, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
    value = *((unsigned *) (ptr + TIMER_0_CTR_REG));
    printf("timer counter register: %08x\n", value);

    /* Enable All Interrupts */
    printf("Enable Interrupt and Start for the first time!\n");
    *((unsigned *)(ptr)) = 0X0;
    
    
    uint32_t info = 1;
   
     
     *((unsigned *)(ptr)) = 0XC0;
  

    printf("before reading\n");
    /* Wait for interrupt */
        ssize_t nb = read(fd, &info, sizeof(info));
        printf("right after reading\n");
         value = *((unsigned *) (ptr + TIMER_0_CTR_REG));
        printf("timer counter register: %08x\n", value);
        if (nb == (ssize_t)sizeof(info)) {
            /* Do something in response to the interrupt. */
            printf("Interrupt #%u!\n", info);
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
