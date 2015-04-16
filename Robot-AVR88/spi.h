#ifndef SPI_H_INCLUDED
#define SPI_H_INCLUDED

#include <stdint.h>

typedef enum{
    MODE0=0b00000000,
    MODE1=0b00000100,
    MODE2=0b00001000,
    MODE3=0b00001100,
}SPI_MODE;

void setup_spi(void);
uint8_t spi_rw(uint8_t data);
void spi_mode(SPI_MODE mode);
void xl7105_read_bytes(uint8_t reg, uint8_t * d, uint8_t n);

// spi soft
void spi_write(uint8_t);
uint8_t spi_read();
#endif // SPI_H_INCLUDED
