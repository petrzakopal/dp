#ifndef REGULATOR_H
#define	REGULATOR_H

#include <stdint.h>
#include <stdlib.h>


/*NASTAVENE VYCHOZI HODNOTY KONSTANT REGULATORU */
// #define DEF_HODNOTA_KP 0.0190
// #define DEF_HODNOTA_KI 0.0001098
// #define DEF_HODNOTA_KC 0.0001098
// #define DEF_HODNOTA_UMAX 127
// #define DEF_HODNOTA_UMIN -127
// #define DEF_HODNOTA_TAU 173

typedef struct
{
    float saturationInput; // input to saturation block
    float wanted; // wanted value
    float measured; // real value
    float saturationOutput; // out from regulator - action value
    float antiWindUpDif; // regulatory error for anti-windup
    float eDif; // regulatory error
    float iSum; // integration - suma
    float kp; // proportional constant value
    float ki; // integral constant value
    float kAntiWindUp; // anti-windup constant value
    int saturationOutputMax; // restriction in saturation block - maximum
    int saturationOutputMin; // restriction in saturation block - minimum
    char stav; // stav v automatu pro reseni spousteni regulatoru
}RegulatorType;


class RegulatorClass
{
    public:
        RegulatorType *fluxRegulator = NULL;
        RegulatorType *velocityRegulator = NULL;
        RegulatorType *iqRegulator = NULL;
        RegulatorType *idRegulator = NULL;



        /*-----------------------------------------------------------------------------------------------------*/
        /*------------------------------- ALLOCATE MEMORY FOR REGULATOR DATA ---------------------------------*/
        void regulatorAllocateMemory(RegulatorType *regulatorData);
        /*----------------------------------------------------------------------------------------------------*/
        




        /*-----------------------------------------------------------------------------------------------------*/
        /*------------------------------ SATURATION BLOCK INSERTED IN REGULATOR ------------------------------*/
        float regSaturationBlock(float saturationInput, float saturationOutputMin, float saturationOutputMax);
        /*--------------------------------------------------------------------------------------------*/


        /*--------------------------------------------------------------------------------------*/
        /*------------------------------ MAIN REGULATOR FUNCTION ------------------------------*/
        void regCalculate(RegulatorType *regulatorData);
        /*--------------------------------------------------------------------------------------*/

};

#endif	/* REGULATOR_H */

