#include "../header/MotorModel.h"
#include <cmath>
#include <stdlib.h>
#include <iostream>


/*---------------------------------------------------------------------------------------*/
/*-------------------- ALLOCATE MEMORY FOR STATE SPACE COEFFICIENTS --------------------*/
void MotorModelClass::motorParametersAllocateMemory()
{
    
    posix_memalign((void **)&motorParameters , 4096 , sizeof(motorParametersType) );
    
}


/*---------------------------------------------------------------------------------------*/
/*-------------------- ALLOCATE MEMORY FOR STATE SPACE COEFFICIENTS --------------------*/
void MotorModelClass::stateSpaceCoeffAllocateMemory()
{
    
    posix_memalign((void **)&stateSpaceCoeff , 4096 , sizeof(stateSpaceCoeffType) );
    
}
/*---------------------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------------------*/
/*-------------------- ALLOCATE MEMORY ODE SOLVE SETTINGS --------------------*/
void MotorModelClass::odeCalculationSettingsAllocateMemory()
{
    
    posix_memalign((void **)&odeCalculationSettings , 4096 , sizeof(odeCalculationSettingsType) );
    
}
/*---------------------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------------------*/
/*---------------------- ALLOCATE MEMORY FOR ODE MODEL VARIABLES -----------------------*/
void MotorModelClass::modelVariablesAllocateMemory()
{
    posix_memalign((void **)&modelVariables , 4096 , ((int)((odeCalculationSettings->finalCalculationTime - odeCalculationSettings->initialCalculationTime)/odeCalculationSettings->calculationStep)) * sizeof(modelVariablesType) );

    
    
}
/*---------------------------------------------------------------------------------------*/


 void MotorModelClass::setOdeCalculationSettings(float initialCalculationTimeInput, float finalCalculationTimeInput, float calculationStepInput)
 {
    
    odeCalculationSettings->initialCalculationTime = initialCalculationTimeInput;
    odeCalculationSettings->finalCalculationTime = finalCalculationTimeInput;
    odeCalculationSettings->calculationStep = calculationStepInput;
    odeCalculationSettings->calculationTime = initialCalculationTimeInput;

 }


/*----------------------------------------------------------------------*/
/*-------------------- SET MOTOR MODEL PARAMETERS --------------------*/
// maybe move to main.c
void MotorModelClass::setMotorParameters()
{
    motorParameters->R1 = 0.370f; // Ohm, stator rezistance
    motorParameters->R2 = 0.225f; // Ohm, rotor rezistance
    motorParameters->L1s = 0.00227f; // H, stator leakage inductance
    motorParameters->L2s = 0.00227f; // H, rotor leakage inductance
    motorParameters->Lm = 0.0825f; // H, main flux inductance
    motorParameters->L1 = 0.08477f; // H, inductance
    motorParameters->L2 = 0.08477f; // H, inductance
    motorParameters->sigma = 0.05283f; // = 0.0528396032, sigma = 1 - Lm^(2)/L1L2
    motorParameters->nOfPolePairs = 2; // number of pole pairs
    motorParameters->momentOfIntertia = 0.4; // J, moment of inertia
}
/*----------------------------------------------------------------------*/


/*-------------------------------------------------------------------------------------*/
/*-------------------- STATIC STATE SPACE COEFFICIENT CALCULATION --------------------*/
void MotorModelClass::setStateSpaceCoeff()
{
    stateSpaceCoeff->a11 =  -(( motorParameters->R2 * motorParameters->Lm * motorParameters->Lm) + ( motorParameters->L2 * motorParameters->L2 * motorParameters->R1) / (motorParameters->sigma * motorParameters->L1 * motorParameters->L2 * motorParameters->L2));

    stateSpaceCoeff->a12 = 0; // same asi a21 = 0;
    stateSpaceCoeff->a13 = (motorParameters->Lm * motorParameters->R2)/(motorParameters->sigma * motorParameters->L1 * motorParameters->L2 * motorParameters->L2);
    // stateSpaceCoeff->a14 is calculated in solo function with motorElectricalAngularVelocity



    stateSpaceCoeff->a21 = 0;
    stateSpaceCoeff->a22 = stateSpaceCoeff->a11;
    // stateSpaceCoeff->a23 is caculated in solo function with motorElectricalAngularVelocity and is negative value of stateSpaceCoeff->a14
    stateSpaceCoeff->a24 = stateSpaceCoeff->a13; // maybe delete in the future to make calculations faster

    stateSpaceCoeff->a31 = (motorParameters->Lm * motorParameters->R2)/(motorParameters->L2);
    stateSpaceCoeff->a32 = 0;
    stateSpaceCoeff->a33 = -((motorParameters->R2)/(motorParameters->L2));
    // stateSpaceCoeff->a34 calculated in solo function with motorElectricalAngularVelocity and is negative value of stateSpaceCoeff->a43

    stateSpaceCoeff->a41 = 0;
    stateSpaceCoeff->a42 = stateSpaceCoeff->a31;
    // stateSpaceCoeff->a43 calculated in solo function with motorElectricalAngularVelocity
    stateSpaceCoeff->a44 = stateSpaceCoeff->a33;


    stateSpaceCoeff->b11 = 1/(motorParameters->sigma * motorParameters->L1);
    stateSpaceCoeff->b22 = stateSpaceCoeff->b11;

}
/*-------------------------------------------------------------------------------------*/




