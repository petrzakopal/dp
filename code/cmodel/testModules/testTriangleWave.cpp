// @file testTriangleWave.cpp
// compilation command:  gcc -std=c++14 testTriangleWave.cpp -o run/testTriangleWave -lstdc++

// test program for triangle wave creation, need to implement in a class for FOC
// source for math explanations of functions
// https://en.m.wikipedia.org/wiki/Triangle_wave

/*
 * Comments:
 * For this settings of calculationStep, u are able to use period of 0.001
 */

#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <fstream>
#include <math.h>

#define PI 3.141592

int main()
{

    // file output streaming
    std::ofstream triangleWaveData;
    triangleWaveData.open("triangleWaveData.csv", std::ofstream::out | std::ofstream::trunc);

    // definition of base values
    float amplitude = 0;
    float period = 0;
    float trianglePoint;

    float finalCalculationTime = 1.8;
    float initialCalculationTime = 0.98;
    float calculationStep = 0.0000001;
    int step = ((int)ceil(((finalCalculationTime - initialCalculationTime) / calculationStep)));
    float time = initialCalculationTime;
    float triangleTime = 0;

    // just input values
    printf("Enter values:\n");
    printf("amplitude:\n");
    scanf("%f", &amplitude);
    printf("period:\n");
    scanf("%f", &period);
    // there was no input or the input is invalid
    if ((amplitude == 0) || (period == 0))
    {
        printf("Values not selected by user, setting default.\n\ramplitude = 5\n\rperiod = 4\n\r");
        amplitude = 5;
        period = 4;
    }

    // demo generation of all samplesą
    for (int i = 0; i <= step; i++)
    {
        if (triangleTime >= period)
        {
            triangleTime = 0;
        }

        // numerical method with abs value and modulo
        trianglePoint = (((4 * amplitude) / period) * abs(fmod((fmod((triangleTime - (period / 4)), period) + period), period) - (period / 2)) - amplitude);

        // trigoniometric method
        // trianglePoint = ((2 * amplitude) / PI) * asin(sin(((2 * PI) / period) * time));

        triangleWaveData << i << "," << trianglePoint << "\n";
        time = time + calculationStep;
        triangleTime = triangleTime + calculationStep;
    }
    triangleWaveData.close(); // close streaming file

    // program ends
    std::cout << "Program terminated!"
              << "\n\r";

    return (0);
}