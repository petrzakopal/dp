#include "../header/initialization.h"


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
    posix_memalign((void **)&modelVariables , 4096 , ((int)((odeCalculationSettings->finalCalculationTime - odeCalculationSettings->initialCalculationTime)/odeCalculationSettings->calculationStep)+1) * sizeof(modelVariablesType) );

    
    
}
/*---------------------------------------------------------------------------------------*/


 void MotorModelClass::setOdeCalculationSettings(float initialCalculationTimeInput, float finalCalculationTimeInput, float calculationStepInput)
 {
    
    odeCalculationSettings->initialCalculationTime = initialCalculationTimeInput;
    odeCalculationSettings->finalCalculationTime = finalCalculationTimeInput;
    odeCalculationSettings->calculationStep = calculationStepInput;

 }


/*----------------------------------------------------------------------*/
/*-------------------- SET MOTOR MODEL PARAMETERS --------------------*/
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
void MotorModelClass::calculateStateSpaceCoeff(float motorElectricalAngularVelocity)
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
    modelVariables[0].u1alpha = 0;
    modelVariables[0].u2alpha = 0;
}
/*-----------------------------------------------------------------------*/


/*---------------------------------------------------------------*/
/*--------------------- SET MODEL VARIABLE ---------------------*/
void MotorModelClass::setModelVariable(float &variable, float input)
{
    variable = input;
}
/*---------------------------------------------------------------*/


