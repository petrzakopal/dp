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


class svmCoreClass
{
    public:
    PhaseWantedVoltageType *phaseWantedVoltage = NULL;

    void phaseWantedVoltageAllocateMemory();

    float minMaxCommonModeVoltage(PhaseWantedVoltageType *phaseWantedVoltage );

};





#endif