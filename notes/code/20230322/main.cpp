#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <fstream>
#include <chrono>

void testMethod(int *input)
{
    *input = *input + 5;
}


void testInputMethod(int *input)
{
    input[0] = 222;
}
int main()
{

    int input = 6;


    int* input_ptr = &input;
    testMethod(input_ptr);

    std::cout << input << "\n";



    int testInput[2];
    testInput[0]=111;

    testInputMethod(testInput);

    std::cout << "test input " << testInput[0] << "\n"; 

    return(0);
}