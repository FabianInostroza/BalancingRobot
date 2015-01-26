/*
 */

#include <avr/io.h>
#include "pwm.h"
#include <avr/interrupt.h>

ISR(ADC_vect)
{
    OCR1A = ADC;
    OCR1B = ADC;
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
    setup_adc();
    setup_pwm();
    sei();
    while(1)
    ;

    return 0;
}