/*----------------------------------------------------------------------------------------------*/
/*-------------------- SAMPLE VARIABLE STATE SPACE COEFFICIENT CALCULATION --------------------*/

// Function for calculating only stateSpaceCoefficients which are not static.
/* Some coefficients are changing with angular velocity of motor, so to provide more resources/time efficient calculation
    calculate some of the coefficients only once and some every new time sample
*/
void MotorModelClass::calculateStateSpaceCoeff(stateSpaceCoeffType *stateSpaceCoeff, motorParametersType *motorParameters, float motorElectricalAngularVelocity)
{
    stateSpaceCoeff->a14 = (motorParameters->Lm * motorElectricalAngularVelocity)/(motorParameters->sigma * motorParameters->L1 * motorParameters->L2);
    stateSpaceCoeff->a23 = - stateSpaceCoeff->a14;
    stateSpaceCoeff->a34 = - motorElectricalAngularVelocity;
    stateSpaceCoeff->a43 = motorElectricalAngularVelocity;
}


/*----------------------------------------------------------------------------------------------*/


/*-----------------------------------------------------------------------*/
/*-------------------- GET STATE SPACE COEFFICIENT --------------------*/
stateSpaceCoeffType* MotorModelClass::getStateSpaceCoeff()
{
    return (stateSpaceCoeff);
}
/*-----------------------------------------------------------------------*/



/*-----------------------------------------------------------------------*/
/*------------------------ GET MOTOR PARAMETERS ------------------------*/
motorParametersType* MotorModelClass::getMotorParameters()
{
    return (motorParameters);
}
/*-----------------------------------------------------------------------*/


/*-----------------------------------------------------------------------*/
/*------------------------ GET MOTOR VARIABLES ------------------------*/
modelVariablesType* MotorModelClass::getMotorVariable(int numberOfSampleInput)
{
    return(&modelVariables[numberOfSampleInput]);
}
/*-----------------------------------------------------------------------*/

/*-----------------------------------------------------------------------*/
/*--------------------- SET INITIAL MODEL VARIABLES ---------------------*/
// Maybe in the future there will be used odeInitialConditions and allocate memory function preceding this part of function definitions.
// Saving initial conditions to index=0, but the starting time can be different from index=0, but calculation will be start from index=0
void MotorModelClass::setInitialModelVariables()
{
    modelVariables[0].i1alpha = 0;
    modelVariables[0].i1beta = 0;
    modelVariables[0].psi2alpha = 0;
    modelVariables[0].motorTorque = 0;
    modelVariables[0].loadTorque = 0;
    modelVariables[0].motorMechanicalAngularVelocity = 0;
}
/*-----------------------------------------------------------------------*/


/*---------------------------------------------------------------*/
/*--------------------- SET MODEL VARIABLE ---------------------*/
void MotorModelClass::setModelVariable(float &variable, float input)
{
    variable = input;
}
/*---------------------------------------------------------------*/



// Defining the differential equation to be solved



float MotorModelClass::i1alpha(stateSpaceCoeffType *stateSpaceCoeff, float i1alpha, float i1beta, float psi2alpha, float psi2beta, float u1alpha)
{
    
    return((stateSpaceCoeff->a11 * i1alpha /* + stateSpaceCoeff->a12 * i1beta => is 0 */ + stateSpaceCoeff->a13 * psi2alpha + stateSpaceCoeff->a14 * psi2beta) + stateSpaceCoeff->b11 * u1alpha);
 
}

float MotorModelClass::i1beta(stateSpaceCoeffType *stateSpaceCoeff, float i1alpha, float i1beta, float psi2alpha, float psi2beta, float u1beta)
{
    
    return((stateSpaceCoeff->a21 * i1alpha + stateSpaceCoeff->a22 * i1beta + stateSpaceCoeff->a23 * psi2alpha + stateSpaceCoeff->a24 * psi2beta)+stateSpaceCoeff->b22 * u1beta);
 
}

