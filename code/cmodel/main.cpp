#include <iostream>
#include <cmath>
#include <vector>
#include "header/MotorModel.h"
#include "header/transformation.h"
#include "header/CurVelModel.h"
#include "header/svmCore.h"

#include <string>

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
float globalInitialCalculationTime = 0;
float globalFinalCalculationTime = 1;
MotorModel.odeCalculationSettingsAllocateMemory();
MotorModel.setOdeCalculationSettings(globalInitialCalculationTime, globalFinalCalculationTime, globalCalculationStep); // initial time, final time, calculation step; if you want to calculate just one sample at a time (in for cycle of RK4), use (0, 1, 1)
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



/*------------------------------------------------------------------------*/
/*-------------------- FREE THE POINTERS TO MEMORY ----------------------*/
// free(MotorModel.motorParameters);
// free(MotorModel.stateSpaceCoeff);
// free(MotorModel.modelVariables);
// free(MotorModel.odeCalculationSettings);
// free(MotorModel.voltageGeneratorData);
/*------------------------------------------------------------------------*/

/*||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||*/
/*||||||||||||||||||||||||||||||||||||||||||||| END OF ASM MOTOR GENERATION |||||||||||||||||||||||||||||||||||||||||||||*/



/*||||||||||||||||||||||||||||||||||||||||||||||||| CURRENT - VELOCITY MOTOR MODEL DIRECTLY FROM ASM MOTOR |||||||||||||||||||||||||||||||||||||||||||||||||*/
/*||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||*/

/*----------------------------------------------------------------------------------------*/
/*-------------------- DEFINING CLASS OBJECT AND ALLOCATING MEMORY ----------------------*/
CurVelModelClass CurVelModel;
CurVelModel.motorParametersAllocateMemory();
CurVelModel.motorCoeffAllocateMemory();
CurVelModel.modelCVVariablesAllocateMemory();
CurVelModel.odeCVCalculationSettingsAllocateMemory();
/*----------------------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------------------*/
/*------------------------------ DEFINING INPUT PARAMETERS ------------------------------*/
CurVelModel.odeCVCalculationSettings->calculationStep = globalCalculationStep; // just a helper variable defined on top of this file to hava the same number of samples for ASM motor model and Current-Velocity model
CurVelModel.motorParameters->R2 = 0.225f; // Ohm, rotor rezistance
CurVelModel.motorParameters->Lm = 0.0825f; // H, main flux inductance
CurVelModel.motorParameters->L2 = 0.08477f; // H, inductance
CurVelModel.motorParameters->nOfPolePairs = 2; // number of pole pairs
/*----------------------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------------------*/
/*------------------------------ PRECALCULATING MODEL COEFF ------------------------------*/
CurVelModel.calculateMotorCVCoeff(CurVelModel.modelCVCoeff, CurVelModel.motorParameters);
/*----------------------------------------------------------------------------------------*/


// std::cout << "CurVelModel.modelCVCoeff->R2DL2: " << CurVelModel.modelCVCoeff->R2DL2 << "\n";

// std::cout << "MotorModel.getMotorVariable(0)->i1alpha: " << MotorModel.getMotorVariable(2)->i1alpha << "\n";



// this output is shifted from ASM by one sample, for outputCurVel 0. sample I use initialTime+step, because you solve diff equations from initial conditions forward

/*----------------------------------------------------------------------------------------*/
/*----------------------------- FINE EXPORT FILE PREPARATION -----------------------------*/
std::ofstream modelCVOutputDataFile;
modelCVOutputDataFile.open ("outputCurVel.csv",std::ofstream::out | std::ofstream::trunc);
modelCVOutputDataFile<< "time,|psi2|,i1alpha,motorMechanicalAngularVelocity,psi2alpha,psi2beta\n";
/*----------------------------------------------------------------------------------------*/



float psi2Amplitude = 0; // helper variables to have cleader code, mybe change later, but is used to eliminate multiple calculations of amplitude os psi2
float timeCV = MotorModel.odeCalculationSettings->initialCalculationTime;


/*************************************************************************************************/
/*------------------------------ MAIN MODEL LOOP FOR KNOWN INPUTS ------------------------------*/

