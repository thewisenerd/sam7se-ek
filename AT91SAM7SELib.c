/* AT91SAM7SELib.c
 *
 * Copyright (c) 2015, thewisenerd <thewisenerd@protonmail.com>
 *
 * Proprietary, and Confidential till futher notice.
 *
 */

#include "AT91SAM7SELib.h"

AT91S_USART  *_usart_in_device = AT91C_BASE_US0;
AT91S_USART *_usart_out_device = AT91C_BASE_US0;

void delay_ms(unsigned int n)
{
  unsigned int i = MY_OSC_2 * n;
  while(i)i--;
}

void enable_led(unsigned int LED)
{
  if (LED == AT91B_POWERLED)
    AT91C_BASE_PIOA->PIO_SODR = LED;
  else
    AT91C_BASE_PIOA->PIO_CODR = LED;
}

void disable_led(unsigned int LED)
{
  if (LED == AT91B_POWERLED)
    AT91C_BASE_PIOA->PIO_CODR = LED;
  else
    AT91C_BASE_PIOA->PIO_SODR = LED;
}

void blinkenlights(uint8_t LED, size_t delay, size_t n)
{
  while(n--) {
    enable_led(LED);
    delay_ms(delay);
    disable_led(LED);
    delay_ms(delay);
  }
}

/* stdlib overwrites */
#ifndef USART_DMA
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
#endif
/* stdlib overwrites (end) */

/* pmc */
void PMC_EnablePeripheral( uint8_t source )
{
  if (source < 32)
    AT91C_BASE_PMC->PMC_PCER = 1 << source;
}

void PMC_DisablePeripheral( uint8_t source )
{
  if ((AT91C_BASE_PMC->PMC_PCSR & (1 << source)) == (1 << source))
    AT91C_BASE_PMC->PMC_PCDR = 1 << source;
}
void PMC_DisableInterrupts(AT91S_PIO *pio, unsigned int filter)
{
  pio->PIO_IDR = filter; 
}

void PMC_EnableInterrupts(AT91S_PIO *pio, unsigned int filter)
{
  pio->PIO_IER = filter;
}
/* pmc (end) */

/* AIC */
void AIC_ConfigureIT(uint8_t source, uint8_t mode, void (*handler)(void))
{
  // Disable the interrupt first
  AT91C_BASE_AIC->AIC_IDCR = 1 << source;

  // Configure mode and handler
  AT91C_BASE_AIC->AIC_SMR[source] = mode;
  AT91C_BASE_AIC->AIC_SVR[source] = (unsigned int) handler;

  // Clear interrupt
  AT91C_BASE_AIC->AIC_ICCR = 1 << source;
}

void AIC_EnableIT(uint8_t source)
{
  AT91C_BASE_AIC->AIC_IECR = 1 << source;
}

void AIC_DisableIT(uint8_t source)
{
  AT91C_BASE_AIC->AIC_IDCR = 1 << source;
}

void AIC_Init( void )
{
  // disable all interrupts
  AT91C_BASE_AIC->AIC_IDCR = 0xFFFFFFFF;
  // clear all interrupts
  AT91C_BASE_AIC->AIC_ICCR = 0xFFFFFFFF;
}
/* AIC (end) */

/* USART */
void USART_Configure(AT91S_USART *usart, uint mode, uint baudrate, uint masterClock) {
  // Reset and disable receiver & transmitter
  usart->US_CR = AT91C_US_RSTRX | AT91C_US_RSTTX | AT91C_US_RXDIS | AT91C_US_TXDIS;

  // Configure mode
  usart->US_MR = mode;

  // Configure baudrate
  usart->US_BRGR = (masterClock / baudrate) / 16;
}

#ifndef USART_DMA
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
#endif
/* USART (end) */

/* rtc */
unsigned int getRTCVal( void ) {
  return AT91C_BASE_SYS->RTTC_RTVR;
}

void resetRTC( uint8_t resolution, unsigned int filter ) {
  // reset with 1s resolution
  AT91C_BASE_SYS->RTTC_RTMR = (1 << resolution) | filter | AT91C_RTTC_RTTRST;
}

void setRTCMode(unsigned int filter) {
  AT91C_BASE_SYS->RTTC_RTMR = filter;
}

void setRTCAlarm( unsigned int time )
{
  AT91C_BASE_RTTC->RTTC_RTAR = time - 1;
}   
/* rtc (end) */
