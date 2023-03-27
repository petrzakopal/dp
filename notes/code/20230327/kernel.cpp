/*******************************************************************************
Author: FEE CVUT
Purpose: Kernel
*******************************************************************************/
// Includes
#include <stdint.h>
#include <cmath>
#include <iostream>
#include <stdlib.h>
#include <string>
#include "header/MotorModel.h"
#include "header/CurVelModel.h"
#include "header/regulator.h"
// #include "header/svmCore.h" // already in invertor
#include "header/transformation.h"
#include "header/invertor.h"
// #include "function/transformation.cpp"
#define PI 3.141592 




/*-------------------------------------------------------------------------------*/
/*----------------------- MODEL ODE EQUATIONS OF DY/DT -------------------------*/
float psi2alphaTestFce(float R2MLmDL2, float R2DL2, float i1alpha, float i1beta, float psi2alpha, float psi2beta, float motorElectricalAngularVelocity )
{
    return((R2MLmDL2 * i1alpha) - (motorElectricalAngularVelocity * psi2beta) - (R2DL2 * psi2alpha));
}

float psi2betaTestFce(float R2MLmDL2, float R2DL2, float i1alpha, float i1beta, float psi2alpha, float psi2beta, float motorElectricalAngularVelocity)
{
    return((R2MLmDL2 * i1beta) + (motorElectricalAngularVelocity * psi2alpha) - (R2DL2 * psi2beta));
}
/*-------------------------------------------------------------------------------*/
// musí být chyba v algoritmu velocity regulatoru

void computeCurVelKernel(float *masterInput, float *masterOutput)
{
    // coefficients for RK4
    float k1psi2alpha, k2psi2alpha, k3psi2alpha, k4psi2alpha;
    float k1psi2beta, k2psi2beta, k3psi2beta, k4psi2beta;

    // helper variable to reduce calculation fo the same value
    float halfCalculationStep = masterInput[3];
    float inputMotorMechanicalAngularVelocity = masterInput[7];
     float R2MLmDL2 = masterInput[8];
    float R2DL2 = masterInput[9];
    float i1alpha;
    float i1beta;
   
    float psi2alpha = masterInput[37];
    float psi2beta = masterInput[38];
    float inputI1 =masterInput[4];
    float inputI2 =masterInput[5];
    float inputI3 =masterInput[6];
    float motorElectricalAngularVelocity = inputMotorMechanicalAngularVelocity * 2;
    float calculationStep = masterInput[1];

    i1alpha = (0.667 * (inputI1 - (0.5 * inputI2) - (0.5 * inputI3)));
    i1beta = (0.6667 * (0.866 * inputI2 - 0.866 *  inputI3));

    /*------------------------------------------------------------------------------------------------------------------------------------*/
    k1psi2alpha = psi2alphaTestFce(R2MLmDL2, R2DL2, i1alpha, i1beta, psi2alpha, psi2beta, motorElectricalAngularVelocity);

    k1psi2beta = psi2betaTestFce(R2MLmDL2, R2DL2, i1alpha, i1beta, psi2alpha, psi2beta, motorElectricalAngularVelocity);
    /*------------------------------------------------------------------------------------------------------------------------------------*/

    /*------------------------------------------------------------------------------------------------------------------------------------*/
    k2psi2alpha = psi2alphaTestFce(R2MLmDL2, R2DL2, i1alpha, i1beta, (psi2alpha + (halfCalculationStep * k1psi2alpha)), (psi2beta + (halfCalculationStep * k1psi2beta)), motorElectricalAngularVelocity);

    k2psi2beta = psi2betaTestFce(R2MLmDL2, R2DL2, i1alpha, i1beta, (psi2alpha + (halfCalculationStep * k1psi2alpha)), (psi2beta + (halfCalculationStep * k1psi2beta)), motorElectricalAngularVelocity);
    /*------------------------------------------------------------------------------------------------------------------------------------*/

    /*------------------------------------------------------------------------------------------------------------------------------------*/
    k3psi2alpha = psi2alphaTestFce(R2MLmDL2, R2DL2, i1alpha, i1beta, (psi2alpha + (halfCalculationStep * k2psi2alpha)), (psi2beta + (halfCalculationStep * k2psi2beta)), motorElectricalAngularVelocity);

    k3psi2beta = psi2betaTestFce(R2MLmDL2, R2DL2, i1alpha, i1beta, (psi2alpha + (halfCalculationStep * k2psi2alpha)), (psi2beta + (halfCalculationStep * k2psi2beta)), motorElectricalAngularVelocity);
    /*------------------------------------------------------------------------------------------------------------------------------------*/

    /*------------------------------------------------------------------------------------------------------------------------------------*/
    k4psi2alpha = psi2alphaTestFce(R2MLmDL2, R2DL2, i1alpha, i1beta, (psi2alpha + (calculationStep * k3psi2alpha)), (psi2beta + (calculationStep * k3psi2beta)), motorElectricalAngularVelocity);

    k4psi2beta = psi2betaTestFce(R2MLmDL2, R2DL2, i1alpha, i1beta, (psi2alpha + (calculationStep * k3psi2alpha)), (psi2beta + (calculationStep * k3psi2beta)), motorElectricalAngularVelocity);
    /*------------------------------------------------------------------------------------------------------------------------------------*/



    // updating the values based on calculated coefficients
    /*------------------------------------------------------------------------------------------------------------------------------------*/
    masterOutput[12] = psi2alpha + ((calculationStep / 6) * (k1psi2alpha + 2 * k2psi2alpha + 2 * k3psi2alpha + k4psi2alpha));

    masterOutput[13] = psi2beta + ((calculationStep / 6) * (k1psi2beta + 2 * k2psi2beta + 2 * k3psi2beta + k4psi2beta));
    /*------------------------------------------------------------------------------------------------------------------------------------*/
}

    void krnl_calculateCurVelModel(float *masterInput, float *masterOutput )
    {
      

       

         /*
        * @brief Initial values settings
        *
     */
        

        computeCurVelKernel(masterInput, masterOutput);


    }

