#include <cmath>
#include <stdlib.h>
#include <iostream>
#include "../header/svmCore.h"


void svmCoreClass::phaseWantedVoltageAllocateMemory()
{
    posix_memalign((void **)&phaseWantedVoltage , 4096 , sizeof(PhaseWantedVoltageType) );
}


void svmCoreClass::invertorSwitchAllocateMemory()
{
    posix_memalign((void **)&invertorSwitch , 4096 , sizeof(InvertorSwitchType) );
}


void svmCoreClass::triangleWaveSettingsAllocateMemory()
{
    posix_memalign((void **)&triangleWaveSettings , 4096 , sizeof(TriangleWaveSettingsType) );
}

float svmCoreClass::minMaxCommonModeVoltage(PhaseWantedVoltageType *phaseWantedVoltage)
{
    return(((fmax(fmax(phaseWantedVoltage->u1a, phaseWantedVoltage->u1b), phaseWantedVoltage->u1c ) ) + (fmin(fmin(phaseWantedVoltage->u1a, phaseWantedVoltage->u1b), phaseWantedVoltage->u1c ) ))/2);
}


float svmCoreClass::createCompareLevel(float levelConstant, float commonModeVoltage, float phaseWantedVoltage)
{
    return((phaseWantedVoltage - commonModeVoltage)/levelConstant);
}


float svmCoreClass::generateActualValueTriangleWave(TriangleWaveSettingsType *triangleWaveSettings)
 {

        float triangleActualValue; // maybe put in triangleWaveSettings, do not know, this should be used only as a local variable

        // numerical method with abs value and modulo
        triangleActualValue = (((4 * triangleWaveSettings->waveAmplitude)/triangleWaveSettings->wavePeriod) * abs(fmod((fmod((triangleWaveSettings->calculationTime-(triangleWaveSettings->wavePeriod/4)), triangleWaveSettings->wavePeriod) +  triangleWaveSettings->wavePeriod),  triangleWaveSettings->wavePeriod) - ( triangleWaveSettings->wavePeriod/2)) - triangleWaveSettings->waveAmplitude);

        // trigoniometric method
        // triangleActualValue = ((2 * triangleWaveSettings->waveAmplitude)/3.145192)*asin(sin(((2 * 3.145192)/triangleWaveSettings->wavePeriod) * triangleWaveSettings->calculationTime));

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