// if there is no known input for next step, the program needs to wait for the interrupt and data acquisition, but in this cmodel on PC we work with „predefined data“ to make some progress
for(int i = 0; i< MotorModel.odeCalculationSettings->numberOfIterations; i++)
{

    timeCV = timeCV + CurVelModel.odeCVCalculationSettings->calculationStep; // time calculation for data output and graphs

    /*--------------------------------------------------------------------------------------------------------------------------------------------*/
    /*------------------------------ GETTING TRANSFORMED CURRENTS ALPHA, BETA AND VELOCITY DIRECTLY FROM ASM MODEL ------------------------------*/
    CurVelModel.modelCVVariables->i1alpha = MotorModel.getMotorVariable(i)->i1alpha;
    CurVelModel.modelCVVariables->i1beta = MotorModel.getMotorVariable(i)->i1beta;
    CurVelModel.modelCVVariables->motorElectricalAngularVelocity = MotorModel.getMotorVariable(i)->motorMechanicalAngularVelocity * MotorModel.motorParameters->nOfPolePairs;
    /*-------------------------------------------------------------------------------------------------------------------------------------------*/

    /*---------------------------------------------------------------------------------------*/
    /*------------------------------ RK4 ODE SOLVING FUNCTION ------------------------------*/
    CurVelModel.CurVelModelCalculate(CurVelModel.modelCVCoeff, CurVelModel.modelCVVariables, CurVelModel.odeCVCalculationSettings);
    /*--------------------------------------------------------------------------------------*/


    /*------------------------------------------------------------------------------------------------------------------*/
    /*------------------------------ MORE DATA TUNING AND OUTPUT TO A FINE EXPORTED FILE ------------------------------*/
    psi2Amplitude = sqrt((CurVelModel.modelCVVariables->psi2alpha * CurVelModel.modelCVVariables->psi2alpha) + (CurVelModel.modelCVVariables->psi2beta * CurVelModel.modelCVVariables->psi2beta));

    modelCVOutputDataFile << timeCV << "," << psi2Amplitude <<","<<MotorModel.getMotorVariable(i)->i1alpha << "," << MotorModel.getMotorVariable(i)->motorMechanicalAngularVelocity <<","<< CurVelModel.modelCVVariables->psi2alpha <<","<<CurVelModel.modelCVVariables->psi2beta<< "\n";
    /*----------------------------------------------------------------------------------------------------------------*/

}
/**************************************************************************************************/

modelCVOutputDataFile.close(); // closing fine export file

free(CurVelModel.motorParameters);
free(CurVelModel.modelCVCoeff);
free(CurVelModel.modelCVVariables);
free(CurVelModel.odeCVCalculationSettings);
/*|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||*/
/*||||||||||||||||||||||||||||||||||||||||||||||||| END OF CURRENT - VELOCITY MOTOR MODEL DIRECTLY FROM ASM MOTOR |||||||||||||||||||||||||||||||||||||||||||||||||*/


/*||||||||||||||||||||||||||||||||||||||||||||||||| CURRENT - VELOCITY MOTOR MODEL FROM ACQUIRED DATA „MODELATION“ |||||||||||||||||||||||||||||||||||||||||||||||||*/
/*||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||*/


/*------------------------------------------------------------------------------------------------------------*/
/*------------------------------ MODELING OUTPUT DATA / INPUT DATA FROM SENSORS ------------------------------*/
std::ofstream outputData;
outputData.open ("outputData.csv",std::ofstream::out | std::ofstream::trunc);



