// @file testCommonModeVoltage.cpp
// compilation command:  gcc -std=c++14 testCommonModeVoltage.cpp -o run/testCommonModeVoltage -lstdc++ ./../function/svmCore.cpp

#include "./../header/svmCore.h"
#include <stdio.h>
#include <iostream>
#include <stdlib.h>

/*--------------------------------------------------------------------------------------*/
/*------------------------------ TESTING MIN MAX METHOD ------------------------------*/
svmCoreClass svmCore;


int main()
{

svmCore.phaseWantedVoltageAllocateMemory();
svmCore.phaseWantedVoltage->u1a = 230;
svmCore.phaseWantedVoltage->u1b = - 153;
svmCore.phaseWantedVoltage->u1c = -153;
// which value to subtract from wanted voltage to get compare levels
float testValue = svmCore.minMaxCommonModeVoltage(svmCore.phaseWantedVoltage);

std::cout << "min max common mode signal to subtract is = " << testValue << "\n"; 

free(svmCore.phaseWantedVoltage);


return(0);

}



/*--------------------------------------------------------------------------------------*/