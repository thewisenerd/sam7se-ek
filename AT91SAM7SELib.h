/* AT91SAM7SELib.h
 *
 * Copyright (c) 2015, thewisenerd <thewisenerd@protonmail.com>
 *
 * Proprietary, and Confidential till futher notice.
 *
 */

#include <AT91SAM7SE512.H>              /* AT91SAM7SE512 definitions          */
#include "AT91SAM7SE-EK.h"           /* AT91SAM7SE-EK board definitions    */

#include <stdio.h>
#include <stdbool.h>

#define uint8_t unsigned char
#define  int8_t char
#define uint    unsigned int

/* from kernel tree; include/linux/compiler.h */
# define likely(x)       __builtin_expect(!!(x), 1)
# define unlikely(x)     __builtin_expect(!!(x), 0)

#define BOARD_MCK 48000000
#define MY_OSC_2  5000      // iterations for 1ms delay (approx)

#define PIN_USART0_RXD  (1 << 5)
#define PIN_USART0_TXD  (1 << 6)
#define PINS_USART0     (PIN_USART0_RXD | PIN_USART0_TXD)

#define PIN_USART1_RXD  (1 << 21)
#define PIN_USART1_TXD  (1 << 22)
#define PINS_USART1     (PIN_USART1_RXD | PIN_USART1_TXD)

#define USART_RX_LED AT91B_LED2
#define USART_TX_LED AT91B_LED1

#define USART_DMA
#ifdef USART_DMA
#define MAX_CHARBUF 100
extern char __us0_rxbuf[MAX_CHARBUF + 1];
extern char *__us0_rx_ptr;
extern char *__us0_buf_head;

extern char __us1_rxbuf[MAX_CHARBUF + 1];
extern char *__us1_rx_ptr;
extern char *__us1_buf_head;

extern uint8_t __init_us0_endrx_irq;
extern uint8_t __init_us1_endrx_irq;
#endif

void delay_ms(unsigned int n);

void enable_led(unsigned int LED);
void disable_led(unsigned int LED);
void blinkenlights(uint8_t LED, size_t delay, size_t n);

/* pmc */
void PMC_EnablePeripheral( uint8_t source );
void PMC_DisablePeripheral( uint8_t source );
void PMC_DisableInterrupts(AT91S_PIO *pio, unsigned int filter);
void PMC_EnableInterrupts(AT91S_PIO *pio, unsigned int filter);
/* pmc (end) */

/* AIC */
void AIC_ConfigureIT(uint8_t source, uint8_t mode, void (*handler)(void));
void AIC_EnableIT(uint8_t source);
void AIC_DisableIT(uint8_t source);
void AIC_Init( void );
/* AIC (end) */

/* u(s)art functions */
void    USART_Configure(AT91S_USART *usart, uint mode, uint baudrate, uint masterClock);
int     USART_read(AT91S_USART *usart);
int     USART_write(AT91S_USART *usart, int *c, int size);
void    USART_SetTransmitterEnabled(AT91S_USART *usart, uint8_t enabled);
void    USART_SetReceiverEnabled(AT91S_USART *usart, uint8_t enabled);
void    USART_SetRecieverTimeout(AT91S_USART *usart, unsigned int timeout);
void    USART_SetTransmitterTimeguard(AT91S_USART *usart, unsigned int timeguard);
void    USART_DisableInterrupts(AT91S_USART *usart, unsigned int filter);
void    USART_EnableInterrupts(AT91S_USART *usart, unsigned int filter);
bool    USART_PDC_RxStatus(AT91S_USART *usart);
bool    USART_PDC_TxStatus(AT91S_USART *usart);
int     USART_ReadBuffer(AT91S_USART *usart, void *buffer, unsigned int size);
int     USART_WriteBuffer(AT91S_USART *usart, void *buffer, unsigned int size);
#ifdef USART_DMA
void    USART_DMAFixRxPointers(AT91S_USART *usart);
#endif
/* u(s)art functions (end) */

/* u(s)art helpers */
extern AT91S_USART  *__usart_in_device;
extern AT91S_USART *__usart_out_device;
void set_printf_us(AT91S_USART *usart);
void  set_scanf_us(AT91S_USART *usart);
/* u(s)art helpers (end) */

/* rtc */
unsigned int getRTCVal( void );
void         resetRTC( uint8_t resolution, unsigned int filter );
void         setRTCMode(unsigned int filter);
void         setRTCAlarm( unsigned int time );
/* rtc (end) */
