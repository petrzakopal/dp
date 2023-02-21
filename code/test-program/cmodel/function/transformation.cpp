#include "../header/transformation.h"

/*------------------------------------------------------------------------------------------*/
void TransformationClass::clarkeTransformDataAllocateMemory()
{
     posix_memalign((void **)&clarkeTransformData , 4096 , sizeof(clarkeTransformType) );
}


void TransformationClass::clarkeTransform(clarkeTransformType *clarkeTransformData)
{
    

    clarkeTransformData->output1 = clarkeTransformData->transformConstant * (clarkeTransformData->input1 - 0.5 * clarkeTransformData->input2 - 0.5 * clarkeTransformData->input3);

    clarkeTransformData->output2 = clarkeTransformData->transformConstant * (0.866 * clarkeTransformData->input2 - 0.866 * clarkeTransformData->input3);



}
/*------------------------------------------------------------------------------------------*/


float TransformationClass::clarkeTransform1(float input1, float input2, float input3, float transformConstant)
{
    return(transformConstant * (input1 - 0.5 * input2 - 0.5 * input3));
}
float TransformationClass::clarkeTransform2(float input1, float input2, float input3, float transformConstant)
{
    return(transformConstant * (0.866 * input2 - 0.866 * input3));
}