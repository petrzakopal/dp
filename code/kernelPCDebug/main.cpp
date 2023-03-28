/*******************************************************************************
Author: FEE CTU
Purpose: Host program
Comment: Framework for testing algorithms for kernel and host program.
*******************************************************************************/
/*----------------------------------------------------------------------------*/
/*-------------------------------- INCLUDES ---------------------------------*/
#include <iostream>
#include <stdlib.h>
#include <string>
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

#include <stdio.h>
#include <chrono>
#include <thread>
#include <mutex>
#include <future>
#include <cmath>


// constant definition
#define PI 3.141592



// function declaration, maybe move to another file
void krnl_calculateCurVelModel(float *masterInput, float *masterOutput );
void krnl_calculateOnlineInvertorAndMotor(float *masterInput, float *masterOutput);

// Define the function to be called when ctrl-c (SIGINT) is sent to process
void signal_callback_handler(int signum) {
   std::cout << "\nCaught signal " << signum << "\n";

    // create function for setting default values to switches and regulators

   // Terminate program
   exit(signum);
}
/*------------------------------------------------------------------------------------*/
/*-------------------------------- MAIN HOST PROGRAM ---------------------------------*/
int main(int argc, char* argv[]) {

    // Register signal and signal handler
   signal(SIGINT, signal_callback_handler);




    /*
    * @name The App
    * @brief Main CPU and FPGA application.
    */

    int modeSelection = 0;
    printf("Select mode:\n0 - preloaded data (disabled)\n1 - keyboard input data (disabled)\n2 - CPU/FPGA model\n");
    scanf("%i", &modeSelection);
    printf("You have selected: %i\n\r", modeSelection);


    // int masterInputLength = 39;
    // int masterOutputLength = 18;

    int masterInputLength = 4096;
    int masterOutputLength = 4096;

    float *masterInput;
    float *masterOutput;
    
    posix_memalign((void **)&masterInput , 4096 , (masterInputLength)*sizeof(float) );
    posix_memalign((void **)&masterOutput , 4096 , (masterOutputLength)*sizeof(float) );

    float *masterInputMotor;
    float *masterOutputMotor;

    posix_memalign((void **)&masterInputMotor , 4096 , (masterInputLength)*sizeof(float) );
    posix_memalign((void **)&masterOutputMotor , 4096 , (masterOutputLength)*sizeof(float) );

    /*-------------------------------------------------------------*/
    /*-------------------- CLASS DEFINITIONS ---------------------*/
    MotorModelClass MotorModel;
    svmCoreClass svmCore;
    CurVelModelClass CurVelModel;
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
    // float commonModeVoltage;
    // float compareLevel;
    // float trianglePoint;
    float minMaxCommonModeVoltageConstant = 287;
    float uS = 400;
    float uDC = ((3 * sqrt(2))/PI) * uS;
    float globalSimulationTime = globalInitialCalculationTime;
    /*-------------------------------------------------------------*/
    

    /****************************************************************/
    /*-------------------- MEMORY ALLOCATION ---------------------*/


    /*-------------------- ASYNCHRONOUS MOTOR MODEL ---------------------*/
    MotorModel.odeCalculationSettingsAllocateMemory();
    

    // needed for further memory allocation
    MotorModel.setOdeCalculationSettings(globalInitialCalculationTime, globalFinalCalculationTime, globalCalculationStep);

    MotorModel.motorParametersAllocateMemory();
    MotorModel.stateSpaceCoeffAllocateMemory();
    MotorModel.modelVariablesForOnlineCalculationAllocateMemory();
    // MotorModel.voltageGeneratorDataAllocateMemory();
    /*-------------------- END OF ASYNCHRONOUS MOTOR MODEL ---------------------*/

    /*-------------------- CURRENT VELOCITY MOTOR MODEL ---------------------*/
    CurVelModel.motorParametersAllocateMemory();
    CurVelModel.motorCoeffAllocateMemory();
    CurVelModel.modelCVVariablesAllocateMemory();
    CurVelModel.odeCVCalculationSettingsAllocateMemory();
    /*---------------- END OF CURRENT VELOCITY MOTOR MODEL -----------------*/


    /*-------------------- SPACE VECTOR MODULATION CORE ---------------------*/
    // svmCore.phaseWantedVoltageAllocateMemory(); // depracated
    svmCore.invertorSwitchAllocateMemory();
    svmCore.triangleWaveSettingsAllocateMemory();
    svmCore.coreInternalVariablesAllocateMemory();
    /*----------------------------------------------------------------------*/

    /*-------------------- REGULATORS ---------------------*/
    Regulator.regulatorAllocateMemory();
    /*---------------------------------------------------*/


    /*-------------------- INVERTOR ---------------------*/
    Invertor.reconstructedInvertorOutputVoltageAllocateMemory();

    /****************************************************************/


    /*----------------------------- SET ASYNCHRONOUS MOTOR PARAMETERS FOR MODEL ------------------------------*/
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
    MotorModel.setStateSpaceCoeff();
    MotorModel.odeCalculationSettings->numberOfIterations = MotorModel.numberOfIterations();

    // initial values of ASM model
    MotorModel.modelVariables->i1alpha = 0;
    MotorModel.modelVariables->i1beta = 0;
    MotorModel.modelVariables->psi2alpha = 0;
    MotorModel.modelVariables->psi2beta = 0;
    MotorModel.modelVariables->loadTorque = 0;
    MotorModel.modelVariables->motorMechanicalAngularVelocity = 0;
    MotorModel.modelVariables->motorTorque = 0;
    MotorModel.modelVariables->u1alpha = 0;
    MotorModel.modelVariables->u1beta = 0;

    /*------------------------------------------------------------------------------------------*/
    /*-------------------- CURRENT VELOCITY MODEL SETTINGS/ INITIAL VALUES ---------------------*/
    CurVelModel.modelCVVariables->psi2alpha = 0;
    CurVelModel.modelCVVariables->psi2beta = 0;
    CurVelModel.modelCVVariables->transformAngle = 0;
    CurVelModel.odeCVCalculationSettings->calculationStep = globalCalculationStep; // just a helper variable defined on top of this file to hava the same number of samples for ASM motor model and Current-Velocity model
    CurVelModel.motorParameters->R2 = 0.225f; // Ohm, rotor rezistance
    CurVelModel.motorParameters->Lm = 0.0825f; // H, main flux inductance
    CurVelModel.motorParameters->L2 = 0.08477f; // H, inductance
    // CurVelModel.motorParameters->nOfPolePairs = 2; // number of pole pairs
    CurVelModel.calculateMotorCVCoeff(CurVelModel.modelCVCoeff, CurVelModel.motorParameters);
    CurVelModel.modelCVCoeff->nOfPolePairs = 2;
    /*------------------------------------------------------------------------------------------*/

    /*------------------------------------------------------------------*/
    /*-------------------- TRIANGLE WAVE SETTINGS ---------------------*/
    svmCore.triangleWaveSettings->waveAmplitude = 1;
    svmCore.triangleWaveSettings->calculationStep = globalCalculationStep;
    svmCore.triangleWaveSettings->wavePeriod = 0.0001;
    svmCore.triangleWaveSettings->calculationTime = globalInitialCalculationTime;
    /*------------------------------------------------------------------*/

    /*--------------------------------------------------------------------*/
    /*-------------------- IVNVERTOR MODEL SETTINGS ---------------------*/
    float Udcmax = 311.87;
    svmCore.invertorSwitch->sw1 = 0;
    svmCore.invertorSwitch->sw2 = 0;
    svmCore.invertorSwitch->sw3 = 0;
    svmCore.invertorSwitch->sw4 = 0;
    svmCore.invertorSwitch->sw5 = 0;
    svmCore.invertorSwitch->sw6 = 0;
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
    Regulator.idRegulator->saturationOutputMin = 0; // - Udcmax
    Regulator.idRegulator->clampingStatus = false;
    Regulator.idRegulator->saturationCheckStatus = false;
    Regulator.idRegulator->signCheckStatus = false;
    Regulator.idRegulator->measuredValue = 0;
    /*------------------------------------------------------*/

    /*-------------------- IQ REGULATOR ---------------------*/
    Regulator.iqRegulator->ki = 2915.6; // 2915.6
    Regulator.iqRegulator->kp = 22.3961; // 22.3961
    Regulator.iqRegulator->saturationOutputMax = sqrtf((Udcmax * Udcmax) - (Regulator.idRegulator->saturationOutput * Regulator.idRegulator->saturationOutput)); // sqrt(Udcmax^2 - u1d^2) dynamically
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
    Regulator.velocityRegulator->wantedValue = 0;
    Regulator.idRegulator->wantedValue = 0;
    Regulator.iqRegulator->wantedValue = 0;
    /*--------------------------------------------------------------*/

   
    
    

    float inputI1 = 0;
    float inputI2 = 0;
    float inputI3 = 0;
    
    if(modeSelection == 2)
    {

        
    /*------------------------------------------------------------*/
    /*-------------------- PROGRAM SETTINGS ---------------------*/
    int verboseOutput = false;
    int numberOfIterations = 0;
    std::cout << "\n\r------------------------------------------\n";
    std::cout << "Select verbose output:\n0 - disabled\n1 - enabled\n";
    std::cout << "------------------------------------------\n";
    scanf("%i", &verboseOutput);
    // std::cout << "\n\r------------------------------------------\n";
    // std::cout << "Select numberOfIterations\n";
    // std::cout << "------------------------------------------\n";
    // scanf("%i", &numberOfIterations);
    /*------------------------------------------------------------*/
        /*------------------------------------------------------------------------*/
        /*-------------------- OUTPUT CSV DATA FILE CREATION ---------------------*/
        std::ofstream globalSimulationData;
        globalSimulationData.open("outputData/globalSimulationData.csv",std::ofstream::out | std::ofstream::trunc);
        /*-------------------------------------------------------------------*/


        std::chrono::time_point<std::chrono::system_clock> startTime;
        std::chrono::time_point<std::chrono::system_clock> endTime;
   

        /*
        * @brief Initial values settings
        *
        * */

        masterInput[0] = globalSimulationTime;
        masterInput[1] = globalCalculationStep;
        masterInput[2] = minMaxCommonModeVoltageConstant;
        masterInput[3] = globalCalculationStep/2;
        masterInput[4] = inputI1;
        masterInput[5] = inputI2;
        masterInput[6] = inputI3;
        masterInput[7] = MotorModel.modelVariables->motorMechanicalAngularVelocity;
        masterInput[8] = CurVelModel.modelCVCoeff->R2MLmDL2;
        masterInput[9] = CurVelModel.modelCVCoeff->R2DL2;
        masterInput[10] = CurVelModel.modelCVCoeff->nOfPolePairs;
        masterInput[11] = svmCore.triangleWaveSettings->waveAmplitude;
        masterInput[12] = svmCore.triangleWaveSettings->calculationStep;
        masterInput[13] = svmCore.triangleWaveSettings->wavePeriod;
        masterInput[14] = svmCore.triangleWaveSettings->calculationTime;
        masterInput[15] = Udcmax;
        masterInput[16] = Regulator.fluxRegulator->ki;
        masterInput[17] = Regulator.fluxRegulator->kp;
        masterInput[18] = Regulator.fluxRegulator->saturationOutputMax;
        masterInput[19] = Regulator.fluxRegulator->saturationOutputMin;
        masterInput[20] = Regulator.fluxRegulator->iSum;
        masterInput[21] = Regulator.velocityRegulator->ki;
        masterInput[22] = Regulator.velocityRegulator->kp;
        masterInput[23] = Regulator.velocityRegulator->saturationOutputMax;
        masterInput[24] = Regulator.velocityRegulator->saturationOutputMin;
        masterInput[25] = Regulator.velocityRegulator->iSum;
        masterInput[26] = Regulator.idRegulator->ki;
        masterInput[27] = Regulator.idRegulator->kp;
        masterInput[28] = Regulator.idRegulator->saturationOutput;
        masterInput[29] = Regulator.idRegulator->iSum;
        masterInput[30] = Regulator.iqRegulator->ki;
        masterInput[31] = Regulator.iqRegulator->kp;
        masterInput[32] = Regulator.iqRegulator->iSum;
        masterInput[33] = Regulator.fluxRegulator->wantedValue;
        masterInput[34] = Regulator.velocityRegulator->wantedValue;
        masterInput[35] = Regulator.idRegulator->wantedValue;
        masterInput[36] = Regulator.iqRegulator->wantedValue;
        masterInput[37] = CurVelModel.modelCVVariables->psi2alpha;
        masterInput[38] = CurVelModel.modelCVVariables->psi2beta;
        masterInput[39] = Regulator.idRegulator->saturationOutputMax;
        masterInput[40] = Regulator.idRegulator->saturationOutputMin;
        /***************************************************************/
        /*-------------------- MAIN PROGRAM LOOP ---------------------*/


        startTime = std::chrono::system_clock::now();

        for(int i = 0; i<1000000;i++) // replace with while in production, but in this model, it is suitable to use for cycle
        {

            
            /*-------------------- CONSOLE OUTPUT FOR TESTING PURPOSES BASED ON A USER SETTINGS ---------------------*/
            if(verboseOutput and (i<4))
            {
                std::cout << "iteration number: " << i << "\n";
            }
            /*-------------------------------------------------------------------------------------------------------*/
            if(i>=600000)
            {
                Regulator.velocityRegulator->wantedValue = 10;
            }
            masterInput[0] = globalSimulationTime;
            masterInput[1] = globalCalculationStep;
            masterInput[2] = minMaxCommonModeVoltageConstant;
            masterInput[3] = globalCalculationStep/2;
            masterInput[4] = inputI1;
            masterInput[5] = inputI2;
            masterInput[6] = inputI3;
            masterInput[7] = MotorModel.modelVariables->motorMechanicalAngularVelocity;
            masterInput[8] = CurVelModel.modelCVCoeff->R2MLmDL2;
            masterInput[9] = CurVelModel.modelCVCoeff->R2DL2;
            masterInput[10] = CurVelModel.modelCVCoeff->nOfPolePairs;
            masterInput[11] = svmCore.triangleWaveSettings->waveAmplitude;
            masterInput[12] = svmCore.triangleWaveSettings->calculationStep;
            masterInput[13] = svmCore.triangleWaveSettings->wavePeriod;
            masterInput[14] = svmCore.triangleWaveSettings->calculationTime;
            masterInput[15] = Udcmax;
            masterInput[16] = Regulator.fluxRegulator->ki;
            masterInput[17] = Regulator.fluxRegulator->kp;
            masterInput[18] = Regulator.fluxRegulator->saturationOutputMax;
            masterInput[19] = Regulator.fluxRegulator->saturationOutputMin;
            masterInput[20] = Regulator.fluxRegulator->iSum;
            masterInput[21] = Regulator.velocityRegulator->ki;
            masterInput[22] = Regulator.velocityRegulator->kp;
            masterInput[23] = Regulator.velocityRegulator->saturationOutputMax;
            masterInput[24] = Regulator.velocityRegulator->saturationOutputMin;
            masterInput[25] = Regulator.velocityRegulator->iSum;
            masterInput[26] = Regulator.idRegulator->ki;
            masterInput[27] = Regulator.idRegulator->kp;
            masterInput[28] = Regulator.idRegulator->saturationOutput;
            masterInput[29] = Regulator.idRegulator->iSum;
            masterInput[30] = Regulator.iqRegulator->ki;
            masterInput[31] = Regulator.iqRegulator->kp;
            masterInput[32] = Regulator.iqRegulator->iSum;
            masterInput[33] = Regulator.fluxRegulator->wantedValue;
            masterInput[34] = Regulator.velocityRegulator->wantedValue;
            masterInput[35] = Regulator.idRegulator->wantedValue;
            masterInput[36] = Regulator.iqRegulator->wantedValue;
            masterInput[37] = CurVelModel.modelCVVariables->psi2alpha;
            masterInput[38] = CurVelModel.modelCVVariables->psi2beta;
            masterInput[39] = Regulator.idRegulator->saturationOutputMax;
            masterInput[40] = Regulator.idRegulator->saturationOutputMin;




            //kernel
            krnl_calculateCurVelModel(masterInput, masterOutput);
            


            svmCore.invertorSwitch->sw1 = masterOutput[0];
            svmCore.invertorSwitch->sw2 = masterOutput[1];
            svmCore.invertorSwitch->sw3 = masterOutput[2];
            svmCore.invertorSwitch->sw4 = masterOutput[3];
            svmCore.invertorSwitch->sw5 = masterOutput[4];
            svmCore.invertorSwitch->sw6 = masterOutput[5];

            svmCore.triangleWaveSettings->calculationTime = masterOutput[6];
            Regulator.fluxRegulator->iSum = masterOutput[7];
            Regulator.velocityRegulator->iSum = masterOutput[8];
            Regulator.idRegulator->saturationOutput = masterOutput[9];
            Regulator.idRegulator->iSum = masterOutput[10];
            Regulator.iqRegulator->iSum = masterOutput[11];
            CurVelModel.modelCVVariables->psi2alpha = masterOutput[12];
            CurVelModel.modelCVVariables->psi2beta = masterOutput[13];



        // we got switches from kernel

            /*-------------------- CONSOLE OUTPUT FOR TESTING PURPOSES BASED ON A USER SETTINGS ---------------------*/
            if(verboseOutput and (i<4))
            {
                std::cout << "sw1: " << svmCore.invertorSwitch->sw1 << "\n";
                std::cout << "sw2: " << svmCore.invertorSwitch->sw2 << "\n";
                std::cout << "sw3: " << svmCore.invertorSwitch->sw3 << "\n";
                std::cout << "sw4: " << svmCore.invertorSwitch->sw4 << "\n";
                std::cout << "sw5: " << svmCore.invertorSwitch->sw5 << "\n";
                std::cout << "sw6: " << svmCore.invertorSwitch->sw6 << "\n";
        
            }
            /*------------------------------------------------------------------------------------------------------*/


            masterInputMotor[0] = svmCore.invertorSwitch->sw1;
            masterInputMotor[1] = svmCore.invertorSwitch->sw2;
            masterInputMotor[2] = svmCore.invertorSwitch->sw3;
            masterInputMotor[3] = svmCore.invertorSwitch->sw4;
            masterInputMotor[4] = svmCore.invertorSwitch->sw5;
            masterInputMotor[5] = svmCore.invertorSwitch->sw6;
            masterInputMotor[6] = uDC;
            masterInputMotor[7] = MotorModel.modelVariables->psi2alpha;
            masterInputMotor[8] = MotorModel.modelVariables->psi2beta;
            masterInputMotor[9] = MotorModel.modelVariables->i1alpha;
            masterInputMotor[10] = MotorModel.modelVariables->i1beta;
            masterInputMotor[11] = MotorModel.modelVariables->loadTorque;
            masterInputMotor[12] = MotorModel.modelVariables->motorMechanicalAngularVelocity;
            masterInputMotor[13] = globalCalculationStep;
            masterInputMotor[14] = MotorModel.motorParameters->Lm;
            masterInputMotor[15] = MotorModel.motorParameters->sigma;
            masterInputMotor[16] = MotorModel.motorParameters->L1;
            masterInputMotor[17] = MotorModel.motorParameters->L2;
            masterInputMotor[18] = MotorModel.motorParameters->nOfPolePairs;
            



            krnl_calculateOnlineInvertorAndMotor(masterInputMotor,  masterOutputMotor);

            /*-------------------- SIMULATED INVERTOR FOR SIMULATION WITH 3 PHASE CONTROLLED THYRISTOR 400 V ---------------------*/
            // invertor voltage reconstruction for phase A, B, C
            Invertor.invertorReconstructVoltages(svmCore.invertorSwitch, Invertor.reconstructedInvertorOutputVoltage, uDC);
            /*-------------------------------------------------------------------------------------------------------------------*/

            /*-------------------- CONSOLE OUTPUT FOR TESTING PURPOSES BASED ON A USER SETTINGS ---------------------*/
            if(verboseOutput and (i<4))
            {
                std::cout << "reconstructed u1a: " << Invertor.reconstructedInvertorOutputVoltage->u1a << "\n";
                std::cout << "reconstructed u1b: " << Invertor.reconstructedInvertorOutputVoltage->u1b << "\n";
                std::cout << "reconstructed u1c: " << Invertor.reconstructedInvertorOutputVoltage->u1c << "\n";
            }
            /*------------------------------------------------------------------------------------------------------*/




            /******************************************************************************/
            /*-------------------- VIRTUAL ASYNCHRONOUS MOTOR MODEL ---------------------*/
        
            


            /*---------------------------------------------------------------------------*/
            /*-------------------- CLARKE TRANSFORM FOR ASM MODEL ---------------------*/
            MotorModel.modelVariables->u1alpha = Transformation.clarkeTransform1(Invertor.reconstructedInvertorOutputVoltage->u1a, Invertor.reconstructedInvertorOutputVoltage->u1b, Invertor.reconstructedInvertorOutputVoltage->u1c, 0.6667);
            MotorModel.modelVariables->u1beta = Transformation.clarkeTransform2(Invertor.reconstructedInvertorOutputVoltage->u1a, Invertor.reconstructedInvertorOutputVoltage->u1b, Invertor.reconstructedInvertorOutputVoltage->u1c, 0.6667);
            /*---------------------------------------------------------------------------*/

            /*-------------------- CONSOLE OUTPUT FOR TESTING PURPOSES BASED ON A USER SETTINGS ---------------------*/
            if(verboseOutput and (i<4))
                {
                    std::cout << "u1alpha to motor: " << MotorModel.modelVariables->u1alpha << "\n";
                    std::cout << "u1beta to motor: " << MotorModel.modelVariables->u1beta << "\n";
                }
            /*-----------------------------------------------------------------------------------------------------*/
        
            /*-----------------------------------------------------------------------------------*/
            /*-------------------- MAIN ASM MODEL CODE WITH RK4 ODE SOLVING ---------------------*/
            MotorModel.mathModelCalculateOnlineValue(MotorModel.odeCalculationSettings, MotorModel.modelVariables, MotorModel.stateSpaceCoeff, MotorModel.motorParameters);
            /*-----------------------------------------------------------------------------------*/
            /******************************************************************************/

            /*-------------------- CONSOLE OUTPUT FOR TESTING PURPOSES BASED ON A USER SETTINGS ---------------------*/
            if(verboseOutput and (i<4))
            {
                // std::cout << "ASM i1alpha: " << MotorModel.modelVariables->i1alpha << "\n";
                // std::cout << "ASM i1beta: " << MotorModel.modelVariables->i1beta << "\n";
                // std::cout << "ASM motorTorque: " << MotorModel.modelVariables->motorTorque<< "\n";
                // std::cout << "ASM motorMechanicalAngularVelocity: " << MotorModel.modelVariables->motorMechanicalAngularVelocity << "\n";
                // std::cout << "ASM psi2alpha: " << MotorModel.modelVariables->psi2alpha << "\n";
                // std::cout << "ASM psi2beta: " << MotorModel.modelVariables->psi2beta << "\n";
            }
            /*-----------------------------------------------------------------------------------------------------*/

            inputI1 = Transformation.inverseClarkeTransform1(MotorModel.modelVariables->i1alpha, MotorModel.modelVariables->i1beta);
            inputI2 = Transformation.inverseClarkeTransform2(MotorModel.modelVariables->i1alpha, MotorModel.modelVariables->i1beta);
            inputI3 = Transformation.inverseClarkeTransform3(MotorModel.modelVariables->i1alpha, MotorModel.modelVariables->i1beta);

            /*-------------------- CONSOLE OUTPUT FOR TESTING PURPOSES BASED ON A USER SETTINGS ---------------------*/
            if(verboseOutput and (i<4))
            {

            std::cout << "sw1: " << masterOutput[0] << "\n";
            std::cout << "sw2: " << masterOutput[1] << "\n";
            std::cout << "sw3: " << masterOutput[2] << "\n";
            std::cout << "sw4: " << masterOutput[3] << "\n";
            std::cout << "sw5: " << masterOutput[4] << "\n";
            std::cout << "sw6: " << masterOutput[5] << "\n";
            std::cout << "triangleWaveSettings.calculationTime: " << masterOutput[6] << "\n";
            std::cout << "fluxRegulator.iSum: " << masterOutput[7] << "\n";
            std::cout << "velocityRegulator.iSum: " << masterOutput[8] << "\n";
            std::cout << "idRegulator.saturationOutput: " << masterOutput[9] << "\n";
            std::cout << "idRegulator.iSum: " << masterOutput[10] << "\n";
            std::cout << "iqRegulator.iSum: " << masterOutput[11] << "\n";
            std::cout << "psi2alpha: " << masterOutput[12] << "\n";
            std::cout << "psi2beta: " << masterOutput[13] << "\n";
            std::cout << "psi2amplitude: " << masterOutput[14] << "\n";
            std::cout << "idRegulator.measuredValue: " << masterOutput[15] << "\n";
            std::cout << "idRegulator.wantedValue: " << masterOutput[16] << "\n";
            std::cout << "transformationAngle: " << masterOutput[17] << "\n";
            
                
            }
            /*--------------------------------------------------------------------------------------------------------*/

            /*--------------------------------------------------------------------------------------------------------------------------------------------------*/

            /****************************************************************************************************************************************************/


            /*-------------------------------------------------------------------------------*/
            /*-------------------- OUTPUT CSV DATA INSERTING TO A FILE ---------------------*/
            globalSimulationData << globalSimulationTime << "," << masterOutput[14] << ","  << MotorModel.modelVariables->motorMechanicalAngularVelocity << "," <<masterOutput[15] << "," << masterOutput[16]<<"," <<Regulator.velocityRegulator->wantedValue << ","<< masterOutput[18] <<"," << masterOutput[19]<< "," << Regulator.velocityRegulator->iSum << "\n";
            /*-------------------------------------------------------------------------------*/

            /*----------------------------------------------------------------------------------------------------------------------*/
            /*-------------------- UPDATING SIMULATION TIME - ONLY FOR SIMULATION PURPOSES TO OUTPUT DATA CSV ---------------------*/
            globalSimulationTime = globalSimulationTime + globalCalculationStep;
            /*----------------------------------------------------------------------------------------------------------------------*/

            /*-------------------- CONSOLE OUTPUT FOR TESTING PURPOSES BASED ON A USER SETTINGS ---------------------*/
            if(verboseOutput and (i<4))
            {
                std::cout << "------------------------------------------------------"<< "\n";
            }
            /*-------------------------------------------------------------------------------------------------------*/

            /*********************************************************************************/
        }
        endTime = std::chrono::system_clock::now();
        /*-------------------- OUTPUT CSV DATA FILE CLOSING ---------------------*/
        globalSimulationData.close();
        /*----------------------------------------------------------------------*/
        std::chrono::duration<double> diffTime = endTime - startTime;
        std::cout << "Time for the execution is: " << diffTime.count() << "\n";

    }


    if(modeSelection == 1)
    {

         printf("Keyboard input data mode\n\r");
        printf("------------------------------------\n\r");
        printf("Insert data divided by {space symbol}\n\r");
        printf("I1 I2 I3 MechanicalAngularVelocity psi2alpha[0] psi2beta[0] \n\r");

    

    scanf("%f %f %f %f %f %f", &inputI1, &inputI2, &inputI3, &MotorModel.modelVariables->motorMechanicalAngularVelocity, &CurVelModel.modelCVVariables->psi2alpha, &CurVelModel.modelCVVariables->psi2beta);
    printf("------------------------------------\n\r");
    printf("You have entered:\n\r");
    printf("I1 = %f\n\rI2 = %f\n\rI3 = %f\n\rMechanicalAngularVelocity = %f\n\rpsi2alpha[0] = %f\n\rpsi2beta = %f\n\r", inputI1, inputI2, inputI3, MotorModel.modelVariables->motorMechanicalAngularVelocity, CurVelModel.modelCVVariables->psi2alpha, CurVelModel.modelCVVariables->psi2beta);
    printf("------------------------------------\n\r");

        masterInput[0] = globalSimulationTime;
        masterInput[1] = globalCalculationStep;
        masterInput[2] = minMaxCommonModeVoltageConstant;
        masterInput[3] = globalCalculationStep/2;
        masterInput[4] = inputI1;
        masterInput[5] = inputI2;
        masterInput[6] = inputI3;
        masterInput[7] = MotorModel.modelVariables->motorMechanicalAngularVelocity;
        masterInput[8] = CurVelModel.modelCVCoeff->R2MLmDL2;
        masterInput[9] = CurVelModel.modelCVCoeff->R2DL2;
        masterInput[10] = CurVelModel.modelCVCoeff->nOfPolePairs;
        masterInput[11] = svmCore.triangleWaveSettings->waveAmplitude;
        masterInput[12] = svmCore.triangleWaveSettings->calculationStep;
        masterInput[13] = svmCore.triangleWaveSettings->wavePeriod;
        masterInput[14] = svmCore.triangleWaveSettings->calculationTime;
        masterInput[15] = Udcmax;
        masterInput[16] = Regulator.fluxRegulator->ki;
        masterInput[17] = Regulator.fluxRegulator->kp;
        masterInput[18] = Regulator.fluxRegulator->saturationOutputMax;
        masterInput[19] = Regulator.fluxRegulator->saturationOutputMin;
        masterInput[20] = Regulator.fluxRegulator->iSum;
        masterInput[21] = Regulator.velocityRegulator->ki;
        masterInput[22] = Regulator.velocityRegulator->kp;
        masterInput[23] = Regulator.velocityRegulator->saturationOutputMax;
        masterInput[24] = Regulator.velocityRegulator->saturationOutputMin;
        masterInput[25] = Regulator.velocityRegulator->iSum;
        masterInput[26] = Regulator.idRegulator->ki;
        masterInput[27] = Regulator.idRegulator->kp;
        masterInput[28] = Regulator.idRegulator->saturationOutput;
        masterInput[29] = Regulator.idRegulator->iSum;
        masterInput[30] = Regulator.iqRegulator->ki;
        masterInput[31] = Regulator.iqRegulator->kp;
        masterInput[32] = Regulator.iqRegulator->iSum;
        masterInput[33] = Regulator.fluxRegulator->wantedValue;
        masterInput[34] = Regulator.velocityRegulator->wantedValue;
        masterInput[35] = Regulator.idRegulator->wantedValue;
        masterInput[36] = Regulator.iqRegulator->wantedValue;
        masterInput[37] = CurVelModel.modelCVVariables->psi2alpha;
        masterInput[38] = CurVelModel.modelCVVariables->psi2beta;


        masterInput[0] = globalSimulationTime;
        masterInput[4] = inputI1;
        masterInput[5] = inputI2;
        masterInput[6] = inputI3;
        masterInput[7] = MotorModel.modelVariables->motorMechanicalAngularVelocity;
        masterInput[14] = svmCore.triangleWaveSettings->calculationTime;
        masterInput[20] = Regulator.fluxRegulator->iSum;
        masterInput[25] = Regulator.velocityRegulator->iSum;
        masterInput[28] = Regulator.idRegulator->saturationOutput;
        masterInput[29] = Regulator.idRegulator->iSum;
        masterInput[32] = Regulator.iqRegulator->iSum;
        masterInput[37] = CurVelModel.modelCVVariables->psi2alpha;
        masterInput[38] = CurVelModel.modelCVVariables->psi2beta;

        
        for(int i = 0;i<10;i++)
        {
            
            std::cout << "\n\n" << i <<" round\n/************************************/\n";
           krnl_calculateCurVelModel(masterInput, masterOutput);

            
            
            
            std::cout << "sw1: " << masterOutput[0] << "\n";
            std::cout << "sw2: " << masterOutput[1] << "\n";
            std::cout << "sw3: " << masterOutput[2] << "\n";
            std::cout << "sw4: " << masterOutput[3] << "\n";
            std::cout << "sw5: " << masterOutput[4] << "\n";
            std::cout << "sw6: " << masterOutput[5] << "\n";
            std::cout << "triangleWaveSettings.calculationTime: " << masterOutput[6] << "\n";
            std::cout << "fluxRegulator.iSum: " << masterOutput[7] << "\n";
            std::cout << "velocityRegulator.iSum: " << masterOutput[8] << "\n";
            std::cout << "idRegulator.saturationOutput: " << masterOutput[9] << "\n";
            std::cout << "idRegulator.iSum: " << masterOutput[10] << "\n";
            std::cout << "iqRegulator.iSum: " << masterOutput[11] << "\n";
            std::cout << "psi2alpha: " << masterOutput[12] << "\n";
            std::cout << "psi2beta: " << masterOutput[13] << "\n";
            std::cout << "psi2amplitude: " << masterOutput[14] << "\n";
            std::cout << "idRegulator.measuredValue: " << masterOutput[15] << "\n";
            std::cout << "idRegulator.wantedValue: " << masterOutput[16] << "\n";
            std::cout << "transformationAngle: " << masterOutput[17] << "\n";


            masterInput[0] = globalSimulationTime;
        masterInput[1] = globalCalculationStep;
        masterInput[2] = minMaxCommonModeVoltageConstant;
        masterInput[3] = globalCalculationStep/2;
        masterInput[4] = inputI1;
        masterInput[5] = inputI2;
        masterInput[6] = inputI3;
        masterInput[7] = MotorModel.modelVariables->motorMechanicalAngularVelocity;
        masterInput[8] = CurVelModel.modelCVCoeff->R2MLmDL2;
        masterInput[9] = CurVelModel.modelCVCoeff->R2DL2;
        masterInput[10] = CurVelModel.modelCVCoeff->nOfPolePairs;
        masterInput[11] = svmCore.triangleWaveSettings->waveAmplitude;
        masterInput[12] = svmCore.triangleWaveSettings->calculationStep;
        masterInput[13] = svmCore.triangleWaveSettings->wavePeriod;
        masterInput[14] = svmCore.triangleWaveSettings->calculationTime;
        masterInput[15] = Udcmax;
        masterInput[16] = Regulator.fluxRegulator->ki;
        masterInput[17] = Regulator.fluxRegulator->kp;
        masterInput[18] = Regulator.fluxRegulator->saturationOutputMax;
        masterInput[19] = Regulator.fluxRegulator->saturationOutputMin;
        masterInput[20] = Regulator.fluxRegulator->iSum;
        masterInput[21] = Regulator.velocityRegulator->ki;
        masterInput[22] = Regulator.velocityRegulator->kp;
        masterInput[23] = Regulator.velocityRegulator->saturationOutputMax;
        masterInput[24] = Regulator.velocityRegulator->saturationOutputMin;
        masterInput[25] = Regulator.velocityRegulator->iSum;
        masterInput[26] = Regulator.idRegulator->ki;
        masterInput[27] = Regulator.idRegulator->kp;
        masterInput[28] = Regulator.idRegulator->saturationOutput;
        masterInput[29] = Regulator.idRegulator->iSum;
        masterInput[30] = Regulator.iqRegulator->ki;
        masterInput[31] = Regulator.iqRegulator->kp;
        masterInput[32] = Regulator.iqRegulator->iSum;
        masterInput[33] = Regulator.fluxRegulator->wantedValue;
        masterInput[34] = Regulator.velocityRegulator->wantedValue;
        masterInput[35] = Regulator.idRegulator->wantedValue;
        masterInput[36] = Regulator.iqRegulator->wantedValue;
        masterInput[37] = CurVelModel.modelCVVariables->psi2alpha;
        masterInput[38] = CurVelModel.modelCVVariables->psi2beta;


        masterInput[0] = globalSimulationTime;
        masterInput[4] = inputI1;
        masterInput[5] = inputI2;
        masterInput[6] = inputI3;
        masterInput[7] = MotorModel.modelVariables->motorMechanicalAngularVelocity;
        masterInput[14] = svmCore.triangleWaveSettings->calculationTime;
        masterInput[20] = Regulator.fluxRegulator->iSum;
        masterInput[25] = Regulator.velocityRegulator->iSum;
        masterInput[28] = Regulator.idRegulator->saturationOutput;
        masterInput[29] = Regulator.idRegulator->iSum;
        masterInput[32] = Regulator.iqRegulator->iSum;
        masterInput[37] = CurVelModel.modelCVVariables->psi2alpha;
        masterInput[38] = CurVelModel.modelCVVariables->psi2beta;
        }
        

            
    }
    
    std::cout << "the end of the most useful program is here\n";

    /*-----------------------------------------------------------*/
    /*-------------------- MEMORY FREEING ---------------------*/
    free(MotorModel.odeCalculationSettings);
    free(MotorModel.stateSpaceCoeff);
    free(MotorModel.motorParameters);
    free(MotorModel.modelVariables);
    free(CurVelModel.motorParameters);
    free(CurVelModel.modelCVCoeff);
    free(CurVelModel.modelCVVariables);
    free(CurVelModel.odeCVCalculationSettings);
    free(svmCore.invertorSwitch);
    free(svmCore.triangleWaveSettings);
    free(Regulator.fluxRegulator);
    free(Regulator.velocityRegulator);
    free(Regulator.iqRegulator);
    free(Regulator.idRegulator);
    free(Invertor.reconstructedInvertorOutputVoltage);
    free(svmCore.coreInternalVariables);
    free(masterInput);
    free(masterOutput);

    /*-----------------------------------------------------------*/


}

// musí být chyba v algoritmu velocity regulatoru