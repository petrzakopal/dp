/*******************************************************************************
Author: FEE CVUT
Purpose: Kernel
*******************************************************************************/
// Includes
#include <stdint.h>
#include <hls_stream.h>
#include <hls_math.h>


// #include "function/transformation.cpp"
#define PI 3.141592 
extern "C" {



float psi2alphaFce(float R2MLmDL2, float R2DL2, float i1alpha, float i1beta, float psi2alpha, float psi2beta, float motorElectricalAngularVelocity )
{
    return((R2MLmDL2 * i1alpha) - (motorElectricalAngularVelocity * psi2beta) - (R2DL2 * psi2alpha));
}


float psi2betaFce(float R2MLmDL2, float R2DL2, float i1alpha, float i1beta, float psi2alpha, float psi2beta, float motorElectricalAngularVelocity)
{
    return((R2MLmDL2 * i1beta) + (motorElectricalAngularVelocity * psi2alpha) - (R2DL2 * psi2beta));
}


static void sliceInternalVariables4Parts(float variableIn, float *variableOut)
{
    for(int i = 0;i<4;i++)
    {
        variableOut[i] = variableIn;
    }
}

static void sliceInternalVariables8Parts(float variableIn, float *variableOut)
{
    for(int i = 0;i<8;i++)
    {
        variableOut[i] = variableIn;
    }
}



static void computeCurVel(float *psi2alpha, float *psi2beta, float inputI1, float inputI2, float inputI3, float numberOfPolePairs, float *R2MLmDL2Temp, float *R2DL2Temp, int numberOfIterationsTemp1, float inputMotorMechanicalAngularVelocity, float timeCV, float calculationStep, float halfCalculationStep)
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
int numberOfIterations = numberOfIterationsTemp1;

float k4psi2alphaTemp1;
float k4psi2betaTemp1;






    #pragma HLS performance target_ti=1
    #pragma HLS loop_tripcount max=numberOfIterations
    

    
    sliceInternalVariables8Parts(*psi2alpha, psi2alphaTemp);
    sliceInternalVariables8Parts(*psi2beta, psi2betaTemp);



    timeCV = timeCV + calculationStep;

    i1alpha = (0.667 * (inputI1 - (0.5 * inputI2) - (0.5 * inputI3)));
    i1beta = (0.6667 * (0.866 * inputI2 - 0.866 *  inputI3));


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

static void outputProductsCalc( float *masterOutput, float *psi2alphaOut, float *psi2betaOut)
{
    
          
            masterOutput[0] = sqrtf((*psi2alphaOut * *psi2alphaOut) + (*psi2betaOut * *psi2betaOut));
            masterOutput[1] = atan2f(*psi2betaOut, *psi2alphaOut);

}

// budu používat pouze když bude známá velikost vstupních hodnot, resp. když to bude pro býpočet jen jedné hodnoty
// static void outputBurst(float *dataIn, float *dataOut, int numberOfIterations)
// {
//     for(int i = 0;i< numberOfIterations;i++)
//     {
//         dataOut[i] = dataIn[i];
//     }
// }

// second type kernel
// void krnl_calculateCurVelModel(float *odeCVCalculationSettingsArray, float *modelCVCoeffArray, float *inputI1, float *inputI2, float *inputI3, float *inputMotorMechanicalAngularVelocity, float *psi2alphaOut, float *psi2betaOut, float *psi2Amplitude, float *transformAngle )

void krnl_calculateCurVelModel(float *masterInput, float *masterOutput ) {


// nevejde se do ZynQ
#pragma HLS INTERFACE mode = m_axi port = masterInput bundle = gmem0
#pragma HLS INTERFACE mode = m_axi port = masterOutput bundle = gmem1

#pragma HLS performance target_ti=1
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

float *psi2alpha_ptr = &psi2alpha;
float *psi2beta_ptr = &psi2beta;

sliceInternalVariables8Parts(R2MLmDL2, R2MLmDL2Temp);
sliceInternalVariables8Parts(R2DL2, R2DL2Temp);




computeCurVel(psi2alpha_ptr, psi2beta_ptr, inputI1, inputI2, inputI3, numberOfPolePairs, R2MLmDL2Temp, R2DL2Temp, numberOfIterationsTemp, inputMotorMechanicalAngularVelocity, timeCV, calculationStep, halfCalculationStep);
 



outputProductsCalc(masterOutput, psi2alpha_ptr, psi2beta_ptr);




}

}

