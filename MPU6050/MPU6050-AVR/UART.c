#include "UART.h"
#include <avr/io.h>
#include "UART_config.h"
#include <util/setbaud.h>

void setupUART0(uint8_t enTX, uint8_t enRX)
{
    #ifndef __AVR_ATmega328P__
    PRR0 &= ~(1 << PRUSART0);
    #endif // __AVR_ATmega328P__
    UBRR0H = UBRRH_VALUE;
    UBRR0L = UBRRL_VALUE;
    #if USE_2X
    UCSR0A |= (1 << U2X0);
    #else
    UCSR0A &= ~(1 << U2X0);
    #endif

    if (enTX){
        UCSR0B |= (1 << TXEN0);
    }else{
        UCSR0B &= ~(1 << TXEN0);
    }

    if (enRX){
        UCSR0B |= (1 << RXEN0);
    }else{
        UCSR0B &= ~(1 << RXEN0);
    }

    #ifdef USE_RX_INTERRUPT
    UART0_enRxInt(1);
    #endif // USE_RX_INTERRUPT

    #ifdef USE_TX_INTERRUPT
    UART0_enTxInt(1);
    #endif
}

inline void UART0_enRxInt(uint8_t en)
{
    if (en){
        UCSR0B |= (1 << RXCIE0);
    }else{
        UCSR0B &= ~(1 << RXCIE0);
    }
}

inline void UART0_enTxInt(uint8_t en)
{
    if (en){
        UCSR0B |= (1 << TXCIE0);
    }else{
        UCSR0B &= ~(1 << TXCIE0);
    }
}

void UART0_Tx(char c)
{
    #ifndef USE_TX_INTERRUPT
    while( !(UCSR0A & 1 << UDRE0));
    UDR0 = c;
    #else
    #error "No implementado"
    #endif
}

void UART0_sends(char * s)
{
    while(*s){
        UART0_Tx(*s++);
    }
}

void UART0_send_hex8(uint8_t d)
{
    UART0_send_hex(d >> 4);
    UART0_send_hex(d);
}

void UART0_send_hex16(uint16_t d)
{
    UART0_send_hex(d >> 12);
    UART0_send_hex(d >> 8);
    UART0_send_hex(d >> 4);
    UART0_send_hex(d);
}

void UART0_send_hex32(uint32_t d)
{
    UART0_send_hex16(d >> 16);
    UART0_send_hex16(d);
}

void UART0_send_hex(uint8_t h)
{
    h = h & 0xF;
    if ( h >= 10 ){
        UART0_Tx('A'+h-10);
    }else{
        UART0_Tx('0' + h);
    }
}
