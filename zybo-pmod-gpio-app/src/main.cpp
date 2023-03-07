/*******************************************************************************
Author: FEE CTU
Purpose: Host program
Comment: Refactoring for HLS
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
/*----------------------------------------------------------------------------*/




/*----------------------------------------------------------------------------*/
/*-------------------------------- INCLUDES ---------------------------------*/
#include "allocator.h"
#include <fstream>
#include <iostream>
#include <stdlib.h>
#include <hls_math.h>
// #include "header/MotorModel.h"
#include "header/CurVelModel.h"
#include <string>
#include <iostream>
#include <fstream>
#include <fcntl.h>
#include<unistd.h>



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
            OCL_CHECK(err, krnl_calculateCurVelModel = cl::Kernel(program, "krnl_calculateCurVelModel", &err)); // assign to krnl_compute the new PL kernel, which is found in kernel_main and called kernel_main

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
    


/*-------------------------------------------------------------------------------------------------------------------------------*/
/*------------------------------ DEFINING CLASS OBJECT AND ALLOCATING MEMORY FOR SECOND I-N MODEL ------------------------------*/
    // CurVelModelClass CurVelModel;
/*-------------------------------------------------------------------------------------------------------------------------------*/
   
float *odeCVCalculationSettingsArray;
// int *numberOfIterations;
posix_memalign((void **)&odeCVCalculationSettingsArray , 4096 , 5*sizeof(float) );
// posix_memalign((void **)&numberOfIterations , 4096 , 4*sizeof(int) );
odeCVCalculationSettingsArray[0] = 0; // initialCalculationTime
odeCVCalculationSettingsArray[1] = 1; // finalCalculationTime
odeCVCalculationSettingsArray[2] = 0.0001; // calculationStep
odeCVCalculationSettingsArray[3] = 0; // calculationTime
odeCVCalculationSettingsArray[4] = (int)ceil(((odeCVCalculationSettingsArray[1] - odeCVCalculationSettingsArray[0])/odeCVCalculationSettingsArray[2]));


float *motorParametersArray;
posix_memalign((void **)&motorParametersArray , 4096 , 4*sizeof(float) );
motorParametersArray[0] = 0.225f; // R2
motorParametersArray[1] = 0.0825f; // Lm
motorParametersArray[2] = 0.08477f;  // L2
motorParametersArray[3] = 2; // nOfPolePairs

float * modelCVCoeffArray;
posix_memalign((void **)&modelCVCoeffArray , 4096 , 3*sizeof(float) );
modelCVCoeffArray[0] = motorParametersArray[0]/motorParametersArray[2]; // R2DL2
modelCVCoeffArray[1] = (motorParametersArray[0] * motorParametersArray[1])/motorParametersArray[2]; // R2MLmDL2
modelCVCoeffArray[2] = 2;


float *modelCVVariablesArray;
posix_memalign((void **)&modelCVVariablesArray , 4096 , 12*sizeof(float) );

/*---------------------------------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------------------------------------*/
/*------------------------------ LOADING DATA FROM A LIGHTWEIGHT EXPORT FORMAT ------------------------------*/

float *inputTime;
float *inputI1;
float *inputI2;
float *inputI3;
float *inputMotorMechanicalAngularVelocity;

// float *psi2Amplitude;
// float *transformAngle;

float *psi2alpha;
float *psi2beta;







int modeSelection = 0;
printf("Select mode:\n0 - preloaded data\n1 - keyboard input data\n");
scanf("%i", &modeSelection);

printf("You have selected: %i\n\r", modeSelection);

