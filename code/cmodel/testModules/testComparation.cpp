// @file testComparation.cpp
// compilation command: gcc -std=c++14 testComparation.cpp -o run/testComparation -lstdc++ ./../function/svmCore.cpp

#include "./../header/MotorModel.h"
#include "./../header/svmCore.h"
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <fstream>



int main()
{

    svmCoreClass svmCore;
    svmCore.phaseWantedVoltageAllocateMemory();
    svmCore.invertorSwitchAllocateMemory();
    svmCore.triangleWaveSettingsAllocateMemory();


    // svmCore.phaseWantedVoltage->u1a = 230;
    // svmCore.phaseWantedVoltage->u1b = - 153;
    // svmCore.phaseWantedVoltage->u1c = -153;


    svmCore.triangleWaveSettings->waveAmplitude = 1;
    svmCore.triangleWaveSettings->calculationStep = 0.000001;
    svmCore.triangleWaveSettings->wavePeriod = 0.0001;
    svmCore.triangleWaveSettings->calculationTime = 0;
    float globalFinalCalculationTime = 1;

    MotorModelClass MotorModel;

   
    

    MotorModel.odeCalculationSettingsAllocateMemory();
    MotorModel.setOdeCalculationSettings(svmCore.triangleWaveSettings->calculationTime, globalFinalCalculationTime, svmCore.triangleWaveSettings->calculationStep);

    MotorModel.voltageGeneratorDataAllocateMemory();
    MotorModel.voltageGeneratorData->voltageFrequency = 50;
    MotorModel.voltageGeneratorData->voltageAmplitude = 325.26;


    MotorModel.precalculateVoltageSource(MotorModel.odeCalculationSettings, MotorModel.voltageGeneratorData->voltageAmplitude, MotorModel.voltageGeneratorData->voltageFrequency);

    // for(int i = 0;i<250; i++)
    // {
    //     std::cout << "Motor voltage: " << MotorModel.getVoltage(i)->u1 << "\n";
    // }

    // float commonModeVoltage = svmCore.minMaxCommonModeVoltage(svmCore.phaseWantedVoltage);
    float commonModeVoltage;
    float compareLevel;
    float trianglePoint;

    


    // for testing

    // file output streaming
    std::ofstream triangleWaveData;
    std::ofstream switchCompareData;
    triangleWaveData.open ("outputData/triangleWaveData.csv",std::ofstream::out | std::ofstream::trunc);
     switchCompareData.open ("outputData/switchCompareData.csv",std::ofstream::out | std::ofstream::trunc);

    // creating just one period now, in normal program there would be reset of svmCore.triangleWaveSettings->calculationTime and not sawing data
    int maxIterations = ((int)ceil(((1 - 0)/svmCore.triangleWaveSettings->calculationStep)));

    for(int i = 0; i<maxIterations;i++) // when implemented for testing
    // while(true) // this would be main while backgrund program
    {
        

        trianglePoint = svmCore.generateActualValueTriangleWave(svmCore.triangleWaveSettings);


        svmCore.phaseWantedVoltage->u1a = MotorModel.getVoltage(i)->u1;
        svmCore.phaseWantedVoltage->u1b = MotorModel.getVoltage(i)->u2;
        svmCore.phaseWantedVoltage->u1c = MotorModel.getVoltage(i)->u3;

        commonModeVoltage = svmCore.minMaxCommonModeVoltage(svmCore.phaseWantedVoltage);


        svmCore.phaseWantedVoltage->u1a = MotorModel.getVoltage(i)->u1;
        compareLevel = svmCore.createCompareLevel(287, commonModeVoltage, svmCore.phaseWantedVoltage->u1a);

        svmCore.invertorSwitch->sw1 = svmCore.comparationLevelTriangleWaveComparation(compareLevel, trianglePoint);

        
        triangleWaveData << svmCore.triangleWaveSettings->calculationTime << "," << trianglePoint << "\n";

        switchCompareData << svmCore.triangleWaveSettings->calculationTime << "," << commonModeVoltage << "," << compareLevel << "," << svmCore.invertorSwitch->sw1 << "\n";

        // when implemeted in main program probably
        // if(svmCore.triangleWaveSettings->calculationTime >=  svmCore.triangleWaveSettings->wavePeriod)
        // {
        //     svmCore.triangleWaveSettings->calculationTime = 0;
        // }

    }
    triangleWaveData.close(); // close streaming file
    switchCompareData.close();



    free(svmCore.phaseWantedVoltage);
    free(svmCore.invertorSwitch);
    free(svmCore.triangleWaveSettings);
    free(MotorModel.voltageGeneratorData);
    free(MotorModel.odeCalculationSettings);

    
    return (0);
}