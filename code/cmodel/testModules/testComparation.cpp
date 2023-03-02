// @file testComparation.cpp
// compilation command: gcc -std=c++14 testComparation.cpp -o run/testComparation -lstdc++ ./../function/svmCore.cpp


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


    svmCore.phaseWantedVoltage->u1a = 230;
    svmCore.phaseWantedVoltage->u1b = - 153;
    svmCore.phaseWantedVoltage->u1c = -153;


    svmCore.triangleWaveSettings->waveAmplitude = 1;
    svmCore.triangleWaveSettings->calculationStep = 0.00001;
    svmCore.triangleWaveSettings->wavePeriod = 0.001;
    svmCore.triangleWaveSettings->calculationTime = 0; 

    float commonModeVoltage = svmCore.minMaxCommonModeVoltage(svmCore.phaseWantedVoltage);

    std::cout << svmCore.createCompareLevel(270, commonModeVoltage, svmCore.phaseWantedVoltage->u1a) << "\n";


    // for testing

    // file output streaming
    std::ofstream triangleWaveData;
    triangleWaveData.open ("outputData/triangleWaveData.csv",std::ofstream::out | std::ofstream::trunc);

    int maxIterations = ((int)ceil(((1 - 0)/svmCore.triangleWaveSettings->calculationStep)));

    for(int i = 0; i<maxIterations;i++)
    {
        float trianglePoint;
        trianglePoint = svmCore.generateActualValueTriangleWave(svmCore.triangleWaveSettings);
        std::cout << "triangle wave actual value:" << trianglePoint << "\n";
        triangleWaveData << svmCore.triangleWaveSettings->calculationTime << "," << trianglePoint << "\n";
    }
    triangleWaveData.close(); // close streaming file
   



    free(svmCore.phaseWantedVoltage);
    free(svmCore.invertorSwitch);
    free(svmCore.triangleWaveSettings);
    return (0);
}