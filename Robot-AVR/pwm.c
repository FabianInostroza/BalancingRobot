#include "pwm.h"
#include <avr/interrupt.h>

void setup_pwm(void)
{

    #ifndef __AVR_ATmega328P__
    DDRD |= (1 << PIN4) | (1 << PIN5);
    DDRB |= (1 << PIN6) | (1 << PIN7);
    // phase and frequency correct pwm, ~7.5kHz
    // borrar OC1A en compare match en subida, fijar OC1A en compare match en bajada
    // OC1B invertida
    //TCCR1A = (1 << COM1A1) | (0 << COM1A0) | (1 << COM1B1) | (1 << COM1B0);
    // reloj = clk/1
    //TCCR1B = (1 << WGM13) | (0 << CS12) | (1 << CS10);

    // fast pwm, ~15 kHz
    TCCR1A = (1 << COM1A1) | (1 << COM1A0) | (1 << COM1B1) | (1 << COM1B0) | (1 << WGM11);
    TCCR1B = (1 << WGM12) | (1 << WGM13) | (0 << CS12) | (1 << CS10);

    TCCR3A = (1 << COM3A1) | (1 << COM3A0) | (1 << COM3B1) | (1 << COM3B0) | (1 << WGM31);
    TCCR3B = (1 << WGM32) | (1 << WGM33) | (0 << CS32) | (1 << CS30);

    ICR1 = 0x3FF;
    ICR3 = 0x3FF;

    OCR1A = 0;
    OCR1B = 0;

    OCR3A = 0;
    OCR3B = 0;

    #else
    PRR &= ~(1 << PRTIM1);
    DDRB |= (1 << PIN1) | (1 << PIN2);

    // fast pwm 10 bits, modo no invertido
    TCCR1A = (1 << COM1A1) | (0 << COM1A0) | (1 << COM1B1) | (0 << COM1B0) | (1 << WGM10) | (1 << WGM11);
    // sin preescaler
    TCCR1B = (1 << WGM12) | (0 << WGM13) | (1 << CS10) | (0 << CS11) | (0 << CS12);

    TIMSK1 = (1 << OCIE1A);

    OCR1A = 0x0;
    OCR1B = 0x0;
    #endif // __AVR_ATmega328P__
}

#ifndef __AVR_ATmega328P__

inline void pwmL_duty(int16_t d)
{
    if (d > 0x3ff)
        d = 0x3ff;

    if (d < -0x3ff)
        d = -0x3ff;

    if ( d < 0 ){
        OCR3A = -d; // B-IA
        OCR3B = 0; // B-IB
    }else{
        OCR3A = 0; // B-IA
        OCR3B = d; // B-IB
    }
}

inline void pwmR_duty(int16_t d)
{
    if (d > 0x3ff)
        d = 0x3ff;

    if (d < -0x3ff)
        d = -0x3ff;

    if ( d < 0){
        OCR1A = -d; // A-IA
        OCR1B = 0; // A-IB
    }else{
        OCR1A = 0; // A-IA
        OCR1B = d; // A-IB
    }
}
#else
static uint8_t dirL = 0;
static uint8_t dirR = 0;
ISR(TIMER1_COMPA_vect){
    if( dirL ){
        PORTB &= ~(1 << PINB0);
        PORTD |= (1 << PIND7);
    }else{
        PORTB |= (1 << PINB0);
        PORTD &= ~(1 << PIND7);
    }
    TIMSK1 &= ~(1 << OCIE1A);
}

ISR(TIMER1_COMPB_vect){
    if( dirR ){
        PORTB = (PORTB & ~(1 << PINB3)) | (1 << PINB4);
    }else{
        PORTB = (PORTB & ~(1 << PINB4)) | (1 << PINB3);
    }
    TIMSK1 &= ~(1 << OCIE1B);
}

void pwmL_duty(int16_t dc)
{
    static uint8_t last_sign = 0; // 1 positivo
    uint8_t sign = dc >= 0 ? 1 : 0;

    if (dc > 0x3ff)
        dc = 0x3ff;

    if (dc < -0x3ff)
        dc = -0x3ff;

    if( dc < 0){
        dirL = 0;
        OCR1A = (-dc) & 0x3ff;
        //pwm1ab = (1 << PINB0);
    }else{
        dirL = 1;
        OCR1A = dc & 0x3ff;
        //pwm1ab = (1 << PIND7);
    }

    if( sign != last_sign){
        TIMSK1 |= (1 << OCIE1A);
    }
    last_sign = sign;
}

void pwmR_duty(int16_t dc)
{
    static uint8_t last_sign = 0; // 1 positivo
    uint8_t sign = dc >= 0 ? 1 : 0;

    if (dc > 0x3ff)
        dc = 0x3ff;

    if (dc < -0x3ff)
        dc = -0x3ff;

    if( dc < 0){
        dirR = 0;
        OCR1B = (-dc) & 0x3ff;
        //pwm2ab = (1 << PINB3);
    }else{
        dirR = 1;
        OCR1B = dc & 0x3ff;
        //pwm2ab = (1 << PINB4);
    }

    if( sign != last_sign){
        TIMSK1 |= (1 << OCIE1B);
    }
    last_sign = sign;
}
#endif
