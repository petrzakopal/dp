#include "../header/MotorModel.h"
#include <cmath>
#include <stdlib.h>
#include <iostream>
#include "../header/transformation.h"
#include <fstream>

#define PI 3.141592 


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


/*----------------------  -----------------------*/
void MotorModelClass::voltageGeneratorDataAllocateMemory()
{
    posix_memalign((void **)&voltageGeneratorData , 4096 , ((int)ceil(((odeCalculationSettings->finalCalculationTime - odeCalculationSettings->initialCalculationTime)/odeCalculationSettings->calculationStep))) * sizeof(voltageGeneratorType) );

    
    
}
/*---------------------------------------------------------------------------------------*/



void MotorModelClass::modelVariablesAllocateMemory()
{
    posix_memalign((void **)&modelVariables , 4096 , ((int)ceil(((odeCalculationSettings->finalCalculationTime - odeCalculationSettings->initialCalculationTime)/odeCalculationSettings->calculationStep))) * sizeof(modelVariablesType) );

    
    
}


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
    modelVariables[0].loadTorque = 0.;
    modelVariables[0].motorMechanicalAngularVelocity = 0;
}
/*-----------------------------------------------------------------------*/


/*---------------------------------------------------------------*/
/*--------------------- SET MODEL VARIABLE ---------------------*/
void MotorModelClass::setVariable(float &variable, float input)
{
    variable = input;
}
/*---------------------------------------------------------------*/



// Defining the differential equation to be solved



/*----------------------------------------------------------------------------------------------------*/
/*--------------------- DIFFERENTIAL EQUATIONS FROM STATE SPACE MODEL DEFINITION ---------------------*/
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

/*----------------------------------------------------------------------------------------------------*/




/*******************************************************************************************************/
/*---------------------------- MOTOR MODEL EQUATIONS FOR COMPLETE MODEL -----------------------------*/

/*-----------------------------------------------------------------------------------------*/
/*---------------------------- MOTOR ELECTROMAGNETIC TORQUE -----------------------------*/
float MotorModelClass::motorTorque(motorParametersType *motorParameters, modelVariablesType *modelVariables)
{
    return(3/2 * motorParameters->nOfPolePairs * (motorParameters->Lm / motorParameters->L2) * (modelVariables->psi2alpha * modelVariables->i1beta - modelVariables->psi2beta * modelVariables->i1alpha));
}
/*-----------------------------------------------------------------------------------------*/


/*-------------------------------------------------------------------------------------------------------------------*/
/*---------------------------- MOTOR MECHANICAL ANGULAR VELOCITY DIFFERENTIAL EQUATION -----------------------------*/
// TO BE SOLVED BY RK4 AFTER MAIN STATE SPACE CALCULATION
 float MotorModelClass::motorMechanicalAngularVelocity(float motorTorque, motorParametersType *motorParameters, modelVariablesType *modelVariables)
 {
    return((motorTorque - modelVariables->loadTorque)/motorParameters->momentOfIntertia);
 }

 /*-------------------------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------*/
/*---------------------------- MOTOR ELECTRICAL ANGULAR VELOCITY -----------------------------*/
 float MotorModelClass::motorElectricalAngularVelocity(float motorMechanicalAngularVelocity)
 {
    return((motorParameters->nOfPolePairs * motorMechanicalAngularVelocity));
 }
 /*---------------------------------------------------------------------------------------------*/

/*******************************************************************************************************/




/*---------------------------------------------------------------------------------------------*/
/*------------------------------ VOTAGE SINUS SOURCE GENERATOR -------------------------------*/
 float MotorModelClass::voltageGenerator(float calculationTime, float phase, float amplitude, float frequency)
 {
    return(amplitude * sin((2 * PI * frequency * calculationTime) + phase));
 }
 /*---------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------*/
/*------------------------------ VOTAGE VARIABLE GETTER -------------------------------*/
 voltageGeneratorType* MotorModelClass::getVoltage(int numberOfSampleInput)
{
    return(&voltageGeneratorData[numberOfSampleInput]);
}
/*--------------------------------------------------------------------------------------*/


