#include <AT91SAM7SE512.H>
#include "../AT91SAM7SE-EK.h"

#include "../AT91SAM7SELib.h"

void usart1_handler(void) {
  if ((AT91C_BASE_US1->US_CSR & AT91C_US_RXRDY) == AT91C_US_RXRDY) {
    unsigned int data;
    data = AT91C_BASE_US1->US_RHR;

    if (data >= '0' && data <= '9') {
      unsigned int now   = getRTCVal();
      unsigned int alarm = now + ((data - '0') * 10);
      if (alarm == now) {
        alarm = 0xFFFFFFFF;
        setRTCAlarm(alarm);
        printf("alarm cancelled!\n");
      } else {
        setRTCAlarm(alarm);
        printf("alarm set at: %ds\n", alarm);
      }
      return;
    }

  } // if
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

  /* AIC */
  AIC_Init();
  AIC_DisableIT(AT91C_ID_US1);
  AIC_ConfigureIT(AT91C_ID_US1, 0, usart1_handler);
  AIC_EnableIT(AT91C_ID_US1);
  /* AIC (end) */

  USART_SetReceiverEnabled(AT91C_BASE_US1, 1);
  USART_SetTransmitterEnabled(AT91C_BASE_US1, 1);
}

void sys_aic_handler( void ) {
  unsigned int status = AT91C_BASE_RTTC->RTTC_RTSR;
  if (status & AT91C_RTTC_RTTINC) {
    printf("%d\n", getRTCVal());
  }

  if (status & AT91C_RTTC_ALMS) {
    unsigned int now = getRTCVal();
    printf("!!!!alarm now at %d!!!!\n", now);
    while ((getRTCVal() - now) < 10) {
      blinkenlights(AT91B_POWERLED, 100, 1);
    }
  }
}

void setupRTC( void ) {
  // 1s resolution, increment interrupt, alarm interrupt!
  resetRTC(15, (AT91C_RTTC_RTTINCIEN | AT91C_RTTC_ALMIEN));

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

  ConfigureUsart1(); // configure usart1
  set_printf_us(AT91C_BASE_US1);
  set_scanf_us(AT91C_BASE_US1);

  setupRTC();

  // infinity
  while (1);

  ; // return 0; /* statement unreachable as-is */
}
