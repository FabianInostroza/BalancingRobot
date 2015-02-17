#include <avr/io.h>
#include <util/delay.h>

#define CLK_PIN PIN6
#define SDIO_PIN PIN5
#define CLK_PORT PORTA
#define SDIO_PORT PORTA
#define SDIO_PIN_PORT PINA
#define SDIO_DDR DDRA
#define CLK_DDR DDRA

void setup_spi(void)
{
    SDIO_DDR |= (1 << SDIO_PIN);
    CLK_DDR |= (1 << CLK_PIN);
}


uint8_t spi_write(uint8_t data)
{
    uint8_t i;

    CLK_PORT &= ~(1 << CLK_PIN);
    SDIO_DDR |= (1 << SDIO_PIN);

    for (i = 0; i < 8; i++){
        if (data  & 0x80)
            SDIO_PORT |= (1 << SDIO_PIN);
        else
            SDIO_PORT &= ~(1 << SDIO_PIN);

        CLK_PORT |= (1 << CLK_PIN); // subir el pin
        _delay_us(1);
        data = data << 1;
        CLK_PORT &= ~(1 << CLK_PIN); // bajar clk
        _delay_us(1);
    }

    return 0;
}

uint8_t spi_read(void)
{
    uint8_t i, data;

    CLK_PORT &= ~(1 << CLK_PIN);
    SDIO_DDR &= ~(1 << SDIO_PIN);

    data = 0;

    for (i = 0; i < 8; i++){
        data = data << 1;
        if (SDIO_PIN_PORT & (1 << SDIO_PIN))
            data |= 1;

        CLK_PORT |= (1 << CLK_PIN); // subir el pin
        _delay_us(1);
        CLK_PORT &= ~(1 << CLK_PIN); // bajar clk
        _delay_us(1);
    }

    return data;
}
