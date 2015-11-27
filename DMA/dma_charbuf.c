#include <AT91SAM7SE512.H>
#include "../AT91SAM7SE-EK.h"

#include "../AT91SAM7SELib.h"

#define MAX_CHARBUF 512
char charbuf[MAX_CHARBUF + 1] = {0};
char *ptr = charbuf;

uint8_t __init_us0_endrx_irq = 0;

uint8_t USART_read(AT91S_USART *usart)
{
  enable_led(USART_RX_LED);
  while ((usart->US_CSR & AT91C_US_RXRDY) == 0); // while !rxrdy, wait.
  disable_led(USART_RX_LED);
  return usart->US_RHR;
}

size_t USART_write(AT91S_USART *usart, uint8_t c)
{
  enable_led(USART_TX_LED);
  while ((usart->US_CSR & AT91C_US_TXRDY) == 0); // while !txrdy, wait.
  usart->US_THR = c;
  while ((usart->US_CSR & AT91C_US_TXRDY) == 0); // while !txrdy, wait.
  disable_led(USART_TX_LED);
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

unsigned char USART_ReadBuffer(AT91S_USART *usart, void *buffer, unsigned int size)
{
  // Check if the first PDC bank is free
  if ((usart->US_RCR == 0) && (usart->US_RNCR == 0)) {
    usart->US_RPR = (unsigned int) buffer;
    usart->US_RCR = size;
    usart->US_PTCR = AT91C_PDC_RXTEN;
    return 1;
  } else if (usart->US_RNCR == 0) {  // Check if the second PDC bank is free
    usart->US_RNPR = (unsigned int) buffer;
    usart->US_RNCR = size;
    return 1;
  } else {
    return 0;
  }
}

unsigned char USART_WriteBuffer(AT91S_USART *usart, void *buffer, unsigned int size)
{
  // Check if the first PDC bank is free
  if ((usart->US_TCR == 0) && (usart->US_TNCR == 0)) {
    usart->US_TPR = (unsigned int) buffer;
    usart->US_TCR = size;
    usart->US_PTCR = AT91C_PDC_TXTEN;
    return 1;
  } else if (usart->US_RNCR == 0) {  // Check if the second PDC bank is free
    usart->US_TNPR = (unsigned int) buffer;
    usart->US_TNCR = size;
    return 1;
  } else {
    return 0;
  }
}

void usart0_handler(void) {
  unsigned int status = AT91C_BASE_US0->US_CSR;

  if ((status & AT91C_US_ENDRX) == AT91C_US_ENDRX) {
    if (likely(__init_us0_endrx_irq)) {
      //USART_WriteBuffer(AT91C_BASE_US1, ((void *)ptr), 1);
      ptr = ptr + 1;
      if (unlikely(ptr == &charbuf[MAX_CHARBUF])) {
        ptr = &charbuf[0];
      }
    } else {
      // do not do anything at first interrupt
      __init_us0_endrx_irq++;
    }
    USART_ReadBuffer(AT91C_BASE_US0, ptr, 1);
  } // if
}

void ConfigureUsart0( void ) {
  // enable PIOA pins for US0 tx and rx
  AT91C_BASE_PIOA->PIO_PDR = PINS_USART0; // disable rx, tx
  AT91C_BASE_PIOA->PIO_ASR = PINS_USART0; // enable rx, tx

  PMC_EnablePeripheral(AT91C_ID_US0);
  USART_Configure(AT91C_BASE_US0, (AT91C_US_CHRL | AT91C_US_PAR_NONE), 9600, BOARD_MCK);

  USART_SetRecieverTimeout(AT91C_BASE_US0, 0);            // recieve timeout: inf
  USART_SetTransmitterTimeguard(AT91C_BASE_US0, 0);       //transmit timeguard: inf

  USART_DisableInterrupts(AT91C_BASE_US0, 0xFFFFFFFF);    // disable all interrupts
  USART_EnableInterrupts(AT91C_BASE_US0, AT91C_US_ENDRX); // send rxrdy interrupt

  /* AIC */
  AIC_Init();
  AIC_DisableIT(AT91C_ID_US0);
  AIC_ConfigureIT(AT91C_ID_US0, 0, usart0_handler);
  AIC_EnableIT(AT91C_ID_US0);
  /* AIC (end) */

  USART_SetReceiverEnabled(AT91C_BASE_US0, 1);
  USART_SetTransmitterEnabled(AT91C_BASE_US0, 1);
}

void ConfigureUsart1(void) {
  // enable PIOA pins for US0 tx and rx
  AT91C_BASE_PIOA->PIO_PDR = PINS_USART1; // disable rx, tx
  AT91C_BASE_PIOA->PIO_ASR = PINS_USART1; // enable rx, tx

  PMC_EnablePeripheral(AT91C_ID_US1);
  USART_Configure(AT91C_BASE_US1, (AT91C_US_CHRL | AT91C_US_PAR_NONE), 9600, BOARD_MCK);

  USART_SetRecieverTimeout(AT91C_BASE_US1, 0);            // recieve timeout: inf
  USART_SetTransmitterTimeguard(AT91C_BASE_US1, 0);       //transmit timeguard: inf

  USART_DisableInterrupts(AT91C_BASE_US1, 0xFFFFFFFF);    // disable all interrupts
  USART_EnableInterrupts(AT91C_BASE_US1, AT91C_US_RXRDY); // send rxrdy interrupt

  USART_SetReceiverEnabled(AT91C_BASE_US1, 1);
  USART_SetTransmitterEnabled(AT91C_BASE_US1, 1);
}

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

int main(void) {

  // enable PIOA for LEDs and UART
  PMC_EnablePeripheral(AT91C_ID_PIOA);

  // enable PIOB for joystick?
  PMC_EnablePeripheral(AT91C_ID_PIOB);

  /* enable output on leds */
  AT91C_BASE_PIOA->PIO_PER  = AT91B_LED_MASK;
  AT91C_BASE_PIOA->PIO_OER  = AT91B_LED_MASK;

  AT91C_BASE_PIOA->PIO_SODR = AT91B_LED2 | AT91B_LED1;
  AT91C_BASE_PIOA->PIO_CODR = AT91B_POWERLED;
  /* enable output on leds (end) */

  ConfigureUsart1(); // configure usart1
  set_printf_us(AT91C_BASE_US1);
  set_scanf_us(AT91C_BASE_US1);
  
  ConfigureUsart0(); // configure usart0

  // initial read to get things rolling
  /* we are bombarded with endrx int. do not add extra */
  // USART_ReadBuffer(AT91C_BASE_US0, ptr, 4);

  // infinity
  while (1);

  ; // return 0; /* statement unreachable as-is */
}


/*
===========================================================================
*/
