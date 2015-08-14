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
#include "spi.h"
#include "XL7105.h"

//#define ENVIAR_DATOS
//#define USAR_XL7105

#ifndef __AVR_ATmega328P__
#define PIN_LED PIN0
#else
#define PIN_LED PIN5
#endif // __AVR_ATmega328P__

static volatile uint8_t data_ready = 0;
static volatile uint8_t update_ks = 0;
static volatile int16_t kpid = 0;
static volatile uint8_t enviar_datos = 0;

typedef union{
    struct{
        uint8_t ID;
        struct{
            uint8_t left:1;
            uint8_t down:1;
            uint8_t right:1;
            uint8_t up:1;
            uint8_t start:1;
            uint8_t R3:1;
            uint8_t L3:1;
            uint8_t select:1;
            // 2do byte
            uint8_t square:1;
            uint8_t X:1;
            uint8_t circle:1;
            uint8_t triangle:1;
            uint8_t R1:1;
            uint8_t L1:1;
            uint8_t R2:1;
            uint8_t L2:1;
        }buttons;
        struct {
            int8_t RX;
            int8_t RY;
            int8_t LX;
            int8_t LY;
        }analog;
    }PS2;
    uint8_t ubytes[7];
    int8_t sbytes[7];
}PS2_joy;

#ifndef __AVR_ATmega328P__
ISR(USART0_RX_vect)
#else
ISR(USART_RX_vect)
#endif
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
                case 'O':
                case 'o':
                    st2 = 5;
                    break;
                case 'T':
                    enviar_datos = 1;
                    break;
                case 't':
                    enviar_datos = 0;
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

