#include <stdlib.h>
#include <cmath>
#include "./../header/svmCore.h"

#ifndef INVERTOR_H
#define	INVERTOR_H


typedef struct
{
    float u1a;
    float u1b;
    float u1c;
}ReconstructedInvertorOutputVoltageType;

class InvertorClass
{
    public:
        ReconstructedInvertorOutputVoltageType *reconstructedInvertorOutputVoltage;
        void reconstrutedInvertorOutputVoltageAllocateMemory();
        void invertorReconstructVoltages(InvertorSwitchType *invertorSwitch, ReconstructedInvertorOutputVoltageType *ReconstructedInvertorOutputVoltage, float uDC);
};



#endif
