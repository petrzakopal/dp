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

#include <string>
#include <iostream>
#include <fstream>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include "header/MotorModel.h"
#include "header/CurVelModel.h"
#include "header/regulator.h"
// #include "header/svmCore.h" // already in invertor
#include "header/transformation.h"
#include "header/invertor.h"



#include "experimental/xrt_profile.h" // for profiling host program
/*----------------------------------------------------------------------------*/



// Define the function to be called when ctrl-c (SIGINT) is sent to process
void signal_callback_handler(int signum) {
   std::cout << "\nCaught signal " << signum << "\n";
   // Terminate program
   exit(signum);
}


/*------------------------------------------------------------------------------------*/
/*-------------------------------- MAIN HOST PROGRAM ---------------------------------*/
int main(int argc, char* argv[]) {

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
/*-------------------------------------------------------------------------------------------------------------------------------*/

/*-------------------------------------------------------------*/
/*-------------------- CLASS DEFINITIONS ---------------------*/
MotorModelClass MotorModel;
svmCoreClass svmCore;
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
float commonModeVoltage;
float compareLevel;
float trianglePoint;
float minMaxCommonModeVoltageConstant = 287;
float uS = 400;
float uDC = ((3 * sqrt(2))/PI) * uS;
float globalSimulationTime = globalInitialCalculationTime;
/*-------------------------------------------------------------*/




/*-------------------- ASYNCHRONOUS MOTOR MODEL ---------------------*/
MotorModel.odeCalculationSettingsAllocateMemory();
    

// needed for further memory allocation
MotorModel.setOdeCalculationSettings(globalInitialCalculationTime, globalFinalCalculationTime, globalCalculationStep);

MotorModel.motorParametersAllocateMemory();
MotorModel.stateSpaceCoeffAllocateMemory();
MotorModel.modelVariablesForOnlineCalculationAllocateMemory();
/*-------------------- END OF ASYNCHRONOUS MOTOR MODEL ---------------------*/
/*-------------------- SPACE VECTOR MODULATION CORE ---------------------*/
// svmCore.phaseWantedVoltageAllocateMemory(); // depracated
svmCore.invertorSwitchAllocateMemory(); 
svmCore.triangleWaveSettingsAllocateMemory();
svmCore.coreInternalVariablesAllocateMemory();
/*----------------------------------------------------------------------*/

// Not fancy definitions in the PS, maybe change later in the future
float *odeCVCalculationSettingsArray;
// int *numberOfIterations;
posix_memalign((void **)&odeCVCalculationSettingsArray , 4096 , 5*sizeof(float) );
// posix_memalign((void **)&numberOfIterations , 4096 , 4*sizeof(int) );
odeCVCalculationSettingsArray[0] = 0; // initialCalculationTime
odeCVCalculationSettingsArray[1] = 1; // finalCalculationTime
odeCVCalculationSettingsArray[2] = 0.0001; // calculationStep
odeCVCalculationSettingsArray[3] = 0; // calculationTime
odeCVCalculationSettingsArray[4] = (int)ceil(((odeCVCalculationSettingsArray[1] - odeCVCalculationSettingsArray[0])/odeCVCalculationSettingsArray[2])); // number of iterations - should not be used in a realtime model

/**
 * Model Parameters
*/
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
modelCVCoeffArray[2] = 2; // number of pole pairs




/*-------------------- REGULATORS ---------------------*/
Regulator.regulatorAllocateMemory();
/*---------------------------------------------------*/


/*-------------------- INVERTOR ---------------------*/
Invertor.reconstructedInvertorOutputVoltageAllocateMemory();
/*-------------------------------------------------*/





/*------------------------------------------------------------------*/
/*-------------------- TRIANGLE WAVE SETTINGS ---------------------*/
svmCore.triangleWaveSettings->waveAmplitude = 1;
svmCore.triangleWaveSettings->calculationStep = 0.000001;
svmCore.triangleWaveSettings->wavePeriod = 0.0001;
svmCore.triangleWaveSettings->calculationTime = 0;
/*------------------------------------------------------------------*/

/*--------------------------------------------------------------------*/
/*-------------------- IVNVERTOR MODEL SETTINGS ---------------------*/
float Udcmax = 311.87;
/*--------------------------------------------------------------------*/


/****************************************************************/
/*-------------------- REGULATOR SETTINGS ---------------------*/
    

/*-------------------- FLUX REGULATOR ---------------------*/
Regulator.fluxRegulator->ki = 30300; // 30300
Regulator.fluxRegulator->kp = 11410; //11410
Regulator.fluxRegulator->saturationOutputMax = 16.4223;
Regulator.fluxRegulator->saturationOutputMin = 0;
Regulator.fluxRegulator->saturationOutput = 0;
Regulator.fluxRegulator->clampingStatus = false;
Regulator.fluxRegulator->saturationCheckStatus = false;
Regulator.fluxRegulator->signCheckStatus = false;
Regulator.fluxRegulator->measuredValue = 0;
/*------------------------------------------------------*/

/*-------------------- VELOCITY REGULATOR ---------------------*/
Regulator.velocityRegulator->ki = 4650000; // 4650000
Regulator.velocityRegulator->kp = 3720; // 3720
Regulator.velocityRegulator->saturationOutputMax = 29.1228;
Regulator.velocityRegulator->saturationOutputMin = -29.1228;
Regulator.velocityRegulator->saturationOutput = 0;
Regulator.velocityRegulator->clampingStatus = false;
Regulator.velocityRegulator->saturationCheckStatus = false;
Regulator.velocityRegulator->signCheckStatus = false;
Regulator.velocityRegulator->measuredValue = 0;
/*------------------------------------------------------*/

/*-------------------- ID REGULATOR ---------------------*/
Regulator.idRegulator->ki = 2915.6; // 2915.6
Regulator.idRegulator->kp = 22.3961;// 22.3961
Regulator.idRegulator->saturationOutputMax = Udcmax; // (3*sqrt(2))/(pi*sqrt(3))*Us = 400(3*sqrt(2))/(3.141592*sqrt(3)) = 311.87
Regulator.idRegulator->saturationOutput = - Udcmax;
Regulator.idRegulator->clampingStatus = false;
Regulator.idRegulator->saturationCheckStatus = false;
Regulator.idRegulator->signCheckStatus = false;
Regulator.idRegulator->measuredValue = 0;
/*------------------------------------------------------*/

/*-------------------- IQ REGULATOR ---------------------*/
Regulator.iqRegulator->ki = 2915.6; // 2915.6
Regulator.iqRegulator->kp = 22.3961; // 22.3961
Regulator.iqRegulator->saturationOutputMax = sqrt((Udcmax * Udcmax) - (Regulator.idRegulator->saturationOutput * Regulator.idRegulator->saturationOutput)); // sqrt(Udcmax^2 - u1d^2) dynamically
Regulator.iqRegulator->saturationOutputMin = - Regulator.iqRegulator->saturationOutputMax;
Regulator.iqRegulator->clampingStatus = false;
Regulator.iqRegulator->saturationCheckStatus = false;
Regulator.iqRegulator->signCheckStatus = false;
Regulator.iqRegulator->measuredValue = 0;
/*------------------------------------------------------*/

/****************************************************************/




/*--------------------------------------------------------------*/
/*-------------------- WANTED VALUES INPUT ---------------------*/
// now hardcoded, change later
Regulator.fluxRegulator->wantedValue = 1;
Regulator.velocityRegulator->wantedValue = 50;
Regulator.idRegulator->wantedValue = 0;
Regulator.iqRegulator->wantedValue = 0;
/*--------------------------------------------------------------*/



/*---------------------------------------------------------------------------------------------------------*/


/*------------------------------------------------------------------------------------------------------------*/
/*------------------------------ LOADING DATA FROM A LIGHTWEIGHT EXPORT FORMAT ------------------------------*/

float *inputTime;
float *inputI1;
float *inputI2;
float *inputI3;
float *inputMotorMechanicalAngularVelocity;

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

}
/*------------------------------ LOADING DATA STREAM ------------------------------*/
/*------------------------------------------------------------------------------------------------------------*/

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