TransformationClass Transformation;
float i1a, i1b, i1c, motorMechanicalAngularVelocity;
timeCV = MotorModel.odeCalculationSettings->initialCalculationTime;
for(int i = 0; i< MotorModel.odeCalculationSettings->numberOfIterations; i++)
{
    timeCV = timeCV + globalCalculationStep;
    
    // std::cout << Transformation.inverseClarkeTransform1(MotorModel.getMotorVariable(i)->i1alpha,MotorModel.getMotorVariable(i)->i1beta, 0.6667) << "\n";
    // std::cout << Transformation.inverseClarkeTransform2(MotorModel.getMotorVariable(i)->i1alpha,MotorModel.getMotorVariable(i)->i1beta, 0.6667) << "\n";
    // std::cout << Transformation.inverseClarkeTransform3(MotorModel.getMotorVariable(i)->i1alpha,MotorModel.getMotorVariable(i)->i1beta, 0.6667) << "\n";
    i1a = Transformation.inverseClarkeTransform1(MotorModel.getMotorVariable(i)->i1alpha,MotorModel.getMotorVariable(i)->i1beta);
    i1b = Transformation.inverseClarkeTransform2(MotorModel.getMotorVariable(i)->i1alpha,MotorModel.getMotorVariable(i)->i1beta);
    i1c = Transformation.inverseClarkeTransform3(MotorModel.getMotorVariable(i)->i1alpha,MotorModel.getMotorVariable(i)->i1beta);
    motorMechanicalAngularVelocity = MotorModel.getMotorVariable(i)->motorMechanicalAngularVelocity;
    outputData<< timeCV << "," << i1a << "," << i1b << "," << i1c << "," << motorMechanicalAngularVelocity<< ","; // lightweight format - one line, data divided by „,“, will be imported back to cmodel, could be done better, but this is lightweight and not meant to be read by a human
    // the structure of data is important – the structure is supposed when importing data to new variables

    
}
outputData.close(); // closing lightweight data sensor export
/*--------------------------------------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------------------------------------------------------------*/
/*------------------------------ DEFINING CLASS OBJECT AND ALLOCATING MEMORY FOR SECOND I-N MODEL ------------------------------*/
CurVelModelClass CurVelModel2;
CurVelModel2.motorParametersAllocateMemory();
CurVelModel2.motorCoeffAllocateMemory();
CurVelModel2.modelCVVariablesAllocateMemory();
CurVelModel2.odeCVCalculationSettingsAllocateMemory();
/*-------------------------------------------------------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------------------------------------*/
/*------------------------------ LOADING DATA FROM A LIGHTWEIGHT EXPORT FORMAT ------------------------------*/

// float *inputTime;
// float *inputI1;
// float *inputI2;
// float *inputI3;
// float *inputMotorMechanicalAngularVelocity;
posix_memalign((void **)&CurVelModel2.modelCVVariables->inputTime , 4096 , MotorModel.odeCalculationSettings->numberOfIterations*sizeof(float) );
posix_memalign((void **)&CurVelModel2.modelCVVariables->inputI1 , 4096 , MotorModel.odeCalculationSettings->numberOfIterations*sizeof(float) );
posix_memalign((void **)&CurVelModel2.modelCVVariables->inputI2 , 4096 , MotorModel.odeCalculationSettings->numberOfIterations*sizeof(float) );
posix_memalign((void **)&CurVelModel2.modelCVVariables->inputI3 , 4096 , MotorModel.odeCalculationSettings->numberOfIterations*sizeof(float) );
posix_memalign((void **)&CurVelModel2.modelCVVariables->inputMotorMechanicalAngularVelocity , 4096 , MotorModel.odeCalculationSettings->numberOfIterations*sizeof(float) );

std::ifstream inputData( "outputData.csv" );

int switchReadingIndex = 0;
int timeIndex = 0;
int i1Aindex = 0;
int i1Bindex = 0;
int i1Cindex = 0;
int motorMechanicalAngularVelocityIndex = 0;

for( std::string line; std::getline( inputData, line, ','); )
{


        // data structure is important, defined in a lightweight export file like {time,i1,i2,i3,motorMechanicalAngularVelocity}
         switch(switchReadingIndex)
            {
                case 0:
               
                CurVelModel2.modelCVVariables->inputTime[timeIndex] = std::stof (line, NULL);
                timeIndex++;
                switchReadingIndex++;
                break;

                case 1:

                CurVelModel2.modelCVVariables->inputI1[i1Aindex] = std::stof (line, NULL);
                i1Aindex++;
                switchReadingIndex++;
                break;


                case 2:

                CurVelModel2.modelCVVariables->inputI2[i1Bindex] = std::stof (line, NULL);
                i1Bindex++;
                switchReadingIndex++;
                break;

                case 3:

                CurVelModel2.modelCVVariables->inputI3[i1Cindex] = std::stof (line, NULL);
                i1Cindex++;
                switchReadingIndex++;
                break;

                case 4:

                CurVelModel2.modelCVVariables->inputMotorMechanicalAngularVelocity[motorMechanicalAngularVelocityIndex] = std::stof (line, NULL);
                motorMechanicalAngularVelocityIndex++;
                switchReadingIndex = 0;
                break;

        }
    
  
}
inputData.close(); // close that file
/*------------------------------------------------------------------------------------------------------------*/





