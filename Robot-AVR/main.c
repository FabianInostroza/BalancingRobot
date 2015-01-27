#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <math.h>

#include "twi.h"
#include "UART.h"
#include "MPU6050.h"
#include "pwm.h"

_Accum short test_asd;

static volatile uint8_t data_ready;

ISR(INT2_vect)
{
    data_ready = 1;
}

int main(void)
{
    int16_t mpu_buf[6];
    char buf[20];
    int16_t pwmA, pwmB;
    uint8_t err;
    const float alpha = 0.02;
    const float t0 = 1.0/200.0;
//    const float gyro_k = 1/65.5*t0; // +/-500 deg/s
    const float gyro_k = 1/32.8*t0; // +/-1000 deg/s
    float tilt = 0, tilt_r = 0;

    DDRB = (1 << PIN0);

    err = setupMPU6050(0x68);

    EICRA = (1 << ISC21); // interrupcion INT2 falling edge
    EIMSK = (1 << INT2); // activar interrupcion INT2

    setup_pwm();
    setupUART0(1, 0);

    sei();

    //PINC |= (1 << PIN0) | (1 << PIN1);
    PORTB |= (1 << PIN0);

    while(1){
        if( data_ready ){
            data_ready = 0;
//            mpu6050_readReg(0x68, MPU6050_RA_FIFO_COUNTH, &tmp);
//            UART0_send_hex8(tmp);
//            mpu6050_readReg(0x68, MPU6050_RA_FIFO_COUNTL, &tmp);
//            UART0_send_hex8(tmp);
//            if( mpu6050_burstRead(0x68, MPU6050_RA_FIFO_R_W, i2c_buf, 12) )
            if( mpu6050_burstReadWord(0x68, MPU6050_RA_FIFO_R_W, mpu_buf, 6) )
                UART0_sends("error r\n");

//            for (i = 0; i < 6; i++){
//                UART0_send_hex16(mpu_buf[i]);
//                UART0_Tx(';');
//            }
//            UART0_Tx('\n');

            tilt_r = atan2f(mpu_buf[1], mpu_buf[2])*180/M_PI;

            tilt = (1.0-alpha)*(tilt + mpu_buf[3]*gyro_k) + alpha*tilt_r;

            if (tilt >= 5){
                tilt = 5;
            }
            if ( tilt <= -5)
                tilt = -5;

            pwmA = 0x1ff + /* tilt*120 + */ mpu_buf[3]/4;

            if (pwmA >= 0x3ff){
                pwmA = 0x3fe;
            }
            if (pwmA < 0){
                pwmA = 1;
            }

            pwmA &= 0x3ff;

            OCR1A = pwmA; //IN1
            OCR1B = pwmA; //IN2

            OCR3A = pwmA; //IN4
            OCR3B = pwmA; //IN3

            sprintf(buf, "%.2f\t%.4X\n", tilt, pwmA);
            UART0_sends(buf);
        }
    }

    return 0;
}
