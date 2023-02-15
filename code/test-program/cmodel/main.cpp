#include <iostream>
#include <cmath>
#include "header/modelFunctions.h"
#include <vector>
#include "header/mathSolver.h"

int main()
{
   // motorParametersType motorParametersData = initializeMotorParameters();
   // motorParametersType motorParameters = initializeMotorParameters();; // testing purposes
motorParametersType motorParametersData;

   motorParametersData.R1 = 0.370f; // Ohm, stator rezistance
   motorParametersData.R2 = 0.225f; // Ohm, rotor rezistance
   motorParametersData.L1s = 0.00227f; // H, stator leakage inductance
   motorParametersData.L2s = 0.00227f; // H, rotor leakage inductance
   motorParametersData.Lm = 0.0825f; // H, main flux inductance
   motorParametersData.L1 = 0.08477f; // H, inductance
   motorParametersData.L2 = 0.08477f; // H, inductance
   motorParametersData.sigma = 0.05283f; // = 0.0528396032, sigma = 1 - Lm^(2)/L1L2

MotorModelClass MotorModel;
MotorModel.setMotorParameters(motorParametersData);


float t0 = 0; // starting time
float out0 = 1; // starting output - basically => fce(t0) = out0
float t = 2; // time of solution
float h = 0.000001; // step of RK method
std::cout << "Testing RK4\n" << "| ----------------------- |\n";

// float a11 = -(( motorParameters.R2 * motorParameters.Lm * motorParameters.Lm) + ( motorParameters.L2 * motorParameters.L2 * motorParameters.R1) / (motorParameters.sigma * motorParameters.L1 * motorParameters.L2 * motorParameters.L2));



std::cout << "motor initialized parameters R1= "<< MotorModel.getMotorParameters().R1 << "\n";

// std::cout << "a11= "<<a11<<"\n";
// std::cout << "i1alpha: " << rungeKutta(t0, out0, t, h, motorParametersData, &i1alpha) << "\n";
return 0;
}
