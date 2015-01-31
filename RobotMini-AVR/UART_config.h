#ifndef UART_CONFIG_H_INCLUDED
#define UART_CONFIG_H_INCLUDED

#ifndef F_CPU
#warning "Definiendo F_CPU"
#define F_CPU 16000000L
#endif

//#define BAUD 1200
//#define BAUD 2400
//#define BAUD 9600
//#define BAUD  38400
//#define BAUD  57600
#define BAUD  115200
//#define BAUD  230400

#define USE_UART0_TX_INTERRUPT
//#define USE_UART0_RX_INTERRUPT

#define USE_UART1_TX_INTERRUPT
//#define USE_UART1_RX_INTERRUPT

#define TX0_BUFFER_SIZE 32
#define TX1_BUFFER_SIZE 32

#endif // UART_CONFIG_H_INCLUDED