ISR(INT0_vect)
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
    int16_t pwm, pwmb=0, pwma = 0, pwm_cmp = 0;
    int16_t pwm_offset = 0;
    uint8_t err;
    uint8_t init = 1;
    pid_Params_f pid;
    //float kc = -300, ti = 0.1, td = 0.1; // con 2*3.7v
    //float kc = -170, ti = 0.1, td = 0.135; // con 3*3.7v
    // con 3*3.7v y nueva config mecanica, 2piso mas bajo
    // y placa de terciado en la parte superior
    float kc = -207, ti = 0.04, td = 0.007;
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
    float error = 0;
    float sp_tilt = 0;

    #ifdef ENVIAR_DATOS
    char buf[30];
    #endif

    #ifdef USAR_XL7105
    uint8_t xl7105_ok = 0;
    uint8_t xl7105_cnt = 0;
    uint8_t reset_sps = 1;
    setup_spi();

    xl7105_ok = setup_xl7105();

    PS2_joy joy;
    uint8_t modo_joy = 2;
    #else
    uint8_t xl7105_ok = 0;
    #endif // USAR_XL7105

    DDRB = (1 << PIN_LED);

    // activar el watchdog
    // despues se reconfigura para
    // un tiempo mas pequeno
    wdt_enable(WDTO_2S);

    err = setupMPU6050(0x68);

    wdt_disable();

    // si falla la inicializacion no continuar
    // y parpadear el led
    if( err ){
        setupUART0(1,0);
        sei();
        UART0_sends("Error: ");
        UART0_send_hex8(err);
        UART0_Tx('\n');
        while(1){
            PINB = (1 << PIN_LED);
            _delay_ms(200);
        }
    }

    #ifndef __AVR_ATmega328P__
    DDRD &= ~(1 << PIN2);

    EICRA = (1 << ISC01); // interrupcion INT0 falling edge
    EIMSK = (1 << INT0); // activar interrupcion INT0
    #else
    #warning "configurar interrupcion de IMU"
    #endif // __AVR_ATmega328P__

    setup_pwm();

    // activar el watchdog
    wdt_enable(WDTO_250MS);

    setupUART0(1, 1);
    UART0_enRxInt(1);

    sei();

    //PINC |= (1 << PIN0) | (1 << PIN1);
    if ( err == 0 && xl7105_ok == 0)
        PORTB |= (1 << PIN_LED);

    UART0_sends("Holaa\n");
    initPIDParams_f(&pid, kc, ti, td, 0, 0, 0x3ff, 0x3ff, t0);
    err = 0;

    // para no leer basura a la primera
    mpu6050_resetFifo(0x68);
    data_ready = 0;

    while(1) {
        if( data_ready ){
            data_ready = 0;

            err = mpu6050_burstReadWord(0x68, MPU6050_RA_FIFO_R_W, mpu_buf, 4);

            tilt_r = atan2f(mpu_buf[1], mpu_buf[2])*180/M_PI;
            //tilt_r = mpu_buf[1]*180/M_PI;

            if( init ){
                if( err == 0 )
                    init = 0;
                tilt = tilt_r;
            }else{
                tilt = (1.0-alpha)*(tilt + mpu_buf[3]*gyro_k) + alpha*tilt_r;

                derror = 0 - mpu_buf[3]*gyro_sens;

                tilt_r = tilt;

                if (tilt_r >= 10) {
                    tilt_r = 10;
                }
                if ( tilt_r <= -10)
                    tilt_r = -10;

                if ( tilt >= 40.0 || tilt <= -40.0){
                    pwm = 0;
                    pwm_cmp = 0;
                }else{
                    error = sp_tilt - tilt_r;

                    pwm = pid_loop_robot(&pid, error, derror);
                    //pwm_cmp = deadBand_comp(pwm);
                    pwm_cmp = pwm;
                }

                pwma = 0.92*(pwm_cmp + pwm_offset);
                pwmb = pwm_cmp - pwm_offset;

//                if (pwma > 0x3ff)
//                    pwma = 0x3ff;
//
//                if (pwma < -0x3ff)
//                    pwma = -0x3ff;
//
//                if (pwmb > 0x3ff)
//                    pwmb = 0x3ff;
//
//                if (pwmb < -0x3ff)
//                    pwmb = -0x3ff;

                pwmL_duty(pwma);
                pwmR_duty(pwmb);

            }

            #ifdef ENVIAR_DATOS
            if( enviar_datos){
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
            }
            #endif

            #ifdef USAR_XL7105
            if ( xl7105_ok == 0){
                if( xl7105_rx(joy.ubytes, 7, 1) ){
                    xl7105_cnt = 0;
                    if( modo_joy == 0){ // solo joystick izquierdo
                        if ( joy.PS2.buttons.R1 ) // boton no presionado
                            pwm_offset = joy.PS2.analog.LX*3;
                        else
                            pwm_offset = joy.PS2.analog.LX*5;
                        if ( joy.PS2.buttons.L1 ) // boton no presionado
                            sp_tilt = joy.PS2.analog.LY*0.013;
                        else
                            sp_tilt = joy.PS2.analog.LY*0.018;
                    }else if( modo_joy == 1){ // solo joystick derecho
                        if ( joy.PS2.buttons.R1 ) // boton no presionado
                            pwm_offset = joy.PS2.analog.RX*3;
                        else
                            pwm_offset = joy.PS2.analog.RX*5;
                        if ( joy.PS2.buttons.L1 ) // boton no presionado
                            sp_tilt = joy.PS2.analog.RY*0.013;
                        else
                            sp_tilt = joy.PS2.analog.RY*0.018;
                    }else{ // izq adelante/atras, derecho izq/derecha
                        if ( joy.PS2.buttons.R1 ) // boton no presionado
                            pwm_offset = joy.PS2.analog.RX*3;
                        else
                            pwm_offset = joy.PS2.analog.RX*5;
                        if ( joy.PS2.buttons.L1 ) // boton no presionado
                            sp_tilt = joy.PS2.analog.LY*0.013;
                        else
                            sp_tilt = joy.PS2.analog.LY*0.018;
                    }

                    if ( joy.PS2.buttons.X == 0 ){
                        if ( joy.PS2.buttons.L2 == 0 && joy.PS2.buttons.R2){
                            modo_joy = 0;
                        }
                        if ( joy.PS2.buttons.L2 && joy.PS2.buttons.R2 == 0){
                            modo_joy = 1;
                        }
                        if ( joy.PS2.buttons.L2 == 0 && joy.PS2.buttons.R2 == 0){
                            modo_joy = 2;
                        }
                    }

                    reset_sps = 1;
                    xl7105_cnt = 0;
                }else{
                    xl7105_cnt++;
                    // si no se recibe paquetes de datos
                    // mas de 40 veces (aprox 0.2s en un loop de 200Hz)
                    // , asumir que el mando
                    // esta apagado o fuera de alcance y
                    // resetear los setpoint y offset
                    if (xl7105_cnt >= 39){
                        if( reset_sps ){
                            sp_tilt = 0;
                            pwm_offset = 0;
                            reset_sps = 0;
                        }
                    }
                }
            }
            #endif // USAR_XL7105
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
                case 5:
                    pwm_offset = kpid;
                    break;
                default:
                    break;
            }
            updatePIDParams_f(&pid, kc, ti, td);
//            sprintf(buf, "%i\t%i\t%i\n", kp, ki, kd);
            #ifdef ENVIAR_DATOS
            if( enviar_datos ){
                sprintf(buf, "%.2f\t%.2f\t%.2f\n", pid.Kc, pid.Ki, pid.Kd_r);
                UART0_sends(buf);
            }
            #endif
            update_ks = 0;
        }
        wdt_reset();
    }

    return 0;
}
