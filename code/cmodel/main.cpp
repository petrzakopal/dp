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
MotorModel.setOdeCalculationSettings(0, 1, 0.001); // initial time, final time, calculation step; if you want to calculate just one sample at a time (in for cycle of RK4), use (0, 1, 1)
MotorModel.motorParametersAllocateMemory();
MotorModel.stateSpaceCoeffAllocateMemory();
MotorModel.modelVariablesAllocateMemory(); // on index [0] there are initialConditions, RK4 starts from 1 to <=n when n is (final-initial)/step





/*-------------------- USE FUNCTION WITH HARDCODED VALUE OF MOTOR OR SET IT MANUALLY ---------------------*/

// MotorModel.setMotorParameters(); // hardcoded values
MotorModel.motorParameters->R1 = 0.370f; // Ohm, stator rezistance
MotorModel.motorParameters->R2 = 0.225f; // Ohm, rotor rezistance
MotorModel.motorParameters->L1s = 0.00227f; // H, stator leakage inductance
MotorModel.motorParameters->L2s = 0.00227f; // H, rotor leakage inductance
MotorModel.motorParameters->Lm = 0.0825f; // H, main flux inductance
MotorModel.motorParameters->L1 = 0.08477f; // H, inductance
MotorModel.motorParameters->L2 = 0.08477f; // H, inductance
MotorModel.motorParameters->sigma = 0.05283f; // = 0.0528396032, sigma = 1 - Lm^(2)/L1L2
MotorModel.motorParameters->nOfPolePairs = 2; // number of pole pairs
MotorModel.motorParameters->momentOfIntertia = 0.4; // J, moment of inertia
/*--------------------------------------------------------------------------------------------------------*/

MotorModel.setStateSpaceCoeff();
MotorModel.voltageGeneratorDataAllocateMemory();





/*----------------------------------------------------------------------------------*/


/*------------------------------------------------------------------------*/






std::cout << "| ----------------------- |\n";

/*---------------------------------------------------------*/
/*-------------------- TEST OUTPUTS ----------------------*/

int numberOfIterations = MotorModel.numberOfIterations();
MotorModel.odeCalculationSettings->numberOfIterations = numberOfIterations;

std::cout << "number of modelVariables: " << ((int)ceil((MotorModel.odeCalculationSettings->finalCalculationTime - MotorModel.odeCalculationSettings->initialCalculationTime)/MotorModel.odeCalculationSettings->calculationStep)) << "\n";
std::cout << "number of iterations: " << numberOfIterations << "\n";





std::cout << "test state space coeff= " << MotorModel.getStateSpaceCoeff()->a13 << "\n";

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

    for(int i = 0; i<= MotorModel.odeCalculationSettings->numberOfIterations;i++)
    {
        std::cout << "motor voltage u1 at " << i << " : " << MotorModel.getVoltage(i)->u1 << "\n";
    }
/*--------------------------------------------------------------------------------------------------------------------------------*/

MotorModel.mathModelCalculate(MotorModel.odeCalculationSettings, MotorModel.modelVariables, MotorModel.stateSpaceCoeff, MotorModel.motorParameters);

// MotorModel.setVariable(MotorModel.getVoltage(0)->u1, 2500);
// std::cout << "motor voltage u1 at 0: " << MotorModel.getVoltage(0)->u1 << "\n";

// std::cout << "motor variable from calc: "<<MotorModel.getMotorVariable(0)->i1alpha<<"\n";
// std::cout << "torque is= " << MotorModel.motorTorque(MotorModel.motorParameters,MotorModel.getMotorVariable(800)) << "\n";
// std::cout << "motor variable mechanicalAngularVelocity: "<<MotorModel.getMotorVariable(800)->motorMechanicalAngularVelocity<<"\n";


// use for defining parameters not in class function (it is just for fun to have it in class, maybe change later, need to ask by boss)
// MotorModel.motorParameters->R1 = 25;
// std::cout << "motor parameters changed R1 = " << MotorModel.motorParameters->R1 << "\n";

std::cout << MotorModel.voltageGeneratorData[210].u1 << "\n";



free(MotorModel.motorParameters);
free(MotorModel.stateSpaceCoeff);
free(MotorModel.modelVariables);
free(MotorModel.odeCalculationSettings);
free(MotorModel.voltageGeneratorData);


/*---------------------------------------------------------*/

return 0;
}
