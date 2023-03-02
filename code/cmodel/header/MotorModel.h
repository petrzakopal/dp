#include <stdlib.h>
#include <cmath>
// #include "transformation.h"
/* MotorModel.h
   basic model of asynchronous motor not for regulation but for motor model simulation
*/



/*---------------------------------------------------------------------------------------------------*/
/*-------------------- TYPE/STRUCT FOR MOTOR MODEL PARAMETERS WHICH ARE MEASURED --------------------*/
typedef struct motorParametersStruct
{
    float R1;
    float R2;
    float L1s;
    float L2s;
    float Lm;
    float L1;
    float L2;
    float sigma; // sigma = 1 - Lm^(2)/L1L2, it would be probably more clear to read value from struct, but it would mean more operations
    int nOfPolePairs;
    float momentOfIntertia;
}motorParametersType;
/*---------------------------------------------------------------------------------------------------*/






/*----------------------------------------------------------------------------------*/
/*-------------------- TYPE/STRUCT FOR FOR ODE SETTINGS --------------------------*/
typedef struct odeCalculationSettingsStruct
{
    float initialCalculationTime;
    float finalCalculationTime;
    float calculationStep;
    float calculationTime;
    int numberOfIterations;
}odeCalculationSettingsType;
/*---------------------------------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------------------------------------------*/
/*-------------------- TYPE/STRUCT FOR VARIABLE VARIABLES FOR MODEL/ODE SOLVING ------------------------------------------*/
typedef struct modelVariablesStruct
{
    float i1alpha;
    float i1beta;
    float psi2alpha;
    float psi2beta;
    // float u1alpha;
    // float u1beta;
    float motorTorque;
    float loadTorque;
    float motorMechanicalAngularVelocity;
}modelVariablesType;
/*------------------------------------------------------------------------------------------------------------------*/


typedef struct voltageGeneratorStruct
{
    float u1;
    float u2;
    float u3;
    float u1alpha;
    float u1beta;
    float voltageFrequency;
    float voltageAmplitude;

}voltageGeneratorType;


/*-----------------------------------------------------------------------------------------*/
/*-------------------- TYPE/STRUCT FOR STATE SPACE MATRIX COEFFICIENTS -------------------*/

/* state space matrix coefficients
   they need to be calculated only once and very fast
   should be calculated in kernel before ODE Numerical method calculation




amn
m = row
n = column

state matrix
( a11 a12 a13 a14 )
( a21 a22 a23 a24 )
( a31 a32 a33 a34 )
( a41 a42 a43 a44 )



*/
typedef struct stateSpaceCoeffStruct
{

    // state matrix

    // first row
    float a11;
    float a12;
    float a13;
    float a14;

    // second row
    float a21;
    float a22;
    float a23;
    float a24;

    // third row
    float a31;
    float a32;
    float a33;
    float a34;

    // fourth row
    float a41;
    float a42;
    float a43;
    float a44;

    // end of state matrix

    // input matrix
    float b11; // = b22
    float b22;
    // end of input matrix

    
}stateSpaceCoeffType;
/*-----------------------------------------------------------------------------------------*/



/*-----------------------------------------------------------*/
/*-------------------- MAIN MODEL CLASS -------------------*/
class MotorModelClass
{
    public:
    motorParametersType *motorParameters = NULL;
    stateSpaceCoeffType *stateSpaceCoeff = NULL;
    modelVariablesType *modelVariables = NULL;
    odeCalculationSettingsType *odeCalculationSettings = NULL;
    voltageGeneratorType *voltageGeneratorData = NULL;

    


    void motorParametersAllocateMemory();
    void stateSpaceCoeffAllocateMemory();
    void modelVariablesAllocateMemory();
    void odeCalculationSettingsAllocateMemory();
    void voltageGeneratorDataAllocateMemory();

    void setMotorParameters();
    void setStateSpaceCoeff();
    void calculateStateSpaceCoeff(stateSpaceCoeffType *stateSpaceCoeff, motorParametersType *motorParameters, float motorElectricalAngularVelocity);
    void setInitialModelVariables();
    void setVariable(float &variable, float input);
    void setOdeCalculationSettings(float initialCalculationTimeInput, float finalCalculationTimeInput, float calculationStepInput);
   
    voltageGeneratorType* getVoltage(int numberOfSampleInput);
    motorParametersType* getMotorParameters();
    stateSpaceCoeffType* getStateSpaceCoeff();
    modelVariablesType* getMotorVariable(int indexOfSample);

    float i1alpha(stateSpaceCoeffType *stateSpaceCoeff, float i1alpha, float i1beta, float psi2alpha, float psi2beta, float u1alpha);
    float i1beta(stateSpaceCoeffType *stateSpaceCoeff, float i1alpha, float i1beta, float psi2alpha, float psi2beta, float u1beta);
    float psi2alpha(stateSpaceCoeffType *stateSpaceCoeff, float i1alpha, float i1beta, float psi2alpha, float psi2beta);
    float psi2beta(stateSpaceCoeffType *stateSpaceCoeff, float i1alpha, float i1beta, float psi2alpha, float psi2beta);

    float motorTorque(motorParametersType *motorParameters, modelVariablesType *modelVariables);
    float motorMechanicalAngularVelocity(float motorTorque, motorParametersType *motorParameters, modelVariablesType *modelVariables);
    float motorElectricalAngularVelocity(float motorMechanicalAngularVelocity);


    float voltageGenerator(float calculationTime, float phase, float amplitude, float frequency);

    void precalculateVoltageSource(odeCalculationSettingsType *odeCalculationSettings, float amplitude, float frequency);

    void precalculateVoltageClarke(voltageGeneratorType *voltageGeneratorData, odeCalculationSettingsType *odeCalculationSettings);


    /*-------------------------------------------------------------------------------------------------------------------*/
    void mathModelCalculate(odeCalculationSettingsType *odeCalculationSettings, modelVariablesType *modelVariables, stateSpaceCoeffType *stateSpaceCoeff, motorParametersType *motorParameters);
    /*-------------------------------------------------------------------------------------------------------------------*/

    
    int numberOfIterations();
};
/*-----------------------------------------------------------*/