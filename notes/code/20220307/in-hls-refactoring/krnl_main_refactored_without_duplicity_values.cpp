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



// first type kernel
// void krnl_calculateCurVelModel(odeCVCalculationSettingsType *odeCVCalculationSettings,modelCVVariablesType *modelCVVariables, modelCVCoeffType *modelCVCoeff, float *inputI1, float *inputI2, float *inputI3, float *inputMotorMechanicalAngularVelocity, float * psi2Amplitude ) {

// CurVelModelClass CurVelModel;
// TransformationClass Transformation;

// float timeCV = odeCVCalculationSettings->initialCalculationTime;

// /****************************************************************************************************/
// /*------------------------------ MAIN MODEL LOOP FOR ACQUIRED INPUTS ------------------------------*/
// for(int i = 0; i< odeCVCalculationSettings->numberOfIterations; i++)
// {
//     #pragma HLS performance target_ti=100
//     #pragma HLS loop_tripcount max=odeCVCalculationSettings->numberOfIterations

//     timeCV = timeCV + odeCVCalculationSettings->calculationStep;

//     modelCVVariables->i1alpha = Transformation.clarkeTransform1(inputI1[i],inputI2[i], inputI3[i], 0.6667);
//     modelCVVariables->i1beta = Transformation.clarkeTransform2(inputI1[i], inputI2[i], inputI3[i], 0.6667);
//     modelCVVariables->motorElectricalAngularVelocity = inputMotorMechanicalAngularVelocity[i] * modelCVCoeff->nOfPolePairs;

//    CurVelModel.CurVelModelCalculate(modelCVCoeff, modelCVVariables, odeCVCalculationSettings);



//     psi2Amplitude[i] = sqrtf((modelCVVariables->psi2alpha * modelCVVariables->psi2alpha) + (modelCVVariables->psi2beta * modelCVVariables->psi2beta));

    
// /****************************************************************************************************/



// }





// }


float psi2alphaFce(float R2MLmDL2, float R2DL2, float i1alpha, float i1beta, float psi2alpha, float psi2beta, float motorElectricalAngularVelocity )
{
    return((R2MLmDL2 * i1alpha) - (motorElectricalAngularVelocity * psi2beta) - (R2DL2 * psi2alpha));
}


float psi2betaFce(float R2MLmDL2, float R2DL2, float i1alpha, float i1beta, float psi2alpha, float psi2beta, float motorElectricalAngularVelocity)
{
    return((R2MLmDL2 * i1beta) + (motorElectricalAngularVelocity * psi2alpha) - (R2DL2 * psi2beta));
}

