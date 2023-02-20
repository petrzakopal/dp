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


int* test = NULL; // defining a pointer
std::cout << test << "\n"; // null adress output 0x0
std::cout << &test << "\n"; // adress of a pointer variable in memory
int testValue = 25; // setting new value variable
std::cout << "saved value testValue\n"; 
test = &testValue; // value of pointer holds adress of testValue
std::cout << "testvalue adress= " << &testValue << "\n"; // printing out adress of testValue
std::cout << test << "\n"; // test pointer value is adress of testValue 
std::cout << &test << "\n"; // still printing adress of a pointer variable in memory
std::cout << *test << "\n"; // dereferencing value => getting value of testValue ,for struct it would be structName->variable
std::cout << (int **)test << "\n"; // same as test - it is value of test so adress of testValue variable
std::cout << (int **)&test << "\n";

float* testikFloat = NULL;                                        // defining pointer
float testAdr;
std::cout <<  "testikFloat" << "\n";
std::cout <<  "testikFloat= "<< testikFloat << "\n";                                // adress to which pointer points = NULL
std::cout <<  "&testikFloat= "<< &testikFloat << "\n"; 
posix_memalign((void **)&testikFloat , 4096 , sizeof(float) );    // 
std::cout << "testikFloat= "<< testikFloat << "\n";                                // value of testikFloat so adress of a memory
std::cout << "&testikFloat= "<< &testikFloat << "\n";                               // adress of a pointer testikFloat
std::cout <<  "*testikFloat= "<< *testikFloat << "\n";                               // value in a memory allocated memory place - dereferencing
std::cout << "(void**)testikFloat= "<< (void **)testikFloat << "\n";                       // value of a testikFloat so adress of a memory
std::cout <<  "(void**)&testikFloat= "<< (void **)&testikFloat << "\n";
std::cout << "(void*)testikFloat= "<< (void *)testikFloat << "\n";
std::cout << "(void*)&testikFloat= "<< (void *)&testikFloat << "\n";
*testikFloat = 5;
std::cout <<  *testikFloat << "\n";                               // setting value in a memory to which is pointed by pointer testikFloat, so dereference testikFloat and paste value 5 here
std::cout << "ref test\n";
int valueTest= 25;                                                // new integer value
int &ra = valueTest;                                              // ra is reference to a
std::cout << ra << "\n";
std::cout << &valueTest << "\n";                                  // adress of a variable valueTest
std::cout << &ra << "\n";                                         // basically adress of a variable valueTest
std::cout << "changing ra\n";
ra = 12;                                                          // changing value of ra so value of valueTest
std::cout << "ra: " << ra << "\n";
std::cout <<  "valueTest: " << valueTest << "\n";

posix_memalign((void **)&testAdr , 4096 , sizeof(float) );



int var = 500;
int* ptrvar;
int** ptrptrvar;
ptrvar = &var;
ptrptrvar = &ptrvar;
int *ptr2;
ptr2 = ptrvar;

std::cout << "var= " << var << "\n";
std::cout << "&var= " << &var << "\n";
std::cout << "ptrvar= " << ptrvar << "\n";
std::cout << "*ptrvar= " << *ptrvar << "\n";
std::cout << "&ptrvar= " << &ptrvar << "\n";
std::cout << "ptrptrvar= " << ptrptrvar << "\n";
std::cout << "*ptrptrvar= " << *ptrptrvar << "\n";
std::cout << "**ptrptrvar= " << **ptrptrvar << "\n";
std::cout << "ptr2= " << ptr2 << "\n";
std::cout << "*ptr2= " << *ptr2 << "\n";
/*---------------------------------------------------------*/


return 0;
}
