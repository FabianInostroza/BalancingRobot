#include "twi.h"
#include "MPU6050.h"

#include <util/delay.h>

uint8_t setupMPU6050(uint8_t addr)
{
    uint8_t err;
    setupTWI();
    err = mpu6050_writeReg(addr, MPU6050_RA_PWR_MGMT_1, 0);
    _delay_ms(1);
    err = mpu6050_writeReg(addr, MPU6050_RA_CONFIG, 1);
    //err = mpu6050_writeReg(addr, MPU6050_RA_SMPLRT_DIV, 3); // 250Hz
    err = mpu6050_writeReg(addr, MPU6050_RA_SMPLRT_DIV, 4); // 200Hz
    //err = mpu6050_writeReg(addr, MPU6050_RA_SMPLRT_DIV, 19); // 50Hz
    // activar pruebas del giroscopio, rango = +/-250 deg/s
//    err = mpu6050_writeReg(addr, MPU6050_RA_GYRO_CONFIG, 0);
    err = mpu6050_writeReg(addr, MPU6050_RA_GYRO_CONFIG, 0x80);// +/- 500 deg/s
    // activar pruebas del acelerometro, rango = +/-2g
    err = mpu6050_writeReg(addr, MPU6050_RA_ACCEL_CONFIG, 0);
    // activar fifo para giroscopio y acelerometro
    err = mpu6050_writeReg(addr, MPU6050_RA_FIFO_EN, 0x78);
    // configuracion del pin de interrupcion
    // active high, push-pull, pulso de 50us, flag se borra en cualquier lectura
    err = mpu6050_writeReg(addr, MPU6050_RA_INT_PIN_CFG, 0x10);
    // activar interrupcion en nuevos datos
    err = mpu6050_writeReg(addr, MPU6050_RA_INT_ENABLE, 0x01);
    err = mpu6050_writeReg(addr, MPU6050_RA_USER_CTRL, 0x44);
    return err;
    // agregar mas inicializacion
}

uint8_t mpu6050_readReg(uint8_t addr, uint8_t reg, uint8_t * d)
{
    uint8_t err;

    err = twi_start(addr, 1);
    if ( err )
        return err;

    err = twi_write(reg);
    if ( err )
        return err;

    _delay_ms(1);

    err = twi_start(addr, 0);
    if ( err )
        return err;

    *d = twi_read(0);
    twi_stop();
    return 0;
}

uint8_t mpu6050_writeReg(uint8_t addr, uint8_t reg, uint8_t d)
{
    uint8_t err;
    err = twi_start(addr, 1);
    if( err )
        return err;

    err = twi_write(reg);
    if( err )
        return 2*err;

    err = twi_write(d);
    if( err )
        return 3*err;

    twi_stop();

    return 0;
}

uint8_t mpu6050_burstRead(uint8_t addr, uint8_t reg, uint8_t * data, uint8_t n)
{
    uint8_t err, i;
    err = twi_start(addr, 1);
    if (err)
        return err;

    err = twi_write(reg);
    if (err)
        return err;

    err = twi_start(addr, 0);
    if ( err )
        return err;

    n--;
    for ( i = 0; i < n; i++){
        data[i] = twi_read(1);
    }

    data[n] = twi_read(0);

    twi_stop();
    return 0;
}
