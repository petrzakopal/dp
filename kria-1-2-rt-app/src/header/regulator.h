
#include <stdint.h>
#include <stdlib.h>



/*----------------------------------------------------------------------*/
/*-------------------- TYPE/STRUCT FOR REGULATORS --------------------*/
typedef struct
{
    float saturationInput; // input to saturation block
    float wantedValue; // wanted value
    float measuredValue; // real value
    float saturationOutput; // out from regulator - action value
    float antiWindUpDif; // regulatory error for anti-windup // depracated
    float eDif; // regulatory error
    float iSum; // integration - suma
    float kp; // proportional constant value
    float ki; // integral constant value
    // float kAntiWindUp; // anti-windup constant value // depracated
    float saturationOutputMax; // restriction in saturation block - maximum
    float saturationOutputMin; // restriction in saturation block - minimum
    bool clampingStatus;
    bool saturationCheckStatus;
    bool signCheckStatus;
}RegulatorType;
/*----------------------------------------------------------------------*/


class RegulatorClass
{
    public:
        RegulatorType *fluxRegulator = NULL;
        RegulatorType *velocityRegulator = NULL;
        RegulatorType *iqRegulator = NULL;
        RegulatorType *idRegulator = NULL;



        /*-----------------------------------------------------------------------------------------------------*/
        /*------------------------------- ALLOCATE MEMORY FOR REGULATOR DATA ---------------------------------*/
        void regulatorAllocateMemory();
        /*----------------------------------------------------------------------------------------------------*/
        
        /*-----------------------------------------------------------------------------------------------------*/
        /*------------------------------- ANTI WIND UP CHECKS + ENABLE STATUS ---------------------------------*/
        void checkSaturationStatus(RegulatorType *regulatorData);
        void checkSignStatus(RegulatorType *regulatorData);
        void enableClamping(RegulatorType *regulatorData);
        /*-----------------------------------------------------------------------------------------------------*/

        /*-----------------------------------------------------------------------------------------------------*/
        /*------------------------------ SATURATION BLOCK INSERTED IN REGULATOR ------------------------------*/
        float regSaturationBlock(float saturationInput, float saturationOutputMin, float saturationOutputMax);
        /*--------------------------------------------------------------------------------------------*/


        /*--------------------------------------------------------------------------------------*/
        /*------------------------------ MAIN REGULATOR FUNCTION ------------------------------*/
        void regCalculate(RegulatorType *regulatorData);
        /*--------------------------------------------------------------------------------------*/

};  