/*-----------------------------------------------------------------------------------------------------------------------------------------*/
/*------------------------------ PRE CALCULATING SOURCE VOLTAGE THROUGH TIME BASED OD NUMBER OF ITERATIONS -------------------------------*/
void MotorModelClass::precalculateVoltageSource(voltageGeneratorType *voltageGeneratorData, odeCalculationSettingsType *odeCalculationSettings, float amplitude, float frequency)
{
    int n = ceil((odeCalculationSettings->finalCalculationTime - odeCalculationSettings->initialCalculationTime)/odeCalculationSettings->calculationStep);
    float time = odeCalculationSettings->initialCalculationTime;

    for(int i = 0; i<=n;i++)
    {
        setVariable(getVoltage(i)->u1, voltageGenerator(time, 0, amplitude, frequency));
       

        setVariable(getVoltage(i)->u2, voltageGenerator(time, 2.0943, amplitude, frequency));
       
        setVariable(getVoltage(i)->u3, voltageGenerator(time, -2.0943, amplitude, frequency));
        time = time + odeCalculationSettings->calculationStep;
    }
    
}
/*----------------------------------------------------------------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------------------------------------------------------------------*/
/*------------------------------ PRE CALCULATING CLARKE VOLTAGE THROUGH TIME BASED OD NUMBER OF ITERATIONS -------------------------------*/
void MotorModelClass::precalculateVoltageClarke(voltageGeneratorType *voltageGeneratorData, odeCalculationSettingsType *odeCalculationSettings)
{
    int n = ceil((odeCalculationSettings->finalCalculationTime - odeCalculationSettings->initialCalculationTime)/odeCalculationSettings->calculationStep);
    float time = odeCalculationSettings->initialCalculationTime;

    TransformationClass Transformation;
    for(int i = 0; i<=n;i++)
    {
        setVariable(getVoltage(i)->u1alpha, Transformation.clarkeTransform1(getVoltage(i)->u1, getVoltage(i)->u2, getVoltage(i)->u3, 0.666));

    
        setVariable(getVoltage(i)->u1beta, Transformation.clarkeTransform2(getVoltage(i)->u1, getVoltage(i)->u2, getVoltage(i)->u3, 0.666));

        time = time + odeCalculationSettings->calculationStep;

    }
    
}
/*----------------------------------------------------------------------------------------------------------------------------------------*/


