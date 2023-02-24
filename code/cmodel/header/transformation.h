#include <cmath>

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


    float reverseClarkeTransform1(float inputAlpha, float inputBeta, float transformConstant);
    float reverseClarkeTransform2(float inputAlpha, float inputBeta, float transformConstant);
    float reverseClarkeTransform3(float inputAlpha, float inputBeta, float transformConstant);


    float parkTransform1(float inputAlpha, float inputBeta, float transformAngle);
    float parkTransform2(float inputAlpha, float inputBeta, float transformAngle);

    float reverseParkTransform1(float inputD, float inputQ, float transformAngle);
    float reverseParkTransform2(float inputD, float inputQ, float transformAngle);
    float reverseParkTransform3(float inputD, float inputQ, float transformAngle);
};