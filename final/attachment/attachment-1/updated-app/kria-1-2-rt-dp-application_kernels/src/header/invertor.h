#include <stdlib.h>
#include <hls_math.h>

#include "../header/svmCore.h"


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
        void reconstructedInvertorOutputVoltageAllocateMemory();
        /*-------------------------------------------------------------------------------------------------------------------*/


        /*--------------------------------------------------------------------------------------------------------------*/
        /*------------------------------- SIMULATION INVERTOR PHASE VOLTAGE GENERATION ---------------------------------*/
        void invertorReconstructVoltages(InvertorSwitchType *invertorSwitch, ReconstructedInvertorOutputVoltageType *ReconstructedInvertorOutputVoltage, float uDC);
        /*-------------------------------------------------------------------------------------------------------------*/
};