/*---------------------------------------------------------------------------------------------------------*/
/*------------------------------ DEFINING CLASS PARAMETERS AND COEFFICIENTS ------------------------------*/
CurVelModel2.odeCVCalculationSettings->calculationStep = globalCalculationStep;
CurVelModel2.motorParameters->R2 = 0.225f; // Ohm, rotor rezistance
CurVelModel2.motorParameters->Lm = 0.0825f; // H, main flux inductance
CurVelModel2.motorParameters->L2 = 0.08477f; // H, inductance
CurVelModel2.motorParameters->nOfPolePairs = 2; // number of pole pairs
CurVelModel2.calculateMotorCVCoeff(CurVelModel2.modelCVCoeff, CurVelModel2.motorParameters);
/*---------------------------------------------------------------------------------------------------------*/



/*-----------------------------------------------------------------------------------------------------------*/
/*------------------------------ PREPARING DATA EXPORT FOR GRAPHS AND TESTING ------------------------------*/
std::ofstream modelCVOutputDataFile2;

modelCVOutputDataFile2.open("outputCurVel2.csv",std::ofstream::out | std::ofstream::trunc);
modelCVOutputDataFile2<< "time,|psi2|,i1alpha,motorMechanicalAngularVelocity\n";
/*-----------------------------------------------------------------------------------------------------------*/


std::cout<< &CurVelModel2.modelCVVariables->psi2alpha << "\n";
int modeSelection = 0;
printf("Select mode:\n0 - preloaded data\n1 - keyboard input data\n");
scanf("%i", &modeSelection);

printf("You have selected: %i\n\r", modeSelection);


psi2Amplitude = 0;
float transformAngle = 0;
timeCV = MotorModel.odeCalculationSettings->initialCalculationTime;

