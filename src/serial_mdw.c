/*
   +========================================+
				Includes						
   +========================================+
*/
#include "serial_mdw.h"

#include <stdio.h>
#include "sysclk.h"

/// @cond 0
/**INDENT-OFF**/
#ifdef __cplusplus
extern "C" {
	#endif
	/**INDENT-ON**/
	/// @endcond


/*
   +========================================+
				Defines						
   +========================================+
*/	
#define NUMBER_OF_UART 8
	
typedef enum {
	NOT_INITIALIZED,
	INITIALIZED,
	INIT_ERROR,
	ERROR,
	OVERFLOW
} UART_status_definition_t;

typedef enum {
	UART0_pointer,
	UART1_pointer,
	UART2_pointer,
	UART3_pointer,
	UART4_pointer,
	USART0_pointer,
	USART1_pointer,
	USART2_pointer
} UART_pointer_t;

typedef struct serial_mdw_buffer_t {
	circ_bbuf_t					buffer_rx;
	circ_bbuf_t					buffer_tx;
	UART_status_definition_t	status;
	
	#ifdef SERIAL_MDW_TIMESTAMP_ACTIVATED
	UART_timestamp_t			timestamp_activated;
	timestamp_buf_t				timestamp_buff;
	uint32_t					length_data;
	uint64_t					timestamp;
	#endif
}serial_mdw_buffer_t;
	
typedef struct serial_mdw_uart_id_irq_t {
	uint8_t		id;
	IRQn_Type	irq;
	usart_if	UART;
}serial_mdw_uart_id_irq_t;

/*
   +========================================+
				Global Variables						
   +========================================+
*/

serial_mdw_uart_id_irq_t serial_mdw_uart_id_irq[NUMBER_OF_UART] = {
	{.id = 	ID_UART0,	.irq = UART0_IRQn,	.UART = (usart_if)UART0},
	{.id = 	ID_UART1,	.irq = UART1_IRQn,	.UART = (usart_if)UART1},
	{.id = 	ID_UART2,	.irq = UART2_IRQn,	.UART = (usart_if)UART2},
	{.id = 	ID_UART3,	.irq = UART3_IRQn,	.UART = (usart_if)UART3},
	{.id = 	ID_UART4,	.irq = UART4_IRQn,	.UART = (usart_if)UART4},
	{.id = 	ID_USART0,	.irq = USART0_IRQn,	.UART = (usart_if)USART0},
	{.id = 	ID_USART1,	.irq = USART1_IRQn,	.UART = (usart_if)USART1},
	{.id = 	ID_USART2,	.irq = USART2_IRQn,	.UART = (usart_if)USART2}
};
serial_mdw_buffer_t serial_mdw_buffer[NUMBER_OF_UART];
static uint8_t char_to_compare_for_timestamp[NUMBER_OF_UART] = {
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0
};

extern volatile uint64_t unix_timestamp_ms;

/*
   +========================================+
			Internal functions definition						
   +========================================+
*/
void handle_uart_interrupt(usart_if UART, UART_pointer_t uart_pointer);
void handle_usart_interrupt(usart_if UART, UART_pointer_t uart_pointer);
UART_pointer_t uart_buffer_from_UART(usart_if p_usart);

/*
   +========================================+
			Functions definition						
   +========================================+
*/
void serial_mdw_init_interface(usart_if p_usart, const usart_serial_options_t *opt, uint8_t activate_rx_interrupt)
{
	sam_uart_opt_t uart_settings;
	sam_usart_opt_t usart_settings;
	uint8_t uart_buffer = uart_buffer_from_UART(p_usart);
	
	// Define properties of interface
	if(serial_mdw_buffer[uart_buffer].status == NOT_INITIALIZED)
	{	
		// Creation of the buffers
		circ_bbuf_create_buffer(&serial_mdw_buffer[uart_buffer].buffer_rx, SERIAL_MDW_BUFFER_SIZE);
		circ_bbuf_create_buffer(&serial_mdw_buffer[uart_buffer].buffer_tx, SERIAL_MDW_BUFFER_SIZE);
		#ifdef SERIAL_MDW_TIMESTAMP_ACTIVATED
			serial_mdw_buffer[uart_buffer].timestamp_activated = TIMESTAMP_USED;
			tstp_create_buffer(&serial_mdw_buffer[uart_buffer].timestamp_buff, SERIAL_MDW_BUFFER_TIMESTAMP_SIZE);
			serial_mdw_buffer[uart_buffer].length_data = 0;
		#endif
		
		// Enable peripheral clock
		pmc_enable_periph_clk(serial_mdw_uart_id_irq[uart_buffer].id);
		
		// Configure UART/USART
		if(UART0 == (Uart*)p_usart || UART1 == (Uart*)p_usart || UART2 == (Uart*)p_usart || UART3 == (Uart*)p_usart || UART4 == (Uart*)p_usart ){
			
			uart_settings.ul_mck = sysclk_get_peripheral_hz();
			uart_settings.ul_baudrate = opt->baudrate;
			uart_settings.ul_mode = opt->paritytype;
			
			uart_init((Uart*)p_usart, &uart_settings);
			uart_disable_rx((Uart*)p_usart);
		}
		else if(USART0 == (Usart*)p_usart || USART1 == (Usart*)p_usart || USART2 == (Usart*)p_usart ){
			
			usart_settings.baudrate = opt->baudrate;
			usart_settings.char_length = opt->charlength;
			usart_settings.parity_type = opt->paritytype;
			usart_settings.stop_bits= opt->stopbits;
			usart_settings.channel_mode= US_MR_CHMODE_NORMAL;
			usart_reset((Usart*)p_usart);
			usart_init_rs232((Usart*)p_usart, &usart_settings, sysclk_get_peripheral_hz());
		}
		// Activate RX IRq if asked
		if(activate_rx_interrupt == true)
		{
			serial_mdw_enable_usart_rx_irq(p_usart);
		}
		// Activate NVIC interrupts
		#if !defined(TEST)
		NVIC_ClearPendingIRQ(serial_mdw_uart_id_irq[uart_buffer].irq);
		NVIC_EnableIRQ(serial_mdw_uart_id_irq[uart_buffer].irq);
		#endif
		// Initialization completed
		serial_mdw_buffer[uart_buffer].status = INITIALIZED;
	}
	else
	{
		// TODO: handle when initialization did not went smoothly
	}

}

void serial_mdw_enable_usart_rx_irq(usart_if p_usart)
{
	if(UART0 == (Uart*)p_usart || UART1 == (Uart*)p_usart || UART2 == (Uart*)p_usart || UART3 == (Uart*)p_usart || UART4 == (Uart*)p_usart ){
		uart_enable_rx((Uart*)p_usart);
		uart_enable_interrupt((Uart*)p_usart, UART_IER_RXRDY);
	}
	else if(USART0 == (Usart*)p_usart || USART1 == (Usart*)p_usart  || USART2 == (Usart*)p_usart ){
		usart_enable_rx((Usart*)p_usart);
		usart_enable_interrupt((Usart*)p_usart, US_IER_RXRDY);
	}
}

void serial_mdw_disable_usart_rx_irq(usart_if p_usart)
{
	if(UART0 == (Uart*)p_usart || UART1 == (Uart*)p_usart || UART2 == (Uart*)p_usart || UART3 == (Uart*)p_usart || UART4 == (Uart*)p_usart ){
		uart_disable_rx((Uart*)p_usart);
		uart_disable_interrupt((Uart*)p_usart, UART_IER_RXRDY);
	}
	else if(USART0 == (Usart*)p_usart || USART1 == (Usart*)p_usart || USART2 == (Usart*)p_usart ){
		usart_disable_rx((Usart*)p_usart);
		usart_disable_interrupt((Usart*)p_usart, US_IER_RXRDY);
	}
}

uint8_t serial_mdw_send_byte(usart_if p_usart, const uint8_t data)
{
	UART_pointer_t uart_buffer = uart_buffer_from_UART(p_usart);
		
	uint8_t status = circ_bbuf_push(&serial_mdw_buffer[uart_buffer].buffer_tx, data);
		
	if(UART0 == (Uart*)p_usart || UART1 == (Uart*)p_usart || UART2 == (Uart*)p_usart || UART3 == (Uart*)p_usart || UART4 == (Uart*)p_usart ){
		uart_enable_tx((Uart*)p_usart);
		uart_enable_interrupt((Uart*)p_usart, UART_IER_TXRDY | UART_IER_TXEMPTY);
	}
	else if(USART0 == (Usart*)p_usart || USART1 == (Usart*)p_usart || USART2 == (Usart*)p_usart ){
		usart_enable_tx((Usart*)p_usart);
		usart_enable_interrupt((Usart*)p_usart, US_IER_TXRDY | US_IER_TXEMPTY);
	}
	
	return status;
}

uint8_t serial_mdw_send_bytes(usart_if p_usart, const uint8_t *p_buff, uint32_t ulsize)
{
	UART_pointer_t uart_buffer = uart_buffer_from_UART(p_usart);
	
	uint8_t status = circ_bbuf_push_bytes(&serial_mdw_buffer[uart_buffer].buffer_tx, p_buff, ulsize);

	if(UART0 == (Uart*)p_usart || UART1 == (Uart*)p_usart || UART2 == (Uart*)p_usart || UART3 == (Uart*)p_usart || UART4 == (Uart*)p_usart ){
		uart_enable_tx((Uart*)p_usart);
		uart_enable_interrupt((Uart*)p_usart, UART_IER_TXRDY | UART_IER_TXEMPTY);
	}
	else if(USART0 == (Usart*)p_usart || USART1 == (Usart*)p_usart || USART2 == (Usart*)p_usart ){
		usart_enable_tx((Usart*)p_usart);
		usart_enable_interrupt((Usart*)p_usart, US_IER_TXRDY | US_IER_TXEMPTY);
	}
	
	return status;
}

uint8_t serial_mdw_available(void)
{
	uint8_t available_in_buffer = 0;
	for(uint8_t i = 0; i < NUMBER_OF_UART; i++)
	{	
		#if defined(SERIAL_MDW_TIMESTAMP_ACTIVATED)
		if (tstp_buf_is_empty(&serial_mdw_buffer[i].timestamp_buff) == false)
		{
			available_in_buffer |= 1 << i;
		}
		#else 
		if (circ_bbuf_is_empty(&serial_mdw_buffer[i].buffer_rx) == false )
		{
			available_in_buffer |= 1 << i;
		}
		#endif
	}
	
	return available_in_buffer;
}

uint32_t serial_mdw_available_bytes(usart_if p_usart)
{
	uint8_t uart_buffer = uart_buffer_from_UART(p_usart);
	return circ_bbuf_available_bytes_to_read(&serial_mdw_buffer[uart_buffer].buffer_rx);
}

uint32_t serial_mdw_available_bytes_tx(usart_if p_usart)
{
	uint8_t uart_buffer = uart_buffer_from_UART(p_usart);
	return circ_bbuf_available_bytes_to_read(&serial_mdw_buffer[uart_buffer].buffer_tx);
}

uint8_t serial_mdw_read_byte(usart_if p_usart, uint8_t *data)
{
	uint8_t uart_buffer = uart_buffer_from_UART(p_usart);
	uint8_t success = circ_bbuf_pop(&serial_mdw_buffer[uart_buffer].buffer_rx, data);

	return success;
}

uint8_t serial_mdw_read_bytes(usart_if p_usart, uint8_t *p_buff, uint32_t ulsize)
{
	uint8_t uart_buffer = uart_buffer_from_UART(p_usart);
	uint8_t success = circ_bbuf_pop_bytes(&serial_mdw_buffer[uart_buffer].buffer_rx, ulsize, p_buff);

	return success;
}

#if defined(SERIAL_MDW_TIMESTAMP_ACTIVATED)
void serial_mdw_set_compare_trigger(usart_if p_usart, const uint8_t data)
{
    UART_pointer_t uart_buffer = uart_buffer_from_UART(p_usart);
    char_to_compare_for_timestamp[uart_buffer] = data;

}
uint32_t serial_mdw_timestamp_available(usart_if p_usart)
{
	UART_pointer_t uart_buffer = uart_buffer_from_UART(p_usart);
	uint32_t number_of_bytes = 0;
	
	number_of_bytes = tstp_available_to_read(&serial_mdw_buffer[uart_buffer].timestamp_buff);

	
	return number_of_bytes;
}
uint8_t serial_mdw_timestamp_read(usart_if p_usart, serial_mdw_data_timestamp_t *data_timestamp)
{
	UART_pointer_t uart_buffer = uart_buffer_from_UART(p_usart);
	uint8_t result_read = 0;

	timestamp_t timestamp;

	result_read = tstp_buf_pop(&serial_mdw_buffer[uart_buffer].timestamp_buff, &timestamp);
	data_timestamp->timestamp = timestamp.timestamp;
	data_timestamp->length = timestamp.length;
	
	if(result_read == TB_SUCCESS)
	{
		uint8_t *data = (uint8_t *)malloc(sizeof(uint8_t) * timestamp.length);
		result_read = circ_bbuf_pop_bytes(&serial_mdw_buffer[uart_buffer].buffer_rx, timestamp.length, data);
		data_timestamp->data = data;
	}
	

	return result_read;
}

#endif
	
void UART0_Handler(void)
{
	uint8_t uc_char;
	uint32_t ul_status;

	// Read UART status.
	ul_status = uart_get_status(UART0);
		
	// Transmit interrupt
	if(ul_status & (UART_IER_TXRDY | UART_IER_TXEMPTY)) {
		if (!circ_bbuf_is_empty(&serial_mdw_buffer[UART0_pointer].buffer_tx))
		{
			circ_bbuf_pop(&serial_mdw_buffer[UART0_pointer].buffer_tx, &uc_char);
			uart_write(UART0, uc_char);
		}else
		{
			// Nothing more to transmit for now, deactivating interrupt
			uart_disable_interrupt(UART0, (UART_IER_TXRDY | UART_IER_TXEMPTY));
			uart_disable_tx(UART0);
		}
	}
	// Receive interrupt
	if (ul_status & UART_SR_RXRDY ) {
		uart_read(UART0, &uc_char);

		if(!circ_bbuf_push(&serial_mdw_buffer[UART0_pointer].buffer_rx, uc_char))
		{
			// Check if timestamp has to be acquired
			#ifdef SERIAL_MDW_TIMESTAMP_ACTIVATED
			serial_mdw_buffer[UART0_pointer].length_data += 1;

			if (serial_mdw_buffer[UART0_pointer].timestamp == 0)
			{
				serial_mdw_buffer[UART0_pointer].timestamp = unix_timestamp_ms;
			}
			// If character matches, then next character has to be timestamp and can be saved into timestamp buffer
			if(uc_char == char_to_compare_for_timestamp[UART0_pointer] || circ_bbuf_is_full(&serial_mdw_buffer[UART0_pointer].buffer_rx))
			{
				timestamp_t timestamp;
				// put info into struct
				timestamp.position = serial_mdw_buffer[UART0_pointer].buffer_rx.head;
				timestamp.timestamp = serial_mdw_buffer[UART0_pointer].timestamp;
				timestamp.length = serial_mdw_buffer[UART0_pointer].length_data;
				// push struct into buffer
				tstp_buf_push(&serial_mdw_buffer[UART0_pointer].timestamp_buff, &timestamp);
				// reset
				serial_mdw_buffer[UART0_pointer].timestamp = 0;
				serial_mdw_buffer[UART0_pointer].length_data = 0;
			}
			#endif
		}
	}
}
	
void UART1_Handler(void)
{
	uint8_t uc_char;
	uint32_t ul_status;

	// Read UART status.
	ul_status = uart_get_status(UART1);
		
	// Transmit interrupt
	if(ul_status & (UART_IER_TXRDY | UART_IER_TXEMPTY)) {
		if (!circ_bbuf_is_empty(&serial_mdw_buffer[UART1_pointer].buffer_tx))
		{
			circ_bbuf_pop(&serial_mdw_buffer[UART1_pointer].buffer_tx, &uc_char);
			uart_write(UART1, uc_char);
		}else
		{
			// Nothing more to transmit for now, deactivating interrupt
			uart_disable_interrupt(UART1, (UART_IER_TXRDY | UART_IER_TXEMPTY));
			uart_disable_tx(UART1);
		}
	}
	// Receive interrupt
	if (ul_status & UART_SR_RXRDY ) {
		uart_read(UART1, &uc_char);

		if(!circ_bbuf_push(&serial_mdw_buffer[UART1_pointer].buffer_rx, uc_char))
		{
			// Check if timestamp has to be acquired
			#ifdef SERIAL_MDW_TIMESTAMP_ACTIVATED
			serial_mdw_buffer[UART1_pointer].length_data += 1;

			if (serial_mdw_buffer[UART1_pointer].timestamp == 0)
			{
				serial_mdw_buffer[UART1_pointer].timestamp = unix_timestamp_ms;
			}
			// If character matches, then next character has to be timestamp and can be saved into timestamp buffer
			if(uc_char == char_to_compare_for_timestamp[UART1_pointer] || circ_bbuf_is_full(&serial_mdw_buffer[UART1_pointer].buffer_rx))
			{
				timestamp_t timestamp;
				// put info into struct
				timestamp.position = serial_mdw_buffer[UART1_pointer].buffer_rx.head;
				timestamp.timestamp = serial_mdw_buffer[UART1_pointer].timestamp;
				timestamp.length = serial_mdw_buffer[UART1_pointer].length_data;
				// push struct into buffer
				tstp_buf_push(&serial_mdw_buffer[UART1_pointer].timestamp_buff, &timestamp);
				// reset
				serial_mdw_buffer[UART1_pointer].timestamp = 0;
				serial_mdw_buffer[UART1_pointer].length_data = 0;
			}
			#endif
		}
	}
}
	
void UART2_Handler(void)
{
	uint8_t uc_char;
	uint32_t ul_status;

	// Read UART status.
	ul_status = uart_get_status(UART2);
	
	// Transmit interrupt
	if(ul_status & (UART_IER_TXRDY | UART_IER_TXEMPTY)) {
		if (!circ_bbuf_is_empty(&serial_mdw_buffer[UART2_pointer].buffer_tx))
		{
			circ_bbuf_pop(&serial_mdw_buffer[UART2_pointer].buffer_tx, &uc_char);
			uart_write(UART2, uc_char);
		}else
		{
			// Nothing more to transmit for now, deactivating interrupt
			uart_disable_interrupt(UART2, (UART_IER_TXRDY | UART_IER_TXEMPTY));
			uart_disable_tx(UART2);
		}
	}
	// Receive interrupt
	if (ul_status & UART_SR_RXRDY ) {
		uart_read(UART2, &uc_char);

		if(!circ_bbuf_push(&serial_mdw_buffer[UART2_pointer].buffer_rx, uc_char))
		{
			// Check if timestamp has to be acquired
			#ifdef SERIAL_MDW_TIMESTAMP_ACTIVATED
			serial_mdw_buffer[UART2_pointer].length_data += 1;

			if (serial_mdw_buffer[UART2_pointer].timestamp == 0)
			{
				serial_mdw_buffer[UART2_pointer].timestamp = unix_timestamp_ms;
			}
			// If character matches, then next character has to be timestamp and can be saved into timestamp buffer
			if(uc_char == char_to_compare_for_timestamp[UART2_pointer] || circ_bbuf_is_full(&serial_mdw_buffer[UART2_pointer].buffer_rx))
			{
				timestamp_t timestamp;
				// put info into struct
				timestamp.position = serial_mdw_buffer[UART2_pointer].buffer_rx.head;
				timestamp.timestamp = serial_mdw_buffer[UART2_pointer].timestamp;
				timestamp.length = serial_mdw_buffer[UART2_pointer].length_data;
				// push struct into buffer
				tstp_buf_push(&serial_mdw_buffer[UART2_pointer].timestamp_buff, &timestamp);
				// reset
				serial_mdw_buffer[UART2_pointer].timestamp = 0;
				serial_mdw_buffer[UART2_pointer].length_data = 0;
			}
			#endif
		}
	}
}
	
void UART3_Handler(void)
{
	uint8_t uc_char;
	uint32_t ul_status;

	// Read UART status.
	ul_status = uart_get_status(UART3);

	// Transmit interrupt
	if(ul_status & (UART_IER_TXRDY | UART_IER_TXEMPTY)) {
		if (!circ_bbuf_is_empty(&serial_mdw_buffer[UART3_pointer].buffer_tx))
		{
			circ_bbuf_pop(&serial_mdw_buffer[UART3_pointer].buffer_tx, &uc_char);
			uart_write(UART3, uc_char);
		}else
		{
			// Nothing more to transmit for now, deactivating interrupt
			uart_disable_interrupt(UART3, (UART_IER_TXRDY | UART_IER_TXEMPTY));
			uart_disable_tx(UART3);
		}
	}
	// Receive interrupt
	if (ul_status & UART_SR_RXRDY ) {
		uart_read(UART3, &uc_char);

		if(!circ_bbuf_push(&serial_mdw_buffer[UART3_pointer].buffer_rx, uc_char))
		{
			// Check if timestamp has to be acquired
			#ifdef SERIAL_MDW_TIMESTAMP_ACTIVATED
			serial_mdw_buffer[UART3_pointer].length_data += 1;

			if (serial_mdw_buffer[UART3_pointer].timestamp == 0)
			{
				serial_mdw_buffer[UART3_pointer].timestamp = unix_timestamp_ms;
			}
			// If character matches, then next character has to be timestamp and can be saved into timestamp buffer
			if(uc_char == char_to_compare_for_timestamp[UART3_pointer] || circ_bbuf_is_full(&serial_mdw_buffer[UART3_pointer].buffer_rx))
			{
				timestamp_t timestamp;
				// put info into struct
				timestamp.position = serial_mdw_buffer[UART3_pointer].buffer_rx.head;
				timestamp.timestamp = serial_mdw_buffer[UART3_pointer].timestamp;
				timestamp.length = serial_mdw_buffer[UART3_pointer].length_data;
				// push struct into buffer
				tstp_buf_push(&serial_mdw_buffer[UART3_pointer].timestamp_buff, &timestamp);
				// reset
				serial_mdw_buffer[UART3_pointer].timestamp = 0;
				serial_mdw_buffer[UART3_pointer].length_data = 0;
			}
			#endif
		}
	}
}
	
void UART4_Handler(void)
{
	uint8_t uc_char;
	uint32_t ul_status;

	// Read UART status.
	ul_status = uart_get_status(UART4);

	// Transmit interrupt
	if(ul_status & (UART_IER_TXRDY | UART_IER_TXEMPTY)) {
		if (!circ_bbuf_is_empty(&serial_mdw_buffer[UART4_pointer].buffer_tx))
		{
			circ_bbuf_pop(&serial_mdw_buffer[UART4_pointer].buffer_tx, &uc_char);
			uart_write(UART4, uc_char);
		}else
		{
			// Nothing more to transmit for now, deactivating interrupt
			uart_disable_interrupt(UART4, (UART_IER_TXRDY | UART_IER_TXEMPTY));
			uart_disable_tx(UART4);
		}
	}
	// Receive interrupt
	if (ul_status & UART_SR_RXRDY ) {
		uart_read(UART4, &uc_char);

		if(!circ_bbuf_push(&serial_mdw_buffer[UART4_pointer].buffer_rx, uc_char))
		{
			// Check if timestamp has to be acquired
			#ifdef SERIAL_MDW_TIMESTAMP_ACTIVATED
			serial_mdw_buffer[UART4_pointer].length_data += 1;

			if (serial_mdw_buffer[UART4_pointer].timestamp == 0)
			{
				serial_mdw_buffer[UART4_pointer].timestamp = unix_timestamp_ms;
			}
			// If character matches, then next character has to be timestamp and can be saved into timestamp buffer
			if(uc_char == char_to_compare_for_timestamp[UART4_pointer] || circ_bbuf_is_full(&serial_mdw_buffer[UART4_pointer].buffer_rx))
			{
				timestamp_t timestamp;
				// put info into struct
				timestamp.position = serial_mdw_buffer[UART4_pointer].buffer_rx.head;
				timestamp.timestamp = serial_mdw_buffer[UART4_pointer].timestamp;
				timestamp.length = serial_mdw_buffer[UART4_pointer].length_data;
				// push struct into buffer
				tstp_buf_push(&serial_mdw_buffer[UART4_pointer].timestamp_buff, &timestamp);
				// reset
				serial_mdw_buffer[UART4_pointer].timestamp = 0;
				serial_mdw_buffer[UART4_pointer].length_data = 0;
			}
			#endif
		}
	}
}
	
void USART0_Handler(void)
{
	uint32_t uc_char;
	uint32_t ul_status;
	uint8_t temp_char;

	// Read UART status.
	ul_status = usart_get_status(USART0);
	
	// Transmit interrupt
	if(ul_status & (US_CSR_TXRDY | US_CSR_TXEMPTY)) {
		if (!circ_bbuf_is_empty(&serial_mdw_buffer[USART0_pointer].buffer_tx))
		{
			circ_bbuf_pop(&serial_mdw_buffer[USART0_pointer].buffer_tx, &temp_char);
			uc_char = (uint32_t)temp_char;
			usart_write(USART0, uc_char);
		}else
		{
			// Nothing more to transmit for now, deactivating interrupt
			usart_disable_interrupt(USART0, (US_IER_TXRDY | US_IER_TXEMPTY));
			usart_disable_tx(USART0);
		}
	}
	// Receive interrupt
	if (ul_status & US_CSR_RXRDY ) {
		usart_read(USART0, &uc_char);
		
		if(!circ_bbuf_push(&serial_mdw_buffer[USART0_pointer].buffer_rx, uc_char))
		{
			// Check if timestamp has to be acquired
			#ifdef SERIAL_MDW_TIMESTAMP_ACTIVATED
			serial_mdw_buffer[USART0_pointer].length_data += 1;

			if (serial_mdw_buffer[USART0_pointer].timestamp == 0)
			{
				serial_mdw_buffer[USART0_pointer].timestamp = unix_timestamp_ms;
			}
			// If character matches, then next character has to be timestamp and can be saved into timestamp buffer
			if(uc_char == char_to_compare_for_timestamp[USART0_pointer] || circ_bbuf_is_full(&serial_mdw_buffer[USART0_pointer].buffer_rx))
			{
				timestamp_t timestamp;
				// put info into struct
				timestamp.position = serial_mdw_buffer[USART0_pointer].buffer_rx.head;
				timestamp.timestamp = serial_mdw_buffer[USART0_pointer].timestamp;
				timestamp.length = serial_mdw_buffer[USART0_pointer].length_data;
				// push struct into buffer
				tstp_buf_push(&serial_mdw_buffer[USART0_pointer].timestamp_buff, &timestamp);
				// reset
				serial_mdw_buffer[USART0_pointer].timestamp = 0;
				serial_mdw_buffer[USART0_pointer].length_data = 0;
			}
			#endif
		}
	}
}
	
void USART1_Handler(void)
{
	uint32_t uc_char;
	uint32_t ul_status;
	uint8_t temp_char;

	// Read UART status.
	ul_status = usart_get_status(USART1);
		
	// Transmit interrupt
	if(ul_status & (US_CSR_TXRDY | US_CSR_TXEMPTY)) {
		if (!circ_bbuf_is_empty(&serial_mdw_buffer[USART1_pointer].buffer_tx))
		{
			circ_bbuf_pop(&serial_mdw_buffer[USART1_pointer].buffer_tx, &temp_char);
			uc_char = (uint32_t)temp_char;
			usart_write(USART1, uc_char);
		}else
		{
			// Nothing more to transmit for now, deactivating interrupt
			usart_disable_interrupt(USART1, (US_IER_TXRDY | US_IER_TXEMPTY));
			usart_disable_tx(USART1);
		}
	}
	// Receive interrupt
	if (ul_status & US_CSR_RXRDY ) {
		usart_read(USART1, &uc_char);
		
		if(!circ_bbuf_push(&serial_mdw_buffer[USART1_pointer].buffer_rx, uc_char))
		{
			// Check if timestamp has to be acquired
			#ifdef SERIAL_MDW_TIMESTAMP_ACTIVATED
			serial_mdw_buffer[USART1_pointer].length_data += 1;

			if (serial_mdw_buffer[USART1_pointer].timestamp == 0)
			{
				serial_mdw_buffer[USART1_pointer].timestamp = unix_timestamp_ms;
			}
			// If character matches, then next character has to be timestamp and can be saved into timestamp buffer
			if(uc_char == char_to_compare_for_timestamp[USART1_pointer] || circ_bbuf_is_full(&serial_mdw_buffer[USART1_pointer].buffer_rx))
			{
				timestamp_t timestamp;
				// put info into struct
				timestamp.position = serial_mdw_buffer[USART1_pointer].buffer_rx.head;
				timestamp.timestamp = serial_mdw_buffer[USART1_pointer].timestamp;
				timestamp.length = serial_mdw_buffer[USART1_pointer].length_data;
				// push struct into buffer
				tstp_buf_push(&serial_mdw_buffer[USART1_pointer].timestamp_buff, &timestamp);
				// reset
				serial_mdw_buffer[USART1_pointer].timestamp = 0;
				serial_mdw_buffer[USART1_pointer].length_data = 0;
			}
			#endif
		}
	}
}
	
void USART2_Handler(void)
{
	uint32_t uc_char;
	uint32_t ul_status;
	uint8_t temp_char;

	// Read UART status.
	ul_status = usart_get_status(USART2);
	
	// Transmit interrupt
	if(ul_status & (US_CSR_TXRDY | US_CSR_TXEMPTY)) {
		if (!circ_bbuf_is_empty(&serial_mdw_buffer[USART2_pointer].buffer_tx))
		{
			circ_bbuf_pop(&serial_mdw_buffer[USART2_pointer].buffer_tx, &temp_char);
			uc_char = (uint32_t)temp_char;
			usart_write(USART2, uc_char);
		}else
		{
			// Nothing more to transmit for now, deactivating interrupt
			usart_disable_interrupt(USART2, (US_IER_TXRDY | US_IER_TXEMPTY));
			usart_disable_tx(USART2);
		}
	}
	// Receive interrupt
	if (ul_status & US_CSR_RXRDY ) {
		usart_read(USART2, &uc_char);
		
		if(!circ_bbuf_push(&serial_mdw_buffer[USART2_pointer].buffer_rx, uc_char))
		{
			// Check if timestamp has to be acquired
			#ifdef SERIAL_MDW_TIMESTAMP_ACTIVATED
			serial_mdw_buffer[USART2_pointer].length_data += 1;

			if (serial_mdw_buffer[USART2_pointer].timestamp == 0)
			{
				serial_mdw_buffer[USART2_pointer].timestamp = unix_timestamp_ms;
			}
			// If character matches, then next character has to be timestamp and can be saved into timestamp buffer
			if(uc_char == char_to_compare_for_timestamp[USART2_pointer] || circ_bbuf_is_full(&serial_mdw_buffer[USART2_pointer].buffer_rx))
			{
				timestamp_t timestamp;
				// put info into struct
				timestamp.position = serial_mdw_buffer[USART2_pointer].buffer_rx.head;
				timestamp.timestamp = serial_mdw_buffer[USART2_pointer].timestamp;
				timestamp.length = serial_mdw_buffer[USART2_pointer].length_data;
				// push struct into buffer
				tstp_buf_push(&serial_mdw_buffer[USART2_pointer].timestamp_buff, &timestamp);
				// reset
				serial_mdw_buffer[USART2_pointer].timestamp = 0;
				serial_mdw_buffer[USART2_pointer].length_data = 0;
			}
			#endif
		}
	}
}
	
UART_pointer_t uart_buffer_from_UART(usart_if p_usart){
		
	UART_pointer_t uart_buffer = UART0_pointer;
		
	if((Uart*)UART0 == (Uart*)p_usart){
		uart_buffer = UART0_pointer;
	}else if((Uart*)UART1 == (Uart*)p_usart){
		uart_buffer = UART1_pointer;
	}else if((Uart*)UART2 == (Uart*)p_usart){
		uart_buffer = UART2_pointer;
	}else if((Uart*)UART3 == (Uart*)p_usart){
		uart_buffer = UART3_pointer;
	}else if((Uart*)UART4 == (Uart*)p_usart){
		uart_buffer = UART4_pointer;
	}else if((Usart*)USART0 == (Usart*)p_usart){
		uart_buffer = USART0_pointer;
	}else if((Usart*)USART1 == (Usart*)p_usart){
		uart_buffer = USART1_pointer;
	}else if((Usart*)USART2 == (Usart*)p_usart){
		uart_buffer = USART2_pointer;
	};
		
	return uart_buffer;
}
	
	/// @cond 0
	/**INDENT-OFF**/
	#ifdef __cplusplus
}
#endif

/**INDENT-ON**/
/// @endcond

