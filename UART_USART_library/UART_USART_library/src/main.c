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
	
	/* Setup SysTick Timer for 1 msec interrupts */
	if (SysTick_Config(sysclk_get_cpu_hz() / 1000)) {
		log_error("Error configuring the systick");
	}
	
	NVIC_ClearPendingIRQ(SysTick_IRQn);
	NVIC_EnableIRQ(SysTick_IRQn);
	
	serial_mdw_init();

	/* Configure UART for debug message output. */
	logger_init(LOG_DEBUG);
		
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
 		/*serial_mdw_send_bytes((usart_if)UART0, (const uint8_t*)"UART0", 5);
 		serial_mdw_send_bytes((usart_if)UART1, (const uint8_t*)"UART1", 5);
 		serial_mdw_send_bytes((usart_if)UART2, (const uint8_t*)"UART2", 5);
 		serial_mdw_send_bytes((usart_if)UART3, (const uint8_t*)"UART3", 5);
 		serial_mdw_send_bytes((usart_if)UART4, (const uint8_t*)"UART4", 5);
 		serial_mdw_send_bytes((usart_if)USART0, (const uint8_t*)"USART0", 6);
 		serial_mdw_send_bytes((usart_if)USART1, (const uint8_t*)"USART1", 6);
 		serial_mdw_send_bytes((usart_if)USART2, (const uint8_t*)"USART2", 6);
 		delay_ms(50);*/
		
		// 2. Reception test
		// All UART and USART are OK
		/*for (uint8_t i = 0; i<number_of_uart; i++)
		{
			if(serial_mdw_available_bytes(uart_pointers[i])>0){
				uint8_t received = 0;
				uint8_t point_temp = pointers[i];

				serial_mdw_read_byte(uart_pointers[i], &received);
				buffer[i][point_temp] = received;
				pointers[i] = point_temp + 1;
				if(pointers[i]==5){
					log_buffer("Received:", "\r\n",buffer[i], 5);
					serial_mdw_send_bytes(uart_pointers[i], buffer[i], 5);
					pointers[i] = 0;
				}
			}
		}*/
		
		// 3. Reception test with timestamp
 		for (uint8_t i = 0; i<number_of_uart; i++)
 		{
	 		if(serial_mdw_tmstp_available_bytes(uart_pointers[i])>0){
		 		s_serial_mdw_data_timestamp data_timestamp;
		 		serial_mdw_tmstp_read(uart_pointers[i], &data_timestamp);
		 		log_debug("Timestamp: %llu\r\n", data_timestamp.timestamp);
		 		log_buffer("Received:", "\r\n", data_timestamp.data, data_timestamp.length);
		 		serial_mdw_send_bytes(uart_pointers[i], data_timestamp.data, data_timestamp.length);
	 		}
 		}
 		
	}
}

