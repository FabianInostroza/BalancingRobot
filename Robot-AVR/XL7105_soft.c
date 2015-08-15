#include "spi.h"

#include "XL7105.h"
#include <avr/io.h>
#include <util/delay.h>
#include "A7105_config.h"

#define CS_HIGH() CS_PORT |= (1 << CS_PIN )
#define CS_LOW() CS_PORT &= ~(1 << CS_PIN )

static uint8_t recibiendo; // mejor leer el estado desde el mismo transmisor

uint8_t setup_xl7105(void)
{
    uint8_t i;
    uint8_t id[4];

    CS_DDR |= (1 << CS_PIN); // salida
    WTR_DDR &= ~(1 << WTR_PIN); // entrada

    xl7105_write(0, 0);
    xl7105_write(0, 0);
    _delay_us(10);

    for (i = 1; i<= 0x04; i++)
        xl7105_write(i, pgm_read_byte(&A7105_config[i]));

    for (i = 0x07; i<= 0x22; i++)
        xl7105_write(i, pgm_read_byte(&A7105_config[i]));

    for (i = 0x24; i<= 0x31; i++)
        xl7105_write(i, pgm_read_byte(&A7105_config[i]));

    xl7105_write_id(ID0, ID1, ID2, ID3);

    i = xl7105_if_calibration();
    if ( i )
        return i;
    xl7105_write(0x24, 0x13); // calibracion VCC
    i = xl7105_vco_calibration();
    if ( i )
        return i;

    xl7105_strobe_cmd(STANDBY_MODE);

    xl7105_read_id(id);

    if (id[0] == 0xAA && id[1] == 0xAB)
        return 0;
    else
        return 1;
}

uint8_t xl7105_if_calibration(void)
{
    uint8_t tmp;

    xl7105_write(0x22, 0x00); // MFBS = 0, calibracion automatica

    xl7105_strobe_cmd(PLL_MODE); // fijar modo PLL

    xl7105_write(0x02, 0x01); // FBC = 1;

    _delay_ms(1); // tiempo maximo de calibracion = 256us

    do{
        tmp = xl7105_read(0x02);
        tmp &= 0x01;
    }while(tmp);

//    if ( (tmp & 0x01) == 0 ){ // si termino la calibracion
        tmp = xl7105_read(0x22);
//    }else
//        return tmp;

    return tmp & 0x10; // retornar estado de FBCB

}

uint8_t xl7105_vco_calibration(void)
{
    uint8_t tmp;

    xl7105_write(0x25, 0x00); // MVBS = 0, calibracion automatica

    xl7105_strobe_cmd(PLL_MODE); // fijar modo PLL

    xl7105_write(0x26, 0x3B); //VTH = 0b111, VTL = 0b011;
    xl7105_write(0x02, 2); // VBC=1

    _delay_ms(1); // tiempo maximo de calibracion = 256us

    tmp = xl7105_read(0x02);

    if ( (tmp & 0x02) == 0 ){ // si termino la calibracion
        tmp = xl7105_read(0x25);
    }else
        return tmp;

    return tmp & 0x08; // retornar estado de VBCF

}

uint8_t xl7105_read(uint8_t reg)
{
    uint8_t tmp;

    CS_LOW();

    //spi_mode(MODE0);
    tmp = 0x40 | (reg & 0x3F);
    spi_write(tmp);
    //spi_write(tmp);

    tmp = spi_read();
    //tmp = spi_read();
    CS_HIGH();
    return tmp;

}

void xl7105_write(uint8_t reg, uint8_t data)
{
    uint8_t addr;
    CS_LOW();
    addr = reg & 0x3f;
    spi_write(addr);
    //spi_write(addr);

    spi_write(data);
    //spi_write(data);
    CS_HIGH();
}

void xl7105_strobe_cmd(STROBE_CMD cmd)
{
    CS_LOW();
    spi_write(cmd);
    //spi_write(cmd);
    CS_HIGH();
}

