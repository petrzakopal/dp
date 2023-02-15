#include <iostream>
#include <cmath>
#include "header/modelFunctions.h"
#include <vector>
#include "header/mathSolver.h"

int main()
{
   motorParametersStruct motorParametersData = initializeMotorParameters();
   motorParametersStruct motorParameters = initializeMotorParameters();; // testing purposes
float t0 = 0; // starting time
float out0 = 1; // starting output - basically => fce(t0) = out0
float t = 2; // time of solution
float h = 0.000001; // step of RK method
std::cout << "Testing RK4\n" << "| ----------------------- |\n";

float a11 = -(( motorParameters.R2 * motorParameters.Lm * motorParameters.Lm) + ( motorParameters.L2 * motorParameters.L2 * motorParameters.R1) / (motorParameters.sigma * motorParameters.L1 * motorParameters.L2 * motorParameters.L2));



std::cout << "a11= "<<a11<<"\n";
// std::cout << "i1alpha: " << rungeKutta(t0, out0, t, h, motorParametersData, &i1alpha) << "\n";
return 0;
}
