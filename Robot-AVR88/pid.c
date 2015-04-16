#include "pid.h"

float pid_loop_f(pid_Params_f * pid, float err)
{
    float uk_1 = pid->uk;
    if ( pid->max_error > 0 ){
        if ( err > pid->max_error )
            err = pid->max_error;
        if ( err < -pid->max_error )
            err = -pid->max_error;
    }

    pid->accInt += pid->Ki*(err + pid->ek_1);

    if ( pid->max_int > 0){
        if ( pid->accInt > pid->max_int)
            pid->accInt = pid->max_int;
        if ( pid->accInt < -pid->max_int)
            pid->accInt = -pid->max_int;
    }
    pid->uk = pid->Kc*err  + pid->accInt + pid->Kd*(err - pid->ek_1);

    if( pid->max_duk > 0 ){
        if ( (pid->uk - uk_1) > pid->max_duk)
            pid->uk = uk_1 + pid->max_duk;
        if ( (pid->uk - uk_1) < -pid->max_duk)
            pid->uk = uk_1 - pid->max_duk;
    }

    if ( pid->max_uk > 0 ){
        if ( pid->uk > pid->max_uk)
            pid->uk = pid->max_uk;
        if ( pid->uk < -pid->max_uk )
            pid->uk = -pid->max_uk;
    }
    pid->ek_1 = err;
    return pid->uk;
}

float pid_loop_robot(pid_Params_f * pid, float ek, float dek)
{
    float uk_1 = pid->uk;
    if ( pid->max_error > 0 ){
        if ( ek > pid->max_error )
            ek = pid->max_error;
        if ( ek < -pid->max_error )
            ek = -pid->max_error;
    }

    pid->accInt += pid->Ki*(ek + pid->ek_1);

    if ( pid->max_int > 0){
        if ( pid->accInt > pid->max_int)
            pid->accInt = pid->max_int;
        if ( pid->accInt < -pid->max_int)
            pid->accInt = -pid->max_int;
    }
    pid->uk = pid->Kc*ek  + pid->accInt + pid->Kd_r*dek;

    if( pid->max_duk > 0 ){
        if ( (pid->uk - uk_1) > pid->max_duk)
            pid->uk = uk_1 + pid->max_duk;
        if ( (pid->uk - uk_1) < -pid->max_duk)
            pid->uk = uk_1 - pid->max_duk;
    }

    if ( pid->max_uk > 0 ){
        if ( pid->uk > pid->max_uk)
            pid->uk = pid->max_uk;
        if ( pid->uk < -pid->max_uk )
            pid->uk = -pid->max_uk;
    }
    pid->ek_1 = ek;
    return pid->uk;
}

float pid_loop_f_vel(pid_Params_f_vel * pid, float sp, float yk)
{
    float duk;
    float ek = sp - yk;

    if (pid->max_ek > 0){
        if( ek > pid->max_ek){
            ek = pid->max_ek;
        }
        if( ek < -pid->max_ek){
            ek = -pid->max_ek;
        }
    }
    duk = pid->q0*ek + pid->q1*pid->ek_1 + pid->q2*pid->ek_2 + pid->p0*yk + pid->p1*pid->yk_1 + pid->p2*pid->yk_2;

    if (pid->max_duk > 0){
        if ( duk > pid->max_duk){
            duk = pid->max_duk;
        }
        if( duk < -pid->max_duk){
            duk = -pid->max_duk;
        }
    }

    pid->uk += duk;

    if( pid->max_uk > 0){
        if( pid->uk > pid->max_uk ){
            pid->uk = pid->max_uk;
        }

        if( pid->uk < -pid->max_uk ){
            pid->uk = -pid->max_uk;
        }
    }
    pid->ek_2 = pid->ek_1;
    pid->ek_1 = ek;

    pid->yk_2 = pid->yk_1;
    pid->yk_1 = yk;
    return pid->uk;
}

void updatePIDParams_f(pid_Params_f * pid, float kc, float Ti, float Td)
{
    pid->Kc = kc;
    if(Ti == 0){
        pid->Ki = 0;
    }else{
        pid->Ki = kc*pid->T0/(2*Ti);
    }
    pid->Kd = kc*Td/pid->T0;
    pid->Kd_r = Td*kc;
}


void updatePIDParams_f_vel(pid_Params_f_vel * pid, float kc, float Ti, float Td)
{
    // derivada en PV
    pid->q0 = kc*(1+pid->T0/(2*Ti));
    pid->q1 = kc*(pid->T0/(2*Ti) - 1);
    pid->q2 = 0;
    pid->p0 = -kc*Td/pid->T0;
    pid->p1 = 2*kc*Td/pid->T0;
    pid->p2 = pid->p0;
}

void initPIDParams_f(pid_Params_f * pid, float kc, float Ti, float Td, float max_du, float max_err, float max_u, float max_int, float T0)
{
    pid->accInt = 0;
    pid->ek_1 = 0;
    pid->T0 = T0;
    pid->max_duk = max_du;
    pid->max_error = max_err;
    pid->max_uk = max_u;
    pid->max_int = max_int;
    updatePIDParams_f(pid, kc,Ti,Td);
}

void initPIDParams_f_vel(pid_Params_f_vel * pid, float kc, float Ti, float Td, float max_du, float max_err, float max_u, float T0)
{
    pid->ek_1 = 0;
    pid->ek_2 = 0;
    pid->yk_1 = 0;
    pid->yk_2 = 0;
    pid->T0 = T0;
    pid->max_duk = max_du;
    pid->max_ek = max_err;
    pid->max_uk = max_u;
    updatePIDParams_f_vel(pid, kc,Ti,Td);
}
