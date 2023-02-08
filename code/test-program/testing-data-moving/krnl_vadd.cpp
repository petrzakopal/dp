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

/*******************************************************************************
Description:

    This example uses the load/compute/store coding style which is generally
    the most efficient for implementing kernels using HLS. The load and store
    functions are responsible for moving data in and out of the kernel as
    efficiently as possible. The core functionality is decomposed across one
    of more compute functions. Whenever possible, the compute function should
    pass data through HLS streams and should contain a single set of nested loops.

    HLS stream objects are used to pass data between producer and consumer
    functions. Stream read and write operations have a blocking behavior which
    allows consumers and producers to synchronize with each other automatically.

    The dataflow pragma instructs the compiler to enable task-level pipelining.
    This is required for to load/compute/store functions to execute in a parallel
    and pipelined manner. Here the kernel loads, computes and stores NUM_WORDS integer values per
    clock cycle and is implemented as below:
                                       _____________
                                      |             |<----- Input Vector 1 from Global Memory
                                      |  load_input |       __
                                      |_____________|----->|  |
                                       _____________       |  | in1_stream
Input Vector 2 from Global Memory --->|             |      |__|
                               __     |  load_input |        |
                              |  |<---|_____________|        |
                   in2_stream |  |     _____________         |
                              |__|--->|             |<--------
                                      | compute_add |      __
                                      |_____________|---->|  |
                                       ______________     |  | out_stream
                                      |              |<---|__|
                                      | store_result |
                                      |______________|-----> Output result to Global Memory

*******************************************************************************/


// All of these functions are ACCELERATED on the FPGA device (in PL)

// Includes
// Declares sets of integer types having a specified widths
// https://pubs.opengroup.org/onlinepubs/009695399/basedefs/stdint.h.html
#include <stdint.h> 

// Library for streams - optimization of code - paralelism
// More about streams https://docs.xilinx.com/r/en-US/ug1399-vitis-hls/Using-HLS-Streams
#include <hls_stream.h>

#define DATA_SIZE 4096

// TRIPCOUNT identifier
// The tripcount shows the total number of loop iterations.
// More about tripcount, or loop iterations https://docs.xilinx.com/r/en-US/ug1399-vitis-hls/pragma-HLS-loop_tripcount
// Probably defines number of iterations/tripcounts of loop
const int c_size = DATA_SIZE;


/*******************************************************************************/
// Function for loading inputs from a Global Memory
// Second argument is stream (not RAM but FIFO memory - sequential) When streams are passed into and out of functions, they must be passed-by-reference (eg. &)

static void load_input(uint32_t* in, hls::stream<uint32_t>& inStream, int size) {
mem_rd:
    for (int i = 0; i < size; i++) {
#pragma HLS LOOP_TRIPCOUNT min = c_size max = c_size
        // here << operator does not mean bit shifting but „Insertion Operators“
        // more about Insertion operators at https://faculty.cs.niu.edu/~hutchins/csci241/io-op.htm
        // In xilinx it is called blocking API at https://docs.xilinx.com/r/en-US/ug1399-vitis-hls/Blocking-API
        inStream << in[i];

    }
}
/*******************************************************************************/



/*******************************************************************************/
// Function 
static void compute_add(hls::stream<uint32_t>& in1_stream,
                        hls::stream<uint32_t>& in2_stream,
                        hls::stream<uint32_t>& out_stream,
                        int size) {
// The kernel is operating with vector of NUM_WORDS integers. The + operator performs
// an element-wise add, resulting in NUM_WORDS parallel additions.
execute:
    for (int i = 0; i < size; i++) {
#pragma HLS LOOP_TRIPCOUNT min = c_size max = c_size

        // Read Blocking Read methods READS from the head of the stream and assigns the values to destination variable my_stream.read(dst_var); (not here)
        // Here it reads the data from stream, sums it up and then inserts it in the out_stream stream
        out_stream << (in1_stream.read() + in2_stream.read());
    }
}
/*******************************************************************************/


/*******************************************************************************/
static void store_result(uint32_t* out, hls::stream<uint32_t>& out_stream, int size) {
mem_wr:
    for (int i = 0; i < size; i++) {
#pragma HLS LOOP_TRIPCOUNT min = c_size max = c_size
        out[i] = out_stream.read();
    }
}
/*******************************************************************************/


// Start of the Kernel block
// Vitis requires C++ kernels to be declared as extern “C” to avoid name mangling issues
extern "C" {

/*
    Vector Addition Kernel

    Arguments:
        in1  (input)  --> Input vector 1
        in2  (input)  --> Input vector 2
        out  (output) --> Output vector
        size (input)  --> Number of elements in vector
*/


// Basically a main function for kernel to be executed/implemented in PL
void krnl_vadd(uint32_t* in1, uint32_t* in2, uint32_t* out, int size) {
// more about #pragma HLS INTERFACE https://docs.xilinx.com/r/en-US/ug1399-vitis-hls/pragma-HLS-interface
    // mode=<mode> eg. m_axi – Implements all ports as an AXI4 interface.
    // port=<name> – Specifies the name of the function argument or function return which the INTERFACE pragma applies to.
    // bundle=<string> – By default, the HLS tool groups or bundles function arguments with compatible options into interface ports in the RTL code. Similarly, all function arguments specified as an AXI4 (m_axi) interface are bundled into a single AXI4 port by default. All interface ports with compatible options, such as mode, offset, and bundle, are grouped into a single interface port. The port name is derived automatically from a combination of the mode and bundle, or is named as specified by -name.
#pragma HLS INTERFACE m_axi port = in1 bundle = gmem0
#pragma HLS INTERFACE m_axi port = in2 bundle = gmem1
#pragma HLS INTERFACE m_axi port = out bundle = gmem0

    // defining stream which corresponds to name in the parenthesis because of error logging, otherwise it would show only eg.
    // hls::stream<uint8_t> bytestr_in1;
    // hls::stream<uint8_t> bytestr_in2("input_stream2");
    // WHEN ERROR
    // WARNING: Hls::stream 'hls::stream<unsigned char>.1' contains leftover data, which may result in RTL simulation hanging.
    // WARNING: Hls::stream 'input_stream2' contains leftover data, which may result in RTL simulation hanging.
    static hls::stream<uint32_t> in1_stream("input_stream_1");
    static hls::stream<uint32_t> in2_stream("input_stream_2");
    static hls::stream<uint32_t> out_stream("output_stream");

#pragma HLS dataflow
    // dataflow pragma instruct compiler to run following three APIs in parallel
    // not definetely APIs but functions declared and defined up it the code
    load_input(in1, in1_stream, size);
    load_input(in2, in2_stream, size);
    compute_add(in1_stream, in2_stream, out_stream, size);
    store_result(out, out_stream, size);
}
}
