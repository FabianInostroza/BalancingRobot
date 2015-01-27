#include "pwm.h"

void setup_pwm(void)
{
    DDRD |= (1 << PIN4) | (1 << PIN5);
    DDRB |= (1 << PIN6) | (1 << PIN7);
    // phase and frequency correct pwm, ~7.5kHz
    // borrar OC1A en compare match en subida, fijar OC1A en compare match en bajada
    // OC1B invertida
    //TCCR1A = (1 << COM1A1) | (0 << COM1A0) | (1 << COM1B1) | (1 << COM1B0);
    // reloj = clk/1
    //TCCR1B = (1 << WGM13) | (0 << CS12) | (1 << CS10);

    // fast pwm, ~15 kHz
    TCCR1A = (1 << COM1A1) | (0 << COM1A0) | (1 << COM1B1) | (1 << COM1B0) | (1 << WGM11);
    TCCR1B = (1 << WGM13) | (0 << CS12) | (1 << CS10) | (1 << WGM12) | (1 << WGM13);

    TCCR3A = (1 << COM3A1) | (0 << COM3A0) | (1 << COM3B1) | (1 << COM3B0) | (1 << WGM31);
    TCCR3B = (1 << WGM33) | (0 << CS32) | (1 << CS30) | (1 << WGM32) | (1 << WGM33);

    ICR1 = 0x3FF;
    ICR3 = 0x3FF;

    OCR1A = 0x1FF;
    OCR1B = 0x1FF;

    OCR3A = 0x1FF;
    OCR3B = 0x1FF;
}
