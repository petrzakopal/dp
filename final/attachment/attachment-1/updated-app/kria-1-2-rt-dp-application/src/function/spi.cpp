#include "../header/spi.h"

/*
 * @name     acknowledgeSPIInterrupt
 * @class    spiClass
 * @brief    Basic function to acknowledge an interrupt.
 * @todo     Make API from SPI and Interrupt solving. Solve how it works in Linux.
 */

void spiClass::acknowledgeSPIInterrupt(int fd, void *ptr)
{
    struct pollfd fds = {
        .fd = fd,
        .events = POLLIN | POLLOUT,
    };

    int irq_on = 1;        // for writing 0x1 to /dev/uioX
    uint32_t info = 1;     // in read function of a interrupt checking
    off_t interruptStatus; // interruptStatus register - for asserting bit

    while (true)
    {
        int ret = poll(&fds, 1, -1); // poll on a return value

        // there was change in ret
        if (ret >= 1)
        {
            /* Do something in response to the interrupt. */
            printf("AXI Quad SPI Interrupt!\n");
            interruptStatus = *((unsigned *)(ptr + SPI_IPISR));
            std::this_thread::sleep_for(std::chrono::nanoseconds(1)); // could not resolve other way now, because reading and writing to register takes more time that one tick probably
            *((unsigned *)(ptr + SPI_IPISR)) = interruptStatus;       // resetting SPI interrupt status register
            write(fd, &irq_on, sizeof(irq_on));                       // writing to UIO device to clear interrupt
            break;
        }
    }
}

/*
 * @name     SPI communication functions.
 * @class    spiClass
 * @brief
 * @todo     Resolve when using multiple slaves to automate slave changing. Eg. when using one slave on 1. SS, just changing 0x01 to 0x00, when using slave only on 2. SS change 0x2 to 0x0, but when using slaves on 1. and 2. SS and activating slave only on SS 1. it should probably be 0x3 (both at 1) to 0x2 (first at 0) to 0x3 (both at 1).
 */

/*
 * @name     sendSPIdata
 * @class    spiClass
 * @brief    Send SPI data to desired slave via mapped device.
 * @todo     Implement interrupt and delete sleep_for...
 * @details  For details about registers check PG of AXI SPI Timer
 */
void spiClass::sendSPIdata(void *ptr, int fd, off_t slaveSelect, off_t data)
{
    *((unsigned *)(ptr + SPI_SPICR)) = 0x1E6;
    *((unsigned *)(ptr + SPI_SPISSR)) = slaveSelect; // 0x01 - slave select 1 to high, when slave no. 2 - it would be 0x02
    *((unsigned *)(ptr + SPI_DTR)) = data;
    *((unsigned *)(ptr + SPI_SPISSR)) = 0x0;
    *((unsigned *)(ptr + SPI_SPICR)) = 0x86;
    acknowledgeSPIInterrupt(fd, ptr); // acknowledging interrupt
    *((unsigned *)(ptr + SPI_SPISSR)) = slaveSelect;
}

/*
 * @name     initializeSPI
 * @class    spiClass
 * @brief    Initialize SPI communication in PL for desired slave and mapped device.
 * @todo     Implement interrupt and delete sleep_for...
 */
void spiClass::initializeSPI(void *ptr, off_t slaveSelect)
{
    *((unsigned *)(ptr + SPI_SPICR)) = 0x1E6;

    *((unsigned *)(ptr + SPI_DTR)) = 0x06;

    *((unsigned *)(ptr + SPI_SPISSR)) = 0x00;

    *((unsigned *)(ptr + SPI_SPISSR)) = slaveSelect;

    *((unsigned *)(ptr + SPI_SPICR)) = 0x1E6;

    // Interrupt settings

    // Global Interrupt Enable
    *((unsigned *)(ptr + SPI_DGIER)) = 0x80000000;

    // Interrupt enables
    // *((unsigned *)(ptr + SPI_IPIER)) = 0x3FFF;               // enabling all interrupts
    *((unsigned *)(ptr + SPI_IPIER)) = 0x4; // enabling onty DTR is clear INT

    off_t interruptStatus;

    interruptStatus = *((unsigned *)(ptr + SPI_IPISR));
    std::this_thread::sleep_for(std::chrono::nanoseconds(1)); // could not resolve other way now, because reading and writing to register takes more time that one tick probably
    *((unsigned *)(ptr + SPI_IPISR)) = interruptStatus;       // resetting SPI interrupt status register
}

