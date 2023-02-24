/*******************************************************************************
Author: FEE CVUT
Purpose: Kernel

*******************************************************************************/
// Includes
#include <stdint.h>
#include <hls_stream.h>
// #include <cmath>
#include <hls_math.h>
// #include "header/MotorModel.h"
// #include "header/transformation.h"
#include <stdlib.h>

#include "function/MotorModel.cpp"

// #include "function/transformation.cpp"
#define PI 3.141592 
extern "C" {

/*
    Main Kernel

*/

void krnl_GenerateVoltageSource(voltageGeneratorType *voltageGeneratorData, odeCalculationSettingsType *odeCalculationSettings) {


MotorModelClass MotorModel;

MotorModel.odeCalculationSettings = odeCalculationSettings;
MotorModel.voltageGeneratorData = voltageGeneratorData;



 // number of iterations based on initialCalculationTime and finalCalculationTime and calculationStep
    // int n = ceil((odeCalculationSettings->finalCalculationTime - odeCalculationSettings->initialCalculationTime)/odeCalculationSettings->calculationStep);

    // local variable for tracking time
    float time = odeCalculationSettings->initialCalculationTime;
    float voltageAmplitude = MotorModel.voltageGeneratorData->voltageAmplitude;
    float voltageFrequency = MotorModel.voltageGeneratorData->voltageFrequency;
    int numberOfIterations = MotorModel.odeCalculationSettings->numberOfIterations;
    float phaseU2 = 2.0943;
    float phaseU3 = 2.0943;
    // float generatedVoltage;
    for(int i = 0; i<=numberOfIterations;i++)
    {
        #pragma HLS dependence variable= MotorModel.voltageGeneratorData type=inter false
        #pragma HLS dependence variable=voltageFrequency type=intra false
        #pragma HLS dependence variable=voltageAmplitude type=intra false
        #pragma HLS dependence variable=phaseU2 type=intra false
        #pragma HLS dependence variable=phaseU3 type=intra false
        #pragma HLS loop_tripcount min=numberOfIterations
        #pragma HLS performance target_ti= numberOfIterations;
       
        MotorModel.voltageGeneratorData[i].u1 = MotorModel.voltageGenerator(time, 0, voltageAmplitude,  voltageFrequency);
        

        MotorModel.voltageGeneratorData[i].u2 = MotorModel.voltageGenerator(time, phaseU2, voltageAmplitude,  voltageFrequency);

        MotorModel.voltageGeneratorData[i].u3 = MotorModel.voltageGenerator(time, phaseU3, voltageAmplitude,  voltageFrequency);

        // MotorModel.voltageGeneratorData[i].u1= (voltageAmplitude * sinf((2 * PI * voltageFrequency * time) + 0));

        // MotorModel.voltageGeneratorData[i].u2= (voltageAmplitude * sinf((2 * PI * voltageFrequency * time) + phaseU2));

        // MotorModel.voltageGeneratorData[i].u3= (voltageAmplitude * sinf((2 * PI * voltageFrequency * time) + phaseU3));
       
       
        time = time + odeCalculationSettings->calculationStep;
    }



}




void krnl_main2()
{
   
}


}
