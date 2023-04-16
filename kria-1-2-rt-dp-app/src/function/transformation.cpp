#include "../header/transformation.h"
#include <cmath>

/*------------------------------------------------------------------------------------------*/


/*-----------------------------------------------------------------------------------------------------------------------------------*/
/*---------------------------- ALLOCATING DATA FOR GENERATED CLARKETRANSFORM IF IS DESIRED TO SAVE DATA -----------------------------*/
// NOT USED NOW
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

/*---------------------------------------------------------------------------------------------------------------------------*/
/*-------------------------------- CALTULATION OF CLARKE TRANSFORM BASED ON INPUT VALUES ----------------------------------*/
float TransformationClass::clarkeTransform1(float input1, float input2, float input3, float transformConstant)
{
    return(transformConstant * (input1 - (0.5 * input2) - (0.5 * input3)));
}
float TransformationClass::clarkeTransform2(float input1, float input2, float input3, float transformConstant)
{
    return(transformConstant * (0.866 * input2 - 0.866 * input3));
}
/*---------------------------------------------------------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------------------------------------------------------*/
/*-------------------------------- CALTULATION OF REVERSE CLARKE TRANSFORM BASED ON INPUT VALUES ----------------------------------*/
float TransformationClass::inverseClarkeTransform1(float inputAlpha, float inputBeta)
{
    return(inputAlpha);
}

float TransformationClass::inverseClarkeTransform2(float inputAlpha, float inputBeta)
{
    return((-0.5 * inputAlpha) + 0.866 * inputBeta);
}

float TransformationClass::inverseClarkeTransform3(float inputAlpha, float inputBeta)
{
    return((-0.5 * inputAlpha) - 0.866 * inputBeta); // or -(xa + xb)
}
/*---------------------------------------------------------------------------------------------------------------------------*/




/*---------------------------------------------------------------------------------------------------------------------------*/
/*-------------------------------- CALTULATION OF PARK TRANSFORM BASED ON INPUT VALUES ----------------------------------*/
float TransformationClass::parkTransform1(float inputAlpha, float inputBeta, float transformAngle)
{
    return((inputAlpha * cos(transformAngle)) + (inputBeta * sin(transformAngle)));
}

float TransformationClass::parkTransform2(float inputAlpha, float inputBeta, float transformAngle)
{
    return((-inputAlpha * sin(transformAngle)) + (inputBeta * cos(transformAngle)));
}
/*---------------------------------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------------------------------*/
/*-------------------------------- CALTULATION OF INVERSE PARK TRANSFORM BASED ON INPUT VALUES ----------------------------------*/
float TransformationClass::inverseParkTransform1(float inputD, float inputQ, float transformAngle)
{
    return((inputD * cos(transformAngle)) - (inputQ * sin(transformAngle)));
}

float TransformationClass::inverseParkTransform2(float inputD, float inputQ, float transformAngle)
{
    return((inputD * sin(transformAngle)) + (inputQ * cos(transformAngle)));
}
/*---------------------------------------------------------------------------------------------------------------------------*/