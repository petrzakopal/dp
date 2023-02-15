#include <iostream>
#include <cmath>
#include "header/modelFunctions.h"
#include <vector>
#include "header/mathSolver.h"

int main()
{


MotorModelClass MotorModel;
MotorModel.motorAllocateMemory();
MotorModel.setTestMotorParameters();
MotorModel.setMotorParameters();
MotorModel.setStateSpaceCoeff();

   // motorParametersType *bufStruct = NULL;
   // posix_memalign((void **)&bufStruct , 4096 , sizeof(motorParametersType) );

float t0 = 0; // starting time
float out0 = 1; // starting output - basically => fce(t0) = out0
float t = 2; // time of solution
float h = 0.000001; // step of RK method
std::cout << "Testing RK4\n" << "| ----------------------- |\n";

float a11 = -(( MotorModel.getMotorParameters().R2 * MotorModel.getMotorParameters().Lm * MotorModel.getMotorParameters().Lm) + ( MotorModel.getMotorParameters().L2 * MotorModel.getMotorParameters().L2 * MotorModel.getMotorParameters().R1) / (MotorModel.getMotorParameters().sigma * MotorModel.getMotorParameters().L1 * MotorModel.getMotorParameters().L2 * MotorModel.getMotorParameters().L2));



std::cout << "R1 from posix_memalign: " << MotorModel.getTestMotorParameters()->R1 << "\n"; 

std::cout << "motor initialized parameters R1= "<< MotorModel.getMotorParameters().R1 << "\n";

std::cout << "a11= "<<a11<<"\n";
// std::cout << "i1alpha: " << rungeKutta(t0, out0, t, h, motorParametersData, &i1alpha) << "\n";
return 0;
}
