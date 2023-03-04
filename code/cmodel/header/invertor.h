#include <stdlib.h>
#include <cmath>
#include "./../header/svmCore.h"

#ifndef INVERTOR_H
#define	INVERTOR_H

/*---------------------------------------------------------------------------------------*/
/*-------------------- TYPE/STRUCT FOR INVERTOR PHASE VOLTAGE OUTPUT --------------------*/
typedef struct
{
    float u1a;
    float u1b;
    float u1c;
}ReconstructedInvertorOutputVoltageType;
/*---------------------------------------------------------------------------------*/


class InvertorClass
{
    public:
        ReconstructedInvertorOutputVoltageType *reconstructedInvertorOutputVoltage;

        /*-------------------------------------------------------------------------------------------------------------------*/
        /*------------------------------- ALLOCATE MEMORY FOR INVEROTR PHASE VOLTAGE OUTPUT ---------------------------------*/
        void reconstrutedInvertorOutputVoltageAllocateMemory();
        /*-------------------------------------------------------------------------------------------------------------------*/


        /*--------------------------------------------------------------------------------------------------------------*/
        /*------------------------------- SIMULATION INVERTOR PHASE VOLTAGE GENERATION ---------------------------------*/
        void invertorReconstructVoltages(InvertorSwitchType *invertorSwitch, ReconstructedInvertorOutputVoltageType *ReconstructedInvertorOutputVoltage, float uDC);
        /*-------------------------------------------------------------------------------------------------------------*/
};



#endif
