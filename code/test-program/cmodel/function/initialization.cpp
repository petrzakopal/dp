#include "../header/initialization.h"

// motorParametersType initializeMotorParameters()
// {
//     motorParametersType motorParameters;
//     motorParameters.R1 = 0.370f; // Ohm, stator rezistance
//     motorParameters.R2 = 0.225f; // Ohm, rotor rezistance
//     motorParameters.L1s = 0.00227f; // H, stator leakage inductance
//     motorParameters.L2s = 0.00227f; // H, rotor leakage inductance
//     motorParameters.Lm = 0.0825f; // H, main flux inductance
//     motorParameters.L1 = 0.08477f; // H, inductance
//     motorParameters.L2 = 0.08477f; // H, inductance
//     motorParameters.sigma = 0.05283f; // = 0.0528396032, sigma = 1 - Lm^(2)/L1L2
//     return motorParameters;
// }





void MotorModelClass::setMotorParameters()
{
    motorParameters.R1 = 0.370f; // Ohm, stator rezistance
    motorParameters.R2 = 0.225f; // Ohm, rotor rezistance
    motorParameters.L1s = 0.00227f; // H, stator leakage inductance
    motorParameters.L2s = 0.00227f; // H, rotor leakage inductance
    motorParameters.Lm = 0.0825f; // H, main flux inductance
    motorParameters.L1 = 0.08477f; // H, inductance
    motorParameters.L2 = 0.08477f; // H, inductance
    motorParameters.sigma = 0.05283f; // = 0.0528396032, sigma = 1 - Lm^(2)/L1L2
   
}

motorParametersType MotorModelClass::getMotorParameters()
{
    return (motorParameters);
}