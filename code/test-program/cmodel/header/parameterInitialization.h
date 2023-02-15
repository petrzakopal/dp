
// motorParameters type of struct
// for defining motorParameters for a mathematical model
typedef struct motorParameters
{
    float R1;
    float R2;
    float L1s;
    float L2s;
    float Lm;
    float L1;
    float L2;
    float sigma; // sigma = 1 - Lm^(2)/L1L2, it would be probably more clear to read value from struct, but it would mean more operations
}motorParametersStruct;


motorParameters initializeMotorParameters();