#include <stdlib.h>
#include <cmath>
#include "./../header/svmCore.h"

#ifndef INVERTOR_H
#define	INVERTOR_H

class InvertorClass
{
    public:
        void invertorReconstructVoltages(InvertorSwitchType *invertorSwitch, CoreInternalVariablesType *CoreInternalVariables, float uDC);
};



#endif
