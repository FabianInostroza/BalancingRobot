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
#include "pid.h"

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
                case 's':
                case 'S':
                    st2 = 4;
                    break;
                case ':':
                case '\n':
                    st = 1;
                    tmp = 0;
                    break;
                default:
                    break;
            }
            //UDR0 = c;
            break;
        case 1:
            if (c >= '0' && c <= '9'){
                tmp = tmp*16 + c - '0';
                //UDR0 = c;
            }else
            if ( c >= 'a' && c <= 'f'){
                tmp = tmp*16 + c - 'a' + 10;
                //UDR0 = c;
            }else
            if ( c >= 'A' && c <= 'F'){
                tmp = tmp*16 + c - 'A' + 10;
                //UDR0 = c;
            }else
            if ( c == ':' || c == '\n'){
                st = 0;
                update_ks = st2;
                st2 = 0;
                kpid = tmp;
                //UDR0 = '\n';
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

inline int16_t deadBand_comp(int16_t x)
{
    if(x < 0 )
        return x - 100;
    else
        return x + 100;
}


int main(void)
{
    int16_t mpu_buf[6];
    char buf[30];
    int16_t pwm, pwmb, pwm_cmp = 0;
    uint8_t err;
    uint8_t init = 1;
    pid_Params_f pid;
    float kc = -300, ti = 0.1, td = 0.1;
    // alpha = wc/(1/T0+wc)
    // wc = 1/T0*alpha/(1-alpha) = f0*alpha/(1-alpha)
    // wc = 200*0.02/0.98 = 4.08 rad/s = 0.64 Hz
    const float alpha = 0.02;
    const float t0 = 1.0/200.0;
    const float gyro_sens = 1.0/131.0; // +/-250 deg/s
    //const float gyro_sens = 1/65.5; // +/-500 deg/s
    //const float gyro_sens = 1/32.8; // +/-1000 deg/s
    const float gyro_k = gyro_sens*t0; // pal filtro complementario
    float tilt = 90, tilt_r = 90, derror = 0;
    int16_t kp = -300, ki = 0.1, kd = 0.1;
    float error = 0;
    float sp_tilt = 0;
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
    initPIDParams_f(&pid, kc, ti, td, 0, 0, 0x3ff, 0x3ff, t0);
    err = 0;

    while(1) {
        if( data_ready ){
            data_ready = 0;

            err |= mpu6050_burstReadWord(0x68, MPU6050_RA_FIFO_R_W, mpu_buf, 4);

            tilt_r = atan2(mpu_buf[1], mpu_buf[2])*180/M_PI;

            if( init ){
                init = 0;
                tilt = tilt_r;
            }else{
                tilt = (1.0-alpha)*(tilt + mpu_buf[3]*gyro_k) + alpha*tilt_r;

                derror = 0 - mpu_buf[3]*gyro_sens;

                tilt_r = tilt;

                if (tilt_r >= 5) {
                    tilt_r = 5;
                }
                if ( tilt_r <= -5)
                    tilt_r = -5;

                if ( tilt >= 30.0 || tilt <= -30.0){
                    pwm = 0;
                    pwm_cmp = 0;
                }else{
                    error = sp_tilt - tilt_r;
//                    pwm = kp*error + kd*derror;
                    pwm = pid_loop_robot(&pid, error, derror);
                    pwm_cmp = deadBand_comp(pwm);
                    //pwm_cmp = pwm;  //deadBand_comp(pwm);
                }

                if (pwm_cmp > 0x3ff)
                    pwm_cmp = 0x3ff;

                if (pwm_cmp < -0x3ff)
                    pwm_cmp = -0x3ff;

                pwmb = 0.92*pwm_cmp;

                if ( pwm_cmp < 0 ){
                    OCR1A = -pwm_cmp; // B-IA
                    OCR1B = 0; // B-IB

                    OCR3A = -pwmb; // A-IA
                    OCR3B = 0; // A-IB
                }else{
                    OCR1A = 0; // B-IA
                    OCR1B = pwm_cmp; // B-IB

                    OCR3A = 0; // A-IA
                    OCR3B = pwmb; // A-IB
                }

    #ifdef ENVIAR_DATOS
                UART0_Tx(':');
                UART0_send_hex16(pwm_cmp);
                UART0_Tx('\t');
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
        }

        if (update_ks){
            switch(update_ks){
                case 1:
//                    kp = kpid;
                    kc = kpid;
                    break;
                case 2:
//                    ki = kpid;
                    ti = kpid*0.01;
                    break;
                case 3:
//                    kd = kpid;
                    td = kpid*0.001;
                    break;
                case 4:
                    sp_tilt = kpid*0.01;
                    break;
                default:
                    break;
            }
            updatePIDParams_f(&pid, kc, ti, td);
//            sprintf(buf, "%i\t%i\t%i\n", kp, ki, kd);
            sprintf(buf, "%.2f\t%.2f\t%.2f\n", pid.Kc, pid.Ki, pid.Kd_r);
            UART0_sends(buf);
            update_ks = 0;
        }
        wdt_reset();
    }

    return 0;
}
