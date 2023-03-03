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

svmCore.coreInternalVariablesAllocateMemory();
// svmCore.phaseWantedVoltageAllocateMemory(); // depracated
svmCore.coreInternalVariables->u1a = 230;
svmCore.coreInternalVariables->u1b = - 153;
svmCore.coreInternalVariables->u1c = -153;
// which value to subtract from wanted voltage to get compare levels
float testValue = svmCore.minMaxCommonModeVoltage(svmCore.coreInternalVariables);

std::cout << "min max common mode signal to subtract is = " << testValue << "\n"; 

free(svmCore.coreInternalVariables);


return(0);

}



/*--------------------------------------------------------------------------------------*/