float MotorModelClass::psi2alpha(stateSpaceCoeffType *stateSpaceCoeff, float i1alpha, float i1beta, float psi2alpha, float psi2beta)
{
    return(stateSpaceCoeff->a31 * i1alpha /* + stateSpaceCoeff->a32 * i1beta*/ /* = 0*/ + stateSpaceCoeff->a33 * psi2alpha + stateSpaceCoeff->a34 * psi2beta);
}


float MotorModelClass::psi2beta(stateSpaceCoeffType *stateSpaceCoeff, float i1alpha, float i1beta, float psi2alpha, float psi2beta)
{
    return(/*stateSpaceCoeff->a41 * i1alpha = 0 */ + stateSpaceCoeff->a42 * i1beta + stateSpaceCoeff->a43 * psi2alpha + stateSpaceCoeff->a44 * psi2beta);
}


float MotorModelClass::motorTorque(motorParametersType *motorParameters, modelVariablesType *modelVariables)
{
    return(3/2 * motorParameters->nOfPolePairs * (motorParameters->Lm/motorParameters->L2) * (modelVariables->psi2alpha * modelVariables->i1beta - modelVariables->psi2beta * modelVariables->i1alpha));
}

 float MotorModelClass::motorMechanicalAngularVelocity(motorParametersType *motorParameters, modelVariablesType *modelVariables)
 {
    return((modelVariables->motorTorque - modelVariables->loadTorque)/motorParameters->momentOfIntertia);
 }

 float MotorModelClass::motorElectricalAngularVelocity(float motorMechanicalAngularVelocity)
 {
    return((motorParameters->nOfPolePairs * motorMechanicalAngularVelocity));
 }

 float MotorModelClass::u1alpha(float calculationTime)
 {
    return(calculationTime*2);
 }

  float MotorModelClass::u1beta(float calculationTime)
 {
    return(calculationTime*2);
 }

 void MotorModelClass::mathModelCalculate(odeCalculationSettingsType *odeCalculationSettings, modelVariablesType *modelVariables, stateSpaceCoeffType *stateSpaceCoeff, motorParametersType *motorParameters)
 {






    float k1i1alpha, k2i1alpha, k3i1alpha, k4i1alpha;
    float k1i1beta, k2i1beta, k3i1beta, k4i1beta;
    float k1psi2alpha, k2psi2alpha, k3psi2alpha, k4psi2alpha;
    float k1psi2beta, k2psi2beta, k3psi2beta, k4psi2beta;

    int n = (odeCalculationSettings->finalCalculationTime - odeCalculationSettings->initialCalculationTime)/odeCalculationSettings->calculationStep;
    float halfCalculationStep = odeCalculationSettings->calculationStep/2;

    for(int i = 0; i<n; i++ )
    {

        
        // for now there will be externally calculated time, implement later - to calculate time in an array, length based on number of iterations

        k1i1alpha =  i1alpha(getStateSpaceCoeff(), getMotorVariable(i)->i1alpha, getMotorVariable(i)->i1beta, getMotorVariable(i)->psi2alpha, getMotorVariable(i)->psi2beta, u1alpha(odeCalculationSettings->calculationTime));
        k1i1beta =  i1beta(getStateSpaceCoeff(), getMotorVariable(i)->i1alpha, getMotorVariable(i)->i1beta, getMotorVariable(i)->psi2alpha, getMotorVariable(i)->psi2beta, u1beta(odeCalculationSettings->calculationTime));
        k1psi2alpha =  psi2alpha(getStateSpaceCoeff(), getMotorVariable(i)->i1alpha, getMotorVariable(i)->i1beta, getMotorVariable(i)->psi2alpha, getMotorVariable(i)->psi2beta);
        k1psi2beta =  psi2beta(getStateSpaceCoeff(), getMotorVariable(i)->i1alpha, getMotorVariable(i)->i1beta, getMotorVariable(i)->psi2alpha, getMotorVariable(i)->psi2beta);

        k2i1alpha =  i1alpha(getStateSpaceCoeff(), (getMotorVariable(i)->i1alpha+(halfCalculationStep*k1i1alpha)), (getMotorVariable(i)->i1beta+(halfCalculationStep*k1i1beta)), (getMotorVariable(i)->psi2alpha+(halfCalculationStep * k1psi2alpha)), (getMotorVariable(i)->psi2beta + (halfCalculationStep * k1psi2beta)), u1alpha(odeCalculationSettings->calculationTime));

        // std::cout << "k1i1alpha= " << k1i1alpha << "\n";

        odeCalculationSettings->calculationTime = odeCalculationSettings->calculationTime + odeCalculationSettings->calculationStep;
        calculateStateSpaceCoeff(stateSpaceCoeff, motorParameters, 2);
    }

 }