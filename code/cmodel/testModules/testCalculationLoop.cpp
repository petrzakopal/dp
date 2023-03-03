// @file testCalculationLoop.cpp
// compilation command: gcc -std=c++14 testCalculationLoop.cpp -o run/testCalculationLoop -lstdc++ ./../function/svmCore.cpp ./../function/MotorModel.cpp ./../function/transformation.cpp ./../function/CurVelModel.cpp ./../function/regulator.cpp


#include "./../header/MotorModel.h"
#include "./../header/transformation.h"
#include "./../header/svmCore.h"
#include "./../header/CurVelModel.h"
#include "./../header/regulator.h"
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <fstream>

int main()
{
    /*-------------------------------------------------------------*/
    /*-------------------- CLASS DEFINITIONS ---------------------*/
    MotorModelClass MotorModel;
    svmCoreClass svmCore;
    CurVelModelClass CurVelModel;
    RegulatorClass Regulator;
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
    svmCore.phaseWantedVoltageAllocateMemory();
    svmCore.invertorSwitchAllocateMemory();
    svmCore.triangleWaveSettingsAllocateMemory();
    /*----------------------------------------------------------------------*/

    /*-------------------- REGULATORS ---------------------*/
    Regulator.regulatorAllocateMemory();
    /*---------------------------------------------------*/
    /****************************************************************/




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
    Regulator.idRegulator->saturationOutput = 0;

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
    Regulator.fluxRegulator->wanted = 0.9;
    Regulator.velocityRegulator->wanted = 100;
    /*--------------------------------------------------------------*/

    Regulator.fluxRegulator->measured = 0;
    Regulator.velocityRegulator->measured = 0;
    Regulator.idRegulator->measured = 0;
    Regulator.iqRegulator->measured = 0;


    for(int i = 0; i<1;i++)
    {
        
        Regulator.regCalculate(Regulator.fluxRegulator);
        Regulator.regCalculate(Regulator.velocityRegulator);
        std::cout << "flux regulator output value: " << Regulator.fluxRegulator->saturationOutput << "\n";
        std::cout << "velocity regulator output value: " << Regulator.velocityRegulator->saturationOutput << "\n";

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
    free(svmCore.phaseWantedVoltage);
    free(svmCore.invertorSwitch);
    free(svmCore.triangleWaveSettings);
    free(Regulator.fluxRegulator);
    free(Regulator.velocityRegulator);
    free(Regulator.iqRegulator);
    free(Regulator.idRegulator);
    /*-----------------------------------------------------------*/
    
}