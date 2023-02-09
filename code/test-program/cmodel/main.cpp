#include <iostream>
#include <cmath>
#include "modelFunctions.h"
#include <vector>



// Runge Kutta RK4 Algorithmus
float rungeKutta(float x0, float y0, float x, float h, float (*func)(float, float))
{

    // Number of iterations 
    int n = (int)((x-x0)/h);
    float k1, k2, k3, k4;

    float y = y0;

    // Iterating for number of iterations
    for(int i = 1; i<=n;i++)
    {
        k1 = h*func(x0,y0);
        k2 = h*func(x0 + 0.5*h, y + 0.5*k1);
        k3 = h*func(x0 + 0.5*h, y + 0.5*k2);
        k4 = h*func(x0 + h, y + k3);

        // Updating the y and x values for new iteration
        y = y + (1.0/6.0) * (k1 + 2*k2 + 2*k3 + k4);
        x0 = x0 + h;
    }

    return y;
}

typedef struct motorParameters
{
    float R1;
    float R2;
    float L1s;
    float L2s;
    float Lm;
    float L1;
    float L2;
}motorParametersStruct;



motorParameters initializeMotorParameters()
{
    motorParametersStruct motorParameters;
    motorParameters.R1 = 48;

    return motorParameters;
}


int main()
{
    motorParametersStruct motorParametersData;
    motorParametersData.R1 = 12;
    std::cout << "Motor parameter R1 is: " << motorParametersData.R1 << "\n";
     std::vector<int> v = {7, 5, 16, 8};

     motorParametersStruct motorParameters2;
     motorParameters2 = initializeMotorParameters();
     std::cout << "Motor parameters initialized from function R1 is: " << motorParameters2.R1 << "\n";

float x0 = 0, y0 = 1, x = 2, h = 0.000001;

std::cout << "Result for rungeKutta eq:\n" << rungeKutta(x0, y0, x, h, &dydx) << "\n";

for(int i = 1;i<=10;i++)
{
    std::cout << rungeKutta(x0, y0, i, h, &dydx) << "\n";
}
std::cout <<"Test\n";
std::cout << "vector:\n";
std::cout << v.at(2) <<"\n";
std::cout << v[2] <<"\n";
  std::vector<int>* v_pointer;
v_pointer = &v;
std::cout << v_pointer->at(2) << "\n";
 std::vector<int> &vr = *v_pointer; //Create a reference

vr[2] = 28;
vr = {1,14,56};
  std::cout <<  vr[2]; //Normal access through reference
  vr.push_back(13);
  std::cout << "\npushed back\n";
  std::cout <<  vr[3];
  int testArray[] = {12,21,66};
  std::cout <<"\n" << testArray[2];
  testArray[3] = 44;
  std::cout <<"\n" << testArray[3];
    std::cout << "\ntest data\n";
  std::cout << v.data() << "\n";

  motorParametersStruct *pointerStruct;
  pointerStruct = &motorParametersData;

  std::cout << pointerStruct->R1 <<"\n";

  std::cout << "motorParametersData adress: " << &motorParametersData << "\n";

  std::cout << (0 ? "FAILED" : "PASSES") << std::endl;
return 0;
}
