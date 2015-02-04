#include "twi.h"
#include "MPU6050.h"

#include <util/delay.h>

uint8_t setupMPU6050(uint8_t addr)
{
    uint8_t err = 0;
    setupTWI();
    err = mpu6050_writeReg(addr, MPU6050_RA_PWR_MGMT_1, 0x80); // reset
    _delay_ms(20);
    err |= mpu6050_writeReg(addr, MPU6050_RA_PWR_MGMT_1, 0);
    _delay_ms(1);
    err |= mpu6050_writeReg(addr, MPU6050_RA_CONFIG, 1);
    //err = mpu6050_writeReg(addr, MPU6050_RA_SMPLRT_DIV, 3); // 250Hz
    err |= mpu6050_writeReg(addr, MPU6050_RA_SMPLRT_DIV, 4); // 200Hz
    //err |= mpu6050_writeReg(addr, MPU6050_RA_SMPLRT_DIV, 9); // 100Hz
    //err = mpu6050_writeReg(addr, MPU6050_RA_SMPLRT_DIV, 19); // 50Hz

    err |= mpu6050_writeReg(addr, MPU6050_RA_GYRO_CONFIG, 0x10);// +/- 1000 deg/s
    err |= mpu6050_gyroCal(addr);
    //err |= mpu6050_writeReg(addr, MPU6050_RA_GYRO_CONFIG, 0x08);// +/-500 deg/s
    err = mpu6050_writeReg(addr, MPU6050_RA_GYRO_CONFIG, 0); // +/-250 deg/s
    // activar pruebas del acelerometro, rango = +/-8g
    //err = mpu6050_writeReg(addr, MPU6050_RA_ACCEL_CONFIG, 0x10);
    err |= mpu6050_setAccelOffsets(0x68, (2), (-40), (1756-2048));
    err |= mpu6050_writeReg(addr, MPU6050_RA_ACCEL_CONFIG, 0x0); // +/-2g
    // activar fifo para giroscopios y acelerometro
    //err |= mpu6050_writeReg(addr, MPU6050_RA_FIFO_EN, 0x78);
    // activar fifo para giroscopio x y acelerometro
    //err |= mpu6050_writeReg(addr, MPU6050_RA_FIFO_EN, 0x48);
    // activar fifo para giroscopio x y acelerometro
    err |= mpu6050_writeReg(addr, MPU6050_RA_FIFO_EN, 0x48);
    // configuracion del pin de interrupcion
    // active high, push-pull, pulso de 50us, flag se borra en cualquier lectura
    err |= mpu6050_writeReg(addr, MPU6050_RA_INT_PIN_CFG, 0x10);
    // activar interrupcion en nuevos datos
    _delay_ms(1);
    err |= mpu6050_writeReg(addr, MPU6050_RA_INT_ENABLE, 0x01);
    err |= mpu6050_writeReg(addr, MPU6050_RA_USER_CTRL, 0x44);
    _delay_ms(2);
    // activar fifo
    err |= mpu6050_writeReg(addr, MPU6050_RA_USER_CTRL, 0x44);
    // fifo inactiva
    //err |= mpu6050_writeReg(addr, MPU6050_RA_USER_CTRL, 0x04);
    //err = mpu6050_setGyroOffsets(0x68, 45/2, 212/2, -2/2);
    return err;
    // agregar mas inicializacion
}

void mpu6050_getGyroOffsets(uint8_t addr, int16_t * gx, int16_t * gy, uint16_t * gz)
{

}

uint8_t mpu6050_gyroCal(uint8_t addr)
{
    int16_t gx;
    int16_t gy;
    int16_t gz;
    int32_t offsets[] = {0, 0, 0};
    uint16_t i;

    // descartar las primeras 50 muestras
    for( i = 0; i < 50; i++){
        mpu6050_readGyro(addr, &gx, &gy, &gz);
        _delay_ms(5);
    }

    for( i = 0; i < 0x100; i++){
        if( mpu6050_readGyro(addr, &gx, &gy, &gz) )
            return -1;
        offsets[0] += gx;
        offsets[1] += gy;
        offsets[2] += gz;
        _delay_ms(5);
    }

    offsets[0] /= 0x100;
    offsets[1] /= 0x100;
    offsets[2] /= 0x100;

    mpu6050_setGyroOffsets(addr, -gx, -gy, -gz);
    return 0;
}

