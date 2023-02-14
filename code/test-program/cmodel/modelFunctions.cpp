#include "modelFunctions.h"
#include <cmath>
// Defining the differential equation to be solved


// diff. equation is: dy/dt = t*R1
float i1alpha(float t, float out, motorParametersStruct motorParameters)
{
 return (t * motorParameters.R1);
}