#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <math.h>
#include <avr/wdt.h>

#include "twi.h"
#include "UART.h"
#include "MPU6050.h"
#include "pwm.h"

#define ENVIAR_DATOS

//#include <avr/pgmspace.h>
//
//static const volatile uint32_t tabla[8191] PROGMEM = {[0 ... 8190] = 0xFFFFFFFF};
//static const volatile uint32_t tabla2[8191] PROGMEM = {[0 ... 8190] = 0xFFFFFFFF};

static volatile uint8_t data_ready;

ISR(INT2_vect)
{
    data_ready = 1;
}

int main(void)
{
    int16_t mpu_buf[6];
    char buf[30];
    int16_t pwm;
    uint8_t err;
    const float alpha = 0.02;
    const float t0 = 1.0/200.0;
    const float gyro_k = 1/131.0*t0; // +/-250 deg/s
    //const float gyro_k = 1/65.5*t0; // +/-500 deg/s
//    const float gyro_k = 1/32.8*t0; // +/-1000 deg/s
    float tilt = 0, tilt_r = 0;
    uint8_t tmp;

    DDRB = (1 << PIN0);

    err = setupMPU6050(0x68);

    EICRA = (1 << ISC21); // interrupcion INT2 falling edge
    EIMSK = (1 << INT2); // activar interrupcion INT2

    setup_pwm();
    setupUART0(1, 0);

    // activar el watchdog
    //WDTCSR = (1 << WDE) | (1 << WDP2) | (1 << WDP1) | (1 << WDP0);
    wdt_enable(WDTO_250MS);

    sei();

    //PINC |= (1 << PIN0) | (1 << PIN1);
    PORTB |= (1 << PIN0);

    UART0_sends("Holaa\n");
    err = 0;

    while(1) {
        if( data_ready ){
            data_ready = 0;
//        int_pin = (PINB & (1 << PIN2)) ? 1 : 0;
//        if ( int_pin == 0 && int_pin_1 == 1) {
//            mpu6050_readReg(0x68, MPU6050_RA_FIFO_COUNTH, &tmp);
//            UART0_send_hex8(tmp);
//            mpu6050_readReg(0x68, MPU6050_RA_FIFO_COUNTL, &tmp);
//            UART0_send_hex8(tmp);
//            if( mpu6050_burstRead(0x68, MPU6050_RA_FIFO_R_W, i2c_buf, 12) )

            err |= mpu6050_burstReadWord(0x68, MPU6050_RA_FIFO_R_W, mpu_buf, 4);

//            err |= mpu6050_readReg(0x68, MPU6050_RA_ACCEL_ZOUT_H, &tmp);
//            mpu_buf[1] = tmp << 8;
//            err |= mpu6050_readReg(0x68, MPU6050_RA_ACCEL_ZOUT_L, &tmp);
//            mpu_buf[1] |= tmp;
//
//            err |= mpu6050_readReg(0x68, MPU6050_RA_ACCEL_XOUT_H, &tmp);
//            mpu_buf[2] = tmp << 8;
//            err |= mpu6050_readReg(0x68, MPU6050_RA_ACCEL_XOUT_L, &tmp);
//            mpu_buf[2] |= tmp;
//
//            err |= mpu6050_readReg(0x68, MPU6050_RA_GYRO_YOUT_H, &tmp);
//            mpu_buf[3] = tmp << 8;
//            err |= mpu6050_readReg(0x68, MPU6050_RA_GYRO_YOUT_L, &tmp);
//            mpu_buf[3] |= tmp;



            //            for (i = 0; i < 6; i++){
            //                UART0_send_hex16(mpu_buf[i]);
            //                UART0_Tx(';');
            //            }
            //            UART0_Tx('\n');

            tilt_r = atan2(mpu_buf[2], mpu_buf[0])*180/M_PI;

            tilt = (1.0-alpha)*(tilt + mpu_buf[3]*gyro_k) + alpha*tilt_r;

            tilt_r = tilt;

            if (tilt_r >= 5) {
                tilt_r = 5;
            }
            if ( tilt_r <= -5)
                tilt_r = -5;

            pwm = 600*tilt_r;// + mpu_buf[3]/64;

            if ( tilt >= 60.0 || tilt <= -60.0)
                pwm = 0;

            if (pwm > 0x3ff)
                pwm = 0x3ff;

            if (pwm < -0x3ff)
                pwm = -0x3ff;

            if ( pwm < 0 ){
                OCR1A = -pwm; // B-IA
                OCR1B = 0; // B-IB

                OCR3A = -pwm; // A-IA
                OCR3B = 0; // A-IB
            }else{
                OCR1A = 0; // B-IA
                OCR1B = pwm; // B-IB

                OCR3A = 0; // A-IA
                OCR3B = pwm; // A-IB
            }

#ifdef ENVIAR_DATOS
//            UART0_send_hex16(mpu_buf[1]);
//            UART0_Tx('\t');
//            UART0_send_hex16(mpu_buf[2]);
//            UART0_Tx('\t');
//            UART0_send_hex16(mpu_buf[3]);
//            UART0_Tx('\t');
//            sprintf(buf, "%-5i\t%.2f\t%.2X\n",mpu_buf[3], tilt, err);
////            sprintf(buf, "%i\t%.2X\n", mpu_buf[3], err);
//            UART0_sends(buf);
#endif
            //UART0_sends("hah\n");
        }

        wdt_reset();
        //_delay_ms(1);
    }

    return 0;
}