if(modeSelection == 0)
{
/****************************************************************************************************/
/*------------------------------ MAIN MODEL LOOP FOR ACQUIRED INPUTS ------------------------------*/
for(int i = 0; i< MotorModel.odeCalculationSettings->numberOfIterations; i++)
{


    timeCV = timeCV + CurVelModel2.odeCVCalculationSettings->calculationStep;

    CurVelModel2.modelCVVariables->i1alpha = Transformation.clarkeTransform1(CurVelModel2.modelCVVariables->inputI1[i], CurVelModel2.modelCVVariables->inputI2[i], CurVelModel2.modelCVVariables->inputI3[i], 0.6667);
    CurVelModel2.modelCVVariables->i1beta = Transformation.clarkeTransform2(CurVelModel2.modelCVVariables->inputI1[i], CurVelModel2.modelCVVariables->inputI2[i], CurVelModel2.modelCVVariables->inputI3[i], 0.6667);
    CurVelModel2.modelCVVariables->motorElectricalAngularVelocity = CurVelModel2.modelCVVariables->inputMotorMechanicalAngularVelocity[i] * CurVelModel2.motorParameters->nOfPolePairs;

 std::cout << "psi2alpha: " << CurVelModel2.modelCVVariables->psi2alpha << "\n";
    CurVelModel2.CurVelModelCalculate(CurVelModel2.modelCVCoeff, CurVelModel2.modelCVVariables, CurVelModel2.odeCVCalculationSettings);
 std::cout << "psi2alpha: " << CurVelModel2.modelCVVariables->psi2alpha << "\n";

    std::cout << "i1beta: " << CurVelModel2.modelCVVariables->i1beta << "\n";
    // std::cout << "psi2alpha: " << CurVelModel2.modelCVVariables->psi2alpha << "\n";
    // std::cout << "psi2beta: " << CurVelModel2.modelCVVariables->psi2beta << "\n";


    psi2Amplitude = sqrt((CurVelModel2.modelCVVariables->psi2alpha * CurVelModel2.modelCVVariables->psi2alpha) + (CurVelModel2.modelCVVariables->psi2beta * CurVelModel2.modelCVVariables->psi2beta));
    transformAngle = atan2f(CurVelModel2.modelCVVariables->psi2beta, CurVelModel2.modelCVVariables->psi2alpha);

    // std::cout << "inputI1[" << i << "]: " << CurVelModel2.modelCVVariables->inputI1[i] << "\n";
    // std::cout << "inputI2[" << i << "]: " << CurVelModel2.modelCVVariables->inputI2[i] << "\n";
    std::cout << "|psi2|[" << i <<"] " << " = " << psi2Amplitude << "\n";
    std::cout << "transformAngle[" << i <<"] " << " = " << transformAngle << "\n";

}
/****************************************************************************************************/

modelCVOutputDataFile2.close(); // close export file
}
else if(modeSelection == 1)
{


    printf("Keyboard input data mode\n\r");
    printf("------------------------------------\n\r");
    printf("Insert data divided by {space symbol}\n\r");
    printf("I1 I2 I3 MechanicalAngularVelocity psi2alpha[0] psi2beta[0] \n\r");
    CurVelModel.odeCVCalculationSettings->numberOfIterations = 1;

    scanf("%f %f %f %f %f %f", CurVelModel2.modelCVVariables->inputI1, CurVelModel2.modelCVVariables->inputI2, CurVelModel2.modelCVVariables->inputI3, CurVelModel2.modelCVVariables->inputMotorMechanicalAngularVelocity, &CurVelModel2.modelCVVariables->psi2alpha, &CurVelModel2.modelCVVariables->psi2beta);


    printf("------------------------------------\n\r");
    printf("You have entered:\n\r");
    printf("I1 = %f\n\rI2 = %f\n\rI3 = %f\n\rMechanicalAngularVelocity = %f\n\rpsi2alpha[0] = %f\n\rpsi2beta = %f\n\r", CurVelModel2.modelCVVariables->inputI1[0], CurVelModel2.modelCVVariables->inputI2[0], CurVelModel2.modelCVVariables->inputI3[0], CurVelModel2.modelCVVariables->inputMotorMechanicalAngularVelocity[0], CurVelModel2.modelCVVariables->psi2alpha, CurVelModel2.modelCVVariables->psi2beta);
    printf("------------------------------------\n\r");

    for(int i = 0; i< CurVelModel.odeCVCalculationSettings->numberOfIterations; i++)
    {
    timeCV = timeCV + CurVelModel2.odeCVCalculationSettings->calculationStep;

    CurVelModel2.modelCVVariables->i1alpha = Transformation.clarkeTransform1(CurVelModel2.modelCVVariables->inputI1[i], CurVelModel2.modelCVVariables->inputI2[i], CurVelModel2.modelCVVariables->inputI3[i], 0.6667);
    CurVelModel2.modelCVVariables->i1beta = Transformation.clarkeTransform2(CurVelModel2.modelCVVariables->inputI1[i], CurVelModel2.modelCVVariables->inputI2[i], CurVelModel2.modelCVVariables->inputI3[i], 0.6667);
    CurVelModel2.modelCVVariables->motorElectricalAngularVelocity = CurVelModel2.modelCVVariables->inputMotorMechanicalAngularVelocity[i] * CurVelModel2.motorParameters->nOfPolePairs;

    

    CurVelModel2.CurVelModelCalculate(CurVelModel2.modelCVCoeff, CurVelModel2.modelCVVariables, CurVelModel2.odeCVCalculationSettings);

    psi2Amplitude = sqrt((CurVelModel2.modelCVVariables->psi2alpha * CurVelModel2.modelCVVariables->psi2alpha) + (CurVelModel2.modelCVVariables->psi2beta * CurVelModel2.modelCVVariables->psi2beta));
     transformAngle = atan2f(CurVelModel2.modelCVVariables->psi2beta, CurVelModel2.modelCVVariables->psi2alpha);

    
    std::cout << "|psi2|[" << i <<"] " << " = " << psi2Amplitude << "\n";
    std::cout << "transformAngle[" << i <<"] " << " = " << transformAngle << "\n";
    
    }

    

}



/*---------------------------------------------------------*/


free(CurVelModel2.motorParameters);
free(CurVelModel2.modelCVCoeff);
free(CurVelModel2.modelCVVariables->inputTime);
free(CurVelModel2.modelCVVariables->inputI1);
free(CurVelModel2.modelCVVariables->inputI2);
free(CurVelModel2.modelCVVariables->inputI3);
free(CurVelModel2.modelCVVariables->inputMotorMechanicalAngularVelocity);
free(CurVelModel2.modelCVVariables);
free(CurVelModel2.odeCVCalculationSettings);

free(MotorModel.motorParameters); // used in I-n model 2. for acquired data, needs to be freed here

// could be freed after ASM model, but better be together with motorParameters for better visibility
free(MotorModel.stateSpaceCoeff);
free(MotorModel.modelVariables);
free(MotorModel.odeCalculationSettings);
free(MotorModel.voltageGeneratorData);

/*---------------------------------------------------------*/




return 0;
}
