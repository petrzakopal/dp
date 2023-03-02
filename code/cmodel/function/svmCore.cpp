#include <cmath>
#include <stdlib.h>
#include <iostream>
#include "../header/svmCore.h"


void svmCoreClass::phaseWantedVoltageAllocateMemory()
{
    posix_memalign((void **)&phaseWantedVoltage , 4096 , sizeof(PhaseWantedVoltageType) );
}


float svmCoreClass::minMaxCommonModeVoltage(PhaseWantedVoltageType *phaseWantedVoltage)
{
    return(((fmax(fmax(phaseWantedVoltage->u1a, phaseWantedVoltage->u1b), phaseWantedVoltage->u1c ) ) + (fmin(fmin(phaseWantedVoltage->u1a, phaseWantedVoltage->u1b), phaseWantedVoltage->u1c ) ))/2);
}