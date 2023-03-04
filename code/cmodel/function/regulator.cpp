#include "../header/regulator.h"
#include <stdlib.h>
#include <stdio.h>
#include <iostream>

/*-----------------------------------------------------------------------------------------------------*/
/*------------------------------- ALLOCATE MEMORY FOR REGULATOR DATA ---------------------------------*/
void RegulatorClass::regulatorAllocateMemory()
{
        posix_memalign((void **)&fluxRegulator , 4096 , sizeof(RegulatorType) );
        posix_memalign((void **)&velocityRegulator , 4096 , sizeof(RegulatorType) );
        posix_memalign((void **)&idRegulator , 4096 , sizeof(RegulatorType) );
        posix_memalign((void **)&iqRegulator , 4096 , sizeof(RegulatorType) );
 
        
}
/*-----------------------------------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------------------------------------------------------*/
/*------------------------------- REGULATOR SATURATION BLOCK TO SIMULATE ACTUATOR SATURATION ---------------------------------*/
float RegulatorClass::regSaturationBlock(float saturationInput, float saturationOutputMin, float saturationOutputMax)
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
/*----------------------------------------------------------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------------------------------------------------------------------------*/
/*------------------------------- STATUS CHECK IF THE SATURATION HAPPENED - USED FOR ANTI WINDUP VIA CLAMPLING ---------------------------------*/
void RegulatorClass::checkSaturationStatus(RegulatorType *regulatorData)
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
/*----------------------------------------------------------------------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------------------------------------------------------------------------------------*/
/*------------------------------- STATUS CHECK OF SIGNS FROM ERROR AND SATURATION INPUT - USED FOR ANTI WINDUP VIA CLAMPLING ---------------------------------*/
void RegulatorClass::checkSignStatus(RegulatorType *regulatorData)
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
/*------------------------------------------------------------------------------------------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------------------------------------------------*/
/*------------------------------- ENABLING CLAMPING BASED OF PRECEDING STATUSS CHECKS ---------------------------------*/
void RegulatorClass::enableClamping(RegulatorType *regulatorData)
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
/*----------------------------------------------------------------------------------------------------------------------*/


/*-----------------------------------------------------------------------------------------------------------------*/
/*------------------------------- ANTI WIND UP VIA BACK CALCULATION - DEPRACATED ---------------------------------*/
// void RegulatorClass::regCalculate(RegulatorType *regulatorData)
// {
//     regulatorData->eDif = regulatorData->wantedValue - regulatorData->measuredValue;
//     regulatorData->saturationInput = regulatorData->eDif * regulatorData->kp + regulatorData->iSum;
//     regulatorData->saturationOutput = regSaturationBlock(regulatorData->saturationInput, regulatorData->saturationOutputMin, regulatorData->saturationOutputMax);
//     regulatorData->antiWindUpDif = regulatorData->saturationOutput - regulatorData->saturationInput;
//     regulatorData->iSum = regulatorData->iSum + (regulatorData->eDif * regulatorData->ki) + (regulatorData->antiWindUpDif * regulatorData->kAntiWindUp);
// }
/*------------------------------------------------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------------------------------*/
/*------------------------------- ANTI WIND UP VIA BACK CLAMPING ---------------------------------*/
void RegulatorClass::regCalculate(RegulatorType *regulatorData)
{
    regulatorData->eDif = regulatorData->wantedValue - regulatorData->measuredValue;
    regulatorData->saturationInput = (regulatorData->eDif * regulatorData->kp) + regulatorData->iSum;
    regulatorData->saturationOutput = regSaturationBlock(regulatorData->saturationInput, regulatorData->saturationOutputMin, regulatorData->saturationOutputMax);

    checkSaturationStatus(regulatorData);
    checkSignStatus(regulatorData);
    enableClamping(regulatorData);

    if(regulatorData->clampingStatus == true)
    {
        regulatorData->iSum = regulatorData->iSum;
    }
    else
    {
        regulatorData->iSum = regulatorData->iSum + (regulatorData->eDif * regulatorData->ki);
    }
}
/*-------------------------------------------------------------------------------------------------*/