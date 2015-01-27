/*
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "pwm.h"
#include "UART.h"

ISR(ADC_vect)
{
    uint16_t adc = ADC;
    OCR1A = adc;
    OCR1B = adc;
}

ISR(USART0_RX_vect)
{
    static uint8_t st, action;
    static uint16_t val_tmp;
    char c;
    c = UDR0;
    switch (st){
        case 0:
            if ( c == 'A' || c == 'a'){
                action = 1;
            }
            if ( c == 'B' || c == 'b'){
                action = 2;
            }
            if (c == ':'){
                val_tmp = 0;
                st = 1;
            }
            break;
        case 1: // leer registro
            if (c >= '0' && c <= '9'){
                val_tmp = val_tmp*16 + c - '0';
            }else
            if ( c >= 'a' && c <= 'f'){
                val_tmp = val_tmp*16 + c - 'a' + 10;
            }else
            if ( c >= 'A' && c <= 'F'){
                val_tmp = val_tmp*16 + c - 'A' + 10;
            }else if(c == ':' || c == '\n' || c == '\r'){
                if (action == 1){
                    action = 0;
                    OCR1A = val_tmp & 0x3FF;
                    OCR1B = val_tmp & 0x3FF;
                }else if ( action == 2){
                    action = 0;
                    OCR3A = val_tmp & 0x3FF;
                    OCR3B = val_tmp & 0x3FF;
                }
                st = 0;
            }
            break;
        default:
            break;

    }
    //UCSR0A &= ~(1 << RXC0);
}

void setup_adc(void)
{
    ADMUX = (1 << REFS0);// | (1 << ADLAR);

    ADCSRB = 0;

    DIDR0 = (1 << ADC0D);

    ADCSRA = (1 << ADEN) | (1 << ADSC) | (1 << ADATE) | (1 << ADIE) | 3;
}

int main(void)
{
    //setup_adc();
    setup_pwm();
    setupUART0(1, 1);
    UART0_enRxInt(1);
    sei();
    while(1){
        _delay_ms(500);
        UART0_send_hex16(OCR1A);
        UART0_Tx('\n');
    }

    return 0;
}
