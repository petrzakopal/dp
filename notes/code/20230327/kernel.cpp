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


    /*
    * @name sliceInternalVariables4Parts
    * @brief Function to calculate slice variables at 4 parts
    */

    void sliceInternalVariables4Parts(float variableIn, float *variableOut)
    {
        for(int i = 0;i<4;i++)
        {
            variableOut[i] = variableIn;
        }
    }

    /*
    * @name sliceInternalVariables8Parts
    * @brief Function to calculate slice variables at 4 parts
    */

    void sliceInternalVariables8Parts(float variableIn, float *variableOut)
    {
        for(int i = 0;i<8;i++)
        {
            variableOut[i] = variableIn;
        }
    }

     void sliceInternalVariables3Parts(float variableIn, float *variableOut)
    {
        for(int i = 0;i<3;i++)
        {
            variableOut[i] = variableIn;
        }
    }


    /*
    * @name psi2alphaFce
    * @brief Function to calculate new psi2alpha in RK4 CurVel Model
    */

    float psi2alphaFce(float R2MLmDL2, float R2DL2, float i1alpha, float i1beta, float psi2alpha, float psi2beta, float motorElectricalAngularVelocity )
    {
        return((R2MLmDL2 * i1alpha) - (motorElectricalAngularVelocity * psi2beta) - (R2DL2 * psi2alpha));
    }

    /*
    * @name psi2betaFce
    * @brief Function to calculate new psi2beta in RK4 CurVel Model
    */

    float psi2betaFce(float R2MLmDL2, float R2DL2, float i1alpha, float i1beta, float psi2alpha, float psi2beta, float motorElectricalAngularVelocity)
    {
        return((R2MLmDL2 * i1beta) + (motorElectricalAngularVelocity * psi2alpha) - (R2DL2 * psi2beta));
    }

    void computeCurVelKernel(float *psi2alpha, float *psi2beta, float inputI1, float inputI2, float inputI3, float numberOfPolePairs, float *R2MLmDL2Temp, float *R2DL2Temp, float inputMotorMechanicalAngularVelocity, float globalSimulationTime, float globalCalculationStep, float halfCalculationStep, float *i1alpha, float *i1beta)
    {

        // coefficients for RK4
        float k1psi2alpha, k2psi2alpha, k3psi2alpha, k4psi2alpha;
        float k1psi2beta, k2psi2beta, k3psi2beta, k4psi2beta;
        // helper variable to reduce calculation fo the same value

        float psi2alphaTemp[8];
        float psi2betaTemp[8];
        float i1alphaTemp[8];
        float i1betaTemp[8];
        float k1psi2alphaTemp[4];
        float k1psi2betaTemp[4];
        float k2psi2alphaTemp[4];
        float k2psi2betaTemp[4];
        float k3psi2alphaTemp[4];
        float k3psi2betaTemp[4];
        float motorElectricalAngularVelocityTemp[8];
        float motorElectricalAngularVelocity;
        float calculationStep = globalCalculationStep;
        float k4psi2alphaTemp1;
        float k4psi2betaTemp1;




        sliceInternalVariables8Parts(*psi2alpha, psi2alphaTemp);
        sliceInternalVariables8Parts(*psi2beta, psi2betaTemp);

        *i1alpha = (0.667 * (inputI1 - (0.5 * inputI2) - (0.5 * inputI3)));
        *i1beta = (0.6667 * (0.866 * inputI2 - 0.866 *  inputI3));

        sliceInternalVariables8Parts(*i1alpha, i1alphaTemp);
        sliceInternalVariables8Parts(*i1beta, i1betaTemp);

        motorElectricalAngularVelocity = inputMotorMechanicalAngularVelocity * numberOfPolePairs;

        sliceInternalVariables8Parts(motorElectricalAngularVelocity, motorElectricalAngularVelocityTemp);


        /*------------------------------------------------------------------------------------------------------------------------------------*/
        k1psi2alpha = psi2alphaFce(R2MLmDL2Temp[0], R2DL2Temp[0] , i1alphaTemp[0], i1betaTemp[0], psi2alphaTemp[0], psi2betaTemp[0], motorElectricalAngularVelocityTemp[0]);

        k1psi2beta = psi2betaFce(R2MLmDL2Temp[1], R2DL2Temp[1], i1alphaTemp[1], i1betaTemp[1], psi2alphaTemp[1], psi2betaTemp[1], motorElectricalAngularVelocityTemp[1]);
    /*------------------------------------------------------------------------------------------------------------------------------------*/


        sliceInternalVariables4Parts(k1psi2alpha, k1psi2alphaTemp);

        sliceInternalVariables4Parts(k1psi2beta, k1psi2betaTemp);

    /*------------------------------------------------------------------------------------------------------------------------------------*/
        k2psi2alpha = psi2alphaFce(R2MLmDL2Temp[2], R2DL2Temp[2] , i1alphaTemp[2], i1betaTemp[2], (psi2alphaTemp[2] + (halfCalculationStep * k1psi2alphaTemp[0])), (psi2betaTemp[2] + (halfCalculationStep * k1psi2betaTemp[0])), motorElectricalAngularVelocityTemp[2]);

        k2psi2beta = psi2betaFce(R2MLmDL2Temp[3], R2DL2Temp[3], i1alphaTemp[3], i1betaTemp[3], (psi2alphaTemp[3] + (halfCalculationStep * k1psi2alphaTemp[1])), (psi2betaTemp[3] + (halfCalculationStep * k1psi2betaTemp[1])), motorElectricalAngularVelocityTemp[3]);
    /*------------------------------------------------------------------------------------------------------------------------------------*/



        sliceInternalVariables4Parts(k2psi2alpha, k2psi2alphaTemp);

        sliceInternalVariables4Parts(k2psi2beta, k2psi2betaTemp);


    /*------------------------------------------------------------------------------------------------------------------------------------*/
        k3psi2alpha = psi2alphaFce(R2MLmDL2Temp[4], R2DL2Temp[4] , i1alphaTemp[4], i1betaTemp[4], (psi2alphaTemp[4] + (halfCalculationStep * k2psi2alphaTemp[0])), (psi2betaTemp[4] + (halfCalculationStep * k2psi2betaTemp[0])), motorElectricalAngularVelocityTemp[4]);

        k3psi2beta = psi2betaFce(R2MLmDL2Temp[5], R2DL2Temp[5] , i1alphaTemp[5], i1betaTemp[5], (psi2alphaTemp[5] + (halfCalculationStep * k2psi2alphaTemp[1])), (psi2betaTemp[5] + (halfCalculationStep * k2psi2betaTemp[1])), motorElectricalAngularVelocityTemp[5]);
    /*------------------------------------------------------------------------------------------------------------------------------------*/

        sliceInternalVariables4Parts(k3psi2alpha, k3psi2alphaTemp);

    
        sliceInternalVariables4Parts(k3psi2beta, k3psi2betaTemp);

    /*------------------------------------------------------------------------------------------------------------------------------------*/
        k4psi2alpha = psi2alphaFce(R2MLmDL2Temp[6], R2DL2Temp[6] , i1alphaTemp[6], i1betaTemp[6], (psi2alphaTemp[6] + (calculationStep * k3psi2alphaTemp[0])), (psi2betaTemp[6] + (calculationStep * k3psi2betaTemp[0])), motorElectricalAngularVelocityTemp[6]);

        k4psi2beta = psi2betaFce(R2MLmDL2Temp[7], R2DL2Temp[7] , i1alphaTemp[7], i1betaTemp[7], (psi2alphaTemp[7] + (calculationStep * k3psi2alphaTemp[1])), (psi2betaTemp[7] + (calculationStep * k3psi2betaTemp[1])), motorElectricalAngularVelocityTemp[7]);
    /*------------------------------------------------------------------------------------------------------------------------------------*/

        k4psi2alphaTemp1 = k4psi2alpha;
        k4psi2betaTemp1 = k4psi2beta;



        *psi2alpha = *psi2alpha + ((calculationStep / 6) * (k1psi2alphaTemp[3] + 2 * k2psi2alphaTemp[3] + 2 * k3psi2alphaTemp[3] + k4psi2alphaTemp1));


        *psi2beta = *psi2beta + ((calculationStep / 6) * (k1psi2betaTemp[3] + 2 * k2psi2betaTemp[3] + 2 * k3psi2betaTemp[3] + k4psi2betaTemp1));

    }


    void outputCurVelProductsCalc(float *transformAngle, float *psi2amplitude, float psi2alpha, float psi2beta)
    {
        float psi2alphaTemp = psi2alpha;
        float psi2betaTemp = psi2beta;

        *psi2amplitude = sqrt((psi2alphaTemp * psi2alphaTemp) + (psi2betaTemp * psi2betaTemp));
        *transformAngle = atan2(psi2betaTemp, psi2alphaTemp);
    }

    float regSaturationBlockKernel(float saturationInput, float saturationOutputMin, float saturationOutputMax)
    {
        float localSaturationInput = saturationInput;

        if(localSaturationInput > saturationOutputMax)
        {
            localSaturationInput = saturationOutputMax;
        }
        else if(localSaturationInput < saturationOutputMin)
        {
            localSaturationInput = saturationOutputMin;
        }

        return(localSaturationInput);

    }

    void checkSaturationStatusKernel(RegulatorType *regulatorData)
    {
        if(regulatorData->saturationInput == regulatorData->saturationOutput)
        {
            regulatorData->saturationCheckStatus = false; // saturation did not happen
        }
        else
        {
            regulatorData->saturationCheckStatus = true; // saturation did happen
        }
    }

    void checkSignStatusKernel(RegulatorType *regulatorData)
    {
        if(((regulatorData->eDif > 0) and (regulatorData->saturationInput > 0)) or ((regulatorData->eDif < 0) and (regulatorData->saturationInput < 0)))
        {
            regulatorData->signCheckStatus = true;
        }
        else
        {
            regulatorData->signCheckStatus = false;
        }
    }

    void enableClampingKernel(RegulatorType *regulatorData)
    {
        if((regulatorData->saturationCheckStatus == true) and (regulatorData->signCheckStatus == true))
        {
            regulatorData->clampingStatus = true;
        }
        else
        {
            regulatorData->clampingStatus = false;
        }

    }

    void regCalculateKernel(RegulatorType *regulatorData)
    {
        regulatorData->eDif = regulatorData->wantedValue - regulatorData->measuredValue;
        regulatorData->saturationInput = (regulatorData->eDif * regulatorData->kp) + regulatorData->iSum;
        regulatorData->saturationOutput = regSaturationBlockKernel(regulatorData->saturationInput, regulatorData->saturationOutputMin, regulatorData->saturationOutputMax);

        checkSaturationStatusKernel(regulatorData);
        checkSignStatusKernel(regulatorData);
        enableClampingKernel(regulatorData);

        if(regulatorData->clampingStatus == true)
        {
            regulatorData->iSum = regulatorData->iSum;
        }
        else
        {
            regulatorData->iSum = regulatorData->iSum + (regulatorData->eDif * regulatorData->ki);
        }
    }

    

    float generateActualValueTriangleWaveKernel(TriangleWaveSettingsType *triangleWaveSettings)
    {
        float triangleActualValue; // maybe put in triangleWaveSettings, and make this function void, do not know, this should be used only as a local variable

        // numerical method with abs value and modulo
        triangleActualValue = (((4 * triangleWaveSettings->waveAmplitude)/triangleWaveSettings->wavePeriod) * abs(fmod((fmod((triangleWaveSettings->calculationTime-(triangleWaveSettings->wavePeriod/4)), triangleWaveSettings->wavePeriod) +  triangleWaveSettings->wavePeriod),  triangleWaveSettings->wavePeriod) - ( triangleWaveSettings->wavePeriod/2)) - triangleWaveSettings->waveAmplitude);

        // trigoniometric method
        // triangleActualValue = ((2 * triangleWaveSettings->waveAmplitude)/3.145192)*asin(sin(((2 * 3.145192)/triangleWaveSettings->wavePeriod) * triangleWaveSettings->calculationTime));

        // updating inner wave calculation time based on set initial value of calculationTime...
        triangleWaveSettings->calculationTime = triangleWaveSettings->calculationTime + triangleWaveSettings->calculationStep;

        return(triangleActualValue);
    }


    float minMaxCommonModeVoltageKernel(CoreInternalVariablesType *coreInternalVariables)
    {
        return(((fmax(fmax(coreInternalVariables->u1a, coreInternalVariables->u1b), coreInternalVariables->u1c ) ) + (fmin(fmin(coreInternalVariables->u1a, coreInternalVariables->u1b), coreInternalVariables->u1c ) ))/2);
    }

    bool comparationLevelTriangleWaveComparationKernel(float compareLevel, float triangleWaveValue)
    {
        if(compareLevel>=triangleWaveValue)
        {
            return(true);
        }
        else
        {
            return(false);
        }
    }

    float createCompareLevelKernel(float levelConstant, float commonModeVoltage, float phaseWantedVoltage)
    {
        return((phaseWantedVoltage - commonModeVoltage)/levelConstant);
    }

    void krnl_calculateCurVelModel(float *masterInput, float *masterOutput )
    {
      

        RegulatorType idRegulator;
        RegulatorType iqRegulator;
        RegulatorType fluxRegulator;
        RegulatorType velocityRegulator;
        CoreInternalVariablesType coreInternalVariables;
        TriangleWaveSettingsType triangleWaveSettings;
        InvertorSwitchType invertorSwitch;

         /*
        * @brief Initial values settings
        *
        * */
        float R2MLmDL2Temp[8];
        float R2DL2Temp[8];
        float transformationAngle;
        float psi2amplitude;
        float trianglePoint;
        float commonModeVoltage;
        float psi2alpha;
        float psi2beta;
        float i1alpha;
        float i1beta;

        float globalSimulationTime = masterInput[0];
        float globalCalculationStep =masterInput[1];
        float minMaxCommonModeVoltageConstant = masterInput[2];
        float minMaxCommonModeVoltageConstantTemp[3];
        float halfCalculationStep =  masterInput[3];
        float inputI1 = masterInput[4];
        float inputI2 = masterInput[5];
        float inputI3 = masterInput[6];
        float inputMotorMechanicalAngularVelocity = masterInput[7];
        float R2MLmDL2 = masterInput[8];
        float R2DL2 = masterInput[9];
        float numberOfPolePairs = masterInput[10];
        triangleWaveSettings.waveAmplitude = masterInput[11];
        triangleWaveSettings.calculationStep = masterInput[12];
        triangleWaveSettings.wavePeriod = masterInput[13];
        triangleWaveSettings.calculationTime = masterInput[14];
        float Udcmax = masterInput[15];
        fluxRegulator.ki = masterInput[16];
        fluxRegulator.kp = masterInput[17];
        fluxRegulator.saturationOutputMax = masterInput[18];
        fluxRegulator.saturationOutputMin = masterInput[19];
        fluxRegulator.iSum = masterInput[20];
        velocityRegulator.ki = masterInput[21];
        velocityRegulator.kp = masterInput[22];
        velocityRegulator.saturationOutputMax = masterInput[23];
        velocityRegulator.saturationOutputMin = masterInput[24];
        velocityRegulator.iSum = masterInput[25];
        idRegulator.ki = masterInput[26];
        idRegulator.kp = masterInput[27];
        idRegulator.saturationOutput = masterInput[28];
        idRegulator.iSum = masterInput[29];
        iqRegulator.ki = masterInput[30];
        iqRegulator.kp = masterInput[31];
        iqRegulator.iSum = masterInput[32];
        fluxRegulator.wantedValue = masterInput[33];
        velocityRegulator.wantedValue = masterInput[34];
        idRegulator.wantedValue = masterInput[35];
        iqRegulator.wantedValue = masterInput[36];
        psi2alpha = masterInput[37];
        psi2beta = masterInput[38];
        idRegulator.saturationOutputMax = masterInput[39];
        idRegulator.saturationOutputMin = masterInput[40];

        sliceInternalVariables8Parts(R2MLmDL2, R2MLmDL2Temp);
        sliceInternalVariables8Parts(R2DL2, R2DL2Temp);

        float *psi2alpha_ptr = &psi2alpha;
        float *psi2beta_ptr = &psi2beta;
        float *i1alpha_ptr = &i1alpha;
        float *i1beta_ptr = &i1beta;
        float *transformationAngle_ptr = &transformationAngle; 
        float *psi2amplitude_ptr = &psi2amplitude;
        float transformationAngleTemp[8];
        float commonModeVoltageTemp[3];
        float trianglePointTemp[3];

        computeCurVelKernel(psi2alpha_ptr, psi2beta_ptr, inputI1, inputI2, inputI3, numberOfPolePairs, R2MLmDL2Temp, R2DL2Temp, inputMotorMechanicalAngularVelocity, globalSimulationTime, globalCalculationStep, halfCalculationStep, i1alpha_ptr, i1beta_ptr);


        outputCurVelProductsCalc(transformationAngle_ptr, psi2amplitude_ptr, psi2alpha, psi2beta);

        sliceInternalVariables8Parts(transformationAngle, transformationAngleTemp);

        // regulator new values + constrains
        idRegulator.measuredValue = ((i1alpha * cos(transformationAngleTemp[0])) + (i1beta * sin(transformationAngleTemp[1])));
        iqRegulator.measuredValue = ((-i1alpha * sin(transformationAngleTemp[2])) + (i1beta * cos(transformationAngleTemp[3])));


        fluxRegulator.measuredValue = psi2amplitude;
        velocityRegulator.measuredValue = inputMotorMechanicalAngularVelocity;
        iqRegulator.saturationOutputMax = sqrtf((Udcmax * Udcmax) - (idRegulator.saturationOutput * idRegulator.saturationOutput));
        iqRegulator.saturationOutputMin = - iqRegulator.saturationOutputMax;


      

        // calculating first set of regulators
        regCalculateKernel(&fluxRegulator);
        regCalculateKernel(&velocityRegulator);

        // new values
        idRegulator.wantedValue = fluxRegulator.saturationOutput;
        iqRegulator.wantedValue = velocityRegulator.saturationOutput;  

         // calculating second set of regulators
        regCalculateKernel(&idRegulator);
        regCalculateKernel(&iqRegulator);

        coreInternalVariables.u1d = idRegulator.saturationOutput;
        coreInternalVariables.u1q = iqRegulator.saturationOutput;

        coreInternalVariables.u1alpha = ((coreInternalVariables.u1d * cos(transformationAngleTemp[4])) - (coreInternalVariables.u1q * sin(transformationAngleTemp[5])));

        coreInternalVariables.u1beta = ((coreInternalVariables.u1d * sin(transformationAngleTemp[6])) + (coreInternalVariables.u1q * cos(transformationAngleTemp[7])));

        coreInternalVariables.u1a = coreInternalVariables.u1alpha;

        coreInternalVariables.u1b = ((-0.5 * coreInternalVariables.u1alpha) + 0.866 * coreInternalVariables.u1beta);

        coreInternalVariables.u1c = ((-0.5 * coreInternalVariables.u1alpha) - 0.866 * coreInternalVariables.u1beta);

        trianglePoint = generateActualValueTriangleWaveKernel(&triangleWaveSettings);
        commonModeVoltage = minMaxCommonModeVoltageKernel(&coreInternalVariables);

        sliceInternalVariables3Parts(commonModeVoltage, commonModeVoltageTemp);
        sliceInternalVariables3Parts(trianglePoint, trianglePointTemp);
        sliceInternalVariables3Parts(minMaxCommonModeVoltageConstant, minMaxCommonModeVoltageConstantTemp);

        invertorSwitch.sw1 = comparationLevelTriangleWaveComparationKernel(createCompareLevelKernel(minMaxCommonModeVoltageConstantTemp[0], commonModeVoltageTemp[0],coreInternalVariables.u1a), trianglePointTemp[0]);

        invertorSwitch.sw3 = comparationLevelTriangleWaveComparationKernel(createCompareLevelKernel(minMaxCommonModeVoltageConstantTemp[1], commonModeVoltageTemp[1],coreInternalVariables.u1b), trianglePointTemp[1]);

        invertorSwitch.sw5 = comparationLevelTriangleWaveComparationKernel(createCompareLevelKernel(minMaxCommonModeVoltageConstantTemp[2], commonModeVoltageTemp[2],coreInternalVariables.u1c), trianglePointTemp[2]);

        invertorSwitch.sw4 = !invertorSwitch.sw1;
        invertorSwitch.sw6 = !invertorSwitch.sw3;
        invertorSwitch.sw2 = !invertorSwitch.sw5;


        masterOutput[0] = invertorSwitch.sw1;
        masterOutput[1] = invertorSwitch.sw2;
        masterOutput[2] = invertorSwitch.sw3;
        masterOutput[3] = invertorSwitch.sw4;
        masterOutput[4] = invertorSwitch.sw5;
        masterOutput[5] = invertorSwitch.sw6;

        masterOutput[6] = triangleWaveSettings.calculationTime;
        masterOutput[7] = fluxRegulator.iSum;
        masterOutput[8] = velocityRegulator.iSum;
        masterOutput[9] = idRegulator.saturationOutput;
        masterOutput[10] = idRegulator.iSum;
        masterOutput[11] = iqRegulator.iSum;
        masterOutput[12] = psi2alpha;
        masterOutput[13] = psi2beta;
        masterOutput[14] = psi2amplitude;
        masterOutput[15] = idRegulator.measuredValue;
        masterOutput[16] = idRegulator.wantedValue;
        masterOutput[17] = transformationAngle;
        masterOutput[18] = velocityRegulator.saturationOutput;
        masterOutput[19] = velocityRegulator.clampingStatus;
    }