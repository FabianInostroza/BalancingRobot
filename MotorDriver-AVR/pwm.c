#include "pwm.h"

void setup_pwm(void)
{
    DDRD |= (1 << PIN4) | (1 << PIN5);
    // phase and frequency correct pwm
    // borrar OC1A en compare match en subida, fijar OC1A en compare match en bajada
    // OC1B invertida
    TCCR1A = (1 << COM1A1) | (0 << COM1A0) | (1 << COM1B1) | (1 << COM1B0);
    // reloj = clk/1
    TCCR1B = (1 << WGM13) | (0 << CS12) | (1 << CS10);

    ICR1 = 0x3FF;

    OCR1A = 0x200;
    OCR1B = 0x200;
}
