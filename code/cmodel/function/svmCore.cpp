#include <cmath>
#include <stdlib.h>
#include <iostream>
#include "../header/svmCore.h"

// deprecated
// void svmCoreClass::phaseWantedVoltageAllocateMemory()
// {
//     posix_memalign((void **)&phaseWantedVoltage , 4096 , sizeof(PhaseWantedVoltageType) );
// }

/*----------------------------------------------------------------------------------------------------------------------------------*/
/*------------------------------ ALLOCATING MEMORY FOR SWITCHES AS AN OUPUT FROM FOC SVM CODE MODEL --------------------------------*/
void svmCoreClass::invertorSwitchAllocateMemory()
{
    posix_memalign((void **)&invertorSwitch , 4096 , sizeof(InvertorSwitchType) );
}
/*----------------------------------------------------------------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------------------------------------*/
/*------------------------------ ALLOCATING MEMORY TRIANGLE WAVE SETTINGS --------------------------------*/
void svmCoreClass::triangleWaveSettingsAllocateMemory()
{
    posix_memalign((void **)&triangleWaveSettings , 4096 , sizeof(TriangleWaveSettingsType) );
}
/*----------------------------------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------------------------------------------------*/
/*------------------------------ ALLOCATING MEMORY SPACE VECTOR MODULATION CORE VARIABLES --------------------------------*/
void svmCoreClass::coreInternalVariablesAllocateMemory()
{
    posix_memalign((void **)&coreInternalVariables , 4096 , sizeof(CoreInternalVariablesType) );
}
/*------------------------------------------------------------------------------------------------------------------------*/



/*--------------------------------------------------------------------------------------------------------*/
/*------------------------------ CALCULATING MIN MAX COMMON MODE VOLTAGE --------------------------------*/
float svmCoreClass::minMaxCommonModeVoltage(CoreInternalVariablesType *coreInternalVariables)
{
    return(((fmax(fmax(coreInternalVariables->u1a, coreInternalVariables->u1b), coreInternalVariables->u1c ) ) + (fmin(fmin(coreInternalVariables->u1a, coreInternalVariables->u1b), coreInternalVariables->u1c ) ))/2);
}
/*--------------------------------------------------------------------------------------------------------*/


/*-------------------------------------------------------------------------------------------------*/
/*------------------------------ CREATING COMPARE LEVEL VOLTAGES --------------------------------*/
float svmCoreClass::createCompareLevel(float levelConstant, float commonModeVoltage, float phaseWantedVoltage)
{
    return((phaseWantedVoltage - commonModeVoltage)/levelConstant);
}
/*-------------------------------------------------------------------------------------------------*/


/*-------------------------------------------------------------------------------------------------*/
/*------------------------------ GENERATING ACTUAL TRIANGLE WAVE VALUE BASED ON CALCULATION TIME --------------------------------*/
float svmCoreClass::generateActualValueTriangleWave(TriangleWaveSettingsType *triangleWaveSettings)
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





 bool svmCoreClass::comparationLevelTriangleWaveComparation(float compareLevel, float triangleWaveValue)
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