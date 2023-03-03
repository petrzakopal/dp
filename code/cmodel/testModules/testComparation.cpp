// @file testComparation.cpp
// compilation command: gcc -std=c++14 testComparation.cpp -o run/testComparation -lstdc++ ./../function/svmCore.cpp ./../function/MotorModel.cpp ./../function/transformation.cpp

#include "./../header/MotorModel.h"
#include "./../header/svmCore.h"
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <fstream>



int main()
{

    svmCoreClass svmCore;
    // svmCore.phaseWantedVoltageAllocateMemory(); // deprecated
    svmCore.invertorSwitchAllocateMemory();
    svmCore.triangleWaveSettingsAllocateMemory();
    svmCore.coreInternalVariablesAllocateMemory();


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

    // setting voltage generator
    MotorModel.voltageGeneratorDataAllocateMemory();
    MotorModel.voltageGeneratorData->voltageFrequency = 50;
    MotorModel.voltageGeneratorData->voltageAmplitude = 325.26;


    MotorModel.precalculateVoltageSource(MotorModel.odeCalculationSettings, MotorModel.voltageGeneratorData->voltageAmplitude, MotorModel.voltageGeneratorData->voltageFrequency);



    // internal variables for cleaner code
    float commonModeVoltage;
    float compareLevel;
    float trianglePoint;



    // file output streaming
    std::ofstream triangleWaveData;
    std::ofstream switchCompareData;
    triangleWaveData.open ("outputData/triangleWaveData.csv",std::ofstream::out | std::ofstream::trunc);
     switchCompareData.open ("outputData/switchCompareData.csv",std::ofstream::out | std::ofstream::trunc);

    // creating the same ammount of data as for volage
    // in normal ebnedded scenario, there would be an infinite loop of program and triangle wave with some period, after which it could reset the iteration counter for which it generates voltage. so the "time value" which translates for generated voltage of ASM, in this example, as a iteration number would start again, so there would be period after period and so on, the motor would go still forward, because it would not be precalculated as is in this example, in this example the calculation time and number of iterations are connected together, in real example there would be no interation number for voltage asm level and/or currents, because it would be calculated from actual values
    int maxIterations = ((int)ceil(((globalFinalCalculationTime - 0)/svmCore.triangleWaveSettings->calculationStep)));

    for(int i = 0; i<maxIterations;i++) // when implemented for testing
    // while(true) // this would be main while backgrund program
    {
        
        // generating actual value - point on a triangle wave based on settings and mainly on actual time for wave which is determined by triangleWaveSettings->calculationTime which is iterated automatically every time this function is run, it basically can run forever because this function only return back value and does not store anything
        trianglePoint = svmCore.generateActualValueTriangleWave(svmCore.triangleWaveSettings);



        // only in this example getting value from precalculated voltage, in real example it would be an array of just one value for every voltage, it would minimze the memory needed for data storing and basically there is no need to store previous data of a measured voltage...
        svmCore.coreInternalVariables->u1a = MotorModel.getVoltage(i)->u1;
        svmCore.coreInternalVariables->u1b = MotorModel.getVoltage(i)->u2;
        svmCore.coreInternalVariables->u1c = MotorModel.getVoltage(i)->u3;

        // calculating commonModeVoltage to be subtracted from wanted voltage which is output from regulators/clark/park
        commonModeVoltage = svmCore.minMaxCommonModeVoltage(svmCore.coreInternalVariables);

       // creating compare lvl for one phase, there would normally be 3 phase voltage - for every voltage compareLevel local helper variable
        compareLevel = svmCore.createCompareLevel(287, commonModeVoltage, svmCore.coreInternalVariables->u1a);

        // comparation of edited voltage called "prdele" with actual triangle value from triangle generator
        svmCore.invertorSwitch->sw1 = svmCore.comparationLevelTriangleWaveComparation(compareLevel, trianglePoint);

        // output data for testing just triangle wave
        triangleWaveData << svmCore.triangleWaveSettings->calculationTime << "," << trianglePoint << "\n";


        // output data for more interresting and data packed testing
        switchCompareData << svmCore.triangleWaveSettings->calculationTime << "," << commonModeVoltage << "," << compareLevel << "," << svmCore.invertorSwitch->sw1 << "\n";

        // when implemeted in main program probably could be but does not have to be, function generateActualValueTriangleWave generates values infinitely
        // if(svmCore.triangleWaveSettings->calculationTime >=  svmCore.triangleWaveSettings->wavePeriod)
        // {
        //     svmCore.triangleWaveSettings->calculationTime = 0;
        // }

    }
    triangleWaveData.close(); // close streaming file
    switchCompareData.close(); // close steraming file



    // freeing pointers in memory to avoid memory leaks
    free(svmCore.coreInternalVariables);
    free(svmCore.invertorSwitch);
    free(svmCore.triangleWaveSettings);
    free(MotorModel.voltageGeneratorData);
    free(MotorModel.odeCalculationSettings);

    
    return (0);
}