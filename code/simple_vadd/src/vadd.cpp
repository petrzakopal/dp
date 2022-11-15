/**
* Copyright (C) 2019-2021 Xilinx, Inc
*
* Licensed under the Apache License, Version 2.0 (the "License"). You may
* not use this file except in compliance with the License. A copy of the
* License is located at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
* WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
* License for the specific language governing permissions and limitations
* under the License.
*/

// Function used to check if the function went well
#define OCL_CHECK(error, call)                                                                   \
    call;                                                                                        \
    if (error != CL_SUCCESS) {                                                                   \
        printf("%s:%d Error calling " #call ", error code is: %d\n", __FILE__, __LINE__, error); \
        exit(EXIT_FAILURE);                                                                      \
    }

// Include libraries
#include "vadd.h"
#include <fstream>
#include <iostream>
#include <stdlib.h>

// Defining data size same as in kernel
// It is the size of the vector - in position - not in sizeof(variable_type)
static const int DATA_SIZE = 4096;

// Consntant static string of error message
static const std::string error_message =
    "Error: Result mismatch:\n"
    "i = %d CPU result = %d Device result = %d\n";

// argc = argument count
// argv = argument vector
// basically argc is number of arguments +1, because most programs append number of programm to the arguments vector, but not sure if this is the case
int main(int argc, char* argv[]) {
    // TARGET_DEVICE macro needs to be passed from gcc command line
    if (argc != 2) {
        std::cout << "Usage: " << argv[0] << " <xclbin>" << std::endl;
        return EXIT_FAILURE;
    }

    // Getting the name of the binary file which is passed as an argument in vector
    std::string xclbinFilename = argv[1];

    // Compute the size of array in bytes
    // sizeof(<data_type>) in ARM processors: https://developer.arm.com/documentation/dui0472/k/C-and-C---Implementation-Details/Basic-data-types-in-ARM-C-and-C--
    size_t size_in_bytes = DATA_SIZE * sizeof(int);

    // Creates a vector of DATA_SIZE elements with an initial value of 10 and 32
    // using customized allocator for getting buffer alignment to 4k boundary

    std::vector<cl::Device> devices;
    cl::Device device;
    cl_int err; // cl_int is defined type for OpenCL devices - has ALWAYS fixed size of 32 bits
    cl::Context context;
    cl::CommandQueue q;
    cl::Kernel krnl_vector_add;
    cl::Program program;
    std::vector<cl::Platform> platforms;
    bool found_device = false;

    // traversing all Platforms To find Xilinx Platform and targeted
    // Device in Xilinx Platform
    cl::Platform::get(&platforms); // Gets list of available platforms and pastes the address of the firts one to variable platforms, which is an arrray, better std::vector
    for (size_t i = 0; (i < platforms.size()) & (found_device == false); i++) {
        cl::Platform platform = platforms[i];
        std::string platformName = platform.getInfo<CL_PLATFORM_NAME>();

        // If found the platform name Xilinx
        if (platformName == "Xilinx") {
            // Be sure thah devices vector is empty
            devices.clear();
            // Gets list of devices for found Xilinx platform TYPE ACCELERATOR and pastes the device in devices vector
            platform.getDevices(CL_DEVICE_TYPE_ACCELERATOR, &devices);

            // If there is some device found that matches the requirements (Xilinx and Accelerator)
            // Put the device specs to cl::Device variable
            if (devices.size()) {
                device = devices[0];
                found_device = true;
                break;
            }
        }
    }
    if (found_device == false) {
        std::cout << "Error: Unable to find Target Device " << device.getInfo<CL_DEVICE_NAME>() << std::endl;
        return EXIT_FAILURE;
    }

    // Creating Context and Command Queue for selected device
    OCL_CHECK(err, context = cl::Context(device, NULL, NULL, NULL, &err));

    // CL_QUEUE_PROFILING_ENABLE - profiling objects are enabled by this parameter passed to parameter properties of CommandQueue
    // Event objects can be used to capture profiling information that measure execution time of a command. 
    OCL_CHECK(err, q = cl::CommandQueue(context, device, CL_QUEUE_PROFILING_ENABLE, &err));

    std::cout << "INFO: Reading " << xclbinFilename << std::endl;
    FILE* fp;

    // c_str Returns a pointer to an array that contains a null-terminated sequence of characters (i.e., a C-string) representing the current value of the string object.
    // opening file
    if ((fp = fopen(xclbinFilename.c_str(), "r")) == nullptr) {
        printf("ERROR: %s xclbin not available please build\n", xclbinFilename.c_str());
        exit(EXIT_FAILURE);
    }

    // Load xclbin
    std::cout << "Loading: '" << xclbinFilename << "'\n";

    // Loading binary file to bin_file
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

    // Creating Program from Binary File
    cl::Program::Binaries bins;
    // {} initialization of vector - array
    // push_back - Adds a new element at the end of the vector, after its current last element.
    bins.push_back({buf, nb});

    // Resizes devices vector to contain just one device, which is used
    devices.resize(1);
    // OCL_CHECK wraps andn checks if function completed correctly

    // cl:Program - Construct a program object from a list of devices and a per-device list of binaries.
    // https://github.khronos.org/OpenCL-CLHPP/classcl_1_1_program.html#af804a4cb0e2dc844c0be0b0b76dbd25a
    OCL_CHECK(err, program = cl::Program(context, devices, bins, NULL, &err));

    // This call will get the kernel object from program. A kernel is an
    // OpenCL function that is executed on the FPGA.
    // https://github.khronos.org/OpenCL-CLHPP/classcl_1_1_kernel.html#a351422a7e7125812c23610628b0f9de7
    OCL_CHECK(err, krnl_vector_add = cl::Kernel(program, "krnl_vadd", &err));

    // These commands will allocate memory on the Device. The cl::Buffer objects can
    // be used to reference the memory locations on the device.
    // Allowed flags for cl::Buffer https://man.opencl.org/clCreateBuffer.html
    // size_in_bytes is DATA_SIZE*sizeof(int) here 4096*
    OCL_CHECK(err, cl::Buffer buffer_a(context, CL_MEM_READ_ONLY, size_in_bytes, NULL, &err));
    OCL_CHECK(err, cl::Buffer buffer_b(context, CL_MEM_READ_ONLY, size_in_bytes, NULL, &err));
    OCL_CHECK(err, cl::Buffer buffer_result(context, CL_MEM_WRITE_ONLY, size_in_bytes, NULL, &err));

    // set the kernel Arguments
    int narg = 0;
    // cl::Kernel::setArg(cl uint index, T value)
    // Set the kernel argument; you do not need to specify the type as it is worked out from T using templates.
    OCL_CHECK(err, err = krnl_vector_add.setArg(narg++, buffer_a));
    OCL_CHECK(err, err = krnl_vector_add.setArg(narg++, buffer_b));
    OCL_CHECK(err, err = krnl_vector_add.setArg(narg++, buffer_result));
    OCL_CHECK(err, err = krnl_vector_add.setArg(narg++, DATA_SIZE));

    // We then need to map our OpenCL buffers to get the pointers
    int* ptr_a;
    int* ptr_b;
    int* ptr_result;

    // Enqueues a command to map a region of the buffer object given by buffer into the host address space and returns a pointer to this mapped regio
    // https://registry.khronos.org/OpenCL/sdk/1.0/docs/man/xhtml/clEnqueueMapBuffer.html
    OCL_CHECK(err,
              ptr_a = (int*)q.enqueueMapBuffer(buffer_a, CL_TRUE, CL_MAP_WRITE, 0, size_in_bytes, NULL, NULL, &err));
    OCL_CHECK(err,
              ptr_b = (int*)q.enqueueMapBuffer(buffer_b, CL_TRUE, CL_MAP_WRITE, 0, size_in_bytes, NULL, NULL, &err));
    OCL_CHECK(err, ptr_result = (int*)q.enqueueMapBuffer(buffer_result, CL_TRUE, CL_MAP_READ, 0, size_in_bytes, NULL,
                                                         NULL, &err));

    // Data will be migrated to kernel space
    OCL_CHECK(err, err = q.enqueueMigrateMemObjects({buffer_a, buffer_b}, 0 /* 0 means from host*/));

    // Launch the Kernel
    // Enqueues a command to execute a kernel on a device.
    // https://registry.khronos.org/OpenCL/sdk/1.0/docs/man/xhtml/clEnqueueTask.html
    // https://kosobucki.pl/cl_doc/classcl_1_1CommandQueue.html#a19f0b52a87a8a33d1280ef9e8f5149f3 with example
    // original https://github.khronos.org/OpenCL-CLHPP/classcl_1_1_command_queue.html#a287d13ca6eb15f904419871dfc440ca6
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
    return (match ? EXIT_FAILURE : EXIT_SUCCESS);
}
