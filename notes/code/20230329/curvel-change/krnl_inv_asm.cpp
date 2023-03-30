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

    /*
*
* @brief Calculating online stateStaceCoeff (coefficients dependant on velocity)
*/
void calculateStateSpaceCoeffKernel(float *stateSpaceCoeff, float *motorParameters, float motorElectricalAngularVelocity)
{
    stateSpaceCoeff[3] = ( motorParameters[0] * motorElectricalAngularVelocity)/(motorParameters[1] * motorParameters[2] * motorParameters[3]);

     stateSpaceCoeff[6] = - stateSpaceCoeff[3];

    stateSpaceCoeff[11] = - motorElectricalAngularVelocity;

    stateSpaceCoeff[14] = motorElectricalAngularVelocity;
}

/* RK4 METHODS FOR CALCULATING VARRIABLES */

float i1alphaFce(float *stateSpaceCoeff, float i1alpha, float i1beta, float psi2alpha, float psi2beta, float u1alpha)
{
    
    return((stateSpaceCoeff[0] * i1alpha /* + stateSpaceCoeff->a12 * i1beta => is 0 */ + stateSpaceCoeff[2] * psi2alpha + stateSpaceCoeff[3] * psi2beta) + stateSpaceCoeff[16] * u1alpha);
 
}

float i1betaFce(float *stateSpaceCoeff, float i1alpha, float i1beta, float psi2alpha, float psi2beta, float u1beta)
{
    
    return((stateSpaceCoeff[4] * i1alpha + stateSpaceCoeff[5] * i1beta + stateSpaceCoeff[6] * psi2alpha + stateSpaceCoeff[7] * psi2beta)+stateSpaceCoeff[17] * u1beta);
 
}


float psi2alphaFce(float *stateSpaceCoeff, float i1alpha, float i1beta, float psi2alpha, float psi2beta)
{
    return(stateSpaceCoeff[8] * i1alpha /* + stateSpaceCoeff->a32 * i1beta*/ /* = 0*/ + stateSpaceCoeff[10] * psi2alpha + stateSpaceCoeff[11] * psi2beta);
}


float psi2betaFce(float *stateSpaceCoeff, float i1alpha, float i1beta, float psi2alpha, float psi2beta)
{
    return(/*stateSpaceCoeff->a41 * i1alpha = 0 */ + stateSpaceCoeff[13] * i1beta +  stateSpaceCoeff[14] * psi2alpha + stateSpaceCoeff[15] * psi2beta);
}

float motorTorqueFce(float *motorParameters, float i1alpha, float i1beta, float psi2alpha, float psi2beta)
{
    return(1.5 * motorParameters[5] * (motorParameters[0] / motorParameters[3]) * (psi2alpha * i1beta - psi2beta * i1alpha));
}

float motorMechanicalAngularVelocityFce(float momentOfInertia, float motorTorque, float loadTorque, float calculationStep, float motorMechanicalAngularVelocity)
{
    return(((1/momentOfInertia)*(motorTorque - loadTorque)*(calculationStep))+motorMechanicalAngularVelocity);
}