/*-----------------------------------------------------------------------------------------------------------------------------------------------*/
/*------------------------------ MAIN STATE SPACE MODEL PLUS TORQUE PLUS VELOCITY CALCULATIONS WITH RK4s FOR ODE --------------------------------*/
 void MotorModelClass::mathModelCalculate(odeCalculationSettingsType *odeCalculationSettings, modelVariablesType *modelVariables, stateSpaceCoeffType *stateSpaceCoeff, motorParametersType *motorParameters)
 {
// do kernelu se budou muset namapovat všechny dotčené proměnné => todo: vyřešit, které budou vstupovat díky classe a ne jako vstup jao v c++


    std::ofstream myfile;
    myfile.open ("output.csv");
    myfile<< "time,i1alpha,u1alpha\n";
    /* |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||*/
    /*|||||||||||||||||||||||| RK4 FOR STATE SPACE MODEL ||||||||||||||||||||||||*/
    float k1i1alpha, k2i1alpha, k3i1alpha, k4i1alpha;
    float k1i1beta, k2i1beta, k3i1beta, k4i1beta;
    float k1psi2alpha, k2psi2alpha, k3psi2alpha, k4psi2alpha;
    float k1psi2beta, k2psi2beta, k3psi2beta, k4psi2beta;
    /* |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||*/


    int n = ceil((odeCalculationSettings->finalCalculationTime - odeCalculationSettings->initialCalculationTime)/odeCalculationSettings->calculationStep);
    float halfCalculationStep = odeCalculationSettings->calculationStep/2;
    
    calculateStateSpaceCoeff(stateSpaceCoeff, motorParameters, motorElectricalAngularVelocity(getMotorVariable(0)->motorMechanicalAngularVelocity));
    for(int i = 0; i<n; i++ )
    {

       
        // for now there will be externally calculated time, implement later - to calculate time in an array, length based on number of iterations

        k1i1alpha =  i1alpha(getStateSpaceCoeff(), getMotorVariable(i)->i1alpha, getMotorVariable(i)->i1beta, getMotorVariable(i)->psi2alpha, getMotorVariable(i)->psi2beta, getVoltage(i)->u1alpha);
        k1i1beta =  i1beta(getStateSpaceCoeff(), getMotorVariable(i)->i1alpha, getMotorVariable(i)->i1beta, getMotorVariable(i)->psi2alpha, getMotorVariable(i)->psi2beta, getVoltage(i)->u1beta);
        k1psi2alpha =  psi2alpha(getStateSpaceCoeff(), getMotorVariable(i)->i1alpha, getMotorVariable(i)->i1beta, getMotorVariable(i)->psi2alpha, getMotorVariable(i)->psi2beta);
        k1psi2beta =  psi2beta(getStateSpaceCoeff(), getMotorVariable(i)->i1alpha, getMotorVariable(i)->i1beta, getMotorVariable(i)->psi2alpha, getMotorVariable(i)->psi2beta);
        
        // std::cout << "iteration: "<< i << " k1i1alpha from loop: " << k1i1alpha <<"\n";
        // std::cout << "iteration: "<< i << " k1i1beta from loop: " << k1i1beta <<"\n";
        // std::cout << "iteration: "<< i << " k1psi2alpha from loop: " << k1psi2alpha <<"\n";
        // std::cout << "iteration: "<< i << " k1psi2beta from loop: " << k1psi2beta <<"\n";


        /*------------------------------------------------------------------------------------------------------------------------------------*/
       
       
        // std::cout <<"iteration: "<< i <<  " k2i1alpha from loop: " << k2i1alpha <<"\n";
        // std::cout << "iteration: "<< i << " k2i1beta from loop: " << k2i1beta <<"\n";
        // std::cout << "iteration: "<< i << " k2psi2alpha from loop: " << k2psi2alpha <<"\n";
        // std::cout << "iteration: "<< i << " k2psi2beta from loop: " << k2psi2beta <<"\n";
       
        k2i1alpha =  i1alpha(getStateSpaceCoeff(), (getMotorVariable(i)->i1alpha+(halfCalculationStep*k1i1alpha)), (getMotorVariable(i)->i1beta+(halfCalculationStep*k1i1beta)), (getMotorVariable(i)->psi2alpha+(halfCalculationStep * k1psi2alpha)), (getMotorVariable(i)->psi2beta + (halfCalculationStep * k1psi2beta)), getVoltage(i)->u1alpha);


        k2i1beta =  i1beta(getStateSpaceCoeff(), (getMotorVariable(i)->i1alpha+(halfCalculationStep*k1i1alpha)), (getMotorVariable(i)->i1beta+(halfCalculationStep*k1i1beta)), (getMotorVariable(i)->psi2alpha+(halfCalculationStep * k1psi2alpha)), (getMotorVariable(i)->psi2beta + (halfCalculationStep * k1psi2beta)), getVoltage(i)->u1alpha);

        k2psi2alpha =  psi2alpha(getStateSpaceCoeff(), (getMotorVariable(i)->i1alpha+(halfCalculationStep*k1i1alpha)), (getMotorVariable(i)->i1beta+(halfCalculationStep*k1i1beta)), (getMotorVariable(i)->psi2alpha+(halfCalculationStep * k1psi2alpha)), (getMotorVariable(i)->psi2beta + (halfCalculationStep * k1psi2beta)));

        k2psi2beta =  psi2beta(getStateSpaceCoeff(), (getMotorVariable(i)->i1alpha+(halfCalculationStep*k1i1alpha)), (getMotorVariable(i)->i1beta+(halfCalculationStep*k1i1beta)), (getMotorVariable(i)->psi2alpha+(halfCalculationStep * k1psi2alpha)), (getMotorVariable(i)->psi2beta + (halfCalculationStep * k1psi2beta)));
        /*------------------------------------------------------------------------------------------------------------------------------------*/


        // std::cout << "iteration: "<< i << " k3i1alpha from loop: " << k3i1alpha <<"\n";
        // std::cout << "iteration: "<< i << " k3i1beta from loop: " << k3i1beta <<"\n";
        // std::cout << "iteration: "<< i << " k3psi2alpha from loop: " << k3psi2alpha <<"\n";
        // std::cout << "iteration: "<< i << " k3psi2beta from loop: " << k3psi2beta <<"\n";

        k3i1alpha =  i1alpha(getStateSpaceCoeff(), (getMotorVariable(i)->i1alpha+(halfCalculationStep*k2i1alpha)), (getMotorVariable(i)->i1beta+(halfCalculationStep*k2i1beta)), (getMotorVariable(i)->psi2alpha+(halfCalculationStep * k2psi2alpha)), (getMotorVariable(i)->psi2beta + (halfCalculationStep * k2psi2beta)), getVoltage(i)->u1alpha);

        k3i1beta =  i1beta(getStateSpaceCoeff(), (getMotorVariable(i)->i1alpha+(halfCalculationStep*k2i1alpha)), (getMotorVariable(i)->i1beta+(halfCalculationStep*k2i1beta)), (getMotorVariable(i)->psi2alpha+(halfCalculationStep * k2psi2alpha)), (getMotorVariable(i)->psi2beta + (halfCalculationStep * k2psi2beta)), getVoltage(i)->u1alpha);

        k3psi2alpha =  psi2alpha(getStateSpaceCoeff(), (getMotorVariable(i)->i1alpha+(halfCalculationStep*k2i1alpha)), (getMotorVariable(i)->i1beta+(halfCalculationStep*k2i1beta)), (getMotorVariable(i)->psi2alpha+(halfCalculationStep * k2psi2alpha)), (getMotorVariable(i)->psi2beta + (halfCalculationStep * k2psi2beta)));

        k3psi2beta =  psi2beta(getStateSpaceCoeff(), (getMotorVariable(i)->i1alpha+(halfCalculationStep*k2i1alpha)), (getMotorVariable(i)->i1beta+(halfCalculationStep*k2i1beta)), (getMotorVariable(i)->psi2alpha+(halfCalculationStep * k2psi2alpha)), (getMotorVariable(i)->psi2beta + (halfCalculationStep * k2psi2beta)));
        /*------------------------------------------------------------------------------------------------------------------------------------*/



        // std::cout << "iteration: "<< i << " k4i1alpha from loop: " << k4i1alpha <<"\n";
        // std::cout << "iteration: "<< i << " k4i1beta from loop: " << k4i1beta <<"\n";
        // std::cout << "iteration: "<< i << " k4psi2alpha from loop: " << k4psi2alpha <<"\n";
        // std::cout << "iteration: "<< i << " k4psi2beta from loop: " << k4psi2beta <<"\n";

        k4i1alpha =  i1alpha(getStateSpaceCoeff(), (getMotorVariable(i)->i1alpha+(odeCalculationSettings->calculationStep*k3i1alpha)), (getMotorVariable(i)->i1beta+(odeCalculationSettings->calculationStep*k3i1beta)), (getMotorVariable(i)->psi2alpha+(odeCalculationSettings->calculationStep * k3psi2alpha)), (getMotorVariable(i)->psi2beta + (odeCalculationSettings->calculationStep * k3psi2beta)), getVoltage(i)->u1alpha);

        k4i1beta =  i1beta(getStateSpaceCoeff(), (getMotorVariable(i)->i1alpha+(odeCalculationSettings->calculationStep*k3i1alpha)), (getMotorVariable(i)->i1beta+(odeCalculationSettings->calculationStep*k3i1beta)), (getMotorVariable(i)->psi2alpha+(odeCalculationSettings->calculationStep * k3psi2alpha)), (getMotorVariable(i)->psi2beta + (odeCalculationSettings->calculationStep * k3psi2beta)), getVoltage(i)->u1alpha);

        k4psi2alpha =  psi2alpha(getStateSpaceCoeff(), (getMotorVariable(i)->i1alpha+(odeCalculationSettings->calculationStep*k3i1alpha)), (getMotorVariable(i)->i1beta+(odeCalculationSettings->calculationStep*k3i1beta)), (getMotorVariable(i)->psi2alpha+(odeCalculationSettings->calculationStep * k3psi2alpha)), (getMotorVariable(i)->psi2beta + (odeCalculationSettings->calculationStep * k3psi2beta)));

        k4psi2beta =  psi2beta(getStateSpaceCoeff(), (getMotorVariable(i)->i1alpha+(odeCalculationSettings->calculationStep*k3i1alpha)), (getMotorVariable(i)->i1beta+(odeCalculationSettings->calculationStep*k3i1beta)), (getMotorVariable(i)->psi2alpha+(odeCalculationSettings->calculationStep * k3psi2alpha)), (getMotorVariable(i)->psi2beta + (odeCalculationSettings->calculationStep * k3psi2beta)));
        /*------------------------------------------------------------------------------------------------------------------------------------*/

        // nevím zda tu mám nechat i nebo i+1
        setVariable(getMotorVariable(i)->i1alpha,(getMotorVariable(i)->i1alpha + odeCalculationSettings->calculationStep/6 *(k1i1alpha + 2* k2i1alpha + 2* k3i1alpha + k4i1alpha)));

        setVariable(getMotorVariable(i)->i1beta,(getMotorVariable(i)->i1beta + odeCalculationSettings->calculationStep/6 *(k1i1beta + 2* k2i1beta + 2* k3i1beta + k4i1beta)));

        setVariable(getMotorVariable(i)->psi2alpha,(getMotorVariable(i)->psi2alpha + odeCalculationSettings->calculationStep/6 *(k1psi2alpha + 2* k2psi2alpha + 2* k3psi2alpha + k4psi2alpha)));

        setVariable(getMotorVariable(i)->psi2beta,(getMotorVariable(i)->psi2beta + odeCalculationSettings->calculationStep/6 *(k1psi2beta + 2* k2psi2beta + 2* k3psi2beta + k4psi2beta)));
        /*------------------------------------------------------------------------------------------------------------------------------------*/

        // std::cout << "k1i1alpha= " << k1i1alpha << "\n";

        

        // calculating torque
        setVariable(getMotorVariable(i)->motorTorque, motorTorque(motorParameters, getMotorVariable(i)));

        // std::cout << "motor torque from loop: " << getMotorVariable(i)->motorTorque <<"\n";
        // std::cout << "k2i1alpha from loop: " << k2i1alpha <<"\n";
        std::cout << "calculation time: " <<  odeCalculationSettings->calculationTime << "\n";
        std::cout << "i1alpha from loop: " << getMotorVariable(i)->i1alpha <<"\n";
        myfile<<odeCalculationSettings->calculationTime<< ","<< getMotorVariable(i)->i1alpha<< "," << getVoltage(i)->u1 << "\n";

        /* |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||*/
        /*|||||||||||||||||||||||| VELOCITY BY INTEGRATION||||||||||||||||||||||||*/
        /* assumptions:
        getMotorVariable(i)->motorTorque at (i) is constant value
        getMotorVariable(i)->loadTorque at (i) is a constant value
        momentOfIntertia from motorParameters at (i) is a constant value
       */
        setVariable(getMotorVariable(i)->motorMechanicalAngularVelocity, (1/motorParameters->momentOfIntertia)*(getMotorVariable(i)->motorTorque - getMotorVariable(i)->loadTorque)*(odeCalculationSettings->calculationTime-odeCalculationSettings->initialCalculationTime));

        
        
        // here the angular velocity needs to be calculated - new RK4 in state space RK4
        calculateStateSpaceCoeff(stateSpaceCoeff, motorParameters, motorElectricalAngularVelocity(getMotorVariable(i)->motorMechanicalAngularVelocity));


        

        odeCalculationSettings->calculationTime = odeCalculationSettings->calculationTime + odeCalculationSettings->calculationStep;

        /*------------------------------------------------------------------------------------------------------------------------------------*/
    }

    myfile.close();

 }