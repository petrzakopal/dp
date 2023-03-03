// @file testCalculationLoop.cpp
// compilation command: gcc -std=c++14 testCalculationLoop.cpp -o run/testCalculationLoop -lstdc++ ./../function/svmCore.cpp ./../function/MotorModel.cpp ./../function/transformation.cpp ./../function/CurVelModel.cpp ./../function/regulator.cpp ./../function/invertor.cpp


#include "./../header/MotorModel.h"
#include "./../header/transformation.h"
#include "./../header/svmCore.h"
#include "./../header/CurVelModel.h"
#include "./../header/regulator.h"
#include "./../header/invertor.h"
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <fstream>

#define PI 3.141592

int main()
{
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
    float commonModeVoltage;
    float compareLevel;
    float trianglePoint;
    float minMaxCommonModeVoltageConstant = 287;
    float uS = 400;
    float uDC = ((3 * sqrt(2))/PI) * uS;
    /*-------------------------------------------------------------*/
    

    /****************************************************************/
    /*-------------------- MEMORY ALLOCATION ---------------------*/


    /*-------------------- ASYNCHRONOUS MOTOR MODEL ---------------------*/
    MotorModel.odeCalculationSettingsAllocateMemory();
    

    // needed for further memory allocation
    MotorModel.setOdeCalculationSettings(globalInitialCalculationTime, globalFinalCalculationTime, 1);

    MotorModel.motorParametersAllocateMemory();
    MotorModel.stateSpaceCoeffAllocateMemory();
    MotorModel.modelVariablesAllocateMemory();
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
     Invertor.reconstrutedInvertorOutputVoltageAllocateMemory();

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
    /*--------------------------------------------------------------------------------------------------------*/

    /*------------------------------------------------------------------------------------------*/
    /*-------------------- CURRENT VELOCITY MODEL SETTINGS/ INITIAL VALUES ---------------------*/
    CurVelModel.modelCVVariables->psi2alpha = 0;
    CurVelModel.modelCVVariables->psi2beta = 0;
    CurVelModel.modelCVVariables->transformAngle = 0;
    /*------------------------------------------------------------------------------------------*/

    /*------------------------------------------------------------------*/
    /*-------------------- TRIANGLE WAVE SETTINGS ---------------------*/
    svmCore.triangleWaveSettings->waveAmplitude = 1;
    svmCore.triangleWaveSettings->calculationStep = globalCalculationStep;
    svmCore.triangleWaveSettings->wavePeriod = 0.0001;
    svmCore.triangleWaveSettings->calculationTime = globalInitialCalculationTime;
    /*------------------------------------------------------------------*/


    /*--------------------------------------------------------------*/
    /*-------------------- REGULATOR SETTINGS ---------------------*/
    float Udcmax = 311.87;

    // flux regulator
    Regulator.fluxRegulator->ki = 30300;
    Regulator.fluxRegulator->kp = 11410;
    Regulator.fluxRegulator->kAntiWindUp = 0;
    Regulator.fluxRegulator->saturationOutputMax = 10.9482;
    Regulator.fluxRegulator->saturationOutputMin = 0;
    Regulator.fluxRegulator->saturationOutput = 0;

    // velocity regulator
    Regulator.velocityRegulator->ki = 4650000;
    Regulator.velocityRegulator->kp = 37500;
    Regulator.velocityRegulator->kAntiWindUp = 0;
    Regulator.velocityRegulator->saturationOutputMax = 29.1228;
    Regulator.velocityRegulator->saturationOutputMin = 0;
    Regulator.velocityRegulator->saturationOutput = 0;

    // id regulator
    Regulator.idRegulator->ki = 2915.6;
    Regulator.idRegulator->kp = 22.3961;
    Regulator.idRegulator->kAntiWindUp = 0;
    Regulator.idRegulator->saturationOutputMax = Udcmax; // (3*sqrt(2))/(pi*sqrt(3))*Us = 400(3*sqrt(2))/(3.141592*sqrt(3)) = 311.87
    Regulator.idRegulator->saturationOutput = - Udcmax;

    // iq regulator
    Regulator.iqRegulator->saturationOutputMin = -Udcmax;
    Regulator.iqRegulator->ki = 2915.6;
    Regulator.iqRegulator->kp = 22.3961;
    Regulator.iqRegulator->kAntiWindUp = 0;
    Regulator.iqRegulator->saturationOutputMax = sqrt((Udcmax * Udcmax) - (Regulator.idRegulator->saturationOutput * Regulator.idRegulator->saturationOutput)); // sqrt(Udcmax^2 - u1d^2) dynamically
    Regulator.iqRegulator->saturationOutputMin = - Regulator.iqRegulator->saturationOutputMax;
    /*--------------------------------------------------------------*/

    /*--------------------------------------------------------------*/
    /*-------------------- WANTED VALUES INPUT ---------------------*/

    // now hardcoded, change later
    Regulator.fluxRegulator->wantedValue = 0.9;
    Regulator.velocityRegulator->wantedValue = 100;
    /*--------------------------------------------------------------*/

    Regulator.fluxRegulator->measuredValue = 0;
    Regulator.velocityRegulator->measuredValue = 0;
    Regulator.idRegulator->measuredValue = 0;
    Regulator.iqRegulator->measuredValue = 0;


    for(int i = 0; i<1;i++)
    {
        
        // calculating values from regulators
        Regulator.regCalculate(Regulator.fluxRegulator);
        Regulator.regCalculate(Regulator.velocityRegulator);

        Regulator.idRegulator->wantedValue = Regulator.fluxRegulator->saturationOutput;
        Regulator.iqRegulator->wantedValue = Regulator.velocityRegulator->saturationOutput;

        Regulator.regCalculate(Regulator.idRegulator);
        Regulator.regCalculate(Regulator.velocityRegulator);

        // here or at the end update dynamic saturation for iq regulator


         /* console output for testing purposes */
        /*--------------------------------------------------------------*/
        std::cout << "flux regulator output value: " << Regulator.fluxRegulator->saturationOutput << "\n";
        std::cout << "velocity regulator output value: " << Regulator.velocityRegulator->saturationOutput << "\n";
        std::cout << "id regulator output value: " << Regulator.idRegulator->saturationOutput << "\n";
        std::cout << "iq regulator output value: " << Regulator.iqRegulator->saturationOutput << "\n";
        /*--------------------------------------------------------------*/

        // this is not necessary, but for cleaner code it is good, but change maybe because of SoC constrains

        svmCore.coreInternalVariables->u1d = Regulator.idRegulator->saturationOutput;
        svmCore.coreInternalVariables->u1q = Regulator.iqRegulator->saturationOutput;


        // decoupling is missing now


        // inverse Park
        svmCore.coreInternalVariables->u1alpha = Transformation.inverseParkTransform1(svmCore.coreInternalVariables->u1d, svmCore.coreInternalVariables->u1q,CurVelModel.modelCVVariables->transformAngle);
        svmCore.coreInternalVariables->u1beta = Transformation.inverseParkTransform2(svmCore.coreInternalVariables->u1d, svmCore.coreInternalVariables->u1q,CurVelModel.modelCVVariables->transformAngle);


        // inverse Clark
        svmCore.coreInternalVariables->u1a = Transformation.inverseClarkeTransform1(svmCore.coreInternalVariables->u1alpha, svmCore.coreInternalVariables->u1beta);
        svmCore.coreInternalVariables->u1b = Transformation.inverseClarkeTransform2(svmCore.coreInternalVariables->u1alpha, svmCore.coreInternalVariables->u1beta);
        svmCore.coreInternalVariables->u1c = Transformation.inverseClarkeTransform3(svmCore.coreInternalVariables->u1alpha, svmCore.coreInternalVariables->u1beta);

        /* console output for testing purposes */
        /*--------------------------------------------------------------*/
        std::cout << "u1a: " << svmCore.coreInternalVariables->u1a << "\n";
        std::cout << "u1b: " << svmCore.coreInternalVariables->u1b << "\n";
        std::cout << "u1c: " << svmCore.coreInternalVariables->u1c << "\n";
         /*--------------------------------------------------------------*/

        trianglePoint = svmCore.generateActualValueTriangleWave(svmCore.triangleWaveSettings);

        commonModeVoltage = svmCore.minMaxCommonModeVoltage(svmCore.coreInternalVariables);


        // phase 1 sw1
        svmCore.invertorSwitch->sw1 = svmCore.comparationLevelTriangleWaveComparation(svmCore.createCompareLevel(minMaxCommonModeVoltageConstant, commonModeVoltage, svmCore.coreInternalVariables->u1a), trianglePoint);

        // phase 2 sw3
        svmCore.invertorSwitch->sw3 = svmCore.comparationLevelTriangleWaveComparation(svmCore.createCompareLevel(minMaxCommonModeVoltageConstant, commonModeVoltage, svmCore.coreInternalVariables->u1b), trianglePoint);

        // phase 3 sw5
        svmCore.invertorSwitch->sw5 = svmCore.comparationLevelTriangleWaveComparation(svmCore.createCompareLevel(minMaxCommonModeVoltageConstant, commonModeVoltage, svmCore.coreInternalVariables->u1c), trianglePoint);


        // not needed in simualation
        svmCore.invertorSwitch->sw4 = ~svmCore.invertorSwitch->sw1;
        svmCore.invertorSwitch->sw6 = ~svmCore.invertorSwitch->sw3;
        svmCore.invertorSwitch->sw2 = ~svmCore.invertorSwitch->sw5;



         /* console output for testing purposes */
        /*--------------------------------------------------------------*/
        std::cout << "sw1: " << svmCore.invertorSwitch->sw1 << "\n";
        std::cout << "sw2: " << svmCore.invertorSwitch->sw2 << "\n";
        std::cout << "sw3: " << svmCore.invertorSwitch->sw3 << "\n";
        std::cout << "sw4: " << svmCore.invertorSwitch->sw4 << "\n";
        std::cout << "sw5: " << svmCore.invertorSwitch->sw5 << "\n";
        std::cout << "sw6: " << svmCore.invertorSwitch->sw6 << "\n";
        /*--------------------------------------------------------------*/

        // invertor voltage reconstruction for phase A, B, C
        Invertor.invertorReconstructVoltages(svmCore.invertorSwitch, Invertor.reconstructedInvertorOutputVoltage, uDC);

         /* console output for testing purposes */
        /*--------------------------------------------------------------*/
        std::cout << "reconstructed u1a: " << Invertor.reconstructedInvertorOutputVoltage->u1a << "\n";
        std::cout << "reconstructed u1b: " << Invertor.reconstructedInvertorOutputVoltage->u1b << "\n";
        std::cout << "reconstructed u1c: " << Invertor.reconstructedInvertorOutputVoltage->u1c << "\n";
        /*--------------------------------------------------------------*/


        // clarke for motor model



    }


    /*-----------------------------------------------------------*/
    /*-------------------- MEMORY FREEING ---------------------*/
    free(MotorModel.odeCalculationSettings);
    free(MotorModel.stateSpaceCoeff);
    free(MotorModel.motorParameters);
    // free(MotorModel.voltageGeneratorData);
    free(MotorModel.modelVariables);
    free(CurVelModel.motorParameters);
    free(CurVelModel.modelCVCoeff);
    free(CurVelModel.modelCVVariables);
    free(CurVelModel.odeCVCalculationSettings);
    // free(svmCore.phaseWantedVoltage); // deprecated
    free(svmCore.invertorSwitch);
    free(svmCore.triangleWaveSettings);
    free(Regulator.fluxRegulator);
    free(Regulator.velocityRegulator);
    free(Regulator.iqRegulator);
    free(Regulator.idRegulator);
    free(Invertor.reconstructedInvertorOutputVoltage);
    free(svmCore.coreInternalVariables);
    /*-----------------------------------------------------------*/
    
}