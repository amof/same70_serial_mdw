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
		
	uint8_t buffer[8][255];
	volatile uint8_t pointers[8]={0,0,0,0,0,0,0,0};
	
	while (1)
	{
		// 1. Transmission test
		// All TX are functional
		serial_mdw_sendData(UART0, "UART0", 5);
		serial_mdw_sendData(UART1, "UART1", 5);
		serial_mdw_sendData(UART2, "UART2", 5);
		serial_mdw_sendData(UART3, "UART3", 5);
		serial_mdw_sendData(UART4, "UART4", 5);
		serial_mdw_sendData(USART0, "USART0", 6);
		serial_mdw_sendData(USART1, "USART1", 6);
		serial_mdw_sendData(USART2, "USART2", 6);
		delay_ms(50);
		
		// 2. Reception test
		// A. All UART and USART are OK
// 		if(serial_mdw_available(UART0)>0){ 
// 			uint8_t received = serial_mdw_readChar(UART0) & 0xFF;
// 			uint8_t point_temp = pointers[0];
// 			buffer[0][point_temp] = received;
// 			pointers[0] = point_temp + 1;
// 			if(point_temp==25){
// 				//srl_mdw_debug_buffer(buffer[0], 26);
// 				serial_mdw_sendData(UART0, buffer[0], 26);
// 				pointers[0] = 0;
// 			}
// 		}
// 		if(serial_mdw_available(UART1)>0){
// 			uint8_t received = serial_mdw_readChar(UART1) & 0xFF;
// 			uint8_t point_temp = pointers[1];
// 			buffer[1][point_temp] = received;
// 			pointers[1] = point_temp + 1;
// 			if(point_temp==25){
// 				serial_mdw_sendData(UART1, buffer[1], 26);
// 				pointers[1]=0;
// 			}
// 		}
// 		if(serial_mdw_available(UART2)>0){
// 			uint8_t received = serial_mdw_readChar(UART2) & 0xFF;
// 			uint8_t point_temp = pointers[2];
// 			buffer[2][point_temp] = received;
// 			pointers[2] = point_temp + 1;
// 			if(point_temp==25){
// 				serial_mdw_sendData(UART2, buffer[2], 26);
// 				pointers[2]=0;
// 			}
// 		}
// 		if(serial_mdw_available(UART3)>0){
// 			uint8_t received = serial_mdw_readChar(UART3) & 0xFF;
// 			uint8_t point_temp = pointers[3];
// 			buffer[3][point_temp] = received;
// 			pointers[3] = point_temp + 1;
// 			if(point_temp==25){
// 				serial_mdw_sendData(UART3, buffer[3], 26);
// 				pointers[3]=0;
// 			}
// 		}
// 		if(serial_mdw_available(UART4)>0){
// 			uint8_t received = serial_mdw_readChar(UART4) & 0xFF;
// 			uint8_t point_temp = pointers[4];
// 			buffer[4][point_temp] = received;
// 			pointers[4] = point_temp + 1;
// 			if(point_temp==25){
// 				serial_mdw_sendData(UART4, buffer[4], 26);
// 				pointers[4]=0;
// 			}
// 		}
// 		if(serial_mdw_available(USART0)>0){ 
// 			uint8_t received = serial_mdw_readChar(USART0) & 0xFF;
// 			uint8_t point_temp = pointers[5];
// 			buffer[5][point_temp] = received;
// 			pointers[5] = point_temp + 1;
// 			if(point_temp==25){
// 				serial_mdw_sendData(USART0, buffer[5], 26);
// 				pointers[5]=0;
// 			}
// 		}
// 		if(serial_mdw_available(USART1)>0){
// 			uint8_t received = serial_mdw_readChar(USART1) & 0xFF;
// 			uint8_t point_temp = pointers[6];
// 			buffer[6][point_temp] = received;
// 			pointers[6] = point_temp + 1;
// 			if(point_temp==25){
// 				//serial_mdw_sendData(USART1, buffer[6], 26);
// 				pointers[6]=0;
// 			}
// 		}
// 		
// 		if(serial_mdw_available(USART2)>0){
// 			uint8_t received = serial_mdw_readChar(USART2) & 0xFF;
// 			uint8_t point_temp = pointers[7];
// 			buffer[7][point_temp] = received;
// 			pointers[7] = point_temp + 1;
// 			if(point_temp==25){
// 				serial_mdw_sendData(USART2, buffer[7], 26);
// 				pointers[7]=0;
// 			}
// 		}

	}
}
