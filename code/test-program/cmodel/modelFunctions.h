#include <cmath>


// motorParameters type of struct
// for defining motorParameters for a mathematical model
typedef struct motorParameters
{
    float R1;
    float R2;
    float L1s;
    float L2s;
    float Lm;
    float L1;
    float L2;
}motorParametersStruct;



// stator current in alpha, beta system => not rotating system (connected with stator)
// t -> input time variable
// out -> output variable = i1alpha
// motorParameters -> motor parameters defined for used motor
float i1alpha(float t, float out, motorParametersStruct motorParameters);