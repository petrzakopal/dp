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
MotorModel.odeCalculationSettingsAllocateMemory();
MotorModel.setOdeCalculationSettings(0.0, 10.0, 0.0001); // initial time, final time, calculation step
MotorModel.motorParametersAllocateMemory();
MotorModel.stateSpaceCoeffAllocateMemory();
MotorModel.modelVariablesAllocateMemory();
MotorModel.setMotorParameters();
MotorModel.setStateSpaceCoeff();





/*----------------------------------------------------------------------------------*/


/*------------------------------------------------------------------------*/






std::cout << "| ----------------------- |\n";


// std::cout << "no of samples: "<< MotorModel.numberOfSamples<< "\n";
// // Number of samples - this is ending time when you want to generate output based on time and not actual value
// MotorModel.numberOfSamples = 250;
// std::cout << "edited no of samples: "<< MotorModel.numberOfSamples<< "\n";
/*---------------------------------------------------------*/
/*-------------------- TEST OUTPUTS ----------------------*/

std::cout << "R1 from posix_memalign: " << MotorModel.getMotorParameters()->R1 << "\n"; 

std::cout << "motor initialized parameters R1= "<< MotorModel.getMotorParameters()->R1 << "\n";
std::cout << "\n" << "| ----------------------- |\n";
std::cout << "a11= " << MotorModel.getStateSpaceCoeff()->a11 <<"\n";
std::cout << "a14= " << MotorModel.getStateSpaceCoeff()->a14 <<"\n";
std::cout << "a44= " << MotorModel.getStateSpaceCoeff()->a44 <<"\n";
// std::cout << "i1alpha: " << rungeKutta(t0, out0, t, h, motorParametersData, &i1alpha) << "\n";

// std::cout << i1alpha(0,MotorModel.getStateSpaceCoeff(), MotorModel.getMotorVariables()) << "\n" ;

int actualSample = 0; // desired time for calculation

// initial conditions will be saved to 

std::cout << "Motor initial condition [0] for i1alpha: " <<MotorModel.getMotorVariables(0)->i1alpha << "\n";
std::cout << "setting i1alpha\n";
MotorModel.setModelVariable(MotorModel.getMotorVariables(actualSample)->i1alpha, 12.5);
std::cout << "i1alpha is: " << MotorModel.getMotorVariables(actualSample)->i1alpha << "\n";
std::cout << "i1alpha at 3. position is: " << MotorModel.getMotorVariables(actualSample)->i1alpha << "\n";

// needs to be calculated every sample
// 12 is angular speed
MotorModel.calculateStateSpaceCoeff(12);
std::cout << "i1alpha after calculation= "<< i1alpha(MotorModel.getStateSpaceCoeff(), MotorModel.getMotorVariables(actualSample)) << "\n" ;
std::cout << "i1beta is: " << MotorModel.getMotorVariables(actualSample)->i1beta << "\n";


std::cout << "number of data: " << (int)(MotorModel.odeCalculationSettings->finalCalculationTime - MotorModel.odeCalculationSettings->initialCalculationTime)/MotorModel.odeCalculationSettings->calculationStep << "\n";



// MotorModel.setModelVariable(MotorModel.getMotorVariables(1)->i1alpha, 44);
// std::cout << "index 1 of motor model variables: " << MotorModel.getMotorVariables(1)->i1alpha << "\n";
/*---------------------------------------------------------*/


return 0;
}
