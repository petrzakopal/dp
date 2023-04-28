#ifndef SVMCORE_H
#define SVMCORE_H

#include <stdlib.h>
#include <cmath>

// deprecated
// typedef struct
// {
//     float u1a;
//     float u1b;
//     float u1c;
// }PhaseWantedVoltageType;

/*-----------------------------------------------------------------------------------------------------*/
/*------------------------------ CORE INTERNAL VALUES OF SVM CORE CODE --------------------------------*/
typedef struct
{
    float u1d;
    float u1q;
    float u1alpha;
    float u1beta;
    float u1a;
    float u1b;
    float u1c;
} CoreInternalVariablesType;
/*-----------------------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------*/
/*------------------------------ INVERTOR SWITCHES --------------------------------*/
typedef struct
{
    bool sw1;
    bool sw2;
    bool sw3;
    bool sw4;
    bool sw5;
    bool sw6;
} InvertorSwitchType;
/*----------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------*/
/*------------------------------ TRIANGLE WAVE SETTINGS --------------------------------*/
typedef struct
{
    float waveAmplitude;
    float wavePeriod;
    float calculationStep;
    float calculationTime;

} TriangleWaveSettingsType;
/*---------------------------------------------------------------------------------------*/

class svmCoreClass
{
public:
    // PhaseWantedVoltageType *phaseWantedVoltage = NULL; // deprecated
    InvertorSwitchType *invertorSwitch = NULL;
    TriangleWaveSettingsType *triangleWaveSettings = NULL;
    CoreInternalVariablesType *coreInternalVariables = NULL;

    // void phaseWantedVoltageAllocateMemory(); // deprecated
    void invertorSwitchAllocateMemory();
    void triangleWaveSettingsAllocateMemory();
    void coreInternalVariablesAllocateMemory();

    float minMaxCommonModeVoltage(CoreInternalVariablesType *coreInternalVariables);

    float createCompareLevel(float levelConstant, float commonModeVoltage, float phaseWantedVoltage);

    float generateActualValueTriangleWave(TriangleWaveSettingsType *triangleWaveSettings);

    bool comparationLevelTriangleWaveComparation(float compareLevel, float triangleWaveValue);
};

#endif /* SVMCORE_H */