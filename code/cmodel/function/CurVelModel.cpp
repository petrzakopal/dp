#include "../header/CurVelModel.h"
#include <cmath>
#include <stdlib.h>
#include <iostream>
#include "../header/transformation.h"
#include <fstream>


/*-------------------------------------------------------------------------------------------*/
/*-------------------- ALLOCATE MEMORY FOR MOTOR PARAMATERS IN CV MODEL --------------------*/
void CurVelModelClass::motorParametersAllocateMemory()
{
    
    posix_memalign((void **)&motorParameters , 4096 , sizeof(motorCVParametersType) );
    
}
/*---------------------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------------*/
/*-------------------- ALLOCATE MEMORY FOR ODE COEFFICIENTS --------------------*/
void CurVelModelClass::motorCoeffAllocateMemory()
{
    
    posix_memalign((void **)&modelCVCoeff , 4096 , sizeof(modelCVCoeffType) );
    
}
/*-------------------------------------------------------------------------------*/





/*-------------------------------------------------------------------------------*/
/*-------------------- ALLOCATE MEMORY FOR MODEL VARIABLES --------------------*/
void CurVelModelClass::modelCVVariablesAllocateMemory()
{
    
    posix_memalign((void **)&modelCVVariables , 4096 , sizeof(modelCVVariablesType) );
    
}
/*-------------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------------*/
/*----------------------- PRECALCULATE ODE COEFFICIENTS -------------------------*/
void CurVelModelClass::calculateMotorCVCoeff(modelCVCoeffType *modelCVCoeff, motorCVParametersType *motorParameters)
{
    modelCVCoeff->R2DL2 = motorParameters->R2 / motorParameters->L2;
    modelCVCoeff->R2MLmDL2 = (motorParameters->R2 * motorParameters->Lm) / motorParameters->L2;
}
/*--------------------------------------------------------------------------------*/



/*-------------------------------------------------------------------------------*/
/*----------------------- MODEL ODE EQUATIONS OF DY/DT -------------------------*/
float CurVelModelClass::psi2alpha(modelCVCoeffType *modelCVCoeff, float i1alpha, float i1beta, float psi2alpha, float psi2beta, float motorElectricalAngularVelocity )
{
    return((modelCVCoeff->R2MLmDL2 * i1alpha) - (motorElectricalAngularVelocity * psi2beta) - (modelCVCoeff->R2DL2 * psi2alpha));
}

float CurVelModelClass::psi2beta(modelCVCoeffType *modelCVCoeff, float i1alpha, float i1beta, float psi2alpha, float psi2beta, float motorElectricalAngularVelocity)
{
    return((modelCVCoeff->R2MLmDL2 * i1beta) + (motorElectricalAngularVelocity * psi2alpha) - (modelCVCoeff->R2DL2 * psi2beta));
}
/*-------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------*/
/*-------------------- ALLOCATE MEMORY ODE SOLVE SETTINGS --------------------*/
void CurVelModelClass::odeCVCalculationSettingsAllocateMemory()
{
    
    posix_memalign((void **)&odeCVCalculationSettings , 4096 , sizeof(odeCVCalculationSettingsType) );
    
}
/*---------------------------------------------------------------------------------------*/



void CurVelModelClass::CurVelModelCalculate(modelCVCoeffType *modelCVCoeff, modelCVVariablesType *modelCVVariables, odeCVCalculationSettingsType *odeCVCalculationSettings)
{
    float k1psi2alpha, k2psi2alpha, k3psi2alpha, k4psi2alpha;
    float k1psi2beta, k2psi2beta, k3psi2beta, k4psi2beta;

    float halfCalculationStep = odeCVCalculationSettings->calculationStep/2;


    k1psi2alpha = psi2alpha(modelCVCoeff, modelCVVariables->i1alpha, modelCVVariables->i1beta, modelCVVariables->psi2alpha, modelCVVariables->psi2beta, modelCVVariables->motorElectricalAngularVelocity);

    k1psi2beta = psi2beta(modelCVCoeff, modelCVVariables->i1alpha, modelCVVariables->i1beta, modelCVVariables->psi2alpha, modelCVVariables->psi2beta, modelCVVariables->motorElectricalAngularVelocity);

    /*------------------------------------------------------------------------------------------------------------------------------------*/


    k2psi2alpha = psi2alpha(modelCVCoeff, modelCVVariables->i1alpha, modelCVVariables->i1beta, (modelCVVariables->psi2alpha + (halfCalculationStep * k1psi2alpha)), (modelCVVariables->psi2beta + (halfCalculationStep * k1psi2beta)), modelCVVariables->motorElectricalAngularVelocity);

    k2psi2beta = psi2beta(modelCVCoeff, modelCVVariables->i1alpha, modelCVVariables->i1beta, (modelCVVariables->psi2alpha + (halfCalculationStep * k1psi2alpha)), (modelCVVariables->psi2beta + (halfCalculationStep * k1psi2beta)), modelCVVariables->motorElectricalAngularVelocity);

    /*------------------------------------------------------------------------------------------------------------------------------------*/

    k3psi2alpha = psi2alpha(modelCVCoeff, modelCVVariables->i1alpha, modelCVVariables->i1beta, (modelCVVariables->psi2alpha + (halfCalculationStep * k2psi2alpha)), (modelCVVariables->psi2beta + (halfCalculationStep * k2psi2beta)), modelCVVariables->motorElectricalAngularVelocity);

    k3psi2beta = psi2beta(modelCVCoeff, modelCVVariables->i1alpha, modelCVVariables->i1beta, (modelCVVariables->psi2alpha + (halfCalculationStep * k2psi2alpha)), (modelCVVariables->psi2beta + (halfCalculationStep * k2psi2beta)), modelCVVariables->motorElectricalAngularVelocity);


    /*------------------------------------------------------------------------------------------------------------------------------------*/

    k4psi2alpha = psi2alpha(modelCVCoeff, modelCVVariables->i1alpha, modelCVVariables->i1beta, (modelCVVariables->psi2alpha + (odeCVCalculationSettings->calculationStep * k3psi2alpha)), (modelCVVariables->psi2beta + (odeCVCalculationSettings->calculationStep * k3psi2beta)), modelCVVariables->motorElectricalAngularVelocity);

    k4psi2beta = psi2beta(modelCVCoeff, modelCVVariables->i1alpha, modelCVVariables->i1beta, (modelCVVariables->psi2alpha + (odeCVCalculationSettings->calculationStep * k3psi2alpha)), (modelCVVariables->psi2beta + (odeCVCalculationSettings->calculationStep * k3psi2beta)), modelCVVariables->motorElectricalAngularVelocity);


    /*------------------------------------------------------------------------------------------------------------------------------------*/

    modelCVVariables->psi2alpha = modelCVVariables->psi2alpha + ((odeCVCalculationSettings->calculationStep / 6) * (k1psi2alpha + 2 * k2psi2alpha + 2 * k3psi2alpha + k4psi2alpha));

    modelCVVariables->psi2beta = modelCVVariables->psi2beta + ((odeCVCalculationSettings->calculationStep / 6) * (k1psi2beta + 2 * k2psi2beta + 2 * k3psi2beta + k4psi2beta));
    


}