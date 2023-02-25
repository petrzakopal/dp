/*******************************************************************************
Author: FEE CVUT
Purpose: Kernel

*******************************************************************************/
// Includes
#include <stdint.h>
#include <hls_stream.h>
#include <hls_math.h>
#include <stdlib.h>
#include "function/CurVelModel.cpp"
#include "function/transformation.cpp"


// #include "function/transformation.cpp"
#define PI 3.141592 
extern "C" {



// first type kernel
// void krnl_calculateCurVelModel(odeCVCalculationSettingsType *odeCVCalculationSettings,modelCVVariablesType *modelCVVariables, modelCVCoeffType *modelCVCoeff, float *inputI1, float *inputI2, float *inputI3, float *inputMotorMechanicalAngularVelocity, float * psi2Amplitude, float *transformAngle ) {

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

//     transformAngle[i] = atanf(modelCVVariables->psi2beta/modelCVVariables->psi2alpha);
    

    
// /****************************************************************************************************/



// }





// }


// second type kernel
void krnl_calculateCurVelModel(odeCVCalculationSettingsType *odeCVCalculationSettings, modelCVCoeffType *modelCVCoeff, float *inputI1, float *inputI2, float *inputI3, float *inputMotorMechanicalAngularVelocity, float *psi2alpha, float *psi2beta) {

CurVelModelClass CurVelModel;

// float timeCV = odeCVCalculationSettings->initialCalculationTime;
float i1alpha;
float i1beta;
float motorElectricalAngularVelocity;



/****************************************************************************************************/
/*------------------------------ MAIN MODEL LOOP FOR ACQUIRED INPUTS ------------------------------*/
for(int i = 0; i< odeCVCalculationSettings->numberOfIterations; i++)
{
    #pragma HLS performance target_ti=273
    #pragma HLS loop_tripcount max=odeCVCalculationSettings->numberOfIterations

    // timeCV = timeCV + odeCVCalculationSettings->calculationStep;

    i1alpha = (0.667 * (inputI1[i] - (0.5 * inputI2[i]) - (0.5 * inputI3[i])));
    i1beta = (0.6667 * (0.866 * inputI2[i] - 0.866 *  inputI3[i]));
    motorElectricalAngularVelocity = inputMotorMechanicalAngularVelocity[i] * modelCVCoeff->nOfPolePairs;


 // coefficients for RK4
    float k1psi2alpha, k2psi2alpha, k3psi2alpha, k4psi2alpha;
    float k1psi2beta, k2psi2beta, k3psi2beta, k4psi2beta;

    // helper variable to reduce calculation fo the same value
    // float halfCalculationStep = odeCVCalculationSettings->calculationStep/2;


    /*------------------------------------------------------------------------------------------------------------------------------------*/
    k1psi2alpha = CurVelModel.psi2alpha(modelCVCoeff, i1alpha, i1beta, psi2alpha[i],  psi2beta[i], motorElectricalAngularVelocity);

    k1psi2beta = CurVelModel.psi2beta(modelCVCoeff, i1alpha, i1beta, psi2alpha[i],  psi2beta[i], motorElectricalAngularVelocity);
    /*------------------------------------------------------------------------------------------------------------------------------------*/

    /*------------------------------------------------------------------------------------------------------------------------------------*/
    k2psi2alpha = CurVelModel.psi2alpha(modelCVCoeff, i1alpha, i1beta, (psi2alpha[i] + (odeCVCalculationSettings->halfCalculationStep * k1psi2alpha)), ( psi2beta[i] + (odeCVCalculationSettings->halfCalculationStep * k1psi2beta)), motorElectricalAngularVelocity);

    k2psi2beta = CurVelModel.psi2beta(modelCVCoeff, i1alpha, i1beta, (psi2alpha[i] + (odeCVCalculationSettings->halfCalculationStep * k1psi2alpha)), ( psi2beta[i] + (odeCVCalculationSettings->halfCalculationStep * k1psi2beta)), motorElectricalAngularVelocity);
    /*------------------------------------------------------------------------------------------------------------------------------------*/

    /*------------------------------------------------------------------------------------------------------------------------------------*/
    k3psi2alpha = CurVelModel.psi2alpha(modelCVCoeff, i1alpha, i1beta, (psi2alpha[i] + (odeCVCalculationSettings->halfCalculationStep * k2psi2alpha)), ( psi2beta[i] + (odeCVCalculationSettings->halfCalculationStep * k2psi2beta)), motorElectricalAngularVelocity);

    k3psi2beta = CurVelModel.psi2beta(modelCVCoeff, i1alpha, i1beta, (psi2alpha[i] + (odeCVCalculationSettings->halfCalculationStep * k2psi2alpha)), ( psi2beta[i] + (odeCVCalculationSettings->halfCalculationStep * k2psi2beta)), motorElectricalAngularVelocity);
    /*------------------------------------------------------------------------------------------------------------------------------------*/

    /*------------------------------------------------------------------------------------------------------------------------------------*/
    k4psi2alpha = CurVelModel.psi2alpha(modelCVCoeff, i1alpha, i1beta, (psi2alpha[i] + (odeCVCalculationSettings->calculationStep * k3psi2alpha)), ( psi2beta[i] + (odeCVCalculationSettings->calculationStep * k3psi2beta)), motorElectricalAngularVelocity);

    k4psi2beta = CurVelModel.psi2beta(modelCVCoeff, i1alpha, i1beta, (psi2alpha[i] + (odeCVCalculationSettings->calculationStep * k3psi2alpha)), ( psi2beta[i] + (odeCVCalculationSettings->calculationStep * k3psi2beta)), motorElectricalAngularVelocity);
    /*------------------------------------------------------------------------------------------------------------------------------------*/



    // updating the values based on calculated coefficients
    /*------------------------------------------------------------------------------------------------------------------------------------*/
    psi2alpha[i] = psi2alpha[i] + ((odeCVCalculationSettings->calculationStep / 6) * (k1psi2alpha + 2 * k2psi2alpha + 2 * k3psi2alpha + k4psi2alpha));

    psi2beta[i] = psi2beta[i] + ((odeCVCalculationSettings->calculationStep / 6) * (k1psi2beta + 2 * k2psi2beta + 2 * k3psi2beta + k4psi2beta));

/****************************************************************************************************/



}





}

}