if(modeSelection == 0)
{

    
posix_memalign((void **)&inputTime , 4096 , odeCVCalculationSettingsArray[4]*sizeof(float) );
posix_memalign((void **)&inputI1 , 4096 , odeCVCalculationSettingsArray[4]*sizeof(float) );
posix_memalign((void **)&inputI2 , 4096 , odeCVCalculationSettingsArray[4]*sizeof(float) );
posix_memalign((void **)&inputI3 , 4096 , odeCVCalculationSettingsArray[4]*sizeof(float) );
posix_memalign((void **)&inputMotorMechanicalAngularVelocity , 4096 , odeCVCalculationSettingsArray[4]*sizeof(float) );
posix_memalign((void **)&psi2alpha , 4096 , odeCVCalculationSettingsArray[4]*sizeof(float) );
posix_memalign((void **)&psi2beta , 4096 , odeCVCalculationSettingsArray[4]*sizeof(float) );

psi2alpha[0] = 0;
psi2beta[0] = 0;

    /*------------------------------ LOADING DATA STREAM ------------------------------*/
printf("------------------------------------\n\r");
printf("Preloaded data mode\n");
printf("------------------------------------\n\r");
std::ifstream inputData( "outputData.csv" );

int switchReadingIndex = 0;
int timeIndex = 0;
int i1Aindex = 0;
int i1Bindex = 0;
int i1Cindex = 0;
int motorMechanicalAngularVelocityIndex = 0;

for( std::string line; std::getline( inputData, line, ','); )
{


        // data structure is important, defined in a lightweight export file like {time,i1,i2,i3,motorMechanicalAngularVelocity}
         switch(switchReadingIndex)
            {
                case 0:
               
                inputTime[timeIndex] = std::stof (line, NULL);
                timeIndex++;
                switchReadingIndex++;
                break;

                case 1:

                inputI1[i1Aindex] = std::stof (line, NULL);
                i1Aindex++;
                switchReadingIndex++;
                break;


                case 2:

                inputI2[i1Bindex] = std::stof (line, NULL);
                i1Bindex++;
                switchReadingIndex++;
                break;

                case 3:

                inputI3[i1Cindex] = std::stof (line, NULL);
                i1Cindex++;
                switchReadingIndex++;
                break;

                case 4:

                inputMotorMechanicalAngularVelocity[motorMechanicalAngularVelocityIndex] = std::stof (line, NULL);
                motorMechanicalAngularVelocityIndex++;
                switchReadingIndex = 0;
                break;

        }
    
  
}
    inputData.close(); // close that file

/*------------------------------ LOADING DATA STREAM ------------------------------*/
/*------------------------------------------------------------------------------------------------------------*/
}
else if(modeSelection == 1)
{
    
    printf("Keyboard input data mode\n\r");
    printf("------------------------------------\n\r");
    printf("Insert data divided by {space symbol}\n\r");
    printf("I1 I2 I3 MechanicalAngularVelocity psi2alpha[0] psi2beta[0] \n\r");
    odeCVCalculationSettingsArray[4] = 1;

    
posix_memalign((void **)&inputTime , 4096 , odeCVCalculationSettingsArray[4]*sizeof(float) );
posix_memalign((void **)&inputI1 , 4096 , odeCVCalculationSettingsArray[4]*sizeof(float) );
posix_memalign((void **)&inputI2 , 4096 , odeCVCalculationSettingsArray[4]*sizeof(float) );
posix_memalign((void **)&inputI3 , 4096 , odeCVCalculationSettingsArray[4]*sizeof(float) );
posix_memalign((void **)&inputMotorMechanicalAngularVelocity , 4096 , odeCVCalculationSettingsArray[4]*sizeof(float) );
posix_memalign((void **)&psi2alpha , 4096 , odeCVCalculationSettingsArray[4]*sizeof(float) );
posix_memalign((void **)&psi2beta , 4096 , odeCVCalculationSettingsArray[4]*sizeof(float) );


inputTime[0] = 0;

    scanf("%f %f %f %f %f %f", inputI1, inputI2, inputI3, inputMotorMechanicalAngularVelocity, psi2alpha, psi2beta);
    printf("------------------------------------\n\r");
    printf("You have entered:\n\r");
    printf("I1 = %f\n\rI2 = %f\n\rI3 = %f\n\rMechanicalAngularVelocity = %f\n\rpsi2alpha[0] = %f\n\rpsi2beta = %f\n\r", inputI1[0], inputI2[0], inputI3[0], inputMotorMechanicalAngularVelocity[0], psi2alpha[0], psi2beta[0]);
    printf("------------------------------------\n\r");
}


    
// posix_memalign((void **)&psi2alpha , 4096 , CurVelModel.odeCVCalculationSettings->numberOfIterations*sizeof(float) );
// posix_memalign((void **)&psi2beta , 4096 , CurVelModel.odeCVCalculationSettings->numberOfIterations*sizeof(float) );
// posix_memalign((void **)&psi2Amplitude , 4096 , CurVelModel.odeCVCalculationSettings->numberOfIterations*sizeof(float) );


