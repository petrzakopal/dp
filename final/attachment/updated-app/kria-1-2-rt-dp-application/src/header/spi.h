#ifndef SPI_H
#define SPI_H

#define MAP_SIZE 0x10000
#define SPI_SPICR 0x60
#define SPI_SPISR 0x64
#define SPI_DTR 0x68
#define SPI_DRR 0x6C
#define SPI_SPISSR 0x70
#define SPI_TRANSMIT_FIFO_OCUPANCY 0x74
#define SPI_RECEIVE_FIFO_OCUPANCY 0x78
#define SPI_DGIER 0x1C
#define SPI_IPISR 0x20
#define SPI_IPIER 0x28

#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <poll.h>
#include <stdio.h>
#include <chrono>
#include <thread>
#include <mutex>
#include <future>

class spiClass
{
public:
    void acknowledgeSPIInterrupt(int fd, void *ptr);
    void sendSPIdata(void *ptr, int fd, off_t slaveSelect, off_t data);
    void initializeSPI(void *ptr, off_t slaveSelect);
    void initializeLEDmatrix(void *ptr, int fd, off_t slaveSelect);
    void clearLEDmatrix(void *ptr, int fd, off_t slaveSelect);
    void printLetterOnLEDMatrix(void *ptr, int fd, off_t slaveSelect, off_t *pismeno);
};

#endif /* SPI_H */