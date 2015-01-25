#ifndef UART_H_INCLUDED
#define UART_H_INCLUDED

#include <stdint.h>

void setupUART0(uint8_t enTX, uint8_t enRX);
void UART0_enRxInt(uint8_t en);
void UART0_enTxInt(uint8_t en);
void UART0_Tx(char c);
void UART0_sends(char * s);
void UART0_send_hex8(uint8_t);
void UART0_send_hex16(uint16_t);
void UART0_send_hex32(uint32_t d);
void UART0_send_hex(uint8_t);


#endif // UART_H_INCLUDED
