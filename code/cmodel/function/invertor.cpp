#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <cmath>
#include "./../header/invertor.h"
#include "./../header/svmCore.h"

void InvertorClass::invertorReconstructVoltages(InvertorSwitchType *invertorSwitch, CoreInternalVariablesType *coreInternalVariables, float uDC)
{
    float uDCd3 = (uDC)/3;

    coreInternalVariables->u1a = uDCd3 * ((2 * invertorSwitch->sw1) - ( invertorSwitch->sw3) - (invertorSwitch->sw5));
    coreInternalVariables->u1b = uDCd3 * (- (invertorSwitch->sw1) + (2 * invertorSwitch->sw3) - (invertorSwitch->sw5));
    coreInternalVariables->u1c = uDCd3 * (- (invertorSwitch->sw1) - ( invertorSwitch->sw3) + (2 * invertorSwitch->sw5));
  
}