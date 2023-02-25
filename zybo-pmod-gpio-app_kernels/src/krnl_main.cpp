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

/*
    Main Kernel

*/

void krnl_calculateCurVelModel(odeCVCalculationSettingsType *odeCVCalculationSettings,modelCVVariablesType *modelCVVariables, modelCVCoeffType *modelCVCoeff, float *inputI1, float *inputI2, float *inputI3, float *inputMotorMechanicalAngularVelocity, float * psi2Amplitude ) {

CurVelModelClass CurVelModel;
TransformationClass Transformation;

float timeCV = odeCVCalculationSettings->initialCalculationTime;


/****************************************************************************************************/
/*------------------------------ MAIN MODEL LOOP FOR ACQUIRED INPUTS ------------------------------*/
for(int i = 0; i<= odeCVCalculationSettings->numberOfIterations; i++)
{

    timeCV = timeCV + odeCVCalculationSettings->calculationStep;

    modelCVVariables->i1alpha = Transformation.clarkeTransform1(inputI1[i],inputI2[i], inputI3[i], 0.6667);
    modelCVVariables->i1beta = Transformation.clarkeTransform2(inputI1[i], inputI2[i], inputI3[i], 0.6667);
    modelCVVariables->motorElectricalAngularVelocity = inputMotorMechanicalAngularVelocity[i] * modelCVCoeff->nOfPolePairs;

   CurVelModel.CurVelModelCalculate(modelCVCoeff, modelCVVariables, odeCVCalculationSettings);



    psi2Amplitude[i] = sqrtf((modelCVVariables->psi2alpha * modelCVVariables->psi2alpha) + (modelCVVariables->psi2beta * modelCVVariables->psi2beta));

    
/****************************************************************************************************/



}





}

}