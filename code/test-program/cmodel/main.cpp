#include <iostream>
#include <cmath>
#include "modelFunctions.h"
#include <vector>



// Runge Kutta RK4 Algorithmus
float rungeKutta(float t0, float out0, float t, float h, motorParametersStruct motorParameters, float (*func)(float, float, motorParametersStruct ))
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

// typedef struct motorParameters
// {
//     float R1;
//     float R2;
//     float L1s;
//     float L2s;
//     float Lm;
//     float L1;
//     float L2;
// }motorParametersStruct;



motorParameters initializeMotorParameters()
{
    motorParametersStruct motorParameters;
    motorParameters.R1 = 48;

    return motorParameters;
}


int main()
{
   motorParametersStruct motorParametersData = initializeMotorParameters();
   

float t0 = 0; // starting time
float out0 = 1; // starting output - basically => fce(t0) = out0
float t = 2; // time of solution
float h = 0.000001; // step of RK method

std::cout << "Result for rungeKutta eq:\n" << rungeKutta(t0, out0, t, h, motorParametersData, &i1alpha) << "\n";
return 0;
}
