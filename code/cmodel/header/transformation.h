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
};