// second type kernel
void krnl_calculateCurVelModel(float *odeCVCalculationSettingsArray, float *modelCVCoeffArray, float *inputI1, float *inputI2, float *inputI3, float *inputMotorMechanicalAngularVelocity, float *psi2alphaOut, float *psi2betaOut) {


float timeCV = odeCVCalculationSettingsArray[0];
float i1alpha;
float i1beta;
float motorElectricalAngularVelocity;
float halfCaclculationStep = odeCVCalculationSettingsArray[2]/2;
int numberOfIterations = (int) odeCVCalculationSettingsArray[4];
float R2MLmDL2 = modelCVCoeffArray[1];

float R2DL2 = modelCVCoeffArray[0];

float psi2alpha = psi2alphaOut[0];
float psi2beta = psi2betaOut[0];


/****************************************************************************************************/
/*------------------------------ MAIN MODEL LOOP FOR ACQUIRED INPUTS ------------------------------*/
for(int i = 0; i< numberOfIterations; i++)
{

    #pragma HLS performance target_ti=100
    #pragma HLS loop_tripcount max=numberOfIterations


    timeCV = timeCV + odeCVCalculationSettingsArray[2];

    i1alpha = (0.667 * (inputI1[i] - (0.5 * inputI2[i]) - (0.5 * inputI3[i])));
    i1beta = (0.6667 * (0.866 * inputI2[i] - 0.866 *  inputI3[i]));


    motorElectricalAngularVelocity = inputMotorMechanicalAngularVelocity[i] * modelCVCoeffArray[2];


   


 // coefficients for RK4
    float k1psi2alpha, k2psi2alpha, k3psi2alpha, k4psi2alpha;
    float k1psi2beta, k2psi2beta, k3psi2beta, k4psi2beta;

    // helper variable to reduce calculation fo the same value
    float halfCalculationStep = odeCVCalculationSettingsArray[2]/2;


    /*------------------------------------------------------------------------------------------------------------------------------------*/
    k1psi2alpha = psi2alphaFce(R2MLmDL2, R2DL2 , i1alpha, i1beta, psi2alpha, psi2beta, motorElectricalAngularVelocity);

    k1psi2beta = psi2betaFce(R2MLmDL2, R2DL2, i1alpha, i1beta, psi2alpha, psi2beta, motorElectricalAngularVelocity);
    /*------------------------------------------------------------------------------------------------------------------------------------*/

   

    /*------------------------------------------------------------------------------------------------------------------------------------*/
    k2psi2alpha = psi2alphaFce(R2MLmDL2, R2DL2 , i1alpha, i1beta, (psi2alpha + (halfCalculationStep * k1psi2alpha)), (psi2beta + (halfCalculationStep * k1psi2beta)), motorElectricalAngularVelocity);

    k2psi2beta = psi2betaFce(R2MLmDL2, R2DL2, i1alpha, i1beta, (psi2alpha + (halfCalculationStep * k1psi2alpha)), (psi2beta + (halfCalculationStep * k1psi2beta)), motorElectricalAngularVelocity);
    /*------------------------------------------------------------------------------------------------------------------------------------*/



    /*------------------------------------------------------------------------------------------------------------------------------------*/
    k3psi2alpha = psi2alphaFce(R2MLmDL2, R2DL2 , i1alpha, i1beta, (psi2alpha + (halfCalculationStep * k2psi2alpha)), (psi2beta + (halfCalculationStep * k2psi2beta)), motorElectricalAngularVelocity);

    k3psi2beta = psi2betaFce(R2MLmDL2, R2DL2 , i1alpha, i1beta, (psi2alpha + (halfCalculationStep * k2psi2alpha)), (psi2beta + (halfCalculationStep * k2psi2beta)), motorElectricalAngularVelocity);
    /*------------------------------------------------------------------------------------------------------------------------------------*/


    /*------------------------------------------------------------------------------------------------------------------------------------*/
    k4psi2alpha = psi2alphaFce(R2MLmDL2, R2DL2 , i1alpha, i1beta, (psi2alpha + (odeCVCalculationSettingsArray[2] * k3psi2alpha)), (psi2beta + (odeCVCalculationSettingsArray[2] * k3psi2beta)), motorElectricalAngularVelocity);

    k4psi2beta = psi2betaFce(R2MLmDL2, R2DL2 , i1alpha, i1beta, (psi2alpha + (odeCVCalculationSettingsArray[2] * k3psi2alpha)), (psi2beta + (odeCVCalculationSettingsArray[2] * k3psi2beta)), motorElectricalAngularVelocity);
    /*------------------------------------------------------------------------------------------------------------------------------------*/


    // updating the values based on calculated coefficients
    /*------------------------------------------------------------------------------------------------------------------------------------*/

    psi2alpha = psi2alpha + ((odeCVCalculationSettingsArray[2] / 6) * (k1psi2alpha + 2 * k2psi2alpha + 2 * k3psi2alpha + k4psi2alpha));

    psi2alphaOut[i] = psi2alpha;

    psi2beta = psi2beta + ((odeCVCalculationSettingsArray[2] / 6) * (k1psi2beta + 2 * k2psi2beta + 2 * k3psi2beta + k4psi2beta));

    psi2betaOut[i] = psi2beta;


    

    
/****************************************************************************************************/



}





}

}