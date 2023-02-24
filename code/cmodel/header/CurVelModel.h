#include <stdlib.h>
#include <cmath>



/*---------------------------------------------------------------------------------------------------*/
/*-------------------- TYPE/STRUCT FOR MOTOR MODEL PARAMETERS WHICH ARE MEASURED --------------------*/
typedef struct motorCVParametersStruct
{
    float R2;
    float Lm;
    float L2;
    int nOfPolePairs;
}motorCVParametersType;
/*---------------------------------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------------------------------*/
/*-------------------- TYPE/STRUCT FOR MOTOR MODEL PARAMETERS WHICH ARE MEASURED --------------------*/
typedef struct modelCVCoeffStruct
{
    float R2MLmDL2;
    float R2DL2;
}modelCVCoeffType;
/*---------------------------------------------------------------------------------------------------*/


/*------------------------------------------------------------------------------------------------------------------*/
/*-------------------- TYPE/STRUCT FOR VARIABLE VARIABLES FOR MODEL/ODE SOLVING ------------------------------------------*/
typedef struct modelCVVariablesStruct
{
    float i1alpha;
    float i1beta;
    float psi2alpha;
    float psi2beta;
    float motorElectricalAngularVelocity;

    // vector regulation paste in a separate type struct
    // float psi2;
    // float u1alpha;
    // float u1beta;
    // float u1d;
    // float u1q;

}modelCVVariablesType;
/*------------------------------------------------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------------*/
/*-------------------- TYPE/STRUCT FOR FOR ODE SETTINGS --------------------------*/
typedef struct odeCVCalculationSettingsStruct
{
    float initialCalculationTime;
    float finalCalculationTime;
    float calculationStep;
    float calculationTime;
    int numberOfIterations;
}odeCVCalculationSettingsType;
/*---------------------------------------------------------------------------------------------------*/


class CurVelModelClass
{

    public:
    motorCVParametersType *motorParameters = NULL;
    modelCVCoeffType *modelCVCoeff = NULL;
    modelCVVariablesType *modelCVVariables = NULL;
    odeCVCalculationSettingsType *odeCVCalculationSettings = NULL;
    void odeCVCalculationSettingsAllocateMemory();


    void motorParametersAllocateMemory();
    void motorCoeffAllocateMemory();
    void modelCVVariablesAllocateMemory();

    void calculateMotorCVCoeff(modelCVCoeffType *modelCVCoeff, motorCVParametersType *motorParameters);

    float psi2alpha(modelCVCoeffType *modelCVCoeff, float i1alpha, float i1beta, float psi2alpha, float psi2beta, float motorElectricalAngularVelocity );
    float psi2beta(modelCVCoeffType *modelCVCoeff, float i1alpha, float i1beta, float psi2alpha, float psi2beta, float motorElectricalAngularVelocity );

    void CurVelModelCalculate(modelCVCoeffType *modelCVCoeff, modelCVVariablesType *modelCVVariables, odeCVCalculationSettingsType *odeCVCalculationSettings);

};