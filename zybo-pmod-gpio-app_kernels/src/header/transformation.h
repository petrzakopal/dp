#include <hls_math.h>

typedef struct clarkeTransformStruct
{
   float input1;
   float input2;
   float input3;
   float output1;
   float output2;
   float transformConstant;
}clarkeTransformType;





class TransformationClass
{
    public:
    clarkeTransformType *clarkeTransformData = NULL;

    void clarkeTransformDataAllocateMemory();
    void clarkeTransform(clarkeTransformType *clarkeTransformData);


    float clarkeTransform1(float input1, float input2, float input3, float transformConstant);
    float clarkeTransform2(float input1, float input2, float input3, float transformConstant);


    float inverseClarkeTransform1(float inputAlpha, float inputBeta);
    float inverseClarkeTransform2(float inputAlpha, float inputBeta);
    float inverseClarkeTransform3(float inputAlpha, float inputBeta);


    float parkTransform1(float inputAlpha, float inputBeta, float transformAngle);
    float parkTransform2(float inputAlpha, float inputBeta, float transformAngle);

    float inverseParkTransform1(float inputD, float inputQ, float transformAngle);
    float inverseParkTransform2(float inputD, float inputQ, float transformAngle);
    float inverseParkTransform3(float inputD, float inputQ, float transformAngle);
};