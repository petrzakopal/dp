#include "modelFunctions.h"
#include <cmath>
// Defining the differential equation to be solved
float dydx(float x, float y)
{
    return((x-y)/2);
}