void xl7105_read_id(uint8_t * id)
{
    uint8_t i;
    CS_LOW();

    spi_write(0x46);

    for (i = 0; i < 4; i++){
        id[i] = spi_read();
    }

    CS_HIGH();
}

void xl7105_read_bytes(uint8_t reg, uint8_t * d, uint8_t n)
{
    uint8_t i;
    CS_LOW();

    reg = 0x40 | (reg & 0x3f);
    spi_write(reg);

    for (i = 0; i < n; i++){
        d[i] = spi_read();
    }

    CS_HIGH();
}

void xl7105_write_id(uint8_t id0, uint8_t id1, uint8_t id2, uint8_t id3)
{
    CS_LOW();
    spi_write(0x06);
    spi_write(id0);
    spi_write(id1);
    spi_write(id2);
    spi_write(id3);
    CS_HIGH();
}

void xl7105_write_bytes(uint8_t reg, uint8_t *d, uint8_t n)
{
    uint8_t i;
    CS_LOW();
    spi_write(reg);

    for (i = 0; i < n; i++){
        spi_write(d[i]);
    }
    CS_HIGH();
}

uint8_t xl7105_tx(uint8_t * data, uint8_t n, uint8_t block)
{
    if ( ( WTR_PORT & (1 << WTR_PIN ) ) == 0){
        recibiendo = 0;
        xl7105_strobe_cmd(FIFO_WP_R);
        if (n > FL){
            n = FL;
        }
        xl7105_write_bytes(0x05, data, n);
        xl7105_strobe_cmd(TX_MODE);
        _delay_us(5);
        if ( block ){
            while ( WTR_PORT & (1 << WTR_PIN));
        }
        return n;
    }else{
        return 0;
    }
}

/*
 Si estamos en modo RX revisa el pin WTR para
 ver si se recibio el paquete, si se recibio
 un paquete entonces lo lee y vuelve a poner
 el chip en modo RX.
 Si no se esta recibiendo pone al chip en
 modo RX.

 Valor de retorno = 0 indica que el puntero
 data no contiene datos validos.

 lectura de 7 bytes tarda aprox 300us @16MHz
*/
uint8_t xl7105_rx(uint8_t * data, uint8_t n, uint8_t cont)
{
    uint8_t ret = 0;
    if (recibiendo){
        if ( ( WTR_PORT & (1 << WTR_PIN ) ) == 0){
            xl7105_strobe_cmd(FIFO_WP_R);
            // si el largo del array data es mayor
            // al largo del buffer fifo entonces
            // solo leer FL bytes
            if (n > FL){
                n = FL;
            }
            xl7105_read_bytes(0x05, data, n);
            // continuamente recibiendo
            // se puede interrumpir recepcion
            // con comando strobe o tx
            if (cont){
                xl7105_strobe_cmd(RX_MODE);
                _delay_us(5);
            }else{
                recibiendo = 0;
            }
            #if defined(EN_CRC) || defined(EN_FEC)
            // revisar flag de crc
            cont = xl7105_read(0);
            #endif

            // CRC y FEC activo
            #if defined(EN_CRC) && defined(EN_FEC)
            if ( cont & 0x60){
                //error de crc o fec
                ret = 0;
            }else{
                ret = n;
            }
            #endif

            // Solo CRC activo
            #if defined(EN_CRC) && !defined(EN_FEC)
            if (cont & 0x20 ){
                // error CRC
                ret = 0;
            }else{
                ret = n;
            }
            #endif

            // SOLO FEC activo
            #if !defined(EN_CRC) && defined(EN_FEC)
            if (cont & 0x40 ){
                // error FEC
                ret = 0;
            }else{
                ret = n;
            }
            #endif

            // ni CRC y FEC activo
            #if !defined(EN_CRC) && !defined(EN_FEC)
            ret = n;
            #endif
        }
    }else{
        xl7105_strobe_cmd(RX_MODE);
        _delay_us(5); // para asegurarse que el pin WTR cambia de estado
        recibiendo = 1;
    }
    return ret;
}

inline uint8_t xl7105_rssi(void){
    return xl7105_read(0x1d);
}