uint8_t mpu6050_readGyro(uint8_t addr, int16_t * gx, int16_t * gy, int16_t * gz)
{
    uint8_t h, l;
    if( mpu6050_readReg(addr, MPU6050_RA_GYRO_XOUT_H, &h) )
        return -1;
    if( mpu6050_readReg(addr, MPU6050_RA_GYRO_XOUT_L, &l) )
        return -1;
    *gx = (h << 8) | l;

    if( mpu6050_readReg(addr, MPU6050_RA_GYRO_YOUT_H, &h) )
        return -1;
    if( mpu6050_readReg(addr, MPU6050_RA_GYRO_YOUT_L, &l) )
        return -1;
    *gy = (h << 8) | l;

    if( mpu6050_readReg(addr, MPU6050_RA_GYRO_ZOUT_H, &h) )
        return -1;
    if( mpu6050_readReg(addr, MPU6050_RA_GYRO_ZOUT_L, &l) )
        return -1;
    *gz = (h << 8) | l;
    return 0;
}

uint8_t mpu6050_setAccelOffsets(uint8_t addr, int16_t ax, int16_t ay, int16_t az)
{
    int16_t offsets[3];
    uint8_t err;
    if( (err = mpu6050_burstReadWord(addr, MPU6050_RA_XA_OFFS_H, offsets, 3) ) )
        return err;
//
//    UART0_send_hex16(offsets[0]);
//    UART0_Tx('\t');
//    UART0_send_hex16(offsets[1]);
//    UART0_Tx('\t');
//    UART0_send_hex16(offsets[2]);
//    UART0_Tx('\n');

    // es necesario conservar el bit0 de los offsets (datasheet).
    // suma en complemento a 2 es igual a suma comun por lo que
    // la operacion de abajo no modifica el bit 0
    offsets[0] -= (ax & ~1);
    offsets[1] -= (ay & ~1);
    offsets[2] -= (az & ~1);

//    offsets[0] &= 0x0001;
//    offsets[1] &= 0x0001;
//    offsets[2] &= 0x0001;

    if ( (err = mpu6050_burstWriteWord(addr, MPU6050_RA_XA_OFFS_H, offsets, 3) ) )
        return err;

}

uint8_t mpu6050_setGyroOffsets(uint8_t addr, int16_t gx, int16_t gy, int16_t gz)
{
    uint8_t err=0;
    uint8_t offsets[] = {(gx >> 8 & 0xff), gx & 0xff, (gy >> 8) & 0xff, \
                         gy & 0xff, (gz >> 8) & 0xff, gz & 0xff};
//    for (i=0; i<6;i++){
//        err |= mpu6050_writeReg(addr, MPU6050_RA_XG_OFFS_USRH + i, offsets[i]);
//    }
    err = mpu6050_burstWrite(addr, MPU6050_RA_XG_OFFS_USRH, offsets, 6);
    if (err)
        return err;
    return 0;
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

uint8_t mpu6050_burstReadWord(uint8_t addr, uint8_t reg, int16_t * data, uint8_t n)
{
    uint8_t err, i, j;
    err = twi_start(addr, 1);
    if (err)
        return err;

    err = twi_write(reg);
    if (err)
        return err;

    err = twi_start(addr, 0);
    if ( err )
        return err;

    j = 0;
    for ( i = 0; i < (2*n-1); i++){
        data[j] = (data[j] << 8) | twi_read(1);
        if (i & 0x01){
            j++;
        }
    }

    data[n-1] = ((data[n-1]) << 8) |twi_read(0);

    twi_stop();
    return 0;
}

uint8_t mpu6050_burstWrite(uint8_t addr, uint8_t reg, uint8_t * data, uint8_t n)
{
    uint8_t err, i;
    err = twi_start(addr, 1);
    if (err)
        return err;

    err = twi_write(reg);
    if (err)
        return err;

    for ( i = 0; i < n; i++){
        err = twi_write(data[i]);
        if ( err )
            return err;
    }

    twi_stop();
    return 0;
}

uint8_t mpu6050_burstWriteWord(uint8_t addr, uint8_t reg, int16_t * data, uint8_t n)
{
    uint8_t err, i, d;
    err = twi_start(addr, 1);
    if (err)
        return err;

    err = twi_write(reg);
    if (err)
        return err;

    for ( i = 0; i < n; i++){
        d = (data[i] >> 8) & 0xff;
        err |= twi_write(d);
        d = data[i] & 0xff;
        err |= twi_write(d);
        if ( err )
            return err;
    }

    twi_stop();
    return 0;
}
