#ifndef PID_H_INCLUDED
#define PID_H_INCLUDED

#include <stdint.h>

// declaraciones incompletas (opaque pointers)
//struct _PID_PARAMS_F;
//typedef struct _PID_PARAMS_F pid_Params_f;
//typedef struct pid_Params_int16;
typedef struct _PID_PARAMS_F{
    float Kc;
    float Ki;
    float Kd;
    float T0;
    float td;
    float accInt;
    float uk;
    float max_uk;
    float ek_1;
    float max_error;
    float max_duk;
    float max_int;
}pid_Params_f;

typedef struct _PID_PARAMS_F_VEL{
    float q0;
    float q1;
    float q2;
    float p0;
    float p1;
    float p2;
    float T0;
    float uk;
    float ek_1;
    float ek_2;
    float yk_1;
    float yk_2;
    float max_uk;
    float max_ek;
    float max_duk;
}pid_Params_f_vel;

typedef struct _PID_PARAMS_INT16{
    int16_t Kc;
    int16_t Ki;
    int16_t Kd;
    int16_t T0;
    int16_t accInt;
    int16_t cv;
    int16_t max_cv;
}pid_Params_int16;

float pid_loop_f1(pid_Params_f * pid, float err);
float pid_loop_robot(pid_Params_f * pid, float ek, float dek);
float pid_loop_f_vel(pid_Params_f_vel * pid, float sp, float yk);
void updatePIDParams_f(pid_Params_f * pid, float kc, float Ti, float Td);
void updatePIDParams_f_vel(pid_Params_f_vel * pid, float kc, float Ti, float Td);
void initPIDParams_f(pid_Params_f * pid, float kc, float Ti, float Td, float max_du, float max_err, float max_um, float max_int, float T0);
void initPIDParams_f_vel(pid_Params_f_vel * pid, float kc, float Ti, float Td, float max_du, float max_err, float max_u, float T0);

#endif // PID_H_INCLUDED
