#include <avr/io.h>
#include "spi.h"

void setup_spi(void)
{
    PRR0 &= ~(1 << PRSPI);
    // SS, MOSI y SCK como salidas
    DDRB |= (1<<PIN4) | (1 << PIN5) | (1 << PIN7);

    // activar SPI, Modo 0
    // se transmite MSB primero, SCK = fosc/16 (1MHz)
    SPCR = (1 << SPE) | (1 << MSTR) | (1 << SPR0);
}

void spi_mode(SPI_MODE mode)
{
    uint8_t reg;
    reg = SPCR & 0b11110011;
    mode &= 0b0001100;
    SPCR = reg | mode;
}


uint8_t spi_rw(uint8_t data)
{
    SPDR = data;

    while( !( SPSR & (1 << SPIF) ) );

    return SPDR;
}
