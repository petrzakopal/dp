#include "../header/regulator.h"
#include <stdlib.h>

/*-----------------------------------------------------------------------------------------------------*/
/*------------------------------- ALLOCATE MEMORY FOR REGULATOR DATA ---------------------------------*/
void RegulatorClass::regulatorAllocateMemory(RegulatorType *regulatorData)
{
        posix_memalign((void **)&regulatorData , 4096 , sizeof(RegulatorType) );
}
/*-----------------------------------------------------------------------------------------------------*/

float RegulatorClass::regSaturationBlock(float saturationInput, float saturationOutputMin, float saturationOutputMax)
{
        float localSaturationInput = saturationInput;

        if(localSaturationInput > saturationOutputMax)
        {
            localSaturationInput = saturationOutputMax;
        }
        else if(localSaturationInput < saturationOutputMin)
        {
            localSaturationInput = saturationOutputMin;
        }

        return(localSaturationInput);

}




void RegulatorClass::regCalculate(RegulatorType *regulatorData)
{
    regulatorData->eDif = regulatorData->wanted - regulatorData->measured;
    regulatorData->saturationInput = regulatorData->eDif * regulatorData->kp + regulatorData->iSum;
    regulatorData->saturationOutput = regSaturationBlock(regulatorData->saturationInput, regulatorData->saturationOutputMin, regulatorData->saturationOutputMax);
    regulatorData->antiWindUpDif = regulatorData->saturationOutput - regulatorData->saturationInput;
    regulatorData->iSum = regulatorData->iSum + (regulatorData->eDif * regulatorData->ki) + (regulatorData->antiWindUpDif * regulatorData->kAntiWindUp);
}