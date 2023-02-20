#include <stdlib.h>
/* initialization.h
    file for:
            initialization of parameters, structs and types
            declaration of functions to initialize values
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
}motorParametersType;
/*---------------------------------------------------------------------------------------------------*/






/*---------------------------------------------------------------------------------------------------*/
/*-------------------- TYPE/STRUCT FOR INITIAL CONDITIONS FOR ODE SOLVING --------------------------*/
typedef struct odeInitialConditionsStruct
{
    float t0;
    float i1alpha0;
    float i1beta0;
    float psi2alpha0;
    float psi2beta0;
}odeInitialConditionsType;
/*---------------------------------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------------------------------------------*/
/*-------------------- TYPE/STRUCT FOR VARIABLE VARIABLES FOR MODEL/ODE SOLVING ------------------------------------------*/
typedef struct ModelVariablesStruct
{
    float i1alpha;
    float i1beta;
    float psi2alpha;
    float psi2beta;
    float u1alpha;
    float u2alpha;
}ModelVariablesType;
/*------------------------------------------------------------------------------------------------------------------*/





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
    odeInitialConditionsType *odeInitialConditions = NULL;
    ModelVariablesType *ModelVariables = NULL;

    void motorParametersAllocateMemory();
    void stateSpaceCoeffAllocateMemory();
    void odeInitialConditionsAllocateMemory();
    void ModelVariablesAllocateMemory();

    void setMotorParameters();
    void setStateSpaceCoeff();
    void calculateStateSpaceCoeff(float motorElectricalAngularVelocity);

    motorParametersType* getMotorParameters();
    stateSpaceCoeffType* getStateSpaceCoeff();


};
/*-----------------------------------------------------------*/