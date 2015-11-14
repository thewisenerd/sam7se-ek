#include <AT91SAM7SE512.H>
#include "AT91SAM7SE-EK.h"

#include "AT91SAM7SELib.h"

void aic_handler(void) {
  if ((AT91C_BASE_US0->US_CSR & AT91C_US_RXRDY) == AT91C_US_RXRDY) {
    unsigned int data;
    data = AT91C_BASE_US0->US_RHR;

    switch (data) {
      case 'a':
      case  28: //left arrow
        printf("left\n");
        break;
      case 'd':
      case  29: //right arrow
      printf("right\n");
        break;
      case 'w':
      case  30: //left arrow
        printf("up\n");
        break;
      case 's':
      case  31: //left arrow
        printf("down\n");
        break;
      default:
        ; // dbg: printf("def: %d\n", data);
    }
  } // if AT91C_US_RXRDY
}

void usart1_handler(void) {
  if ((AT91C_BASE_US1->US_CSR & AT91C_US_RXRDY) == AT91C_US_RXRDY) {
    unsigned int data;
    data = AT91C_BASE_US1->US_RHR;

    if (data >= '0' && data <= '9') {
      unsigned int alarm = getRTCVal() + ((data - '0') * 10);
      printf("alarm set at: %ds\n", alarm);
      setRTCAlarm(alarm);
      return;
    }

    switch (data) {
      case 'a':
      case  28: //left arrow
        printf("left\n");
        break;
      case 'd':
      case  29: //right arrow
      printf("right\n");
        break;
      case 'w':
      case  30: //left arrow
        printf("up\n");
        break;
      case 's':
      case  31: //left arrow
        printf("down\n");
        break;
      default:
        ; // dbg: printf("def: %d\n", data);
    }
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
  USART_EnableInterrupts(AT91C_BASE_US0, AT91C_US_RXRDY); // send rxrdy interrupt

  /* AIC */
  AIC_Init();
  AIC_DisableIT(AT91C_ID_US0);
  AIC_ConfigureIT(AT91C_ID_US0, 0, aic_handler);
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

  /* AIC */
  //AIC_Init();
  AIC_DisableIT(AT91C_ID_US1);
  AIC_ConfigureIT(AT91C_ID_US1, 0, usart1_handler);
  AIC_EnableIT(AT91C_ID_US1);
  /* AIC (end) */

  USART_SetReceiverEnabled(AT91C_BASE_US1, 1);
  USART_SetTransmitterEnabled(AT91C_BASE_US1, 1);
}

void piob_handler ( void ) {
  unsigned int change = AT91C_BASE_PIOB->PIO_ISR;
  if (change & AT91B_SW1) {
    while ((AT91C_BASE_PIOB->PIO_PDSR & AT91B_SW1) == 0) {
      printf("up\n");
    }
  } else if (change & AT91B_SW2) {
    while ((AT91C_BASE_PIOB->PIO_PDSR & AT91B_SW2) == 0) {
      printf("down\n");
    }
  } else if (change & AT91B_SW3) {
    while ((AT91C_BASE_PIOB->PIO_PDSR & AT91B_SW3) == 0) {
      printf("left\n");
    }
  } else if (change & AT91B_SW4) {
    while ((AT91C_BASE_PIOB->PIO_PDSR & AT91B_SW4) == 0) {
      printf("right\n");
    }
  } else if (change & AT91B_BP1) {
    while ((AT91C_BASE_PIOB->PIO_PDSR & AT91B_BP1) == 0) {
      printf("left btn\n");
    }
  } else if (change & AT91B_BP2) {
    while ((AT91C_BASE_PIOB->PIO_PDSR & AT91B_BP2) == 0) {
      printf("right btn\n");
    }
  }
}

#define BP_MASK (AT91B_SW_MASK | AT91B_BP_MASK)
void ConfigurePIOB_Interrupts( void ) {
  // enable PIOB pins for US0 tx and rx
  AT91C_BASE_PIOA->PIO_PDR = BP_MASK; // disable rx, tx
  AT91C_BASE_PIOA->PIO_ASR = BP_MASK; // enable rx, tx

  PMC_EnablePeripheral(AT91C_ID_PIOB);
  AT91C_BASE_PIOB->PIO_IFER = BP_MASK;
  
  PMC_DisableInterrupts(AT91C_BASE_PIOB, 0xFFFFFFFF);    // disable all interrupts
  PMC_EnableInterrupts(AT91C_BASE_PIOB, BP_MASK); // send only BP_MASK interrupt

  /* AIC */
  AIC_DisableIT(AT91C_ID_PIOB);
  AIC_ConfigureIT(AT91C_ID_PIOB, 0, piob_handler);
  AIC_EnableIT(AT91C_ID_PIOB);
  /* AIC (end) */
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

  ConfigureUsart0(); // configure usart0

  ConfigureUsart1(); // configure usart1
  set_printf_us(AT91C_BASE_US1);
  set_scanf_us(AT91C_BASE_US1);

  ConfigurePIOB_Interrupts();

  setupRTC();

  // infinity
  while (1);

  ; // return 0; /* statement unreachable as-is */
}