/*
 * @name     initializeLEDmatrix
 * @class    spiClass
 * @brief    Initialize LED matrix with MAX7219.
 * @todo     Implement interrupt and delete sleep_for...
 */
void spiClass::initializeLEDmatrix(void *ptr, int fd, off_t slaveSelect)
{
    off_t initilSeq[5] = {0x0900, 0x0a01, 0x0b07, 0x0c01, 0x0f00};

    for (int i = 0; i < 5; i++)
    {
        *((unsigned *)(ptr + SPI_SPICR)) = 0x1E6;
        *((unsigned *)(ptr + SPI_SPISSR)) = slaveSelect; // 0x01 - slave select 1 to high, when slave no. 2 - it would be 0x02
        *((unsigned *)(ptr + SPI_DTR)) = initilSeq[i];
        *((unsigned *)(ptr + SPI_SPISSR)) = 0x0;
        *((unsigned *)(ptr + SPI_SPICR)) = 0x86;
        acknowledgeSPIInterrupt(fd, ptr); // acknowledging interrupt
        *((unsigned *)(ptr + SPI_SPISSR)) = slaveSelect;
    }
}

/*
 * @name     clearLEDmatrix
 * @class    spiClass
 * @brief    Clear LED matrix (set all LEDs to 0).
 * @todo     Implement interrupt and delete sleep_for...
 */
void spiClass::clearLEDmatrix(void *ptr, int fd, off_t slaveSelect)
{

    off_t clearSeq[8] = {0x100, 0x200, 0x300, 0x400, 0x500, 0x600, 0x700, 0x800};
    for (int i = 0; i < 8; i++)
    {
        *((unsigned *)(ptr + SPI_SPICR)) = 0x1E6;
        *((unsigned *)(ptr + SPI_SPISSR)) = slaveSelect; // 0x01 - slave select 1 to high, when slave no. 2 - it would be 0x02
        *((unsigned *)(ptr + SPI_DTR)) = clearSeq[i];
        *((unsigned *)(ptr + SPI_SPISSR)) = 0x0;
        *((unsigned *)(ptr + SPI_SPICR)) = 0x86;
        acknowledgeSPIInterrupt(fd, ptr); // acknowledging interrupt
        *((unsigned *)(ptr + SPI_SPISSR)) = slaveSelect;
    }
}

/*
 * @name     printLetterOnLEDMatrix
 * @class    spiClass
 * @brief    Print desired letter on a LED matrix.
 * @todo     Implement interrupt and delete sleep_for...
 */
void spiClass::printLetterOnLEDMatrix(void *ptr, int fd, off_t slaveSelect, off_t *pismeno)
{
    for (int i = 0; i < 8; i++)
    {
        *((unsigned *)(ptr + SPI_SPICR)) = 0x1E6;
        *((unsigned *)(ptr + SPI_SPISSR)) = slaveSelect; // 0x01 - slave select 1 to high, when slave no. 2 - it would be 0x02
        *((unsigned *)(ptr + SPI_DTR)) = pismeno[i];
        *((unsigned *)(ptr + SPI_SPISSR)) = 0x0;
        *((unsigned *)(ptr + SPI_SPICR)) = 0x86;
        acknowledgeSPIInterrupt(fd, ptr); // acknowledging interrupt
        *((unsigned *)(ptr + SPI_SPISSR)) = slaveSelect;
    }
}