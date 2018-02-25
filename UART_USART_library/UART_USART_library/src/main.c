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

uint8_t i=0;

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
	
	/* Initialize UART interfaces. */
	serial_mdw_init(UART0, &usart_console_settings);
	serial_mdw_init(UART1, &usart_console_settings);
	serial_mdw_init(UART2, &usart_console_settings);
	serial_mdw_init(UART3, &usart_console_settings);
	serial_mdw_init(UART4, &usart_console_settings);
	serial_mdw_init(USART0, &usart_console_settings);
	serial_mdw_init(USART1, &usart_console_settings);
	serial_mdw_init(USART2, &usart_console_settings);
	
}

int main (void)
{
	/* Initialize the SAM system. */
	sysclk_init();
	board_init();

	/* Configure USART-USART */
	configure_uart();
	uint8_t buffer[255];
	
	uint8_t buffer2[255];
	uint8_t j=0;
	
	while (1)
	{
		// Transmission test
		// All tx are functionnal
		serial_mdw_sendData(UART0, "WORLD HELLO", 11); 
		serial_mdw_sendData(UART1, "WORLD HELLO", 11); 
		serial_mdw_sendData(UART2, "WORLD HELLO", 11); 
		serial_mdw_sendData(UART3, "WORLD HELLO", 11); 
		serial_mdw_sendData(UART4, "HELLO WORLD", 11); 
		serial_mdw_sendData(USART0, "WORLD HELLO", 11); 
		serial_mdw_sendData(USART1, "WORLD HELLO", 11); 
		serial_mdw_sendData(USART2, "WORLD HELLO", 11); 
		delay_ms(50);
		
		/*
		if(serial_mdw_available(UART1)>0){
			uint8_t received = serial_mdw_readChar(UART1) & 0xFF;
			buffer[i++] = received;
			if(i==26){
				i=0;
				serial_mdw_sendData(UART1, buffer, 26);
			}
			
		}
		/*
		if(serial_mdw_available(UART4)>0){
			uint8_t received = serial_mdw_readChar(UART4) & 0xFF;
			buffer[i++] = received;
			if(i==26){
				serial_mdw_sendData(UART4, buffer, 26);
			}
			
		}
		if(serial_mdw_available(USART0)>0){
			uint8_t received = serial_mdw_readChar(USART0) & 0xFF;
			buffer[j++] = received;
			if(j==26){
				serial_mdw_sendData(USART0, buffer, 26);
			}
			
		}*/

	}
}
