#include <iostream>
#include <cmath>
#include "modelFunctions.h"



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


int main()
{

float x0 = 0, y0 = 1, x = 2, h = 0.000001;

std::cout << "Result for rungeKutta eq:\n" << rungeKutta(x0, y0, x, h, &dydx) << "\n";

for(int i = 1;i<=10;i++)
{
    std::cout << rungeKutta(x0, y0, i, h, &dydx) << "\n";
}
std::cout <<"Test\n";
return 0;
}


