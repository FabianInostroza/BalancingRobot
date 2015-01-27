#include "UART.h"
#include <avr/io.h>
#include "UART_config.h"
#include <util/setbaud.h>

#ifdef USE_UART0_TX_INTERRUPT
#warning "Usando transmision por interrupciones"
#include <avr/interrupt.h>

// USART0_RX_vect

static char buf[TX_BUFFER_SIZE];
static volatile uint8_t tx_tail_pointer = 0;
static volatile uint8_t tx_head_pointer = 0;
static volatile uint8_t tx_buffer_full = 0;

ISR(USART0_UDRE_vect)
{
    UDR0 = buf[tx_head_pointer];
    tx_head_pointer = (tx_head_pointer+1)%TX_BUFFER_SIZE;

    // si se envio todo el buffer, desactivar
    // interrupcion en tx complete
    if( tx_head_pointer == tx_tail_pointer )
        UCSR0B &= ~(1 << UDRE0);
    tx_buffer_full = 0;
}
#endif

void setupUART0(uint8_t enTX, uint8_t enRX)
{
    PRR0 &= ~(1 << PRUSART0);
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

    #ifdef USE_UART0_RX_INTERRUPT
    UART0_enRxInt(1);
    #endif // USE_RX_INTERRUPT

    #ifdef USE_UART0_TX_INTERRUPT
    // activar cuando el buffer no esta vacio
    //UART0_enTxInt(1);
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
    #ifndef USE_UART0_TX_INTERRUPT
    while( !(UCSR0A & 1 << UDRE0));
    UDR0 = c;
    #else
    while( tx_buffer_full );
    buf[tx_tail_pointer] = c;
    UCSR0B &= ~(1 << UDRE0);
    tx_tail_pointer = (tx_tail_pointer+1)%TX_BUFFER_SIZE;
    if (tx_tail_pointer == tx_head_pointer )
        tx_buffer_full = 1;
    UCSR0B |= (1 << UDRE0);
    #endif
}

void UART0_sends(char * s)
{
    #ifndef USE_UART0_TX_INTERRUPT
    while(*s){
        while( !(UCSR0A & 1 << UDRE0));
        UDR0 = *s++;
    }
    #else
    while(*s){
        while( tx_buffer_full );
        buf[tx_tail_pointer] = *s++;
        UCSR0B &= ~(1 << UDRE0);
        tx_tail_pointer = (tx_tail_pointer+1)%TX_BUFFER_SIZE;
        if (tx_tail_pointer == tx_head_pointer )
            tx_buffer_full = 1;
        UCSR0B |= (1 << UDRE0);
    }
    #endif
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

inline void UART0_send_hex(uint8_t h)
{
    h = h & 0xF;
    if ( h >= 10 ){
        UART0_Tx('A'+h-10);
    }else{
        UART0_Tx('0' + h);
    }
}