// psi2alpha[0] = 0;
// psi2beta[0] = 0;
// psi2Amplitude[0] = 0;
// transformAngle[0] = 0;






/*------------------------------------------------------------------------------------------------------------*/
    //first type kernel
    OCL_CHECK(err, cl::Buffer buffer_odeCVCalculationSettings(context, CL_MEM_USE_HOST_PTR, 4*sizeof(float),odeCVCalculationSettingsArray,&err));

    // OCL_CHECK(err, cl::Buffer buffer_modelCVVariables(context, CL_MEM_USE_HOST_PTR, sizeof(modelCVVariablesType),CurVelModel.modelCVVariables,&err)); // enable for first type kernel

    OCL_CHECK(err, cl::Buffer buffer_modelCVCoeff(context, CL_MEM_USE_HOST_PTR, 3*sizeof(float),modelCVCoeffArray,&err));

    OCL_CHECK(err, cl::Buffer buffer_inputI1(context, CL_MEM_USE_HOST_PTR, odeCVCalculationSettingsArray[4] * sizeof(float),inputI1,&err));

    OCL_CHECK(err, cl::Buffer buffer_inputI2(context, CL_MEM_USE_HOST_PTR, odeCVCalculationSettingsArray[4] * sizeof(float),inputI2,&err));

    OCL_CHECK(err, cl::Buffer buffer_inputI3(context, CL_MEM_USE_HOST_PTR, odeCVCalculationSettingsArray[4] * sizeof(float),inputI3,&err));

    OCL_CHECK(err, cl::Buffer buffer_inputMotorMechanicalAngularVelocity(context, CL_MEM_USE_HOST_PTR, odeCVCalculationSettingsArray[4] * sizeof(float),inputMotorMechanicalAngularVelocity,&err));

    // OCL_CHECK(err, cl::Buffer buffer_psi2Amplitude(context, CL_MEM_USE_HOST_PTR, CurVelModel.odeCVCalculationSettings->numberOfIterations * sizeof(float),psi2Amplitude,&err));


    //second type kernel – disable for first type kernel
    OCL_CHECK(err, cl::Buffer buffer_psi2alpha(context, CL_MEM_USE_HOST_PTR, odeCVCalculationSettingsArray[4] * sizeof(float),psi2alpha,&err));

    OCL_CHECK(err, cl::Buffer buffer_psi2beta(context, CL_MEM_USE_HOST_PTR, odeCVCalculationSettingsArray[4] * sizeof(float),psi2beta,&err));
    // OCL_CHECK(err, cl::Buffer buffer_transformAngle(context, CL_MEM_USE_HOST_PTR, CurVelModel.odeCVCalculationSettings->numberOfIterations * sizeof(float),transformAngle,&err));



    


    int narg = 0;
    OCL_CHECK(err, err = krnl_calculateCurVelModel.setArg(narg++, buffer_odeCVCalculationSettings));
    // OCL_CHECK(err, err = krnl_calculateCurVelModel.setArg(narg++, buffer_modelCVVariables)); // enable for first type kernel
    OCL_CHECK(err, err = krnl_calculateCurVelModel.setArg(narg++, buffer_modelCVCoeff));
    OCL_CHECK(err, err = krnl_calculateCurVelModel.setArg(narg++, buffer_inputI1));
    OCL_CHECK(err, err = krnl_calculateCurVelModel.setArg(narg++, buffer_inputI2));
    OCL_CHECK(err, err = krnl_calculateCurVelModel.setArg(narg++, buffer_inputI3));
    OCL_CHECK(err, err = krnl_calculateCurVelModel.setArg(narg++, buffer_inputMotorMechanicalAngularVelocity));
    // OCL_CHECK(err, err = krnl_calculateCurVelModel.setArg(narg++, buffer_psi2Amplitude));

    // //second type kernel - disable for frst type kernel
    OCL_CHECK(err, err = krnl_calculateCurVelModel.setArg(narg++, buffer_psi2alpha));
    OCL_CHECK(err, err = krnl_calculateCurVelModel.setArg(narg++, buffer_psi2beta));
    // OCL_CHECK(err, err = krnl_calculateCurVelModel.setArg(narg++, buffer_transformAngle));
  

    // Data will be migrated to kernel space
    // delete buffer_psi2alpha and buffer_psi2beta for first type kernel
    // delete buffer_modelCVVariables for second type kernel and add for first type
    OCL_CHECK(err, err = q.enqueueMigrateMemObjects({buffer_odeCVCalculationSettings,buffer_modelCVCoeff, buffer_inputI1, buffer_inputI2, buffer_inputI3, buffer_inputMotorMechanicalAngularVelocity, buffer_psi2alpha, buffer_psi2beta}, 0 /* 0 means from host*/));

    /*------------------------------------------------------------------------------------------------------------*/

    xrt::profile::user_range rangeKernel("Phase 2", "Kernel run"); // profiling 
    // Launch the Kernel
    OCL_CHECK(err, err = q.enqueueTask(krnl_calculateCurVelModel));
    
    OCL_CHECK(err, q.enqueueMigrateMemObjects({buffer_psi2alpha, buffer_psi2beta}, CL_MIGRATE_MEM_OBJECT_HOST));
    rangeKernel.end();
    OCL_CHECK(err, q.finish());



    // data output
    std::ofstream modelCVOutputDataFile2;
    modelCVOutputDataFile2.open("outputCurVel2.csv",std::ofstream::out | std::ofstream::trunc);
    modelCVOutputDataFile2<< "time,|psi2|,transformAngle\n";
    float psi2Amplitude;
    float transformAngle;
    float timeCV = odeCVCalculationSettingsArray[0];

    for(int i = 0; i<odeCVCalculationSettingsArray[4];i++ )
    {   
        timeCV = timeCV + odeCVCalculationSettingsArray[2];

        psi2Amplitude = sqrtf(psi2alpha[i] * psi2alpha[i] + psi2beta[i] * psi2beta[i] );
        transformAngle = atan2f(psi2beta[i],psi2alpha[i]);
        std::cout << "psi2Amplitude index "<< i << " : " << psi2Amplitude << "\n";

        // std::cout << "psi2Amplitude index "<< i << " : " << psi2Amplitude[i] << "\n";
        std::cout << "transformAngle[" << i << "]: "<< transformAngle << "\n";
        // std::cout << "inputI1[" << i << "]: "<< inputI1[i] << "\n";
        

        modelCVOutputDataFile2<<timeCV<<","<<psi2Amplitude<<","<<transformAngle<<"\n";
    }

    modelCVOutputDataFile2.close();

    std::cout << "the end of the most useful program is here\n";
    

    // free the memory
    free(motorParametersArray);
    free(modelCVCoeffArray);
    free(inputTime);
    free(inputI1);
    free(inputI2);
    free(inputI3);
    // free(psi2Amplitude);
    free(psi2alpha);
    free(psi2beta);
    free(inputMotorMechanicalAngularVelocity);
    free(modelCVVariablesArray);
    free(odeCVCalculationSettingsArray);

/*---------------------------------------------------------------------------*/

  std::cout << "random testing code phase\n";
/*------------------------------ TESTING GPIO ------------------------------*/


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
  
    valuefd = open("/sys/class/gpio/gpio980/value", O_RDWR);
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
/*---------------------------------------------------------------------------*/

    range.end(); // profiling 
    return 0;

 
}
