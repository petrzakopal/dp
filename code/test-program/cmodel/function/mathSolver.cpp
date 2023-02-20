#include "../header/initialization.h"

// Runge Kutta RK4 Algorithmus global algoritmus for one variable EQ, should be changed for 4 variables df/dt and two variables just f(t)
float rungeKutta(float t0, float out0, float t, float h, motorParametersType motorParameters, float (*func)(float, float, motorParametersType ))
{

    // Number of iterations 
    int n = (int)((t-t0)/h);
    float k1, k2, k3, k4;

    float out = out0;

    // Iterating for number of iterations
    for(int i = 1; i<=n;i++)
    {
        k1 = h*func(t0,out0, motorParameters);
        k2 = h*func(t0 + 0.5*h, out + 0.5*k1, motorParameters);
        k3 = h*func(t0 + 0.5*h, out + 0.5*k2, motorParameters);
        k4 = h*func(t0 + h, out + k3, motorParameters);

        // Updating the out and x values for new iteration
        out = out + (1.0/6.0) * (k1 + 2*k2 + 2*k3 + k4);
        t0 = t0 + h;
    }

    return out;
}



// step is better to get from host program, because kernel compilation takes much more longer than host program
// maybe in the future use struct type for inital condictions
float mathModelRK4(float t, float step, modelVariablesType modelVariables)
{
    // Number of iterations 
    // int n = (int)((t-odeInitialConditions.t0)/step);

    float k1i1alpha, k2i1alpha, k3i1alpha, k4i1alpha;
    float k1i1beta, k2i1beta, k3i1beta, k4i1beta;
    float k1psi2alpha, k2psi2alpha, k3psi2alpha, k4psi2alpha;
    float k1psi2beta, k2psi2beta, k3psi2beta, k4psi2beta;

    return(0);
}