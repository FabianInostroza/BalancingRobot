#include "UART.h"
#include <avr/io.h>
#include "UART_config.h"
#include <util/setbaud.h>
#include <util/delay.h>

#if defined(USE_UART0_TX_INTERRUPT) || defined(USE_UART0_RX_INTERRUPT) || \
    defined(USE_UART1_TX_INTERRUPT) || defined(USE_UART1_RX_INTERRUPT)
#warning "Usando transmision por interrupciones"
#warning "Debes activar las interrupciones ejecutando sei()"
#include <avr/interrupt.h>
#endif

#ifdef USE_UART0_TX_INTERRUPT
// USART0_RX_vect

static char buf0[TX0_BUFFER_SIZE];
static volatile uint8_t tx0_tail_pointer = 0;
static volatile uint8_t tx0_head_pointer = 0;
static volatile uint8_t tx0_buffer_full = 0;

ISR(USART0_UDRE_vect)
{
    UDR0 = buf0[tx0_head_pointer];
    tx0_head_pointer = (tx0_head_pointer+1)%TX0_BUFFER_SIZE;

    // si se envio todo el buffer, desactivar
    // interrupcion en tx complete
    if( tx0_head_pointer == tx0_tail_pointer )
        UCSR0B &= ~(1 << UDRE0);
    tx0_buffer_full = 0;
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

    // 8bit, asincrono, 1 bit de parada, sin paridad
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);

    #ifdef USE_UART0_RX_INTERRUPT
    UART0_enRxInt(1);
    #endif // USE_RX_INTERRUPT

    #ifdef USE_UART0_TX_INTERRUPT
    // activar cuando el buffer no esta vacio
    //UART0_enTxInt(1);
    #endif

    // arregla problema con pl2303, buscar
    // otra forma de solucionarlo
    _delay_ms(1);

    #if defined(USE_UART0_TX_INTERRUPT) || defined(USE_UART0_RX_INTERRUPT)
    //sei();
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
    while( !( UCSR0A & (1 << UDRE0) ) );
    UDR0 = c;
    #else
    while( tx0_buffer_full );
    buf0[tx0_tail_pointer] = c;
    UCSR0B &= ~(1 << UDRE0);
    tx0_tail_pointer = (tx0_tail_pointer+1)%TX0_BUFFER_SIZE;
    if (tx0_tail_pointer == tx0_head_pointer )
        tx0_buffer_full = 1;
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
        while( tx0_buffer_full );
        buf0[tx0_tail_pointer] = *s++;
        UCSR0B &= ~(1 << UDRE0);
        tx0_tail_pointer = (tx0_tail_pointer+1)%TX0_BUFFER_SIZE;
        if (tx0_tail_pointer == tx0_head_pointer )
            tx0_buffer_full = 1;
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

/*******************************************************/
#ifdef USE_UART1_TX_INTERRUPT

// USART1_RX_vect

static char buf1[TX1_BUFFER_SIZE];
static volatile uint8_t tx1_tail_pointer = 0;
static volatile uint8_t tx1_head_pointer = 0;
static volatile uint8_t tx1_buffer_full = 0;

ISR(USART1_UDRE_vect)
{
    UDR1 = buf1[tx1_head_pointer];
    tx1_head_pointer = (tx1_head_pointer+1)%TX1_BUFFER_SIZE;

    // si se envio todo el buffer, desactivar
    // interrupcion en tx complete
    if( tx1_head_pointer == tx1_tail_pointer )
        UCSR1B &= ~(1 << UDRE1);
    tx1_buffer_full = 0;
}
#endif

void setupUART1(uint8_t enTX, uint8_t enRX)
{
    PRR0 &= ~(1 << PRUSART1);
    UBRR1H = UBRRH_VALUE;
    UBRR1L = UBRRL_VALUE;
    #if USE_2X
    UCSR1A |= (1 << U2X1);
    #else
    UCSR1A &= ~(1 << U2X1);
    #endif

    if (enTX){
        UCSR1B |= (1 << TXEN1);
    }else{
        UCSR1B &= ~(1 << TXEN1);
    }

    if (enRX){
        UCSR1B |= (1 << RXEN1);
    }else{
        UCSR1B &= ~(1 << RXEN1);
    }

    // 8bit, asincrono, 1 bit de parada, sin paridad
    UCSR1C = (1 << UCSZ11) | (1 << UCSZ10);

    #ifdef USE_UART1_RX_INTERRUPT
    UART1_enRxInt(1);
    #endif // USE_RX_INTERRUPT

    #ifdef USE_UART1_TX_INTERRUPT
    // activar cuando el buffer no esta vacio
    //UART1_enTxInt(1);
    #endif

    // arregla problema con pl2303, buscar
    // otra forma de solucionarlo
    _delay_ms(1);

    #if defined(USE_UART1_TX_INTERRUPT) || defined(USE_UART1_RX_INTERRUPT)
    //sei();
    #endif
}

inline void UART1_enRxInt(uint8_t en)
{
    if (en){
        UCSR1B |= (1 << RXCIE1);
    }else{
        UCSR1B &= ~(1 << RXCIE1);
    }
}

inline void UART1_enTxInt(uint8_t en)
{
    if (en){
        UCSR1B |= (1 << TXCIE1);
    }else{
        UCSR1B &= ~(1 << TXCIE1);
    }
}

void UART1_Tx(char c)
{
    #ifndef USE_UART1_TX_INTERRUPT
    while( !(UCSR1A & (1 << UDRE1)));
    UDR1 = c;
    #else
    while( tx1_buffer_full );
    buf1[tx1_tail_pointer] = c;
    UCSR1B &= ~(1 << UDRE1);
    tx1_tail_pointer = (tx1_tail_pointer+1)%TX1_BUFFER_SIZE;
    if (tx1_tail_pointer == tx1_head_pointer )
        tx1_buffer_full = 1;
    UCSR1B |= (1 << UDRE1);
    #endif
}

void UART1_sends(char * s)
{
    #ifndef USE_UART1_TX_INTERRUPT
    while(*s){
        while( !(UCSR1A & 1 << UDRE1));
        UDR1 = *s++;
    }
    #else
    while(*s){
        while( tx1_buffer_full );
        buf1[tx1_tail_pointer] = *s++;
        UCSR1B &= ~(1 << UDRE1);
        tx1_tail_pointer = (tx1_tail_pointer+1)%TX1_BUFFER_SIZE;
        if (tx1_tail_pointer == tx1_head_pointer )
            tx1_buffer_full = 1;
        UCSR1B |= (1 << UDRE1);
    }
    #endif
}

void UART1_send_hex8(uint8_t d)
{
    UART1_send_hex(d >> 4);
    UART1_send_hex(d);
}

void UART1_send_hex16(uint16_t d)
{
    UART1_send_hex(d >> 12);
    UART1_send_hex(d >> 8);
    UART1_send_hex(d >> 4);
    UART1_send_hex(d);
}

void UART1_send_hex32(uint32_t d)
{
    UART1_send_hex16(d >> 16);
    UART1_send_hex16(d);
}

inline void UART1_send_hex(uint8_t h)
{
    h = h & 0xF;
    if ( h >= 10 ){
        UART1_Tx('A'+h-10);
    }else{
        UART1_Tx('0' + h);
    }
}

