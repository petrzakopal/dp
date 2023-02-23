#include <iostream>
#include <cmath>
#include <vector>
#include "header/mathSolver.h"
#include "header/MotorModel.h"
#include "header/transformation.h"



/* POZOR V ZYBO NA VKLÁDÁNÍ HEADER FILES V KERNELU */
/*

je třeba dělat include #include "function/MotorModel.cpp"
a uvnitř cpp mít #include "../header/MotorModel.h"
jinak to nejde

také je problém v případě, že v MotorModel.cpp includuji transformation a ono to bere, že se to redefinovává i když to tak není, to je probléma  hodí to u v++ error

*/

#define PI 3.141592 


int main()
{



/*----------------------------------------------------------------------------------*/
/*-------------------- INITIALIZATION VIA MOTORMODEL CLASS API ---------------------*/
MotorModelClass MotorModel;
MotorModel.odeCalculationSettingsAllocateMemory();
MotorModel.setOdeCalculationSettings(0, 1, 0.00001); // initial time, final time, calculation step; if you want to calculate just one sample at a time (in for cycle of RK4), use (0, 1, 1)
MotorModel.motorParametersAllocateMemory();
MotorModel.stateSpaceCoeffAllocateMemory();
MotorModel.modelVariablesAllocateMemory(); // on index [0] there are initialConditions, RK4 starts from 1 to <=n when n is (final-initial)/step
MotorModel.setMotorParameters();
MotorModel.setStateSpaceCoeff();
MotorModel.voltageGeneratorDataAllocateMemory();





/*----------------------------------------------------------------------------------*/


/*------------------------------------------------------------------------*/






std::cout << "| ----------------------- |\n";

/*---------------------------------------------------------*/
/*-------------------- TEST OUTPUTS ----------------------*/

int numberOfIterations = MotorModel.numberOfIterations();


std::cout << "number of modelVariables: " << ((int)ceil((MotorModel.odeCalculationSettings->finalCalculationTime - MotorModel.odeCalculationSettings->initialCalculationTime)/MotorModel.odeCalculationSettings->calculationStep)) << "\n";
std::cout << "number of iterations: " << numberOfIterations << "\n";



std::cout << "test state space coeff= " << MotorModel.getStateSpaceCoeff()->a14 << "\n";

MotorModel.voltageGeneratorData->voltageFrequency = 50;
MotorModel.voltageGeneratorData->voltageAmplitude = 325.26;

std::cout << "voltage frequency: " <<MotorModel.voltageGeneratorData->voltageFrequency<< "\n";

/*--------------------------------------------------------------------------------------------------------------------------------*/

// precalculating voltages in a kernel
/*--------------------------------------------------------------------------------------------------------------------------------*/
MotorModel.precalculateVoltageSource(MotorModel.voltageGeneratorData, MotorModel.odeCalculationSettings, MotorModel.voltageGeneratorData->voltageAmplitude, MotorModel.voltageGeneratorData->voltageFrequency);
MotorModel.precalculateVoltageClarke(MotorModel.voltageGeneratorData, MotorModel.odeCalculationSettings);

std::cout << "motor voltage u1 at 0: " << MotorModel.getVoltage(0)->u1 << "\n";
std::cout << "motor clarke voltage u1alpha at 20: " << MotorModel.getVoltage(0)->u1alpha << "\n";
std::cout << "motor clarke voltage u1beta at 20: " << MotorModel.getVoltage(0)->u1beta << "\n";


/*--------------------------------------------------------------------------------------------------------------------------------*/

MotorModel.mathModelCalculate(MotorModel.odeCalculationSettings, MotorModel.modelVariables, MotorModel.stateSpaceCoeff, MotorModel.motorParameters);

// MotorModel.setVariable(MotorModel.getVoltage(0)->u1, 2500);
// std::cout << "motor voltage u1 at 0: " << MotorModel.getVoltage(0)->u1 << "\n";

// std::cout << "motor variable from calc: "<<MotorModel.getMotorVariable(0)->i1alpha<<"\n";
// std::cout << "torque is= " << MotorModel.motorTorque(MotorModel.motorParameters,MotorModel.getMotorVariable(800)) << "\n";
// std::cout << "motor variable mechanicalAngularVelocity: "<<MotorModel.getMotorVariable(800)->motorMechanicalAngularVelocity<<"\n";


// use for defining parameters not in class function (it is just for fun to have it in class, maybe change later, need to ask by boss)
MotorModel.motorParameters->R1 = 25;
std::cout << "motor parameters changed R1 = " << MotorModel.motorParameters->R1 << "\n";

free(MotorModel.motorParameters);
free(MotorModel.stateSpaceCoeff);
free(MotorModel.modelVariables);
free(MotorModel.odeCalculationSettings);
free(MotorModel.voltageGeneratorData);


/*---------------------------------------------------------*/

return 0;
}
