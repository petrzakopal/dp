#include <stdlib.h>
#include <cmath>

#ifndef SVMCORE_H
#define	SVMCORE_H


typedef struct
{
    float u1a;
    float u1b;
    float u1c;
}PhaseWantedVoltageType;



typedef struct
{
    bool sw1;
    bool sw2;
    bool sw3;
    bool sw4;
    bool sw5;
}InvertorSwitchType;



typedef struct
{
    float waveAmplitude;
    float wavePeriod;
    float calculationStep;
    float calculationTime;

}TriangleWaveSettingsType;



class svmCoreClass
{
    public:
    PhaseWantedVoltageType *phaseWantedVoltage = NULL;
    InvertorSwitchType *invertorSwitch = NULL;
    TriangleWaveSettingsType *triangleWaveSettings = NULL;

    void phaseWantedVoltageAllocateMemory();
    void invertorSwitchAllocateMemory();
    void triangleWaveSettingsAllocateMemory();

    float minMaxCommonModeVoltage(PhaseWantedVoltageType *phaseWantedVoltage);

    float createCompareLevel(float levelConstant, float commonModeVoltage, float phaseWantedVoltage);

    float generateActualValueTriangleWave(TriangleWaveSettingsType *triangleWaveSettings);

    bool comparationLevelTriangleWaveComparation();
};





#endif