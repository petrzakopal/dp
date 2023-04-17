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
extern "C"
{

    float psi2alphaFce(float R2MLmDL2, float R2DL2, float i1alpha, float i1beta, float psi2alpha, float psi2beta, float motorElectricalAngularVelocity)
    {
        return ((R2MLmDL2 * i1alpha) - (motorElectricalAngularVelocity * psi2beta) - (R2DL2 * psi2alpha));
    }

    float psi2betaFce(float R2MLmDL2, float R2DL2, float i1alpha, float i1beta, float psi2alpha, float psi2beta, float motorElectricalAngularVelocity)
    {
        return ((R2MLmDL2 * i1beta) + (motorElectricalAngularVelocity * psi2alpha) - (R2DL2 * psi2beta));
    }

    // second type kernel
    void krnl_CurVelLoadLegacy(float *odeCVCalculationSettingsArray, float *modelCVCoeffArray, float *inputI1, float *inputI2, float *inputI3, float *inputMotorMechanicalAngularVelocity, float *psi2alphaOut, float *psi2betaOut)
    {

        float timeCV = odeCVCalculationSettingsArray[0];
        float i1alpha;
        float i1beta;
        float motorElectricalAngularVelocity;
        float halfCaclculationStep = odeCVCalculationSettingsArray[2] / 2;
        int numberOfIterations = (int)odeCVCalculationSettingsArray[4];
        int numberOfIterationsTemp1 = numberOfIterations;
        int numberOfIterationsTemp2 = numberOfIterations;
        float R2MLmDL2 = modelCVCoeffArray[1];

        float R2MLmDL2Temp1 = R2MLmDL2;
        float R2MLmDL2Temp2 = R2MLmDL2;
        float R2MLmDL2Temp3 = R2MLmDL2;
        float R2MLmDL2Temp4 = R2MLmDL2;
        float R2MLmDL2Temp5 = R2MLmDL2;
        float R2MLmDL2Temp6 = R2MLmDL2;
        float R2MLmDL2Temp7 = R2MLmDL2;
        float R2MLmDL2Temp8 = R2MLmDL2;

        float R2DL2 = modelCVCoeffArray[0];

        float R2DL2Temp1 = R2DL2;
        float R2DL2Temp2 = R2DL2;
        float R2DL2Temp3 = R2DL2;
        float R2DL2Temp4 = R2DL2;
        float R2DL2Temp5 = R2DL2;
        float R2DL2Temp6 = R2DL2;
        float R2DL2Temp7 = R2DL2;
        float R2DL2Temp8 = R2DL2;

        float psi2alpha = psi2alphaOut[0];
        float psi2beta = psi2betaOut[0];

        /****************************************************************************************************/
        /*------------------------------ MAIN MODEL LOOP FOR ACQUIRED INPUTS ------------------------------*/
        for (int i = 0; i < numberOfIterationsTemp1; i++)
        {

#pragma HLS performance target_ti = 100
#pragma HLS loop_tripcount max = numberOfIterationsTemp2

            float psi2alphaTemp1 = psi2alpha;
            float psi2alphaTemp2 = psi2alpha;
            float psi2alphaTemp3 = psi2alpha;
            float psi2alphaTemp4 = psi2alpha;
            float psi2alphaTemp5 = psi2alpha;
            float psi2alphaTemp6 = psi2alpha;
            float psi2alphaTemp7 = psi2alpha;
            float psi2alphaTemp8 = psi2alpha;

            float psi2betaTemp1 = psi2beta;
            float psi2betaTemp2 = psi2beta;
            float psi2betaTemp3 = psi2beta;
            float psi2betaTemp4 = psi2beta;
            float psi2betaTemp5 = psi2beta;
            float psi2betaTemp6 = psi2beta;
            float psi2betaTemp7 = psi2beta;
            float psi2betaTemp8 = psi2beta;

            timeCV = timeCV + odeCVCalculationSettingsArray[2];

            i1alpha = (0.667 * (inputI1[i] - (0.5 * inputI2[i]) - (0.5 * inputI3[i])));
            i1beta = (0.6667 * (0.866 * inputI2[i] - 0.866 * inputI3[i]));

            float i1alphaTemp1 = i1alpha;
            float i1alphaTemp2 = i1alpha;
            float i1alphaTemp3 = i1alpha;
            float i1alphaTemp4 = i1alpha;
            float i1alphaTemp5 = i1alpha;
            float i1alphaTemp6 = i1alpha;
            float i1alphaTemp7 = i1alpha;
            float i1alphaTemp8 = i1alpha;

            float i1betaTemp1 = i1beta;
            float i1betaTemp2 = i1beta;
            float i1betaTemp3 = i1beta;
            float i1betaTemp4 = i1beta;
            float i1betaTemp5 = i1beta;
            float i1betaTemp6 = i1beta;
            float i1betaTemp7 = i1beta;
            float i1betaTemp8 = i1beta;

            motorElectricalAngularVelocity = inputMotorMechanicalAngularVelocity[i] * modelCVCoeffArray[2];

            float motorElectricalAngularVelocityTemp1 = motorElectricalAngularVelocity;
            float motorElectricalAngularVelocityTemp2 = motorElectricalAngularVelocity;
            float motorElectricalAngularVelocityTemp3 = motorElectricalAngularVelocity;
            float motorElectricalAngularVelocityTemp4 = motorElectricalAngularVelocity;
            float motorElectricalAngularVelocityTemp5 = motorElectricalAngularVelocity;
            float motorElectricalAngularVelocityTemp6 = motorElectricalAngularVelocity;
            float motorElectricalAngularVelocityTemp7 = motorElectricalAngularVelocity;
            float motorElectricalAngularVelocityTemp8 = motorElectricalAngularVelocity;

            // coefficients for RK4
            float k1psi2alpha, k2psi2alpha, k3psi2alpha, k4psi2alpha;
            float k1psi2beta, k2psi2beta, k3psi2beta, k4psi2beta;

            // helper variable to reduce calculation fo the same value
            float halfCalculationStep = odeCVCalculationSettingsArray[2] / 2;

            /*------------------------------------------------------------------------------------------------------------------------------------*/
            k1psi2alpha = psi2alphaFce(R2MLmDL2Temp1, R2DL2Temp1, i1alphaTemp1, i1betaTemp1, psi2alphaTemp1, psi2betaTemp1, motorElectricalAngularVelocityTemp1);

            k1psi2beta = psi2betaFce(R2MLmDL2Temp2, R2DL2Temp2, i1alphaTemp2, i1betaTemp2, psi2alphaTemp2, psi2betaTemp2, motorElectricalAngularVelocityTemp2);
            /*------------------------------------------------------------------------------------------------------------------------------------*/

            float k1psi2alphaTemp1 = k1psi2alpha;
            float k1psi2alphaTemp2 = k1psi2alpha;
            float k1psi2alphaTemp3 = k1psi2alpha;
            float k1psi2alphaTemp4 = k1psi2alpha;

            float k1psi2betaTemp1 = k1psi2beta;
            float k1psi2betaTemp2 = k1psi2beta;
            float k1psi2betaTemp3 = k1psi2beta;
            float k1psi2betaTemp4 = k1psi2beta;

            /*------------------------------------------------------------------------------------------------------------------------------------*/
            k2psi2alpha = psi2alphaFce(R2MLmDL2Temp3, R2DL2Temp3, i1alphaTemp3, i1betaTemp3, (psi2alphaTemp3 + (halfCalculationStep * k1psi2alphaTemp1)), (psi2betaTemp3 + (halfCalculationStep * k1psi2betaTemp1)), motorElectricalAngularVelocityTemp3);

            k2psi2beta = psi2betaFce(R2MLmDL2Temp4, R2DL2Temp4, i1alphaTemp4, i1betaTemp4, (psi2alphaTemp4 + (halfCalculationStep * k1psi2alphaTemp2)), (psi2betaTemp4 + (halfCalculationStep * k1psi2betaTemp2)), motorElectricalAngularVelocityTemp4);
            /*------------------------------------------------------------------------------------------------------------------------------------*/

            float k2psi2alphaTemp1 = k2psi2alpha;
            float k2psi2alphaTemp2 = k2psi2alpha;
            float k2psi2alphaTemp3 = k2psi2alpha;
            float k2psi2alphaTemp4 = k2psi2alpha;

            float k2psi2betaTemp1 = k2psi2beta;
            float k2psi2betaTemp2 = k2psi2beta;
            float k2psi2betaTemp3 = k2psi2beta;
            float k2psi2betaTemp4 = k2psi2beta;

            /*------------------------------------------------------------------------------------------------------------------------------------*/
            k3psi2alpha = psi2alphaFce(R2MLmDL2Temp5, R2DL2Temp5, i1alphaTemp5, i1betaTemp5, (psi2alphaTemp5 + (halfCalculationStep * k2psi2alphaTemp1)), (psi2betaTemp5 + (halfCalculationStep * k2psi2betaTemp1)), motorElectricalAngularVelocityTemp5);

            k3psi2beta = psi2betaFce(R2MLmDL2Temp6, R2DL2Temp6, i1alphaTemp6, i1betaTemp6, (psi2alphaTemp6 + (halfCalculationStep * k2psi2alphaTemp2)), (psi2betaTemp6 + (halfCalculationStep * k2psi2betaTemp2)), motorElectricalAngularVelocityTemp6);
            /*------------------------------------------------------------------------------------------------------------------------------------*/

            float k3psi2alphaTemp1 = k3psi2alpha;
            float k3psi2alphaTemp2 = k3psi2alpha;
            float k3psi2alphaTemp3 = k3psi2alpha;
            float k3psi2alphaTemp4 = k3psi2alpha;

            float k3psi2betaTemp1 = k3psi2beta;
            float k3psi2betaTemp2 = k3psi2beta;
            float k3psi2betaTemp3 = k3psi2beta;
            float k3psi2betaTemp4 = k3psi2beta;

            /*------------------------------------------------------------------------------------------------------------------------------------*/
            k4psi2alpha = psi2alphaFce(R2MLmDL2Temp7, R2DL2Temp7, i1alphaTemp7, i1betaTemp7, (psi2alphaTemp7 + (odeCVCalculationSettingsArray[2] * k3psi2alphaTemp1)), (psi2betaTemp7 + (odeCVCalculationSettingsArray[2] * k3psi2betaTemp1)), motorElectricalAngularVelocityTemp7);

            k4psi2beta = psi2betaFce(R2MLmDL2Temp8, R2DL2Temp8, i1alphaTemp8, i1betaTemp8, (psi2alphaTemp8 + (odeCVCalculationSettingsArray[2] * k3psi2alphaTemp2)), (psi2betaTemp8 + (odeCVCalculationSettingsArray[2] * k3psi2betaTemp2)), motorElectricalAngularVelocityTemp8);
            /*------------------------------------------------------------------------------------------------------------------------------------*/

            float k4psi2alphaTemp1 = k4psi2alpha;
            float k4psi2betaTemp1 = k4psi2beta;

            // updating the values based on calculated coefficients
            /*------------------------------------------------------------------------------------------------------------------------------------*/

            psi2alpha = psi2alpha + ((odeCVCalculationSettingsArray[2] / 6) * (k1psi2alphaTemp4 + 2 * k2psi2alphaTemp4 + 2 * k3psi2alphaTemp4 + k4psi2alphaTemp1));

            psi2alphaOut[i] = psi2alpha;

            psi2beta = psi2beta + ((odeCVCalculationSettingsArray[2] / 6) * (k1psi2betaTemp4 + 2 * k2psi2betaTemp4 + 2 * k3psi2betaTemp4 + k4psi2betaTemp1));

            psi2betaOut[i] = psi2beta;

            /****************************************************************************************************/
        }
    }
}