void krnl_calculateInvMot(float *masterInput, float *masterOutput)
{   

    // input variables from the host program
    bool sw1;
    bool sw2;
    bool sw3;
    bool sw4;
    bool sw5;
    bool sw6;
    float psi2alpha;
    float psi2beta;
    float i1alpha;
    float i1beta;
    // float halfCalculationStep;
    float motorParameters[7];
    float stateSpaceCoeff[18];
    float motorMechanicalAngularVelocity;
    // float calculationStep;
    // float u1alpha;
    // float u1beta;
    // float loadTorque;
    // float uDC;
    

    // local variables
   
        float i1alphaLocal;
        float i1betaLocal;
        float psi2alphaLocal;
        float psi2betaLocal;
        float motorTorqueLocal;
        // float u1a;
        // float u1b;
        // float u1c;
        // float i1;
        // float i2;
        // float i3;

        // local variable to export
    
        // float motorTorque;
        float motorMechanicalAngularVelocityLocal; // paste new value of motorMechanicalAngularVelociyt and export it to host
        float motorElectricalAngularVelocity;

        // inserting values from host
        sw1 = masterInput[0];
        sw2 = masterInput[1];
        sw3 = masterInput[2];
        sw4 = masterInput[3];
        sw5 = masterInput[4];
        sw6 = masterInput[5];
        const float uDC = masterInput[6];
        psi2alpha = masterInput[7];
        psi2beta = masterInput[8];
        i1alpha = masterInput[9];
        i1beta = masterInput[10];
        const float loadTorque = masterInput[11];
        motorMechanicalAngularVelocity = masterInput[12];
        const float halfCalculationStep = masterInput[13];
        const float calculationStep = masterInput[14];
        motorParameters[0] = masterInput[15]; // Lm
        motorParameters[1] = masterInput[16]; // sigma
        motorParameters[2] = masterInput[17]; // L1
        motorParameters[3] = masterInput[18]; // L2
        motorParameters[5] = masterInput[19]; // numberOfPolePairs
        motorParameters[6] = masterInput[20]; // momentOfInertia
        stateSpaceCoeff[0] = masterInput[21]; // a11
        stateSpaceCoeff[1] = masterInput[22]; // a12
        stateSpaceCoeff[2] = masterInput[23]; // a13
        stateSpaceCoeff[3] = masterInput[24]; // a14
        stateSpaceCoeff[4] = masterInput[25]; // a21
        stateSpaceCoeff[5] = masterInput[26]; // a22
        stateSpaceCoeff[6] = masterInput[27]; // a23
        stateSpaceCoeff[7] = masterInput[28]; // a24
        stateSpaceCoeff[8] = masterInput[29]; // a31
        stateSpaceCoeff[9] = masterInput[30]; // a32
        stateSpaceCoeff[10] = masterInput[31]; // a33
        stateSpaceCoeff[11] = masterInput[32]; // a34
        stateSpaceCoeff[12] = masterInput[33]; // a41
        stateSpaceCoeff[13] = masterInput[34]; // a42
        stateSpaceCoeff[14] = masterInput[35]; // a43
        stateSpaceCoeff[15] = masterInput[36]; // a44
        stateSpaceCoeff[16] = masterInput[37]; // b11
        stateSpaceCoeff[17] = masterInput[38]; // b22


        // calculating input electrical angular velocity
        motorElectricalAngularVelocity = motorMechanicalAngularVelocity *   motorParameters[5];

        const float uDCd3 = (uDC)/3;
        const float u1a = uDCd3 * ((2 * sw1) - (sw3) - (sw5));
        const float u1b = uDCd3 * (- (sw1) + (2 * sw3) - (sw5));
        const float u1c = uDCd3 * (- (sw1) - ( sw3) + (2 * sw5));

        // input voltages for asynchronous motor model
        const float u1alpha = (0.6667 * (u1a - (0.5 * u1b) - (0.5 * u1c)));
        const float u1beta = ((0.6667 * (0.866 * u1b - 0.866 * u1c)));


        /* RK4 solution of asynchronous motor ODE equations */

        /*
        * @name RK4 state space variables
        * @brief ODE helper variables
        */
        float k1i1alpha, k2i1alpha, k3i1alpha, k4i1alpha;
        float k1i1beta, k2i1beta, k3i1beta, k4i1beta;
        float k1psi2alpha, k2psi2alpha, k3psi2alpha, k4psi2alpha;
        float k1psi2beta, k2psi2beta, k3psi2beta, k4psi2beta;
   
    
    
        calculateStateSpaceCoeffKernel(stateSpaceCoeff, motorParameters, motorElectricalAngularVelocity);


        k1i1alpha =  i1alphaFce(stateSpaceCoeff, i1alpha, i1beta, psi2alpha, psi2beta, u1alpha);

        k1i1beta =  i1betaFce(stateSpaceCoeff, i1alpha, i1beta, psi2alpha, psi2beta, u1beta);
    
        k1psi2alpha =  psi2alphaFce(stateSpaceCoeff, i1alpha, i1beta, psi2alpha, psi2beta);
    
        k1psi2beta =  psi2betaFce(stateSpaceCoeff, i1alpha, i1beta, psi2alpha, psi2beta);


        k2i1alpha =  i1alphaFce(stateSpaceCoeff, (i1alpha+(halfCalculationStep*k1i1alpha)), (i1beta+(halfCalculationStep*k1i1beta)), (psi2alpha+(halfCalculationStep * k1psi2alpha)), (psi2beta + (halfCalculationStep * k1psi2beta)), u1alpha);


        k2i1beta =  i1betaFce(stateSpaceCoeff, (i1alpha+(halfCalculationStep*k1i1alpha)), (i1beta+(halfCalculationStep*k1i1beta)), (psi2alpha+(halfCalculationStep * k1psi2alpha)), (psi2beta + (halfCalculationStep * k1psi2beta)), u1beta);

        k2psi2alpha =  psi2alphaFce(stateSpaceCoeff, (i1alpha+(halfCalculationStep*k1i1alpha)), (i1beta+(halfCalculationStep*k1i1beta)), (psi2alpha+(halfCalculationStep * k1psi2alpha)), (psi2beta + (halfCalculationStep * k1psi2beta)));

        k2psi2beta =  psi2betaFce(stateSpaceCoeff, (i1alpha+(halfCalculationStep*k1i1alpha)), (i1beta+(halfCalculationStep*k1i1beta)), (psi2alpha+(halfCalculationStep * k1psi2alpha)), (psi2beta + (halfCalculationStep * k1psi2beta)));


        k3i1alpha =  i1alphaFce(stateSpaceCoeff, (i1alpha+(halfCalculationStep*k2i1alpha)), (i1beta+(halfCalculationStep*k2i1beta)), (psi2alpha+(halfCalculationStep * k2psi2alpha)), (psi2beta + (halfCalculationStep * k2psi2beta)), u1alpha);

        k3i1beta =  i1betaFce(stateSpaceCoeff, (i1alpha+(halfCalculationStep*k2i1alpha)), (i1beta+(halfCalculationStep*k2i1beta)), (psi2alpha+(halfCalculationStep * k2psi2alpha)), (psi2beta + (halfCalculationStep * k2psi2beta)), u1beta);

        k3psi2alpha =  psi2alphaFce(stateSpaceCoeff, (i1alpha+(halfCalculationStep*k2i1alpha)), (i1beta+(halfCalculationStep*k2i1beta)), (psi2alpha+(halfCalculationStep * k2psi2alpha)), (psi2beta + (halfCalculationStep * k2psi2beta)));

        k3psi2beta =  psi2betaFce(stateSpaceCoeff, (i1alpha+(halfCalculationStep*k2i1alpha)), (i1beta+(halfCalculationStep*k2i1beta)), (psi2alpha+(halfCalculationStep * k2psi2alpha)), (psi2beta + (halfCalculationStep * k2psi2beta)));


        k4i1alpha =  i1alphaFce(stateSpaceCoeff, (i1alpha+(calculationStep*k3i1alpha)), (i1beta+(calculationStep*k3i1beta)), (psi2alpha+(calculationStep * k3psi2alpha)), (psi2beta + (calculationStep * k3psi2beta)), u1alpha);

        k4i1beta =  i1betaFce(stateSpaceCoeff, (i1alpha+(calculationStep*k3i1alpha)), (i1beta+(calculationStep*k3i1beta)), (psi2alpha+(calculationStep * k3psi2alpha)), (psi2beta + (calculationStep * k3psi2beta)), u1beta);

        k4psi2alpha =  psi2alphaFce(stateSpaceCoeff, (i1alpha+(calculationStep*k3i1alpha)), (i1beta+(calculationStep*k3i1beta)), (psi2alpha+(calculationStep * k3psi2alpha)), (psi2beta + (calculationStep * k3psi2beta)));

        k4psi2beta =  psi2betaFce(stateSpaceCoeff, (i1alpha+(calculationStep*k3i1alpha)), (i1beta+(calculationStep*k3i1beta)), (psi2alpha+(calculationStep * k3psi2alpha)), (psi2beta + (calculationStep * k3psi2beta)));


        i1alphaLocal = (i1alpha + (calculationStep / 6) *(k1i1alpha + 2* k2i1alpha + 2* k3i1alpha + k4i1alpha));

        i1betaLocal = (i1beta + (calculationStep / 6) *(k1i1beta + 2* k2i1beta + 2* k3i1beta + k4i1beta));

        psi2alphaLocal = (psi2alpha + (calculationStep / 6) *(k1psi2alpha + 2* k2psi2alpha + 2* k3psi2alpha + k4psi2alpha));

        psi2betaLocal = (psi2beta + (calculationStep / 6) *(k1psi2beta + 2* k2psi2beta + 2* k3psi2beta + k4psi2beta));


        motorTorqueLocal = motorTorqueFce(motorParameters, i1alphaLocal, i1betaLocal, psi2alphaLocal, psi2betaLocal);

        motorMechanicalAngularVelocityLocal = ((1/motorParameters[6])*(motorTorqueLocal - loadTorque)*(calculationStep))+motorMechanicalAngularVelocity;


        // motor currents inverseClarkeTransform
        const float i1 = i1alphaLocal;
        const float i2 = ((-0.5 * i1alphaLocal) + 0.866 * i1betaLocal);
        const float i3 = ((-0.5 * i1alphaLocal) - 0.866 * i1betaLocal);


        masterOutput[0] = i1alphaLocal;
        masterOutput[1] = i1betaLocal;
        masterOutput[2] = psi2alphaLocal;
        masterOutput[3] = psi2betaLocal;
        masterOutput[4] = motorTorqueLocal;
        masterOutput[5] = motorMechanicalAngularVelocityLocal;
        masterOutput[6] = i1;
        masterOutput[7] = i2;
        masterOutput[8] = i3;
        masterOutput[9] = u1alpha;
        masterOutput[10] = u1beta;

    }

