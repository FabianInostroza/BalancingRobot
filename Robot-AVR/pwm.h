#ifndef PWM_H_INCLUDED
#define PWM_H_INCLUDED

#include <avr/io.h>
#include <stdint.h>

void setup_pwm();
inline void pwmL_duty(int16_t d);
inline void pwmR_duty(int16_t d);

#endif // PWM_H_INCLUDED
