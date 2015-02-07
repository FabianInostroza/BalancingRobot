#include "pid.h"

typedef struct _PID_PARAMS_F{
    float Kc;
    float Ki;
    float Kd;
    float T0;
    float accInt;
    float cv;
    float max_cv;
}pid_Params_f;

typedef struct _PID_PARAMS_INT16{
    int16_t Kc;
    int16_t Ki;
    int16_t Kd;
    int16_t T0;
    int16_t accInt;
    int16_t cv;
    int16_t max_cv;
}pid_Params_int16;

float pid_loop_f(pid_Params_f * pid, float err)
    ek_0 = sp - yk_0 + yk_i;
    pidLev.accInt += pidLev.Ki*ek_0;
    if(pidLev.accInt > MAX_PWM){
        pidLev.accInt = MAX_PWM;
    }
    #ifdef DIFF_PV
    #warning derivada en PV
    uk_0 =  pidLev.Kc*ek_0 + pidLev.accInt + pidLev.Kd*(yk_1 - yk_0);
    #elif defined(DIFF_ERR)
    #warning derivada en error
    uk_0 = pidLev.Kc*ek_0 + pidLev.accInt + pidLev.Kd*(ek_0 - ek_1);
    #else
    #error Defina una forma de aproximacion a la derivada
    #endif // DIFF_ERR
    if(uk_0 > MAX_PWM){
        OCR1A = MAX_PWM;
        uk_0 = MAX_PWM;
        LED2_ON();
        LED1_OFF();
    }else if(uk_0 < 0){
        OCR1A = 0;
        uk_0 = 0;
        LED1_ON();
        LED2_OFF();
    }else{
        OCR1A = (uint16_t)uk_0;
        LED1_OFF();
        LED2_OFF();
    }

    #ifdef DIFF_PV
    yk_1 = yk_0;
    #elif defined(DIFF_ERR)
    ek_1 = ek_0;
    #endif // DIFF_ERR
}

void updatePIDParams_f(pid_Params_f * pid, float kc, float Ti, float Td, float T0){
    pid->Kc = kc*Kkc;
    if(Ti == 0){
        pid->Ki = 0;
    }else{
        pid->Ki = pid->Kc*T0/(2*Ti*KTi);
    }
    pid->Kd = pid->Kc*Td*KTd/T0;
}

void initPIDParams_f(uint8_t kc,uint8_t Ti,uint8_t Td, volatile pid_Params_f * pid){
    pid->accInt = 0;
    updatePIDParams(kc,Ti,Td,pid);
}
