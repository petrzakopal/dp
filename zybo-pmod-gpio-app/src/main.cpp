/*******************************************************************************
Author: FEE CTU
Purpose: Host program

*******************************************************************************/

/*----------------------------------------------------------------------------*/
/*-------------------- INFO FUNCTION FOR PRINTING ERRORS --------------------*/
#define OCL_CHECK(error, call)                                                                   \
    call;                                                                                        \
    if (error != CL_SUCCESS) {                                                                   \
        printf("%s:%d Error calling " #call ", error code is: %d\n", __FILE__, __LINE__, error); \
        exit(EXIT_FAILURE);                                                                      \
}
/*----------------------------------------------------------------------------*/



/*----------------------------------------------------------------------------*/
/*--------------------------- CONSTANT DEFINITION ---------------------------*/
#define PI 3.141592
static const int DATA_SIZE = 4096; // Size of Arrrays etc. Change later
/*----------------------------------------------------------------------------*/




/*----------------------------------------------------------------------------*/
/*-------------------------------- INCLUDES ---------------------------------*/
#include "allocator.h"
#include <fstream>
#include <iostream>
#include <stdlib.h>
#include <cmath>
#include "header/MotorModel.h"
#include "header/transformation.h"
#include "experimental/xrt_profile.h" // for profiling host program
/*----------------------------------------------------------------------------*/






/*------------------------------------------------------------------------------------*/
/*-------------------------------- MAIN HOST PROGRAM ---------------------------------*/
int main(int argc, char* argv[]) {

xrt::profile::user_range range("Phase 1", "Start of execution to context creation"); // profiling 


    /*-|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||-*/
    /*-------------------------------- SETTING THE ENVIRONMENT ---------------------------------*/
    // TARGET_DEVICE macro needs to be passed from gcc command line
    if (argc != 2)
    {
        std::cout << "Usage: " << argv[0] << " <xclbin>" << std::endl;
        return EXIT_FAILURE;
    }

    std::string xclbinFilename = argv[1];

    // Compute the size of array in bytes
    size_t size_in_bytes = DATA_SIZE * sizeof(float);

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
    cl::Kernel krnl_GenerateVoltage;
    cl::Kernel krnl_compute2;
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
            OCL_CHECK(err, krnl_GenerateVoltage = cl::Kernel(program, "krnl_GenerateVoltage", &err)); // assign to krnl_compute the new PL kernel, which is found in kernel_main and called kernel_main

            // testing second kernel
            OCL_CHECK(err, krnl_compute2 = cl::Kernel(program, "krnl_main2", &err));

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

    /****************************************************************************************************************/

    /*-|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||-*/
    /*------------------------------------ SETTING THE APP -------------------------------------*/
    


    /*----------------------------------------------------------------------------------*/
    /*-------------------- INITIALIZATION VIA MOTORMODEL CLASS API ---------------------*/
    MotorModelClass MotorModel;
    MotorModel.odeCalculationSettingsAllocateMemory();
    MotorModel.setOdeCalculationSettings(0, 1, 0.00001); // initial time, final time, calculation step; if you want to calculate just one sample at a time (in for cycle of RK4), use (0, 1, 1)
    MotorModel.motorParametersAllocateMemory();
    MotorModel.stateSpaceCoeffAllocateMemory();
    MotorModel.modelVariablesAllocateMemory(); // on index [0] there are initialConditions, RK4 starts from 1 to <=n when n is (final-initial)/step





    /*-------------------- USE FUNCTION WITH HARDCODED VALUE OF MOTOR OR SET IT MANUALLY ---------------------*/

    // MotorModel.setMotorParameters(); // hardcoded values
    MotorModel.motorParameters->R1 = 0.370f; // Ohm, stator rezistance
    MotorModel.motorParameters->R2 = 0.225f; // Ohm, rotor rezistance
    MotorModel.motorParameters->L1s = 0.00227f; // H, stator leakage inductance
    MotorModel.motorParameters->L2s = 0.00227f; // H, rotor leakage inductance
    MotorModel.motorParameters->Lm = 0.0825f; // H, main flux inductance
    MotorModel.motorParameters->L1 = 0.08477f; // H, inductance
    MotorModel.motorParameters->L2 = 0.08477f; // H, inductance
    MotorModel.motorParameters->sigma = 0.05283f; // = 0.0528396032, sigma = 1 - Lm^(2)/L1L2
    MotorModel.motorParameters->nOfPolePairs = 2; // number of pole pairs
    MotorModel.motorParameters->momentOfIntertia = 0.4; // J, moment of inertia
    /*--------------------------------------------------------------------------------------------------------*/


    

    MotorModel.setStateSpaceCoeff();
    MotorModel.voltageGeneratorDataAllocateMemory();

    /*----------------------------------------------------------------------------------*/

    int numberOfIterations = MotorModel.numberOfIterations();

    MotorModel.voltageGeneratorData->voltageFrequency = 50;
    MotorModel.voltageGeneratorData->voltageAmplitude = 325.26;

/*----------------------------------------------------------------------------------*/

    /*----------------------------------------------------------------------------------*/
    /*--------------------------------- TEST OUTPUTS ----------------------------------*/

    std::cout << "test state space coeff= " << MotorModel.getStateSpaceCoeff()->a13 << "\n";

    std::cout << "number of modelVariables: " << ((int)ceil((MotorModel.odeCalculationSettings->finalCalculationTime - MotorModel.odeCalculationSettings->initialCalculationTime)/MotorModel.odeCalculationSettings->calculationStep)) << "\n";
    /*----------------------------------------------------------------------------------*/


    OCL_CHECK(err, cl::Buffer buffer_voltageGeneratorData(context, CL_MEM_USE_HOST_PTR, numberOfIterations * sizeof(voltageGeneratorType),MotorModel.voltageGeneratorData,&err));
    OCL_CHECK(err, cl::Buffer buffer_odeCalculationSettings(context, CL_MEM_USE_HOST_PTR, sizeof(odeCalculationSettingsType),MotorModel.odeCalculationSettings,&err));


    std::cout << "test voltage free data= " << MotorModel.voltageGeneratorData->u1 << "\n";
    int narg = 0;
    OCL_CHECK(err, err = krnl_GenerateVoltage.setArg(narg++, buffer_voltageGeneratorData));
    OCL_CHECK(err, err = krnl_GenerateVoltage.setArg(narg++, buffer_odeCalculationSettings));

    // Data will be migrated to kernel space
    OCL_CHECK(err, err = q.enqueueMigrateMemObjects({buffer_voltageGeneratorData, buffer_odeCalculationSettings}, 0 /* 0 means from host*/));


    // Launch the Kernel
    OCL_CHECK(err, err = q.enqueueTask(krnl_GenerateVoltage));

    OCL_CHECK(err, q.enqueueMigrateMemObjects({buffer_voltageGeneratorData}, CL_MIGRATE_MEM_OBJECT_HOST));

    OCL_CHECK(err, q.finish());

    std::cout << "motor voltage u1 at 0: " << MotorModel.getVoltage(0)->u1 << "\n";
    std::cout << "motor clarke voltage u1alpha at 20: " << MotorModel.getVoltage(0)->u1alpha << "\n";
    std::cout << "motor clarke voltage u1beta at 20: " << MotorModel.getVoltage(0)->u1beta << "\n";

    
    


    free(MotorModel.motorParameters);
    free(MotorModel.stateSpaceCoeff);
    free(MotorModel.modelVariables);
    free(MotorModel.odeCalculationSettings);
    free(MotorModel.voltageGeneratorData);

    range.end(); // profiling 
    return 0;

 
}
