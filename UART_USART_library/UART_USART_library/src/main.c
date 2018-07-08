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
#include "lib/logger.h"

static void configure_uart(void)
{
	const usart_serial_options_t serial_option = {
		.baudrate = 115200ul,
		.charlength = US_MR_CHRL_8_BIT,
		.paritytype = US_MR_PAR_NO,
		.stopbits = US_MR_NBSTOP_1_BIT
	};
	
	/* Initialize UART/USART interfaces. */
	serial_mdw_init_interface((usart_if)UART0, &serial_option);
	serial_mdw_init_interface((usart_if)UART1, &serial_option);
	serial_mdw_init_interface((usart_if)UART2, &serial_option);
	serial_mdw_init_interface((usart_if)UART3, &serial_option);
	serial_mdw_init_interface((usart_if)UART4, &serial_option);
	serial_mdw_init_interface((usart_if)USART0, &serial_option);
	serial_mdw_init_interface((usart_if)USART1, &serial_option);
	serial_mdw_init_interface((usart_if)USART2, &serial_option);	
}

int main (void)
{
	/* Initialize the SAM system. */
	sysclk_init();
	board_init();

	/* Configure UART for debug message output. */
	logger_init();
		
	log_info("-- UART_USART Library --\r\n");
	log_info("-- Developed and made by amof 2018--\r\n");		
	
	/* Configure UART-USART */
	configure_uart();
		
	uint8_t buffer[number_of_uart][255];
	volatile uint8_t pointers[number_of_uart]={0};
	const usart_if uart_pointers[number_of_uart] ={(usart_if)UART0, (usart_if)UART1, (usart_if)UART2, (usart_if)UART3, (usart_if)UART4, (usart_if)USART0, (usart_if)USART1, (usart_if)USART2} ;
	
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
