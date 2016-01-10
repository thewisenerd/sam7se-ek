#include <AT91SAM7SE512.H>              /* AT91SAM7SE512 definitions          */
#include "..\AT91SAM7SE-EK.h"           /* AT91SAM7SE-EK board definitions    */

#include <stdio.h>
#include <stdbool.h>

#define uint8_t unsigned char
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

#define PIN_DBGU_RXD    (1 <<  9)
#define PIN_DBGU_TXD    (1 << 10)
#define PINS_DBGU       (PIN_DBGU_RXD | PIN_DBGU_TXD)

#define USART_RX_LED AT91B_LED2
#define USART_TX_LED AT91B_LED1

void ConfigureUsart0( void ) {
  // enable PIOA pins for US0 tx and rx
  AT91C_BASE_PIOA->PIO_PDR = PINS_USART0; // disable rx, tx
  AT91C_BASE_PIOA->PIO_ASR = PINS_USART0; // enable rx, tx

  AT91C_BASE_PMC->PMC_PCER = 1 << AT91C_ID_US0;

  // Reset and disable receiver & transmitter
  AT91C_BASE_US0->US_CR = AT91C_US_RSTRX | AT91C_US_RSTTX | AT91C_US_RXDIS | AT91C_US_TXDIS;

  // Configure mode
  AT91C_BASE_US0->US_MR = (AT91C_US_CHRL | AT91C_US_PAR_NONE);

  // Configure baudrate
  AT91C_BASE_US0->US_BRGR = (BOARD_MCK / 9600) / 16;

  AT91C_BASE_US0->US_RTOR = 0;
	AT91C_BASE_US0->US_TTGR = 0; //transmit timeguard: inf

  AT91C_BASE_US0->US_IDR = 0xFFFFFFFF; 
  // AT91C_BASE_US0->US_IER = AT91C_US_ENDRX; 

	AT91C_BASE_US0->US_CR = AT91C_US_TXEN;
  AT91C_BASE_US0->US_CR = AT91C_US_RXEN;
}

void ConfigureUsart1( void ) {
  // enable PIOA pins for US0 tx and rx
  AT91C_BASE_PIOA->PIO_PDR = PINS_USART1; // disable rx, tx
  AT91C_BASE_PIOA->PIO_ASR = PINS_USART1; // enable rx, tx

  AT91C_BASE_PMC->PMC_PCER = 1 << AT91C_ID_US1;

  // Reset and disable receiver & transmitter
  AT91C_BASE_US1->US_CR = AT91C_US_RSTRX | AT91C_US_RSTTX | AT91C_US_RXDIS | AT91C_US_TXDIS;

  // Configure mode
  AT91C_BASE_US1->US_MR = (AT91C_US_CHRL | AT91C_US_PAR_NONE);

  // Configure baudrate
  AT91C_BASE_US1->US_BRGR = (BOARD_MCK / 9600) / 16;

  AT91C_BASE_US1->US_RTOR = 0;
	AT91C_BASE_US1->US_TTGR = 0; //transmit timeguard: inf

  AT91C_BASE_US1->US_IDR = 0xFFFFFFFF; 
  // AT91C_BASE_US1->US_IER = AT91C_US_ENDRX; 

	AT91C_BASE_US1->US_CR = AT91C_US_TXEN;
  AT91C_BASE_US1->US_CR = AT91C_US_RXEN;
}



extern AT91S_USART  *_usart_in_device;
extern AT91S_USART *_usart_out_device;
void set_printf_us(AT91S_USART *usart);
void  set_scanf_us(AT91S_USART *usart);


/* USART */
void USART_Configure(AT91S_USART *usart, uint mode, uint baudrate, uint masterClock) {
  // Reset and disable receiver & transmitter
  usart->US_CR = AT91C_US_RSTRX | AT91C_US_RSTTX | AT91C_US_RXDIS | AT91C_US_TXDIS;

  // Configure mode
  usart->US_MR = mode;

  // Configure baudrate
  usart->US_BRGR = (masterClock / baudrate) / 16;
}

uint8_t USART_read(AT91S_USART *usart)
{
  //enable_led(USART_RX_LED);
  while ((usart->US_CSR & AT91C_US_RXRDY) == 0); // while !rxrdy, wait.
  //disable_led(USART_RX_LED);
  return usart->US_RHR;
}

size_t USART_write(AT91S_USART *usart, uint8_t c)
{
  //enable_led(USART_TX_LED);
  while ((usart->US_CSR & AT91C_US_TXRDY) == 0); // while !txrdy, wait.
  usart->US_THR = c;
  while ((usart->US_CSR & AT91C_US_TXRDY) == 0); // while !txrdy, wait.
  //disable_led(USART_TX_LED);
  return 1;
}

void USART_SetTransmitterEnabled(AT91S_USART *usart, uint8_t enabled)
{
  if (enabled) {
    usart->US_CR = AT91C_US_TXEN;
  } else {
    usart->US_CR = AT91C_US_TXDIS;
  }
}

void USART_SetReceiverEnabled(AT91S_USART *usart, uint8_t enabled)
{
  if (enabled) {
    usart->US_CR = AT91C_US_RXEN;
  } else {
    usart->US_CR = AT91C_US_RXDIS;
  }
}

void USART_SetRecieverTimeout(AT91S_USART *usart, unsigned int timeout) {
  usart->US_RTOR = timeout;
}

void USART_SetTransmitterTimeguard(AT91S_USART *usart, unsigned int timeguard) {
  usart->US_TTGR = timeguard;
}

void USART_DisableInterrupts(AT91S_USART *usart, unsigned int filter)
{
  usart->US_IDR = filter; 
}

void USART_EnableInterrupts(AT91S_USART *usart, unsigned int filter)
{
  usart->US_IER = filter; 
}

void set_printf_us(AT91S_USART *usart) {
  _usart_out_device = usart;
}
void  set_scanf_us(AT91S_USART *usart) {
  _usart_in_device = usart;
}

bool USART_PDC_RxStatus(AT91S_USART *usart) {
  if ((usart->US_RCR == 0) || (usart->US_RNCR == 0))
    return true;
  return false;
}

bool USART_PDC_TxStatus(AT91S_USART *usart) {
  if ((usart->US_TCR == 0) || (usart->US_TNCR == 0))
    return true;
  return false;
}
/* USART (end) */















/* stdlib overwrites */
signed int fgetc(FILE *pStream)
{
  if (pStream == stdin) {
    return USART_read(_usart_in_device);
  } else {
    return EOF;
  }
}

signed int fputc(signed int c, FILE *pStream)
{
  if ((pStream == stdout) || (pStream == stderr)) {
    USART_write(_usart_out_device, c);
    return c;
  } else {
    return EOF;
  }
}

signed int fputs(const char *pStr, FILE *pStream)
{
  signed int num = 0;
  while (*pStr != 0) {
    if (fputc(*pStr, pStream) == -1) {
      return -1;
    }
    num++;
    pStr++;
  }
  return num;
}

char* fgets(char *str, int size, FILE *pStream)
{
  if (pStream == stdin) {
    while (size--) {
      *str++ = USART_read(_usart_in_device);
    }
    return str;
  } else {
    return NULL;
  }
}

int ferror(FILE *f)
{
  return EOF;
}
/* stdlib overwrites (end) */
