#include "../header/regulator.h"
#include <stdlib.h>
#include <stdio.h>
#include <iostream>

/*-----------------------------------------------------------------------------------------------------*/
/*------------------------------- ALLOCATE MEMORY FOR REGULATOR DATA ---------------------------------*/
void RegulatorClass::regulatorAllocateMemory()
{
        posix_memalign((void **)&fluxRegulator , 4096 , sizeof(RegulatorType) );
        posix_memalign((void **)&velocityRegulator , 4096 , sizeof(RegulatorType) );
        posix_memalign((void **)&idRegulator , 4096 , sizeof(RegulatorType) );
        posix_memalign((void **)&iqRegulator , 4096 , sizeof(RegulatorType) );
 
        
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
    regulatorData->eDif = regulatorData->wantedValue - regulatorData->measuredValue;
    regulatorData->saturationInput = regulatorData->eDif * regulatorData->kp + regulatorData->iSum;
    regulatorData->saturationOutput = regSaturationBlock(regulatorData->saturationInput, regulatorData->saturationOutputMin, regulatorData->saturationOutputMax);
    regulatorData->antiWindUpDif = regulatorData->saturationOutput - regulatorData->saturationInput;
    regulatorData->iSum = regulatorData->iSum + (regulatorData->eDif * regulatorData->ki) + (regulatorData->antiWindUpDif * regulatorData->kAntiWindUp);
}