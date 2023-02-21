#include <iostream>
#include <cmath>
#include <vector>
#include "header/mathSolver.h"
#include "header/MotorModel.h"
#include "header/transformation.h"

#define PI 3.141592 

int main()
{


/*----------------------------------------------------------------------------------*/
/*-------------------- INITIALIZATION VIA MOTORMODEL CLASS API ---------------------*/
MotorModelClass MotorModel;
MotorModel.odeCalculationSettingsAllocateMemory();
MotorModel.setOdeCalculationSettings(0, 1, 0.001); // initial time, final time, calculation step; if you want to calculate just one sample at a time (in for cycle of RK4), use (0, 1, 1)
MotorModel.motorParametersAllocateMemory();
MotorModel.stateSpaceCoeffAllocateMemory();
MotorModel.modelVariablesAllocateMemory(); // on index [0] there are initialConditions, RK4 starts from 1 to <=n when n is (final-initial)/step
MotorModel.setMotorParameters();
MotorModel.setStateSpaceCoeff();
MotorModel.voltageGeneratorDataAllocateMemory();





/*----------------------------------------------------------------------------------*/


/*------------------------------------------------------------------------*/






std::cout << "| ----------------------- |\n";
std::cout << "sizeof= " << sizeof(motorParametersType) << "\n";
std::cout << "sizeof(int)= " << sizeof(int) << "\n";

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

// std::cout << i1alpha(0,MotorModel.getStateSpaceCoeff(), MotorModel.getMotorVariable()) << "\n" ;

int actualSample = 0; // desired time for calculation

// initial conditions will be saved to 

std::cout << "Motor initial condition [0] for i1alpha: " <<MotorModel.getMotorVariable(0)->i1alpha << "\n";
std::cout << "setting i1alpha\n";
MotorModel.setVariable(MotorModel.getMotorVariable(actualSample)->i1alpha, 12.5);
MotorModel.setVariable(MotorModel.getMotorVariable(1)->i1alpha, 32.5);
std::cout << "i1alpha is: " << MotorModel.getMotorVariable(actualSample)->i1alpha << "\n";
std::cout << "i1alpha at 3. position is: " << MotorModel.getMotorVariable(actualSample)->i1alpha << "\n";

// needs to be calculated every sample/change of time in a for loop iteration of RK4
// 12 is angular speed
// MotorModel.calculateStateSpaceCoeff(12);


std::cout << "i1alpha after calculation= "<< MotorModel.i1alpha(MotorModel.getStateSpaceCoeff(), MotorModel.getMotorVariable(actualSample)->i1alpha, MotorModel.getMotorVariable(actualSample)->i1beta, MotorModel.getMotorVariable(actualSample)->psi2alpha, MotorModel.getMotorVariable(actualSample)->psi2beta, MotorModel.u1alpha(0)) << "\n" ;


std::cout << "i1beta is: " << MotorModel.getMotorVariable(actualSample)->i1beta << "\n";


std::cout << "number of modelVariables: " << ((int)ceil((MotorModel.odeCalculationSettings->finalCalculationTime - MotorModel.odeCalculationSettings->initialCalculationTime)/MotorModel.odeCalculationSettings->calculationStep)) << "\n";
std::cout << "number of iterations: " << ((int)ceil((MotorModel.odeCalculationSettings->finalCalculationTime - MotorModel.odeCalculationSettings->initialCalculationTime)/MotorModel.odeCalculationSettings->calculationStep)) << "\n";

std::cout << "torque is= " << MotorModel.motorTorque(MotorModel.motorParameters,MotorModel.getMotorVariable(actualSample)) << "\n";
// MotorModel.setVariable(MotorModel.getMotorVariable(1)->i1alpha, 44);
// std::cout << "index 1 of motor model variables: " << MotorModel.getMotorVariable(1)->i1alpha << "\n";

MotorModel.mathModelCalculate(MotorModel.odeCalculationSettings, MotorModel.modelVariables, MotorModel.stateSpaceCoeff, MotorModel.motorParameters);

std::cout << "test state space coeff= " << MotorModel.getStateSpaceCoeff()->a14 << "\n";


// manual test calculation
/*------------------------------------------------------------------------------------------------*/
// TransformationClass Transformation;
// // not usable now
// // Transformation.clarkeTransformDataAllocateMemory();
// // Transformation.clarkeTransformData->transformConstant = 2/3;
// // Transformation.clarkeTransformData->input1 = 325.26;
// // Transformation.clarkeTransformData->input2 = 281.69;
// // Transformation.clarkeTransformData->input3 = -281.69;

// std::cout << "voltage 1: " << MotorModel.voltageGenerator(0.005,0,325.26,50)<< "\n";
// std::cout << "voltage 2: " << MotorModel.voltageGenerator(0.005,2 * PI / 3,325.26,50)<< "\n";
// std::cout << "voltage 3: " << MotorModel.voltageGenerator(0.005,- 2 * PI / 3,325.26,50)<< "\n";

// MotorModel.setVariable(MotorModel.getVoltage(0)->u1, 25);
// std::cout << "motor voltage 1 at 0: " << MotorModel.getVoltage(0)->u1 << "\n";



// std::cout << "transformed 1: " << Transformation.clarkeTransform1(325.26, 281.69, -281.69, 0.6666)<< "\n";
// std::cout << "transformed 2: " << Transformation.clarkeTransform2(325.26, 281.69, -281.69, 0.6666)<< "\n";

/*--------------------------------------------------------------------------------------------------------------------------------*/

// precalculating voltages in a kernel
/*--------------------------------------------------------------------------------------------------------------------------------*/
MotorModel.precalculateVoltageSource(MotorModel.voltageGeneratorData, MotorModel.odeCalculationSettings,325, 50);
MotorModel.precalculateVoltageClarke(MotorModel.voltageGeneratorData, MotorModel.odeCalculationSettings);

std::cout << "motor voltage u1 at 20: " << MotorModel.getVoltage(20)->u1 << "\n";
std::cout << "motor clarke voltage u1alpha at 20: " << MotorModel.getVoltage(20)->u1alpha << "\n";
std::cout << "motor clarke voltage u1beta at 20: " << MotorModel.getVoltage(20)->u1beta << "\n";
// for(int i = 0; i<=1000; i++)
// {

// std::cout << "motor voltage 1 at "<< i << " : " << MotorModel.getVoltage(i)->u1 << "\n";

// }

// for(int i = 0; i<=1000; i++)
// {


// std::cout << "motor voltage 2 at "<< i << " : " << MotorModel.getVoltage(i)->u2 << "\n";

// }

// for(int i = 0; i<=1000; i++)
// {


// std::cout << "motor voltage 3 at "<< i << " : " << MotorModel.getVoltage(i)->u3 << "\n";
// }

/*--------------------------------------------------------------------------------------------------------------------------------*/
// Transformation.clarkeTransform(Transformation.clarkeTransformData);
// std::cout << "clarke transform constant= " << Transformation.clarkeTransformData->transformConstant << "\n";
// std::cout << "clarke transform input 1= " << Transformation.clarkeTransformData->input1 << "\n";
// std::cout << "clarke transform input 2= " << Transformation.clarkeTransformData->input2 << "\n";
// std::cout << "clarke transform input 3= " << Transformation.clarkeTransformData->input3 << "\n";
// std::cout << "clarke transform output 1= " << Transformation.clarkeTransformData->output1 << "\n";
// std::cout << "clarke transform output 2= " << Transformation.clarkeTransformData->output2 << "\n";

/*--------------------------------------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------*/

return 0;
}
