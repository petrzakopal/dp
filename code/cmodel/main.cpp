#include <iostream>
#include <cmath>
#include <vector>
#include "header/MotorModel.h"
#include "header/transformation.h"
#include "header/CurVelModel.h"



#include <fstream>



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
/*||||||||||||||||||||||||||||||||||||||||||||||||| ASM MOTOR GENERATION |||||||||||||||||||||||||||||||||||||||||||||||||*/
/*||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||*/

/*----------------------------------------------------------------------------------*/
/*-------------------- INITIALIZATION VIA MOTORMODEL CLASS API ---------------------*/
MotorModelClass MotorModel;
float globalCalculationStep = 0.0001;
MotorModel.odeCalculationSettingsAllocateMemory();
MotorModel.setOdeCalculationSettings(0, 1, globalCalculationStep); // initial time, final time, calculation step; if you want to calculate just one sample at a time (in for cycle of RK4), use (0, 1, 1)
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

MotorModel.odeCalculationSettings->numberOfIterations = MotorModel.numberOfIterations();

std::cout << "number of iterations: " << MotorModel.odeCalculationSettings->numberOfIterations << "\n";





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

    // for(int i = 0; i<= MotorModel.odeCalculationSettings->numberOfIterations;i++)
    // {
    //     std::cout << "motor voltage u1 at " << i << " : " << MotorModel.getVoltage(i)->u1 << "\n";
    // }
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




// free(MotorModel.motorParameters);
// free(MotorModel.stateSpaceCoeff);
// free(MotorModel.modelVariables);
// free(MotorModel.odeCalculationSettings);
// free(MotorModel.voltageGeneratorData);

/*||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||*/
/*||||||||||||||||||||||||||||||||||||||||||||| END OF ASM MOTOR GENERATION |||||||||||||||||||||||||||||||||||||||||||||*/


CurVelModelClass CurVelModel;
CurVelModel.motorParametersAllocateMemory();
CurVelModel.motorCoeffAllocateMemory();
CurVelModel.modelCVVariablesAllocateMemory();
CurVelModel.odeCVCalculationSettingsAllocateMemory();

CurVelModel.odeCVCalculationSettings->calculationStep = globalCalculationStep;


CurVelModel.motorParameters->R2 = 0.225f; // Ohm, rotor rezistance
CurVelModel.motorParameters->Lm = 0.0825f; // H, main flux inductance
CurVelModel.motorParameters->L2 = 0.08477f; // H, inductance
CurVelModel.motorParameters->nOfPolePairs = 2; // number of pole pairs

CurVelModel.calculateMotorCVCoeff(CurVelModel.modelCVCoeff, CurVelModel.motorParameters);


std::cout << "CurVelModel.modelCVCoeff->R2DL2: " << CurVelModel.modelCVCoeff->R2DL2 << "\n";

std::cout << "MotorModel.getMotorVariable(0)->i1alpha: " << MotorModel.getMotorVariable(2)->i1alpha << "\n";

    std::ofstream modelCVOutputDataFile;
    modelCVOutputDataFile.open ("outputCurVel.csv",std::ofstream::out | std::ofstream::trunc);
    modelCVOutputDataFile<< "time,|psi2|,i1alpha,motorMechanicalAngularVelocity\n";


    float psi2Amplitude = 0;
    float timeCV = MotorModel.odeCalculationSettings->initialCalculationTime;

for(int i = 1; i<= MotorModel.odeCalculationSettings->numberOfIterations; i++)
{

    timeCV = timeCV + CurVelModel.odeCVCalculationSettings->calculationStep;

    CurVelModel.modelCVVariables->i1alpha = MotorModel.getMotorVariable(i)->i1alpha;
    CurVelModel.modelCVVariables->i1beta = MotorModel.getMotorVariable(i)->i1beta;
    CurVelModel.modelCVVariables->motorElectricalAngularVelocity = MotorModel.getMotorVariable(i)->motorMechanicalAngularVelocity * MotorModel.motorParameters->nOfPolePairs;

    CurVelModel.CurVelModelCalculate(CurVelModel.modelCVCoeff, CurVelModel.modelCVVariables, CurVelModel.odeCVCalculationSettings);

    // std::cout << "psi2alpha: " << CurVelModel.modelCVVariables->psi2alpha << "\n";
    // std::cout << "psi2beta: " << CurVelModel.modelCVVariables->psi2beta << "\n";


    psi2Amplitude = sqrt((CurVelModel.modelCVVariables->psi2alpha * CurVelModel.modelCVVariables->psi2alpha) + (CurVelModel.modelCVVariables->psi2beta * CurVelModel.modelCVVariables->psi2beta));

    // std::cout << "|psi2| = " << psi2Amplitude << "\n";
    // std::cout << i <<"\n";
    modelCVOutputDataFile << timeCV << "," << psi2Amplitude <<","<<MotorModel.getMotorVariable(i)->i1alpha << "," << MotorModel.getMotorVariable(i)->motorMechanicalAngularVelocity <<"\n";
}




// std::cout << "psi2alpha: " << CurVelModel.modelCVVariables->psi2alpha << "\n";
// std::cout << "psi2beta: " << CurVelModel.modelCVVariables->psi2beta << "\n";

free(CurVelModel.motorParameters);
free(CurVelModel.modelCVCoeff);
free(CurVelModel.modelCVVariables);
free(CurVelModel.odeCVCalculationSettings);



free(MotorModel.motorParameters);
free(MotorModel.stateSpaceCoeff);
free(MotorModel.modelVariables);
free(MotorModel.odeCalculationSettings);
free(MotorModel.voltageGeneratorData);
/*---------------------------------------------------------*/

return 0;
}