float *masterInput;
posix_memalign((void **)&masterInput , 4096 , (58)*sizeof(float) );
float *masterOutput;
posix_memalign((void **)&masterOutput , 4096 , (16)*sizeof(float) );

masterInput[0] = odeCVCalculationSettingsArray[0]; // initialCalculationTime
masterInput[1] = odeCVCalculationSettingsArray[1]; // finalCalculationTime
masterInput[2] = odeCVCalculationSettingsArray[2]; // calculationStep
masterInput[3] = odeCVCalculationSettingsArray[3]; // calculationTime
masterInput[4] = odeCVCalculationSettingsArray[4]; // number of iterations - should not be used in a realtime model
masterInput[5] = modelCVCoeffArray[0]; // R2DL2
masterInput[6] = modelCVCoeffArray[1]; // R2MLmDL2
masterInput[7] = modelCVCoeffArray[2]; // number of pole pairs
masterInput[8] = inputI1[0];
masterInput[9] = inputI2[0];
masterInput[10] = inputI3[0];
masterInput[11] = inputMotorMechanicalAngularVelocity[0];
masterInput[12] = masterOutput[2]; // psi2alpha - will not be used in realtime app (will be in kernel)
masterInput[13] = masterOutput[3];  //psi2beta - will not be used in realtime app (will be in kernel)
masterInput[14] = svmCore.triangleWaveSettings->waveAmplitude;
masterInput[15] = svmCore.triangleWaveSettings->calculationStep;
masterInput[16] = svmCore.triangleWaveSettings->wavePeriod;
masterInput[17] = svmCore.triangleWaveSettings->calculationTime = 0;
masterInput[18] =  Udcmax;
masterInput[19] = Regulator.fluxRegulator->ki;
masterInput[20] = Regulator.fluxRegulator->kp;
masterInput[21] = Regulator.fluxRegulator->saturationOutputMax;
masterInput[22] = Regulator.fluxRegulator->saturationOutputMin;
masterInput[23] = Regulator.fluxRegulator->saturationOutput;
masterInput[24] = Regulator.fluxRegulator->clampingStatus;
masterInput[25] = Regulator.fluxRegulator->saturationCheckStatus;
masterInput[26] = Regulator.fluxRegulator->signCheckStatus;
masterInput[27] = Regulator.fluxRegulator->measuredValue;
masterInput[28] = Regulator.velocityRegulator->ki;
masterInput[29] = Regulator.velocityRegulator->kp;
masterInput[30] = Regulator.velocityRegulator->saturationOutputMax;
masterInput[31] = Regulator.velocityRegulator->saturationOutputMin;
masterInput[32] = Regulator.velocityRegulator->saturationOutput;
masterInput[33] = Regulator.velocityRegulator->clampingStatus;
masterInput[34] = Regulator.velocityRegulator->saturationCheckStatus;
masterInput[35] = Regulator.velocityRegulator->signCheckStatus;
masterInput[36] = Regulator.idRegulator->ki;
masterInput[37] = Regulator.idRegulator->kp;
masterInput[38] = Regulator.idRegulator->saturationOutputMax;
masterInput[39] = Regulator.idRegulator->saturationOutput;
masterInput[40] = Regulator.idRegulator->clampingStatus;
masterInput[41] = Regulator.idRegulator->saturationCheckStatus;
masterInput[42] = Regulator.idRegulator->signCheckStatus;
masterInput[43] = Regulator.idRegulator->measuredValue;
masterInput[44] = Regulator.iqRegulator->ki;
masterInput[45] = Regulator.iqRegulator->kp;
masterInput[46] = Regulator.iqRegulator->saturationOutputMax;
masterInput[47] = Regulator.iqRegulator->saturationOutputMin;
masterInput[48] = Regulator.iqRegulator->clampingStatus;
masterInput[49] = Regulator.iqRegulator->saturationCheckStatus;
masterInput[50] = Regulator.iqRegulator->signCheckStatus;
masterInput[51] = Regulator.iqRegulator->measuredValue;
masterInput[52] = Regulator.fluxRegulator->wantedValue;
masterInput[53] = Regulator.velocityRegulator->wantedValue;
masterInput[54] = Regulator.idRegulator->wantedValue;
masterInput[55] = Regulator.iqRegulator->wantedValue;



    OCL_CHECK(err, cl::Buffer buffer_masterInput(context, CL_MEM_USE_HOST_PTR, 14*sizeof(float),masterInput,&err));
    OCL_CHECK(err, cl::Buffer buffer_masterOutput(context, CL_MEM_USE_HOST_PTR, 4*sizeof(float),masterOutput,&err));


    int narg = 0;

    OCL_CHECK(err, err = krnl_calculateCurVelModel.setArg(narg++, buffer_masterInput));
    OCL_CHECK(err, err = krnl_calculateCurVelModel.setArg(narg++, buffer_masterOutput));

    masterOutput[2] = 0;
    masterOutput[3] = 0;
    if(modeSelection == 1)
    {
        std::cout << "Selected mode: " << modeSelection << "\n";
        masterInput[8] = inputI1[0];
        masterInput[9] = inputI2[0];
        masterInput[10] = inputI3[0];
        masterInput[11] = inputMotorMechanicalAngularVelocity[0];
        masterInput[12] = masterOutput[2];
        masterInput[13] = masterOutput[3];

        // Data will be migrated to kernel space
        OCL_CHECK(err, err = q.enqueueMigrateMemObjects({buffer_masterInput, buffer_masterOutput}, 0 /* 0 means from host*/));
        // Launch the Kernel
        OCL_CHECK(err, err = q.enqueueTask(krnl_calculateCurVelModel));
    
        OCL_CHECK(err, q.enqueueMigrateMemObjects({buffer_masterOutput}, CL_MIGRATE_MEM_OBJECT_HOST));
        OCL_CHECK(err, q.finish());
        std::cout << "----------------------------------------\n\r";
        std::cout << "psi2Amplitude"<< " : " << masterOutput[0] << "\n";
        std::cout << "transformAngle : " << masterOutput[1] << "\n";
        std::cout << "----------------------------------------\n\r";
    }



   

    /*------------------------------------------------------------------------------------------------------------*/

  



    // // data output
    // std::ofstream modelCVOutputDataFile2;
    // modelCVOutputDataFile2.open("outputCurVel2.csv",std::ofstream::out | std::ofstream::trunc);
    // modelCVOutputDataFile2<< "time,|psi2|,transformAngle\n";
    // float timeCV = odeCVCalculationSettingsArray[0];


    //  for(int i = 0; i<odeCVCalculationSettingsArray[4];i++ )
    // {   
    //     timeCV = timeCV + odeCVCalculationSettingsArray[2];

    //     std::cout << "----------------------------------------\n\r";
    //     std:: cout << "results from kernel:\n";
    //     std::cout << "----------------------------------------\n\r";

    //     std::cout << "psi2Amplitude index "<< i << " : " << psi2AmplitudeOut[i] << "\n";
    //     std::cout << "transformAngle[" << i << "]: "<< transformAngleOut[i] << "\n";
    //     modelCVOutputDataFile2<<timeCV<<","<<psi2AmplitudeOut[i]<<","<<transformAngleOut[i]<<"\n";
        
    // }

   
    // modelCVOutputDataFile2.close();

    
 // data output
    std::ofstream modelCVOutputDataFile2;
    modelCVOutputDataFile2.open("outputCurVel2.csv",std::ofstream::out | std::ofstream::trunc);
    modelCVOutputDataFile2<< "time,|psi2|,transformAngle\n";
    float timeCV = odeCVCalculationSettingsArray[0];
    if(modeSelection == 0)
    {
        std::cout << "Selected mode: " << modeSelection << "\n";
        for(int i = 0;i<odeCVCalculationSettingsArray[4]-1;i++)
        {

            timeCV = timeCV + odeCVCalculationSettingsArray[2];
            masterInput[8] = inputI1[i];
            masterInput[9] = inputI2[i];
            masterInput[10] = inputI3[i];
            masterInput[11] = inputMotorMechanicalAngularVelocity[i];
            masterInput[12] = masterOutput[2];
            masterInput[13] = masterOutput[3];

            // Data will be migrated to kernel space
            OCL_CHECK(err, err = q.enqueueMigrateMemObjects({buffer_masterInput, buffer_masterOutput}, 0 /* 0 means from host*/));
            // Launch the Kernel
            OCL_CHECK(err, err = q.enqueueTask(krnl_calculateCurVelModel));
    
            OCL_CHECK(err, q.enqueueMigrateMemObjects({buffer_masterOutput}, CL_MIGRATE_MEM_OBJECT_HOST));
            OCL_CHECK(err, q.finish());
            std::cout << "----------------------------------------\n\r";
            std::cout << "psi2Amplitude"<< " : " << masterOutput[0] << "\n";
            std::cout << "transformAngle : " << masterOutput[1] << "\n";
            std::cout << "----------------------------------------\n\r";
            modelCVOutputDataFile2<<timeCV<<","<<masterOutput[0]<<","<<masterOutput[1]<<"\n";
    }
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
    free(psi2alpha);
    free(psi2beta);
    free(inputMotorMechanicalAngularVelocity);
    free(odeCVCalculationSettingsArray);
    // free(psi2AmplitudeOut);
    // free(transformAngleOut);
    free(masterInput);
    free(masterOutput);


    free(MotorModel.odeCalculationSettings);
    free(MotorModel.stateSpaceCoeff);
    free(MotorModel.motorParameters);
   
    free(MotorModel.modelVariables);
    free(svmCore.invertorSwitch);
    free(svmCore.triangleWaveSettings);
    free(Regulator.fluxRegulator);
    free(Regulator.velocityRegulator);
    free(Regulator.iqRegulator);
    free(Regulator.idRegulator);
    free(Invertor.reconstructedInvertorOutputVoltage);
    free(svmCore.coreInternalVariables);
}
