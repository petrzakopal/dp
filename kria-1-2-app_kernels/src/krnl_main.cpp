/*******************************************************************************
Author: FEE CVUT
Purpose: Kernel
*******************************************************************************/
// Includes
#include <stdint.h>
#include <hls_stream.h>
#include <hls_math.h>
#include "function/regulator.cpp"
#include "function/svmCore.cpp"

// #include "function/transformation.cpp"
#define PI 3.141592 
extern "C" {


/*
* @name psi2alphaFce
* @brief Function to calculate new psi2alpha in RK4 CurVel Model
*/

float psi2alphaFce(float R2MLmDL2, float R2DL2, float i1alpha, float i1beta, float psi2alpha, float psi2beta, float motorElectricalAngularVelocity )
{
    return((R2MLmDL2 * i1alpha) - (motorElectricalAngularVelocity * psi2beta) - (R2DL2 * psi2alpha));
}

/*
* @name psi2betaFce
* @brief Function to calculate new psi2beta in RK4 CurVel Model
*/

float psi2betaFce(float R2MLmDL2, float R2DL2, float i1alpha, float i1beta, float psi2alpha, float psi2beta, float motorElectricalAngularVelocity)
{
    return((R2MLmDL2 * i1beta) + (motorElectricalAngularVelocity * psi2alpha) - (R2DL2 * psi2beta));
}

/*
* @name sliceInternalVariables4Parts
* @brief Function to calculate slice variables at 4 parts
*/

static void sliceInternalVariables4Parts(float variableIn, float *variableOut)
{
    for(int i = 0;i<4;i++)
    {
        variableOut[i] = variableIn;
    }
}

/*
* @name sliceInternalVariables8Parts
* @brief Function to calculate slice variables at 4 parts
*/

static void sliceInternalVariables8Parts(float variableIn, float *variableOut)
{
    for(int i = 0;i<8;i++)
    {
        variableOut[i] = variableIn;
    }
}


/*
* @name computeCurVel
* @brief Function to compute new psi2alpha and psi2beta values based on RK4 method
*/

static void computeCurVel(float *psi2alpha, float *psi2beta, float inputI1, float inputI2, float inputI3, float numberOfPolePairs, float *R2MLmDL2Temp, float *R2DL2Temp, float inputMotorMechanicalAngularVelocity, float timeCV, float calculationStep, float halfCalculationStep, float *i1AlphaBeta)
{
    

    // coefficients for RK4
    float k1psi2alpha, k2psi2alpha, k3psi2alpha, k4psi2alpha;
    float k1psi2beta, k2psi2beta, k3psi2beta, k4psi2beta;
    // helper variable to reduce calculation fo the same value

    float psi2alphaTemp[8];
    float psi2betaTemp[8];
    float i1alphaTemp[8];
    float i1betaTemp[8];
    float k1psi2alphaTemp[4];
    float k1psi2betaTemp[4];
    float k2psi2alphaTemp[4];
    float k2psi2betaTemp[4];
    float k3psi2alphaTemp[4];
    float k3psi2betaTemp[4];
    float motorElectricalAngularVelocityTemp[8];
    float i1alpha;
    float i1beta;
    float motorElectricalAngularVelocity;

    float k4psi2alphaTemp1;
    float k4psi2betaTemp1;


    #pragma HLS performance target_ti=1
    #pragma HLS loop_tripcount max=1
    

    
    sliceInternalVariables8Parts(*psi2alpha, psi2alphaTemp);
    sliceInternalVariables8Parts(*psi2beta, psi2betaTemp);



    i1alpha = (0.667 * (inputI1 - (0.5 * inputI2) - (0.5 * inputI3)));
    i1beta = (0.6667 * (0.866 * inputI2 - 0.866 *  inputI3));
    i1AlphaBeta[0] = i1alpha;
    i1AlphaBeta[1] = i1beta;


    sliceInternalVariables8Parts(i1alpha, i1alphaTemp);


    sliceInternalVariables8Parts(i1beta, i1betaTemp);


    motorElectricalAngularVelocity = inputMotorMechanicalAngularVelocity * numberOfPolePairs;



   
   sliceInternalVariables8Parts(motorElectricalAngularVelocity, motorElectricalAngularVelocityTemp);



    /*------------------------------------------------------------------------------------------------------------------------------------*/
    k1psi2alpha = psi2alphaFce(R2MLmDL2Temp[0], R2DL2Temp[0] , i1alphaTemp[0], i1betaTemp[0], psi2alphaTemp[0], psi2betaTemp[0], motorElectricalAngularVelocityTemp[0]);

    k1psi2beta = psi2betaFce(R2MLmDL2Temp[1], R2DL2Temp[1], i1alphaTemp[1], i1betaTemp[1], psi2alphaTemp[1], psi2betaTemp[1], motorElectricalAngularVelocityTemp[1]);
    /*------------------------------------------------------------------------------------------------------------------------------------*/


    sliceInternalVariables4Parts(k1psi2alpha, k1psi2alphaTemp);

    sliceInternalVariables4Parts(k1psi2beta, k1psi2betaTemp);

    /*------------------------------------------------------------------------------------------------------------------------------------*/
    k2psi2alpha = psi2alphaFce(R2MLmDL2Temp[2], R2DL2Temp[2] , i1alphaTemp[2], i1betaTemp[2], (psi2alphaTemp[2] + (halfCalculationStep * k1psi2alphaTemp[0])), (psi2betaTemp[2] + (halfCalculationStep * k1psi2betaTemp[0])), motorElectricalAngularVelocityTemp[2]);

    k2psi2beta = psi2betaFce(R2MLmDL2Temp[3], R2DL2Temp[3], i1alphaTemp[3], i1betaTemp[3], (psi2alphaTemp[3] + (halfCalculationStep * k1psi2alphaTemp[1])), (psi2betaTemp[3] + (halfCalculationStep * k1psi2betaTemp[1])), motorElectricalAngularVelocityTemp[3]);
    /*------------------------------------------------------------------------------------------------------------------------------------*/



    sliceInternalVariables4Parts(k2psi2alpha, k2psi2alphaTemp);

    sliceInternalVariables4Parts(k2psi2beta, k2psi2betaTemp);


    /*------------------------------------------------------------------------------------------------------------------------------------*/
    k3psi2alpha = psi2alphaFce(R2MLmDL2Temp[4], R2DL2Temp[4] , i1alphaTemp[4], i1betaTemp[4], (psi2alphaTemp[4] + (halfCalculationStep * k2psi2alphaTemp[0])), (psi2betaTemp[4] + (halfCalculationStep * k2psi2betaTemp[0])), motorElectricalAngularVelocityTemp[4]);

    k3psi2beta = psi2betaFce(R2MLmDL2Temp[5], R2DL2Temp[5] , i1alphaTemp[5], i1betaTemp[5], (psi2alphaTemp[5] + (halfCalculationStep * k2psi2alphaTemp[1])), (psi2betaTemp[5] + (halfCalculationStep * k2psi2betaTemp[1])), motorElectricalAngularVelocityTemp[5]);
    /*------------------------------------------------------------------------------------------------------------------------------------*/

    sliceInternalVariables4Parts(k3psi2alpha, k3psi2alphaTemp);

    
    sliceInternalVariables4Parts(k3psi2beta, k3psi2betaTemp);

    /*------------------------------------------------------------------------------------------------------------------------------------*/
    k4psi2alpha = psi2alphaFce(R2MLmDL2Temp[6], R2DL2Temp[6] , i1alphaTemp[6], i1betaTemp[6], (psi2alphaTemp[6] + (calculationStep * k3psi2alphaTemp[0])), (psi2betaTemp[6] + (calculationStep * k3psi2betaTemp[0])), motorElectricalAngularVelocityTemp[6]);

    k4psi2beta = psi2betaFce(R2MLmDL2Temp[7], R2DL2Temp[7] , i1alphaTemp[7], i1betaTemp[7], (psi2alphaTemp[7] + (calculationStep * k3psi2alphaTemp[1])), (psi2betaTemp[7] + (calculationStep * k3psi2betaTemp[1])), motorElectricalAngularVelocityTemp[7]);
    /*------------------------------------------------------------------------------------------------------------------------------------*/

    k4psi2alphaTemp1 = k4psi2alpha;
    k4psi2betaTemp1 = k4psi2beta;


    // updating the values based on calculated coefficients
    /*------------------------------------------------------------------------------------------------------------------------------------*/

    *psi2alpha = *psi2alpha + ((calculationStep / 6) * (k1psi2alphaTemp[3] + 2 * k2psi2alphaTemp[3] + 2 * k3psi2alphaTemp[3] + k4psi2alphaTemp1));


    *psi2beta = *psi2beta + ((calculationStep / 6) * (k1psi2betaTemp[3] + 2 * k2psi2betaTemp[3] + 2 * k3psi2betaTemp[3] + k4psi2betaTemp1));



    
    
/****************************************************************************************************/

}


static float regSaturationBlock(float saturationInput, float saturationOutputMin, float saturationOutputMax)
{
    float localSaturationInput = saturationInput;

        if(localSaturationInput > saturationOutputMax)
        {
            localSaturationInput = saturationOutputMax;
        }
        else if(localSaturationInput < saturationOutputMin)
        {
            localSaturationInput = saturationOutputMin;
        }

    return(localSaturationInput);
}


static void checkSaturationStatus(RegulatorType *regulatorData)
{
    if(regulatorData->saturationInput == regulatorData->saturationOutput)
    {
        regulatorData->saturationCheckStatus = false; // saturation did not happen
    }
    else
    {
        regulatorData->saturationCheckStatus = true; // saturation did happen
    }
}


static void checkSignStatus(RegulatorType *regulatorData)
{
    if(((regulatorData->eDif > 0) and (regulatorData->saturationInput > 0)) or ((regulatorData->eDif < 0) and (regulatorData->saturationInput < 0)))
    {
        regulatorData->signCheckStatus = true;
    }
    else
    {
        regulatorData->signCheckStatus = false;
    }
}


static void enableClamping(RegulatorType *regulatorData)
{
    if((regulatorData->saturationCheckStatus == true) and (regulatorData->signCheckStatus == true))
    {
        regulatorData->clampingStatus = true;
    }
    else
    {
        regulatorData->clampingStatus = false;
    }

}

static void regCalculate(RegulatorType *regulatorData)
{
    regulatorData->eDif = regulatorData->wantedValue - regulatorData->measuredValue;
    regulatorData->saturationInput = (regulatorData->eDif * regulatorData->kp) + regulatorData->iSum;
    regulatorData->saturationOutput = regSaturationBlock(regulatorData->saturationInput, regulatorData->saturationOutputMin, regulatorData->saturationOutputMax);

    checkSaturationStatus(regulatorData);
    checkSignStatus(regulatorData);
    enableClamping(regulatorData);

    if(regulatorData->clampingStatus == true)
    {
        regulatorData->iSum = regulatorData->iSum;
    }
    else
    {
        regulatorData->iSum = regulatorData->iSum + (regulatorData->eDif * regulatorData->ki);
    }
}

/*
* @name outputCurVelProductsCalc
* @brief Calculating output products from CurVel model needed in svmCore
*/

static void outputCurVelProductsCalc(float *transformAngle, float *psi2amplitude, float *psi2alphaOut, float *psi2betaOut)
{
    
           float psi2alphaOutTemp = *psi2alphaOut;
           float psi2betaOutTemp = *psi2betaOut;
         
            *psi2amplitude = sqrtf((psi2alphaOutTemp * psi2alphaOutTemp) + (psi2betaOutTemp * psi2betaOutTemp));
            *transformAngle = atan2f(psi2betaOutTemp, psi2alphaOutTemp);


}



static float generateActualValueTriangleWave(TriangleWaveSettingsType *triangleWaveSettings)
{
     float triangleActualValue; // maybe put in triangleWaveSettings, and make this function void, do not know, this should be used only as a local variable

        // numerical method with abs value and modulo
        triangleActualValue = (((4 * triangleWaveSettings->waveAmplitude)/triangleWaveSettings->wavePeriod) * abs(fmod((fmod((triangleWaveSettings->calculationTime-(triangleWaveSettings->wavePeriod/4)), triangleWaveSettings->wavePeriod) +  triangleWaveSettings->wavePeriod),  triangleWaveSettings->wavePeriod) - ( triangleWaveSettings->wavePeriod/2)) - triangleWaveSettings->waveAmplitude);

        // trigoniometric method
        // triangleActualValue = ((2 * triangleWaveSettings->waveAmplitude)/3.145192)*asin(sin(((2 * 3.145192)/triangleWaveSettings->wavePeriod) * triangleWaveSettings->calculationTime));

        // updating inner wave calculation time based on set initial value of calculationTime...
        triangleWaveSettings->calculationTime = triangleWaveSettings->calculationTime + triangleWaveSettings->calculationStep;

        return(triangleActualValue);
}


static float minMaxCommonModeVoltage(CoreInternalVariablesType *coreInternalVariables)
{
    return(((fmax(fmax(coreInternalVariables->u1a, coreInternalVariables->u1b), coreInternalVariables->u1c ) ) + (fmin(fmin(coreInternalVariables->u1a, coreInternalVariables->u1b), coreInternalVariables->u1c ) ))/2);
}


static bool comparationLevelTriangleWaveComparation(float compareLevel, float triangleWaveValue)
{
    if(compareLevel>=triangleWaveValue)
    {
        return(true);
    }
    else
    {
        return(false);
    }
}

static float createCompareLevel(float levelConstant, float commonModeVoltage, float phaseWantedVoltage)
{
    return((phaseWantedVoltage - commonModeVoltage)/levelConstant);
}


// budu používat pouze když bude známá velikost vstupních hodnot, resp. když to bude pro býpočet jen jedné hodnoty
// static void outputBurst(float *dataIn, float *dataOut, int numberOfIterations)
// {
//     for(int i = 0;i< numberOfIterations;i++)
//     {
//         dataOut[i] = dataIn[i];
//     }
// }


/*
* @name krnl_calculateCurVelModel
* @brief Main FOC Kernel
* @todo Change name to corresponding function of kernel
*/

void krnl_calculateCurVelModel(float *masterInput, float *masterOutput ) {

#pragma HLS INTERFACE mode = m_axi port = masterInput bundle = gmem0
#pragma HLS INTERFACE mode = m_axi port = masterOutput bundle = gmem1
#pragma HLS performance target_ti=1

RegulatorType idRegulator;
RegulatorType iqRegulator;
RegulatorType fluxRegulator;
RegulatorType velocityRegulator;
CoreInternalVariablesType coreInternalVariables;
TriangleWaveSettingsType triangleWaveSettings;
InvertorSwitchType invertorSwitch;

// local variables
float timeCV = masterInput[0];
float calculationStep = masterInput[2];
float R2MLmDL2 = masterInput[6];
float R2DL2 = masterInput[5];
int numberOfIterationsTemp = (int) masterInput[4];
float numberOfPolePairs = masterInput[7];
float halfCalculationStep = calculationStep/2;
float R2MLmDL2Temp[8];
float R2DL2Temp[8];
float psi2alpha = masterInput[12];
float psi2beta = masterInput[13];
float inputI1 = masterInput[8];
float inputI2 = masterInput[9];
float inputI3 = masterInput[10];
float inputMotorMechanicalAngularVelocity = masterInput[11];
triangleWaveSettings.waveAmplitude = masterInput[14];
triangleWaveSettings.calculationStep = masterInput[15];
triangleWaveSettings.wavePeriod = masterInput[16];
triangleWaveSettings.calculationTime = masterInput[17];
float Udcmax = masterInput[18];
fluxRegulator.ki = masterInput[19];
fluxRegulator.kp = masterInput[20];
fluxRegulator.saturationOutputMax = masterInput[21];
fluxRegulator.saturationOutputMin = masterInput[22];
fluxRegulator.saturationOutput = masterInput[23];
fluxRegulator.clampingStatus = masterInput[24];
fluxRegulator.saturationCheckStatus = masterInput[25];
fluxRegulator.signCheckStatus = masterInput[26];
fluxRegulator.measuredValue = masterInput[27];
velocityRegulator.ki = masterInput[28];
velocityRegulator.kp = masterInput[29];
velocityRegulator.saturationOutputMax = masterInput[30];
velocityRegulator.saturationOutputMin = masterInput[31];
velocityRegulator.saturationOutput = masterInput[32];
velocityRegulator.clampingStatus = masterInput[33];
velocityRegulator.saturationCheckStatus = masterInput[34];
velocityRegulator.signCheckStatus = masterInput[35];
idRegulator.ki = masterInput[36];
idRegulator.kp = masterInput[37];
idRegulator.saturationOutputMax = masterInput[38];
idRegulator.saturationOutput = masterInput[39];
idRegulator.clampingStatus = masterInput[40];
idRegulator.saturationCheckStatus = masterInput[41];
idRegulator.signCheckStatus = masterInput[42];
idRegulator.measuredValue = masterInput[43];
iqRegulator.ki = masterInput[44];
iqRegulator.kp = masterInput[45];
iqRegulator.saturationOutputMax = masterInput[46];
iqRegulator.saturationOutputMin = masterInput[47];
iqRegulator.clampingStatus = masterInput[48];
iqRegulator.saturationCheckStatus = masterInput[49];
iqRegulator.signCheckStatus = masterInput[50];
iqRegulator.measuredValue = masterInput[51];
fluxRegulator.wantedValue = masterInput[52];
velocityRegulator.wantedValue = masterInput[53];
idRegulator.wantedValue = masterInput[54];
iqRegulator.wantedValue = masterInput[55];
float uDC = masterInput[56];
float minMaxCommonModeVoltageConstant = masterInput[57];
fluxRegulator.iSum = masterInput[58];
velocityRegulator.iSum = masterInput[59];
idRegulator.iSum = masterInput[60];
iqRegulator.iSum = masterInput[61];





// helper variables, resolve later
float *psi2alpha_ptr = &psi2alpha;
float *psi2beta_ptr = &psi2beta;
float i1AlphaBeta[2];
float transformAngle;
float psi2amplitude;
float trianglePoint;
float commonModeVoltage;


// slicing variables to prevent violating single producer and consumer paradigm (hope so)
sliceInternalVariables8Parts(R2MLmDL2, R2MLmDL2Temp);
sliceInternalVariables8Parts(R2DL2, R2DL2Temp);


// computing Cur-Vel model (not using API)
computeCurVel(psi2alpha_ptr, psi2beta_ptr, inputI1, inputI2, inputI3, numberOfPolePairs, R2MLmDL2Temp, R2DL2Temp, inputMotorMechanicalAngularVelocity, timeCV, calculationStep, halfCalculationStep, i1AlphaBeta);
outputCurVelProductsCalc(&transformAngle, &psi2amplitude,psi2alpha_ptr, psi2beta_ptr);

// regulator new values + constrains
idRegulator.measuredValue = ((i1AlphaBeta[0] * cosf(transformAngle)) + (i1AlphaBeta[1] * sinf(transformAngle)));
iqRegulator.measuredValue = ((-i1AlphaBeta[0] * sinf(transformAngle)) + (i1AlphaBeta[1] * cosf(transformAngle)));
fluxRegulator.measuredValue = psi2amplitude;
velocityRegulator.measuredValue = inputMotorMechanicalAngularVelocity;
iqRegulator.saturationOutputMax = sqrtf((Udcmax * Udcmax) - (idRegulator.saturationOutput * idRegulator.saturationOutput));
iqRegulator.saturationOutputMin = - iqRegulator.saturationOutputMax;

// calculating first set of regulators
regCalculate(&fluxRegulator);
regCalculate(&velocityRegulator);

// new values
idRegulator.wantedValue = fluxRegulator.saturationOutput;
iqRegulator.wantedValue = velocityRegulator.saturationOutput;


// calculating second set of regulators
regCalculate(&idRegulator);
regCalculate(&iqRegulator);








coreInternalVariables.u1d = idRegulator.saturationOutput;
coreInternalVariables.u1q = iqRegulator.saturationOutput;


coreInternalVariables.u1alpha = ((coreInternalVariables.u1d * cosf(transformAngle)) - (coreInternalVariables.u1q * sinf(transformAngle)));
coreInternalVariables.u1beta = ((coreInternalVariables.u1d * sinf(transformAngle)) + (coreInternalVariables.u1q * cosf(transformAngle)));

coreInternalVariables.u1a = coreInternalVariables.u1alpha;
coreInternalVariables.u1b = ((-0.5 * coreInternalVariables.u1alpha) + 0.866 * coreInternalVariables.u1beta);
coreInternalVariables.u1c = ((-0.5 * coreInternalVariables.u1alpha) - 0.866 * coreInternalVariables.u1beta);


trianglePoint = generateActualValueTriangleWave(&triangleWaveSettings);
commonModeVoltage = minMaxCommonModeVoltage(&coreInternalVariables);


invertorSwitch.sw1 = comparationLevelTriangleWaveComparation(createCompareLevel(minMaxCommonModeVoltageConstant, commonModeVoltage,coreInternalVariables.u1a), trianglePoint);

invertorSwitch.sw3 = comparationLevelTriangleWaveComparation(createCompareLevel(minMaxCommonModeVoltageConstant, commonModeVoltage,coreInternalVariables.u1b), trianglePoint);

invertorSwitch.sw5 = comparationLevelTriangleWaveComparation(createCompareLevel(minMaxCommonModeVoltageConstant, commonModeVoltage,coreInternalVariables.u1c), trianglePoint);


invertorSwitch.sw4 = ~invertorSwitch.sw1;
invertorSwitch.sw6 = ~invertorSwitch.sw3;
invertorSwitch.sw2 = ~invertorSwitch.sw5;


// solve loopback variables
masterOutput[0] = invertorSwitch.sw1;
masterOutput[1] = invertorSwitch.sw2;
masterOutput[2] = invertorSwitch.sw3;
masterOutput[3] = invertorSwitch.sw4;
masterOutput[4] = invertorSwitch.sw5;
masterOutput[5] = invertorSwitch.sw6;
masterOutput[6] = *psi2alpha_ptr;
masterOutput[7] = *psi2beta_ptr;
masterOutput[8] = triangleWaveSettings.calculationTime;
masterOutput[9] = idRegulator.saturationOutput;
masterOutput[10] = fluxRegulator.iSum;
masterOutput[11] = velocityRegulator.iSum;
masterOutput[12] = idRegulator.iSum;
masterOutput[13] = iqRegulator.iSum;
masterOutput[14] = psi2amplitude;
masterOutput[15] = transformAngle;


// debugging variables
masterOutput[16] = fluxRegulator.clampingStatus;
masterOutput[17] = fluxRegulator.measuredValue;
masterOutput[18] = velocityRegulator.saturationOutput;
masterOutput[19] = velocityRegulator.clampingStatus;
masterOutput[20] = velocityRegulator.measuredValue;
masterOutput[21] = velocityRegulator.eDif;
masterOutput[22] = velocityRegulator.wantedValue;
masterOutput[23] = idRegulator.measuredValue;
masterOutput[24] = iqRegulator.clampingStatus;
masterOutput[25] = iqRegulator.measuredValue;
masterOutput[26] = fluxRegulator.measuredValue;
masterOutput[27] = idRegulator.measuredValue;
masterOutput[28] = iqRegulator.wantedValue;
masterOutput[29] = trianglePoint;
masterOutput[30] = idRegulator.clampingStatus;


}

}

