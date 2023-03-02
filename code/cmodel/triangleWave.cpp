#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <fstream>
#include <math.h>

#define PI 3.141592

int main()
{

    std::ofstream triangleWaveData;
    triangleWaveData.open ("triangleWaveData.csv",std::ofstream::out | std::ofstream::trunc);

    float amplitude = 0;
    float period = 0;
    float trianglePoint;

    printf("Enter values:\n");
    printf("amplitude:\n");
    scanf("%f", &amplitude);
    printf("period:\n");
    scanf("%f", &period);

    float finalCalculationTime = 1;
    float initialCalculationTime = 0;
    float calculationStep = 0.00001;
    int step = ((int)ceil(((finalCalculationTime - initialCalculationTime)/calculationStep)));

    if((amplitude == 0) || (period == 0))
    {
         printf("Values not selected by user, setting default.\n\ramplitude = 5\n\rperiod = 4\n\r");
         amplitude = 5;
         period = 4;
    }

    float time = initialCalculationTime;

    for(int i = 0;i<=step;i++)
    {
       



        trianglePoint = (((4 * amplitude)/period) * abs(fmod((fmod((time-(period/4)), period) + period), period) - (period/2)) - amplitude);

        // trianglePoint = ((2 * amplitude)/PI)*asin(sin(((2 * PI)/period) * time));

        triangleWaveData << time << "," << trianglePoint << "\n";
        time = time + calculationStep;
    

    }
    triangleWaveData.close();

    std::cout << "Program terminated!" << "\n\r";
    

    return(0);
}