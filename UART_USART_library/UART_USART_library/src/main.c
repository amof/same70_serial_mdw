/**
 * \file
 *
 * \brief Empty user application template
 *
 */

/**
 * \mainpage User Application template doxygen documentation
 *
 * \par Empty user application template
 *
 * Bare minimum empty user application template
 *
 * \par Content
 *
 * -# Include the ASF header files (through asf.h)
 * -# "Insert system clock initialization code here" comment
 * -# Minimal main function that starts with a call to board_init()
 * -# "Insert application code here" comment
 *
 */

/*
 * Include header files for all drivers that have been imported from
 * Atmel Software Framework (ASF).
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */
#include <asf.h>
#include "lib/serial_mdw.h"


static void configure_uart(void)
{
	const sam_usart_opt_t usart_console_settings = {
		115200ul,
		US_MR_CHRL_8_BIT,
		US_MR_PAR_NO,
		US_MR_NBSTOP_1_BIT,
		US_MR_CHMODE_NORMAL,
		/* This field is only used in IrDA mode. */
		0
	};


	/* Configure USART in RS232 mode. */
	serial_mdw_init(UART4, &usart_console_settings);
	serial_mdw_init(USART0, &usart_console_settings);

	/* Disable all the interrupts. */
	usart_disable_interrupt(UART4, 0xffffffff);
	usart_disable_interrupt(USART0, 0xffffffff);


	/* Enable TX & RX function. */
	usart_enable_tx(UART4);
	usart_enable_rx(UART4);
	usart_enable_tx(USART0);
	usart_enable_rx(USART0);

	/* Configure and enable interrupt of USART. */
	NVIC_EnableIRQ(UART4_IRQn);
	NVIC_EnableIRQ(USART0_IRQn);
}

int main (void)
{
	/* Initialize the SAM system. */
	sysclk_init();
	board_init();

	/* Configure USART-USART */
	configure_uart();
	
	while (1)
	{
		//serial_mdw_putchar(UART4, 'h');
		serial_mdw_sendData(UART4, "HELLO WORLD", 11);
		serial_mdw_sendData(USART0, "WORLD HELLO", 11);
		delay_ms(50);
	}
}
