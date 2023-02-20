#include <iostream>
#include <cmath>
#include "header/modelFunctions.h"
#include <vector>
#include "header/mathSolver.h"

#define PI 3.141592 

int main()
{


/*----------------------------------------------------------------------------------*/
/*-------------------- INITIALIZATION VIA MOTORMODEL CLASS API ---------------------*/
MotorModelClass MotorModel;
MotorModel.motorParametersAllocateMemory();
MotorModel.stateSpaceCoeffAllocateMemory();
// MotorModel.odeInitialConditionsAllocateMemory();
MotorModel.modelVariablesAllocateMemory();
MotorModel.setMotorParameters();
MotorModel.setStateSpaceCoeff();
MotorModel.setInitialModelVariables();
/*----------------------------------------------------------------------------------*/


/*------------------------------------------------------------------------*/
/*-------------------- OLD RK4 VARIABLE DEFINITIONS ---------------------*/
float t0 = 0; // starting time
float out0 = 1; // starting output - basically => fce(t0) = out0
float t = 2; // time of solution
float h = 0.000001; // step of RK method
/*------------------------------------------------------------------------*/


// Following motorElectricalAngularVelocity is bad interpretation. Real value will be calculated based on torque etc.
/*---------------------------------------------------------------------------------------*/
/*--------------------- PLACEHOLDER FOR CALCULATING MOTOR VELOCITY ----------------------*/
// Placeholder for creating motor angular velocity - this is input from external sensor or measured or desired velocity
float* motorElectricalAngularVelocity;
posix_memalign((void **)&motorElectricalAngularVelocity , 4096 , sizeof(float) );

float rotorRPM = 0;
for(int i = 0; i<=25; i++)
{
   
   motorElectricalAngularVelocity[i] = (2 * PI * rotorRPM * MotorModel.getMotorParameters()->nOfPolePairs)/60; 
   rotorRPM+= 1.5;
   // std::cout << "motor electrical velocity "<< motorElectricalAngularVelocity[i] << " rad/s \n";
}
/*---------------------------------------------------------------------------------------*/


// needs to be calculated every sample
MotorModel.calculateStateSpaceCoeff(12);

std::cout << "| ----------------------- |\n";



/*---------------------------------------------------------*/
/*-------------------- TEST OUTPUTS ----------------------*/

std::cout << "R1 from posix_memalign: " << MotorModel.getMotorParameters()->R1 << "\n"; 

std::cout << "motor initialized parameters R1= "<< MotorModel.getMotorParameters()->R1 << "\n";
std::cout << "\n" << "| ----------------------- |\n";
std::cout << "a11= " << MotorModel.getStateSpaceCoeff()->a11 <<"\n";
std::cout << "a14= " << MotorModel.getStateSpaceCoeff()->a14 <<"\n";
std::cout << "a44= " << MotorModel.getStateSpaceCoeff()->a44 <<"\n";
// std::cout << "i1alpha: " << rungeKutta(t0, out0, t, h, motorParametersData, &i1alpha) << "\n";

std::cout << i1alpha(0,MotorModel.getStateSpaceCoeff(), MotorModel.getMotorVariables()) << "\n" ;

std::cout << "setting i1alpha\n";
MotorModel.setModelVariable(MotorModel.getMotorVariables()->i1alpha, 85.56);
std::cout << "i1alpha is: " << MotorModel.getMotorVariables()->i1alpha << "\n";

std::cout << "i1beta is: " << MotorModel.getMotorVariables()->i1beta << "\n";

/*---------------------------------------------------------*/


return 0;
}
