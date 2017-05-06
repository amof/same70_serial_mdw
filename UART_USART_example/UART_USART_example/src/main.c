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

static void configure_usart(void)
{
	const sam_usart_opt_t usart_console_settings = {
		BOARD_USART_BAUDRATE,
		US_MR_CHRL_8_BIT,
		US_MR_PAR_NO,
		US_MR_NBSTOP_1_BIT,
		US_MR_CHMODE_NORMAL,
		/* This field is only used in IrDA mode. */
		0
	};

	/* Enable the peripheral clock in the PMC. */
	sysclk_enable_peripheral_clock(BOARD_ID_USART);

	/* Configure USART in RS232 mode. */
	usart_init_rs232(BOARD_USART, &usart_console_settings,
	sysclk_get_peripheral_hz());

	/* Disable all the interrupts. */
	usart_disable_interrupt(BOARD_USART, 0xffffffff);

	/* Enable TX & RX function. */
	usart_enable_tx(BOARD_USART);
	usart_enable_rx(BOARD_USART);

	/* Configure and enable interrupt of USART. */
	NVIC_EnableIRQ(USART_IRQn);
}

static void configure_uart(void)
{
	const sam_usart_opt_t usart_console_settings = {
		BOARD_UART_BAUDRATE,
		US_MR_CHRL_8_BIT,
		US_MR_PAR_NO,
		US_MR_NBSTOP_1_BIT,
		US_MR_CHMODE_NORMAL,
		/* This field is only used in IrDA mode. */
		0
	};

	/* Enable the peripheral clock in the PMC. */
	sysclk_enable_peripheral_clock(BOARD_ID_UART);

	/* Configure USART in RS232 mode. */
	usart_init_rs232(BOARD_UART, &usart_console_settings,
	sysclk_get_peripheral_hz());

	/* Disable all the interrupts. */
	usart_disable_interrupt(BOARD_UART, 0xffffffff);

	/* Enable TX & RX function. */
	usart_enable_tx(BOARD_UART);
	usart_enable_rx(BOARD_UART);

	/* Configure and enable interrupt of USART. */
	NVIC_EnableIRQ(UART_IRQn);
}

int main (void)
{
	/* Initialize the SAM system. */
	sysclk_init();
	board_init();

	/* Configure USART-USART */
	configure_usart();
	configure_uart();
	
	while (1)
	{
		usart_write(BOARD_USART, 0x50);
		usart_write(BOARD_UART, 0x60);
		delay_ms(50);
	}
}
