#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <cmath>
#include "./../header/invertor.h"
#include "./../header/svmCore.h"


/*-----------------------------------------------------------------------------------------------------------------------------------*/
/*------------------------------- MEMORY ALLOCATION FOR INVERTOR OUTPUT PHASE VOLTAGE IN SIMULATION ---------------------------------*/
 void InvertorClass::reconstructedInvertorOutputVoltageAllocateMemory()
 {
    posix_memalign((void **)&reconstructedInvertorOutputVoltage , 4096 , sizeof(ReconstructedInvertorOutputVoltageType) );
 }
/*-----------------------------------------------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------------------*/
/*------------------------------- INVERTOR OUTPUT PHASE VOLTAGE IN SIMULATION ---------------------------------*/
void InvertorClass::invertorReconstructVoltages(InvertorSwitchType *invertorSwitch, ReconstructedInvertorOutputVoltageType *reconstructedInvertorOutputVoltage, float uDC)
{
    float uDCd3 = (uDC)/3;

    reconstructedInvertorOutputVoltage->u1a = uDCd3 * ((2 * invertorSwitch->sw1) - ( invertorSwitch->sw3) - (invertorSwitch->sw5));
    reconstructedInvertorOutputVoltage->u1b = uDCd3 * (- (invertorSwitch->sw1) + (2 * invertorSwitch->sw3) - (invertorSwitch->sw5));
    reconstructedInvertorOutputVoltage->u1c = uDCd3 * (- (invertorSwitch->sw1) - ( invertorSwitch->sw3) + (2 * invertorSwitch->sw5));
  
}
/*--------------------------------------------------------------------------------------------------------------*/
/*------------------------------- INVERTOR OUTPUT PHASE VOLTAGE IN SIMULATION ---------------------------------*/