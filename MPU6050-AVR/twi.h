#ifndef TWI_H_INCLUDED
#define TWI_H_INCLUDED

#include <avr/io.h>

void setupTWI(void);
uint8_t twi_start(uint8_t addr, uint8_t w);
void twi_stop(void);
inline uint8_t twi_write(uint8_t d);
uint8_t twi_send(uint8_t sl_addr, uint8_t * data, uint8_t n);
inline uint8_t twi_read(uint8_t);


#endif // TWI_H_INCLUDED
