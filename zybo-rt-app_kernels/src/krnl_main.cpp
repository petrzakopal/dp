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

// void loadParameters1(float parameter, float *modelCVCoeffArrayKernel)
// {
//     for(int i = 0;i<8;i++)
//         {
//         modelCVCoeffArrayKernel[i] = parameter;
//         }
// }
void sliceInternalVariables4Parts(float variableIn, float *variableOut)
{
    for(int i = 0;i<4;i++)
    {
        variableOut[i] = variableIn;
    }
}

void sliceInternalVariables8Parts(float variableIn, float *variableOut)
{
    for(int i = 0;i<8;i++)
    {
        variableOut[i] = variableIn;
    }
}


static void loadVariable(float variableIn, float *variableOut)
{
    *variableOut = variableIn;
}

// second type kernel
void krnl_calculateCurVelModel(float *odeCVCalculationSettingsArray, float *modelCVCoeffArray, float *inputI1, float *inputI2, float *inputI3, float *inputMotorMechanicalAngularVelocity, float *psi2alphaOut, float *psi2betaOut, float *psi2Amplitude, float *transformAngle ) {


// nevejde se do ZynQ
// #pragma HLS INTERFACE mode = m_axi port = odeCVCalculationSettingsArray bundle = gmem0
// #pragma HLS INTERFACE mode = m_axi port = modelCVCoeffArray bundle = gmem0
// #pragma HLS INTERFACE mode = m_axi port = inputI1 bundle = gmem0
// #pragma HLS INTERFACE mode = m_axi port = inputI2 bundle = gmem0
// #pragma HLS INTERFACE mode = m_axi port = inputI3 bundle = gmem0
// #pragma HLS INTERFACE mode = m_axi port = inputMotorMechanicalAngularVelocity bundle = gmem0
// #pragma HLS INTERFACE mode = m_axi port = psi2alphaOut bundle = gmem6
// #pragma HLS INTERFACE mode = m_axi port = psi2betaOut bundle = gmem6

float timeCV[1];
float halfCalculationStep[1];
int numberOfIterations[1];
float psi2alpha[1];
float psi2beta[1];
float R2MLmDL2[1];
float R2DL2[1];
float R2MLmDL2Temp[8];
float R2DL2Temp[8];
float i1alpha;
float i1beta;
float motorElectricalAngularVelocity;
// coefficients for RK4
float k1psi2alpha, k2psi2alpha, k3psi2alpha, k4psi2alpha;
float k1psi2beta, k2psi2beta, k3psi2beta, k4psi2beta;
float psi2alphaTemp[8];
float psi2betaTemp[8];
float i1alphaTemp[8];
float i1betaTemp[8];
float motorElectricalAngularVelocityTemp[8];
float k1psi2alphaTemp[4];
float k1psi2betaTemp[4];
float k2psi2alphaTemp[4];
float k2psi2betaTemp[4];
float k3psi2alphaTemp[4];
float k3psi2betaTemp[4];
float k4psi2alphaTemp1;
float k4psi2betaTemp1;

numberOfIterations[0] = (int)odeCVCalculationSettingsArray[4];

    #pragma HLS pipeline II=1
    loadVariable(odeCVCalculationSettingsArray[0], timeCV);
    loadVariable(odeCVCalculationSettingsArray[2]/2,halfCalculationStep);
    // loadVariable(odeCVCalculationSettingsArray[4],numberOfIterations);
    loadVariable(psi2alphaOut[0],psi2alpha);
    loadVariable(psi2betaOut[0], psi2beta);


    sliceInternalVariables8Parts(modelCVCoeffArray[1], R2MLmDL2Temp);
    sliceInternalVariables8Parts(modelCVCoeffArray[1], R2DL2Temp);





/****************************************************************************************************/
/*------------------------------ MAIN MODEL LOOP FOR ACQUIRED INPUTS ------------------------------*/
for(int i = 0; i< numberOfIterations[0]; i++)
{

    #pragma HLS performance target_ti=1
    #pragma HLS loop_tripcount max=numberOfIterations[0]
    
    // for(int i = 0; i<8;i++)
    // {
    //     psi2alphaTemp[i] = psi2alpha;
    // }
    
    sliceInternalVariables8Parts(psi2alpha[0], psi2alphaTemp);
    sliceInternalVariables8Parts(psi2beta[0], psi2betaTemp);
    
    //  for(int i = 0; i<8;i++)
    //  {
    //     psi2betaTemp[i] = psi2beta;
    //  }
    


    timeCV[0] = timeCV[0] + odeCVCalculationSettingsArray[2];

    i1alpha = (0.667 * (inputI1[i] - (0.5 * inputI2[i]) - (0.5 * inputI3[i])));
    i1beta = (0.6667 * (0.866 * inputI2[i] - 0.866 *  inputI3[i]));


    sliceInternalVariables8Parts(i1alpha, i1alphaTemp);
    //  for(int i = 0; i<8;i++)
    //  {
    //     i1alphaTemp[i] = i1alpha;
    //  }


    sliceInternalVariables8Parts(i1beta, i1betaTemp);
    // for(int i = 0; i<8;i++)
    // {
    //     i1betaTemp[i] = i1beta;
    // }


    motorElectricalAngularVelocity = inputMotorMechanicalAngularVelocity[i] * modelCVCoeffArray[2];


    // for(int i = 0; i<8;i++)
    // {
    //     motorElectricalAngularVelocityTemp[i] = motorElectricalAngularVelocity;
    // }
   
   sliceInternalVariables8Parts(motorElectricalAngularVelocity, motorElectricalAngularVelocityTemp);



    /*------------------------------------------------------------------------------------------------------------------------------------*/
    k1psi2alpha = psi2alphaFce(R2MLmDL2Temp[0], R2DL2Temp[0] , i1alphaTemp[0], i1betaTemp[0], psi2alphaTemp[0], psi2betaTemp[0], motorElectricalAngularVelocityTemp[0]);

    k1psi2beta = psi2betaFce(R2MLmDL2Temp[1], R2DL2Temp[1], i1alphaTemp[1], i1betaTemp[1], psi2alphaTemp[1], psi2betaTemp[1], motorElectricalAngularVelocityTemp[1]);
    /*------------------------------------------------------------------------------------------------------------------------------------*/
    // for(int i = 0;i<4;i++)
    // {
    //     k1psi2alphaTemp[i] = k1psi2alpha;
    // }

    sliceInternalVariables4Parts(k1psi2alpha, k1psi2alphaTemp);

    sliceInternalVariables4Parts(k1psi2beta, k1psi2betaTemp);
    // for(int i = 0;i<4;i++)
    // {
    //     k1psi2betaTemp[i] = k1psi2beta;
    // }


    /*------------------------------------------------------------------------------------------------------------------------------------*/
    k2psi2alpha = psi2alphaFce(R2MLmDL2Temp[2], R2DL2Temp[2] , i1alphaTemp[2], i1betaTemp[2], (psi2alphaTemp[2] + (halfCalculationStep[0] * k1psi2alphaTemp[0])), (psi2betaTemp[2] + (halfCalculationStep[0] * k1psi2betaTemp[0])), motorElectricalAngularVelocityTemp[2]);

    k2psi2beta = psi2betaFce(R2MLmDL2Temp[3], R2DL2Temp[3], i1alphaTemp[3], i1betaTemp[3], (psi2alphaTemp[3] + (halfCalculationStep[0] * k1psi2alphaTemp[1])), (psi2betaTemp[3] + (halfCalculationStep[0] * k1psi2betaTemp[1])), motorElectricalAngularVelocityTemp[3]);
    /*------------------------------------------------------------------------------------------------------------------------------------*/

    // for(int i = 0;i<4;i++)
    // {
    //     k2psi2alphaTemp[i] = k2psi2alpha;
    // }

    sliceInternalVariables4Parts(k2psi2alpha, k2psi2alphaTemp);

    sliceInternalVariables4Parts(k2psi2beta, k2psi2betaTemp);

    // for(int i = 0;i<4;i++)
    // {
    //     k2psi2betaTemp[i] = k2psi2beta;
    // }


    /*------------------------------------------------------------------------------------------------------------------------------------*/
    k3psi2alpha = psi2alphaFce(R2MLmDL2Temp[4], R2DL2Temp[4] , i1alphaTemp[4], i1betaTemp[4], (psi2alphaTemp[4] + (halfCalculationStep[0] * k2psi2alphaTemp[0])), (psi2betaTemp[4] + (halfCalculationStep[0] * k2psi2betaTemp[0])), motorElectricalAngularVelocityTemp[4]);

    k3psi2beta = psi2betaFce(R2MLmDL2Temp[5], R2DL2Temp[5] , i1alphaTemp[5], i1betaTemp[5], (psi2alphaTemp[5] + (halfCalculationStep[0] * k2psi2alphaTemp[1])), (psi2betaTemp[5] + (halfCalculationStep[0] * k2psi2betaTemp[1])), motorElectricalAngularVelocityTemp[5]);
    /*------------------------------------------------------------------------------------------------------------------------------------*/

    sliceInternalVariables4Parts(k3psi2alpha, k3psi2alphaTemp);
    // for(int i = 0;i<4;i++)
    // {
    //     k3psi2alphaTemp[i] = k3psi2alpha;
    // }
    
    sliceInternalVariables4Parts(k3psi2beta, k3psi2betaTemp);
    // for(int i = 0;i<4;i++)
    // {
    //     k3psi2betaTemp[i] = k3psi2beta;
    // }
    /*------------------------------------------------------------------------------------------------------------------------------------*/
    k4psi2alpha = psi2alphaFce(R2MLmDL2Temp[6], R2DL2Temp[6] , i1alphaTemp[6], i1betaTemp[6], (psi2alphaTemp[6] + (odeCVCalculationSettingsArray[2] * k3psi2alphaTemp[0])), (psi2betaTemp[6] + (odeCVCalculationSettingsArray[2] * k3psi2betaTemp[0])), motorElectricalAngularVelocityTemp[6]);

    k4psi2beta = psi2betaFce(R2MLmDL2Temp[7], R2DL2Temp[7] , i1alphaTemp[7], i1betaTemp[7], (psi2alphaTemp[7] + (odeCVCalculationSettingsArray[2] * k3psi2alphaTemp[1])), (psi2betaTemp[7] + (odeCVCalculationSettingsArray[2] * k3psi2betaTemp[1])), motorElectricalAngularVelocityTemp[7]);
    /*------------------------------------------------------------------------------------------------------------------------------------*/

    k4psi2alphaTemp1 = k4psi2alpha;
    k4psi2betaTemp1 = k4psi2beta;


    // updating the values based on calculated coefficients
    /*------------------------------------------------------------------------------------------------------------------------------------*/

    psi2alpha[0] = psi2alpha[0] + ((odeCVCalculationSettingsArray[2] / 6) * (k1psi2alphaTemp[3] + 2 * k2psi2alphaTemp[3] + 2 * k3psi2alphaTemp[3] + k4psi2alphaTemp1));

    psi2alphaOut[i] = psi2alpha[0];

    psi2beta[0] = psi2beta[0] + ((odeCVCalculationSettingsArray[2] / 6) * (k1psi2betaTemp[3] + 2 * k2psi2betaTemp[3] + 2 * k3psi2betaTemp[3] + k4psi2betaTemp1));

    psi2betaOut[i] = psi2beta[0];


    psi2Amplitude[i] = sqrtf((psi2alphaOut[i] * psi2alphaOut[i]) + (psi2betaOut[i] * psi2betaOut[i]));



    transformAngle[i] = atan2f(psi2betaOut[i], psi2alphaOut[i]);
    
    
/****************************************************************************************************/



}





}

}