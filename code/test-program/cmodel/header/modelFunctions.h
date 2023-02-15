#include <cmath>
#include "parameterInitialization.h"




// stator current in alpha, beta system => not rotating system (connected with stator)
// t -> input time variable
// out -> output variable = i1alpha
// motorParameters -> motor parameters defined for used motor
float i1alpha(float t, float out, motorParametersStruct motorParameters);