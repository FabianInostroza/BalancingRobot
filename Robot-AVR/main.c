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

static volatile uint8_t data_ready = 0;
static volatile uint8_t update_ks = 0;
static volatile int16_t kpid = 0;

ISR(USART0_RX_vect)
{
    char c = 0;
    static uint16_t tmp = 0;
    static uint8_t st = 0, st2 = 0;
    c = UDR0;
    switch(st){
        case 0:
            switch(c){
                case 'p':
                case 'P':
                    st2 = 1;
                    break;
                case 'i':
                case 'I':
                    st2 = 2;
                    break;
                case 'd':
                case 'D':
                    st2 = 3;
                    break;
                case ':':
                case '\n':
                    st = 1;
                    tmp = 0;
                    break;
                default:
                    break;
            }
            UDR0 = c;
            break;
        case 1:
            if (c >= '0' && c <= '9'){
                tmp = tmp*16 + c - '0';
                UDR0 = c;
            }else
            if ( c >= 'a' && c <= 'f'){
                tmp = tmp*16 + c - 'a' + 10;
                UDR0 = c;
            }else
            if ( c >= 'A' && c <= 'F'){
                tmp = tmp*16 + c - 'A' + 10;
                UDR0 = c;
            }else
            if ( c == ':' || c == '\n'){
                st = 0;
                update_ks = st2;
                st2 = 0;
                kpid = tmp;
                UDR0 = '\n';
            }
            break;
        default:
            break;
    }
}

ISR(INT2_vect)
{
    data_ready = 1;
}

int main(void)
{
    int16_t mpu_buf[6];
    char buf[30];
    int16_t pwm, pwmb;
    uint8_t err;
    const float alpha = 0.02;
    const float t0 = 1.0/200.0;
    const float gyro_sens = 1.0/131.0;
    const float gyro_k = gyro_sens*t0; // +/-250 deg/s
    //const float gyro_k = 1/65.5*t0; // +/-500 deg/s
//    const float gyro_k = 1/32.8*t0; // +/-1000 deg/s
    float tilt = 90, tilt_r = 90, derror = 0;
    int16_t kp = -600, ki = 0, kd = 2;
    float error = 0;
    uint8_t tmp;

    DDRB = (1 << PIN0);

    err = setupMPU6050(0x68);

    EICRA = (1 << ISC21); // interrupcion INT2 falling edge
    EIMSK = (1 << INT2); // activar interrupcion INT2

    setup_pwm();
    setupUART0(1, 1);
    UART0_enRxInt(1);

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

            err |= mpu6050_burstReadWord(0x68, MPU6050_RA_FIFO_R_W, mpu_buf, 4);

            tilt_r = atan2(mpu_buf[1], mpu_buf[2])*180/M_PI;

            tilt = (1.0-alpha)*(tilt + mpu_buf[3]*gyro_k) + alpha*tilt_r;

            derror = 0 - mpu_buf[3]*gyro_sens;

            tilt_r = tilt;

            if (tilt_r >= 5) {
                tilt_r = 5;
            }
            if ( tilt_r <= -5)
                tilt_r = -5;

            if ( tilt >= 60.0 || tilt <= -60.0){
                pwm = 0;
            }else{
                error = 0 - tilt_r;
                pwm = kp*error + kd*derror;
            }

            if (pwm > 0x3ff)
                pwm = 0x3ff;

            if (pwm < -0x3ff)
                pwm = -0x3ff;

            pwmb = pwm;

            if ( pwm < 0 ){
                OCR1A = -pwm; // B-IA
                OCR1B = 0; // B-IB

                OCR3A = -pwmb; // A-IA
                OCR3B = 0; // A-IB
            }else{
                OCR1A = 0; // B-IA
                OCR1B = pwm; // B-IB

                OCR3A = 0; // A-IA
                OCR3B = pwmb; // A-IB
            }

#ifdef ENVIAR_DATOS
//            UART0_send_hex16(mpu_buf[1]);
//            UART0_Tx('\t');
//            UART0_send_hex16(mpu_buf[2]);
//            UART0_Tx('\t');
//            UART0_send_hex16(mpu_buf[3]);
//            UART0_Tx('\t');
            sprintf(buf, "%.2f\t%.2f\n", derror, tilt);
////            sprintf(buf, "%i\t%.2X\n", mpu_buf[3], err);
            UART0_sends(buf);
#endif
            //UART0_sends("hah\n");
        }

        switch(update_ks){
            case 1:
                kp = kpid;
                update_ks = 0;
                sprintf(buf, "%i\t%i\t%i\n", kp, ki, kd);
                UART0_sends(buf);
                break;
            case 2:
                ki = kpid;
                update_ks = 0;
                sprintf(buf, "%i\t%i\t%i\n", kp, ki, kd);
                UART0_sends(buf);
                break;
            case 3:
                kd = kpid;
                update_ks = 0;
                sprintf(buf, "%i\t%i\t%i\n", kp, ki, kd);
                UART0_sends(buf);
                break;
            default:
                break;
        }
        wdt_reset();
        //_delay_ms(1);
    }

    return 0;
}
