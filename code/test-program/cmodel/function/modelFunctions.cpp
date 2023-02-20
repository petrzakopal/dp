#include "../header/modelFunctions.h"
#include <cmath>
// Defining the differential equation to be solved


// diff. equation is: dy/dt = t*R1
float i1alpha(float t, stateSpaceCoeffType *stateSpaceCoeff, modelVariablesType *modelVariables)
{
    float out;
    out = (stateSpaceCoeff->a11 * modelVariables->i1alpha /* + stateSpaceCoeff->a12 * i1beta => is 0 */ + stateSpaceCoeff->a13 * modelVariables->psi2alpha + stateSpaceCoeff->a14 * modelVariables->psi2beta) + stateSpaceCoeff->b11 * modelVariables->u1alpha;
 
    return (out);

}


