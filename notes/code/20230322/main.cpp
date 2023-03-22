#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <fstream>
#include <chrono>

void testMethod(int *input)
{
    *input = *input + 5;
}

int main()
{

    int input = 6;


    int* input_ptr = &input;
    testMethod(input_ptr);

    std::cout << input << "\n";

    return(0);
}