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

#ifdef SRL_MDW_DEBUG
/**
 *  Configure UART for debug message output.
 */
static void configure_console(void)
{
	const usart_serial_options_t uart_serial_option = {
		.baudrate = 115200ul,
		.charlength = US_MR_CHRL_8_BIT,
		.paritytype = US_MR_PAR_NO,
		.stopbits = US_MR_NBSTOP_1_BIT
	};

	/* Configure console UART. */
	serial_mdw_stdio_init(CONSOLE, &uart_serial_option);
}
#endif

static void configure_uart(void)
{
	const usart_serial_options_t serial_option = {
		.baudrate = 115200ul,
		.charlength = US_MR_CHRL_8_BIT,
		.paritytype = US_MR_PAR_NO,
		.stopbits = US_MR_NBSTOP_1_BIT
	};
	
	/* Initialize UART interfaces. */
	serial_mdw_init(UART0, &serial_option);
	serial_mdw_init(UART1, &serial_option);
	serial_mdw_init(UART2, &serial_option);
	serial_mdw_init(UART3, &serial_option);
	serial_mdw_init(UART4, &serial_option);
	serial_mdw_init(USART0, &serial_option);
	#ifndef SRL_MDW_DEBUG
		serial_mdw_init(USART1, &serial_option);
	#endif
	serial_mdw_init(USART2, &serial_option);	
}

int main (void)
{
	/* Initialize the SAM system. */
	sysclk_init();
	board_init();

	#ifdef SRL_MDW_DEBUG
		/* Configure UART for debug message output. */
		configure_console();
		
		puts("-- UART_USART Library --\r");
		puts("-- Developed and made by amof 2018--\r");
		puts("USART1 initialized with SERIAL_MDW_DEBUG ACTIVATED\r");
	#endif
		
	
	/* Configure USART-USART */
	configure_uart();
		
	uint8_t buffer[number_of_uart][255];
	volatile uint8_t pointers[number_of_uart]={0,0,0,0,0,0,0,0};
	const uint32_t uart_pointers[number_of_uart] ={UART0, UART1, UART2, UART3, UART4, USART0, USART1, USART2} ;
	
	while (1)
	{
		// 1. Transmission test
		// All TX are functional
// 		serial_mdw_sendData(UART0, "UART0", 5);
// 		serial_mdw_sendData(UART1, "UART1", 5);
// 		serial_mdw_sendData(UART2, "UART2", 5);
// 		serial_mdw_sendData(UART3, "UART3", 5);
// 		serial_mdw_sendData(UART4, "UART4", 5);
// 		serial_mdw_sendData(USART0, "USART0", 6);
// 		serial_mdw_sendData(USART1, "USART1", 6);
// 		serial_mdw_sendData(USART2, "USART2", 6);
// 		delay_ms(50);
		
		// 2. Reception test
		// A. All UART and USART are OK
		for (uint8_t i = 0; i<number_of_uart; i++)
		{
			if(serial_mdw_available(uart_pointers[i])>0){
				uint8_t received = serial_mdw_readChar(uart_pointers[i]) & 0xFF;
				uint8_t point_temp = pointers[0];
				buffer[i][point_temp] = received;
				pointers[i] = point_temp + 1;
				if(point_temp==25){
					//srl_mdw_debug_buffer(buffer[0], 26);
					serial_mdw_sendData(uart_pointers[i], buffer[i], 26);
					pointers[i] = 0;
				}
			}
		}
		
	}
}
