#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdio.h>

#include "twi.h"
#include "UART.h"
#include "MPU6050.h"

static volatile uint8_t reg;
static volatile uint8_t read_reg;
static volatile uint8_t write_reg;
static volatile uint8_t scan;
static volatile uint8_t reg_w_val;
static volatile uint8_t addr;
static volatile uint8_t data_ready;

ISR(INT2_vect)
{
    data_ready = 1;
}

ISR(USART0_RX_vect)
{
    static uint8_t st, action;
    static uint8_t reg_tmp, st2;
    char c;
    c = UDR0;
    switch (st){
        case 0:
            if ( c == 'R' || c == 'r'){
                action = 1;
            }
            if ( c == 'W' || c == 'w'){
                action = 2;
            }
            if ( c== 'S' || c == 's'){
                action = 3;
            }
            if (c == ':'){
                if (action == 3){
                    st = 0;
                    scan = 1;
                }else{
                    st = 1;
                }
                st2 = 0;
                reg_tmp = 0;
            }
            break;
        case 1: // leer registro
            if (c >= '0' && c <= '9'){
                reg_tmp = reg_tmp*16 + c - '0';
            }else
            if ( c >= 'a' && c <= 'f'){
                reg_tmp = reg_tmp*16 + c - 'a' + 10;
            }else
            if ( c >= 'A' && c <= 'F'){
                reg_tmp = reg_tmp*16 + c - 'A' + 10;
            }else if(c == ':'){
                switch(st2){
                    case 0:
                        addr = reg_tmp;
                        if ( action == 3){
                            st2 = 0;
                            st = 0;
                            scan = 1;
                        }else{
                            st2++;
                        }
                        break;
                    case 1:
                        reg = reg_tmp;
                        if (action == 1){
                            st = 0;
                            write_reg = 0;
                            read_reg = 1;
                            scan = 0;
                            action = 0;
                        }else{
                            st2++;
                        }
                        break;
                    case 2:
                        reg_w_val = reg_tmp;
                        write_reg = 1;
                        read_reg = 0;
                        scan = 0;
                        st = 0;
                        break;
                }
                reg_tmp = 0;
            }
            break;
        default:
            break;

    }
    //UCSR0A &= ~(1 << RXC0);
}

int main(void)
{
    char buf[20];
    uint8_t i2c_buf[13];
    uint8_t err;
    uint8_t reg_val;
    DDRB = (1 << PIN0);
    uint8_t tmp=0, i;

    setupUART0(1, 1);
    UART0_enRxInt(1);

    EICRA = (1 << ISC21); // interrupcion INT2 falling edge
    EIMSK = (1 << INT2); // activar interrupcion INT2

    sei();
    err = setupMPU6050(0x68);
    //PINC |= (1 << PIN0) | (1 << PIN1);
    sprintf(buf, "E: %i\n", err);
    UART0_sends(buf);
    // Insert code

    while(1){
        if (write_reg){
            write_reg = 0;
            err = mpu6050_writeReg(addr, reg, reg_w_val);
            sprintf(buf, "W:%X:%X:%X:%X\n",addr, reg, reg_w_val, err);
            UART0_sends(buf);
        }

        if (read_reg){
            read_reg = 0;
            err = mpu6050_readReg(addr, reg, &reg_val);
            sprintf(buf, "R:%X:%X:%X:%X\n", addr, reg, reg_val, err);
            UART0_sends(buf);
        }

        if (scan){
            scan = 0;
            for(addr = 0; addr < 0x80; addr++){
                // pedir escritura, si se solicita lectura
                // el dispositivo puede apoderarse del bus
                // para enviar datos
                err = twi_start(addr, 1);
                twi_stop();
                if (err == 0){
                    sprintf(buf,"S:%X\r\n", addr);
                    UART0_sends(buf);
                }
            }
        }

        if( data_ready ){
            data_ready = 0;
//            mpu6050_readReg(0x68, MPU6050_RA_FIFO_COUNTH, &tmp);
//            UART0_send_hex8(tmp);
//            mpu6050_readReg(0x68, MPU6050_RA_FIFO_COUNTL, &tmp);
//            UART0_send_hex8(tmp);
            if (tmp){
                PORTB |= (1 << PIN0);
            }else{
                PORTB &= ~(1 << PIN0);
            }
            if( mpu6050_burstRead(0x68, MPU6050_RA_FIFO_R_W, i2c_buf, 12) )
                UART0_sends("error\n");
            for (i = 0; i < 12; i+=2){
                UART0_send_hex8(i2c_buf[i]);
                UART0_send_hex8(i2c_buf[i+1]);
                UART0_Tx(';');
            }
            UART0_Tx('\n');
        }
    }

    return 0;
}
