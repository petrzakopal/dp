/*******************************************************************************
Author: FEE CVUT
Purpose: Kernel

*******************************************************************************/
// Includes
#include <stdint.h>
#include <hls_stream.h>
#include <cmath>
// #include "header/MotorModel.h"
// #include "header/transformation.h"
#include <stdlib.h>

#include "function/MotorModel.cpp"

// #include "function/transformation.cpp"

extern "C" {

/*
    Main Kernel

*/

void krnl_GenerateVoltage(voltageGeneratorType *voltageGeneratorData, odeCalculationSettingsType *odeCalculationSettings) {
//#pragma HLS interface hostStruct port=structure
//#pragma HLS aggregate variable=hostStruct compact=auto

MotorModelClass MotorModel;

MotorModel.odeCalculationSettings = odeCalculationSettings;
MotorModel.voltageGeneratorData = voltageGeneratorData;

MotorModel.setVariable(MotorModel.getVoltage(0)->u1, 2500);

}




void krnl_main2()
{
   
}


}
