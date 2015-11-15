#include <AT91SAM7SE512.H>
#include "AT91SAM7SE-EK.h"

#include "AT91SAM7SELib.h"

#define MAX_BUF 100
float values[MAX_BUF] = {0};
float *ptr = &values[0];

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

void usart0_handler(void) {
  unsigned int status = AT91C_BASE_US0->US_CSR ;

    uint8_t ret = USART_write(AT91C_BASE_US1, 'a');
    if (ret == 0){ 
      blinkenlights(AT91B_POWERLED, 200, 1);
    } else {
      blinkenlights(AT91B_POWERLED, 200, 2);
    }

  if ((status & AT91C_US_ENDRX) == AT91C_US_ENDRX) {
    //printf("%f\n", *ptr);
  } // if AT91C_US_RXRDY
  *AT91C_AIC_EOICR = 0;
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
  USART_EnableInterrupts(AT91C_BASE_US0, (AT91C_US_ENDRX) ); // send rxrdy interrupt

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

  USART_SetReceiverEnabled(AT91C_BASE_US1, 1);
  USART_SetTransmitterEnabled(AT91C_BASE_US1, 1);
}

void sys_aic_handler( void ) {
  unsigned int status = AT91C_BASE_RTTC->RTTC_RTSR;
  if (status & AT91C_RTTC_RTTINC) {
    if (USART_PDC_RxStatus(AT91C_BASE_US0)) {
      USART_write(AT91C_BASE_US0, 1);
      USART_ReadBuffer(AT91C_BASE_US0, ptr, 4);
    }
  }
}

void setupRTC( void ) {
  // 1s resolution, increment interrupt
  resetRTC(15, AT91C_RTTC_RTTINCIEN);

  AIC_DisableIT(AT91C_ID_SYS);
  AIC_ConfigureIT(AT91C_ID_SYS, 0, sys_aic_handler);
  AIC_EnableIT(AT91C_ID_SYS);
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

  ConfigureUsart0(); // configure usart0

  ConfigureUsart1(); // configure usart1
  set_printf_us(AT91C_BASE_US1);
  set_scanf_us(AT91C_BASE_US1);

  setupRTC();

  // infinity
  while (1);

  ; // return 0; /* statement unreachable as-is */
}
