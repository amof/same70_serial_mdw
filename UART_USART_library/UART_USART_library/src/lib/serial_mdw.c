#include "serial_mdw.h"


/// @cond 0
/**INDENT-OFF**/
#ifdef __cplusplus
extern "C" {
	#endif
	/**INDENT-ON**/
	/// @endcond
	
	/**
	* \defgroup sam_drivers_usart_group Universal Synchronous Asynchronous
	* Receiver Transmitter (USART)
	*
	* The Universal Synchronous Asynchronous Receiver Transceiver (USART)
	* provides one full duplex universal synchronous asynchronous serial link.
	* Data frame format is widely programmable (data length, parity, number of
	* stop bits) to support a maximum of standards. The receiver implements
	* parity error, framing error and overrun error detection. The receiver
	* time-out enables handling variable-length frames and the transmitter
	* timeguard facilitates communications with slow remote devices. Multidrop
	* communications are also supported through address bit handling in reception
	* and transmission. The driver supports the following modes:
	* RS232, RS485, SPI, IrDA, ISO7816, MODEM, Hardware handshaking and LIN.
	*
	* @{
	*/
	
	typedef enum 
	{
		NOT_INITIALIZED, 
		INITIALIZED, 
		INIT_ERROR, 
		ERROR, 
		OVERFLOW
	} UART_status_definition;
	
	typedef enum
	{
		UART0_pointer,
		UART1_pointer,
		UART2_pointer,
		UART3_pointer,
		UART4_pointer,
		USART0_pointer,
		USART1_pointer,
		USART2_pointer
	} UART_pointer;
	
	typedef struct s_serial_mdw_buffer_t
	{
		circ_bbuf_t buffer_rx;
		circ_bbuf_t buffer_tx;
		UART_status_definition status;
		
		#ifdef ACTIVATE_TIMESTAMP_RECORDING
		timestamp_buf_t timestamp_buff;
		uint32_t length_data;
		uint64_t timestamp;
		#endif

	}s_serial_mdw_buffer;

	s_serial_mdw_buffer serial_mdw_buffer[number_of_uart];
	
	static usart_if logger_uart = NULL;
	
	void handle_uart_interrupt(usart_if UART, UART_pointer uart_pointer);
	void handle_usart_interrupt(usart_if UART, UART_pointer uart_pointer);
	uint8_t uart_buffer_from_UART(usart_if p_usart);

	
void serial_mdw_init(void)
{
	// Map the buffers to UART/USART
	for(uint8_t i=0; i<number_of_uart; i++)
	{
		circ_bbuf_create_buffer(&serial_mdw_buffer[i].buffer_rx, SERIAL_MDW_BUFFER_SIZE);
		circ_bbuf_create_buffer(&serial_mdw_buffer[i].buffer_tx, SERIAL_MDW_BUFFER_SIZE);
		#ifdef ACTIVATE_TIMESTAMP_RECORDING
		tstp_create_buffer(&serial_mdw_buffer[i].timestamp_buff, SERIAL_MDW_BUFFER_TIMESTAMP_SIZE);
		#endif
	}
	
	// Set status of UART/USART
	for(uint8_t i=0; i<number_of_uart; i++){
		serial_mdw_buffer[i].status = NOT_INITIALIZED;
		#ifdef ACTIVATE_TIMESTAMP_RECORDING
		serial_mdw_buffer[i].length_data = 0;
		#endif
	}
	
}
	
void serial_mdw_init_interface(usart_if p_usart, const usart_serial_options_t *opt)
{
	sam_uart_opt_t uart_settings;
	sam_usart_opt_t usart_settings;
	uint8_t uart_buffer = uart_buffer_from_UART(p_usart);
	
	// Define properties of interface
	if(UART0 == (Uart*)p_usart || UART1 == (Uart*)p_usart || UART2 == (Uart*)p_usart || UART3 == (Uart*)p_usart || UART4 == (Uart*)p_usart ){
		uart_settings.ul_mck = sysclk_get_peripheral_hz();
		uart_settings.ul_baudrate = opt->baudrate;
		uart_settings.ul_mode = opt->paritytype;
	}
	else if(USART0 == (Usart*)p_usart || USART1 == (Usart*)p_usart || USART2 == (Usart*)p_usart ){
		usart_settings.baudrate = opt->baudrate;
		usart_settings.char_length = opt->charlength;
		usart_settings.parity_type = opt->paritytype;
		usart_settings.stop_bits= opt->stopbits;
		usart_settings.channel_mode= US_MR_CHMODE_NORMAL;
	}
	
	// Initialize interface
	if(serial_mdw_buffer[uart_buffer].status == INITIALIZED || p_usart == (usart_if)logger_uart){
		SRL_MDW_DEBUGF("Interface already initialized");
	}
	else if(serial_mdw_buffer[uart_buffer].status != NOT_INITIALIZED){
		SRL_MDW_DEBUGF("Interface has encountered some error in the past.");
	}
	else if(UART0 == (Uart*)p_usart){
		sysclk_enable_peripheral_clock(ID_UART0);
		uart_init(UART0, &uart_settings);
		NVIC_ClearPendingIRQ(UART0_IRQn);
		NVIC_EnableIRQ(UART0_IRQn);
		uart_enable_interrupt(UART0, UART_IER_RXRDY);
		serial_mdw_buffer[uart_buffer].status = INITIALIZED;
		SRL_MDW_DEBUGF("UART0 initialized");
	}
	else if(UART1 == (Uart*)p_usart){
		sysclk_enable_peripheral_clock(ID_UART1);
		uart_init(UART1, &uart_settings);
		NVIC_ClearPendingIRQ(UART1_IRQn);
		NVIC_EnableIRQ(UART1_IRQn);
		uart_enable_interrupt(UART1, UART_IER_RXRDY);
		serial_mdw_buffer[uart_buffer].status = INITIALIZED;
		SRL_MDW_DEBUGF("UART1 initialized");
	}
	else if(UART2 == (Uart*)p_usart){
		sysclk_enable_peripheral_clock(ID_UART2);
		uart_init(UART2, &uart_settings);
		NVIC_ClearPendingIRQ(UART2_IRQn);
		NVIC_EnableIRQ(UART2_IRQn);
		uart_enable_interrupt(UART2, UART_IER_RXRDY);
		serial_mdw_buffer[uart_buffer].status = INITIALIZED;
		SRL_MDW_DEBUGF("UART2 initialized");
	}
	else if(UART3 == (Uart*)p_usart){
		sysclk_enable_peripheral_clock(ID_UART3);
		uart_init(UART3, &uart_settings);
		NVIC_ClearPendingIRQ(UART3_IRQn);
		NVIC_EnableIRQ(UART3_IRQn);
		uart_enable_interrupt(UART3, UART_IER_RXRDY);
		serial_mdw_buffer[uart_buffer].status = INITIALIZED;
		SRL_MDW_DEBUGF("UART3 initialized");
	}
	else if(UART4 == (Uart*)p_usart){
		sysclk_enable_peripheral_clock(ID_UART4);
		uart_init(UART4, &uart_settings);
		NVIC_ClearPendingIRQ(UART4_IRQn);
		NVIC_EnableIRQ(UART4_IRQn);
		uart_enable_interrupt(UART4, UART_IER_RXRDY);
		serial_mdw_buffer[uart_buffer].status = INITIALIZED;
		SRL_MDW_DEBUGF("UART4 initialized");
	}
	else if(USART0 == (Usart*)p_usart){
		sysclk_enable_peripheral_clock(ID_USART0);
		usart_init_rs232(USART0, &usart_settings, sysclk_get_peripheral_hz());
		usart_enable_rx(USART0);
		NVIC_ClearPendingIRQ(USART0_IRQn);
		NVIC_EnableIRQ(USART0_IRQn);
		usart_enable_interrupt(USART0, US_IER_RXRDY);
		serial_mdw_buffer[uart_buffer].status = INITIALIZED;
		SRL_MDW_DEBUGF("USART0 initialized");
	}
	else if(USART1 == (Usart*)p_usart){
		sysclk_enable_peripheral_clock(ID_USART1);
		usart_init_rs232(USART1, &usart_settings, sysclk_get_peripheral_hz());
		usart_enable_rx(USART1);
		NVIC_ClearPendingIRQ(USART1_IRQn);
		NVIC_EnableIRQ(USART1_IRQn);
		usart_enable_interrupt(USART1, US_IER_RXRDY);
		serial_mdw_buffer[uart_buffer].status = INITIALIZED;
		SRL_MDW_DEBUGF("USART1 initialized");
	}
	else if(USART2 == (Usart*)p_usart){
		sysclk_enable_peripheral_clock(ID_USART2);
		usart_init_rs232(USART2, &usart_settings, sysclk_get_peripheral_hz());
		usart_enable_rx(USART2);
		NVIC_ClearPendingIRQ(USART2_IRQn);
		NVIC_EnableIRQ(USART2_IRQn);
		usart_enable_interrupt(USART2, US_IER_RXRDY);
		serial_mdw_buffer[uart_buffer].status = INITIALIZED;
		SRL_MDW_DEBUGF("USART2 initialized");
	}
	else{
		SRL_MDW_DEBUGF("Unknown error while initializing UART");
		serial_mdw_buffer[uart_buffer].status = INIT_ERROR;
	}

}

void serial_mdw_set_logInterface(usart_if p_usart)
{
	logger_uart = p_usart;
	SRL_MDW_DEBUGF("This interface is used for logging");
}

/**
* \brief Sends a character with the USART.
*
* \param p_usart	Base address of the USART instance.
* \param c			Character to write.
*
* \return status.
* ready to send.
*/
uint8_t serial_mdw_send_byte(usart_if p_usart, const uint8_t c)
{
	uint8_t uart_buffer = uart_buffer_from_UART(p_usart);
		
	uint8_t status = circ_bbuf_push(&serial_mdw_buffer[uart_buffer].buffer_tx, c);
		
	if(UART0 == (Uart*)p_usart || UART1 == (Uart*)p_usart || UART2 == (Uart*)p_usart || UART3 == (Uart*)p_usart || UART4 == (Uart*)p_usart ){
		uart_enable_tx((Uart*)p_usart);
		uart_enable_interrupt((Uart*)p_usart, UART_IER_TXRDY | UART_IER_TXEMPTY);
	}
	else if(USART0 == (Usart*)p_usart || USART1 == (Usart*)p_usart || USART2 == (Usart*)p_usart ){
		usart_enable_tx((Usart*)p_usart);
		usart_enable_interrupt((Usart*)p_usart, UART_IER_TXRDY | UART_IER_TXEMPTY);
	}
	
	return status;
}

/**
* \brief Send multiples characters across the desired usart.
*
* \param p_usart	Base address of the USART instance.
* \param			.
*
* \return None
*/
uint8_t serial_mdw_send_bytes(usart_if p_usart, const uint8_t *p_buff, uint32_t ulsize)
{
		
	uint8_t uart_buffer = uart_buffer_from_UART(p_usart);
	
	uint8_t status = circ_bbuf_push_bytes(&serial_mdw_buffer[uart_buffer].buffer_tx, p_buff, ulsize);

	if(UART0 == (Uart*)p_usart || UART1 == (Uart*)p_usart || UART2 == (Uart*)p_usart || UART3 == (Uart*)p_usart || UART4 == (Uart*)p_usart ){
		uart_enable_tx((Uart*)p_usart);
		uart_enable_interrupt((Uart*)p_usart, UART_IER_TXRDY | UART_IER_TXEMPTY);
	}
	else if(USART0 == (Usart*)p_usart || USART1 == (Usart*)p_usart || USART2 == (Usart*)p_usart ){
		usart_enable_tx((Usart*)p_usart);
		usart_enable_interrupt((Usart*)p_usart, UART_IER_TXRDY | UART_IER_TXEMPTY);
	}
	
	return status;
}
	
void handle_uart_interrupt(usart_if UART, UART_pointer uart_pointer)
{
	uint8_t uc_char;
	uint32_t ul_status;

	// Read UART status.
	ul_status = uart_get_status((Uart*)UART);
		
	// Transmit interrupt
	if(ul_status & (UART_IER_TXRDY | UART_IER_TXEMPTY)) {
		if (!circ_bbuf_is_empty(&serial_mdw_buffer[uart_pointer].buffer_tx)) {
			circ_bbuf_pop(&serial_mdw_buffer[uart_pointer].buffer_tx, &uc_char);
			uart_write((Uart*)UART, uc_char);
			}else{
			// Nothing more to transmit for now, deactivating interrupt
			uart_disable_interrupt((Uart*)UART, (UART_IER_TXRDY | UART_IER_TXEMPTY));
			uart_disable_tx((Uart*)UART);
		}
	}
	// Receive interrupt
	if (ul_status & UART_SR_RXRDY ) {
		uart_read((Uart*)UART, &uc_char);
		circ_bbuf_push(&serial_mdw_buffer[uart_pointer].buffer_rx, uc_char);
		/*if(!circ_bbuf_push(&serial_mdw_buffer[uart_pointer].buffer_rx, uc_char))
		{
			// Check if timestamp has to be acquired
			#ifdef ACTIVATE_TIMESTAMP_RECORDING
			serial_mdw_buffer[uart_pointer].length_data += 1;

			if (serial_mdw_buffer[uart_pointer].timestamp == 0)
			{
				serial_mdw_buffer[uart_pointer].timestamp = unix_timestamp_ms;
			}
			// If character matches, then next character has to be timestamp and can be saved into timestamp buffer
			if(uc_char == char_to_compare_for_timestamp)
			{
				timestamp_t timestamp;
				// put info into struct
				timestamp.position = serial_mdw_buffer[uart_pointer].buffer_rx.head;
				timestamp.timestamp = serial_mdw_buffer[uart_pointer].timestamp;
				timestamp.length = serial_mdw_buffer[uart_pointer].length_data;
				// push struct into buffer
				tstp_buf_push(&serial_mdw_buffer[uart_pointer].timestamp_buff, &timestamp);
				//reset
				serial_mdw_buffer[uart_pointer].timestamp = 0;
				serial_mdw_buffer[uart_pointer].length_data = 0;

			}
			#endif
		}*/
	}
}

void handle_usart_interrupt(usart_if USART, UART_pointer uart_pointer)
{
	uint32_t uc_char;
	uint32_t ul_status;
	uint8_t temp_char;

	// Read UART status.
	ul_status = usart_get_status((Usart*)USART);
	
	// Transmit interrupt
	if(ul_status & (US_CSR_TXRDY | US_CSR_TXEMPTY)) {
		if (!circ_bbuf_is_empty(&serial_mdw_buffer[uart_pointer].buffer_tx)) {
			circ_bbuf_pop(&serial_mdw_buffer[uart_pointer].buffer_tx, &temp_char);
			uc_char = (uint32_t)temp_char;
			usart_write((Usart*)USART, uc_char);
			}else{
			// Nothing more to transmit for now, deactivating interrupt
			usart_disable_interrupt((Usart*)USART, (US_IER_TXRDY | US_IER_TXEMPTY)); 
			usart_disable_tx((Usart*)USART);
		}
	}
	// Receive interrupt
	if (ul_status & US_CSR_RXRDY ) {
		if(circ_bbuf_is_empty(&serial_mdw_buffer[uart_pointer].buffer_rx)){
			usart_read((Usart*)USART, &uc_char);
			circ_bbuf_push(&serial_mdw_buffer[uart_pointer].buffer_rx, uc_char);
			// Check if timestamp has to be acquired
			#ifdef ACTIVATE_TIMESTAMP_RECORDING
			serial_mdw_buffer[uart_pointer].length_data += 1;

			if (serial_mdw_buffer[uart_pointer].timestamp == 0)
			{
				serial_mdw_buffer[uart_pointer].timestamp = unix_timestamp_ms;
			}
			// If character matches, then next character has to be timestamp and can be saved into timestamp buffer
			if(uc_char == char_to_compare_for_timestamp)
			{
				timestamp_t timestamp;
				// put info into struct
				timestamp.position = serial_mdw_buffer[uart_pointer].buffer_rx.head;
				timestamp.timestamp = serial_mdw_buffer[uart_pointer].timestamp;
				timestamp.length = serial_mdw_buffer[uart_pointer].length_data;
				// push struct into buffer
				tstp_buf_push(&serial_mdw_buffer[uart_pointer].timestamp_buff, &timestamp);
				//reset
				serial_mdw_buffer[uart_pointer].timestamp = 0;
				serial_mdw_buffer[uart_pointer].length_data = 0;

			}
			#endif
		}
	}
}

void UART0_Handler(void)
{
	handle_uart_interrupt((usart_if)UART0, UART0_pointer);
}
	
void UART1_Handler(void)
{
	handle_uart_interrupt((usart_if)UART1, UART1_pointer);
}
	
void UART2_Handler(void)
{
	handle_uart_interrupt((usart_if)UART2, UART2_pointer);
}
	
void UART3_Handler(void)
{
	handle_uart_interrupt((usart_if)UART3, UART3_pointer);
}
	
void UART4_Handler(void)
{
	handle_uart_interrupt((usart_if)UART4, UART4_pointer);
}
	
void USART0_Handler(void)
{
	handle_usart_interrupt((usart_if)USART0, USART0_pointer);
}
	
void USART1_Handler(void)
{
	handle_usart_interrupt((usart_if)USART1, USART1_pointer);
}
	
void USART2_Handler(void)
{
	handle_usart_interrupt((usart_if)USART2, USART2_pointer);
}
	
uint32_t serial_mdw_available_bytes(usart_if p_usart)
{
	uint8_t uart_buffer = uart_buffer_from_UART(p_usart);
	return circ_bbuf_available_bytes_to_read(&serial_mdw_buffer[uart_buffer].buffer_rx);
}
	
uint8_t serial_mdw_read_byte(usart_if p_usart, uint8_t *data)	
{
	uint8_t uart_buffer = uart_buffer_from_UART(p_usart);
	return circ_bbuf_pop(&serial_mdw_buffer[uart_buffer].buffer_rx, data);
}

uint8_t serial_mdw_read_bytes(usart_if p_usart, uint8_t *data, uint32_t ulsize)
{
	uint8_t uart_buffer = uart_buffer_from_UART(p_usart);
	return circ_bbuf_pop_bytes(&serial_mdw_buffer[uart_buffer].buffer_rx, ulsize, data);
}
#ifdef ACTIVATE_TIMESTAMP_RECORDING
void serial_mdw_tmstp_available(uint8_t *buffer)
{
	for(uint8_t i=0;i<number_of_uart;i++)
	{
		if(tstp_available_to_read(&serial_mdw_buffer[i].timestamp_buff))
		{
			buffer[i] = true;
		}
	}
}
uint32_t serial_mdw_tmstp_available_bytes(usart_if p_usart)
{
	uint8_t uart_buffer = uart_buffer_from_UART(p_usart);
	return tstp_available_to_read(&serial_mdw_buffer[uart_buffer].timestamp_buff);
}
uint8_t serial_mdw_tmstp_read(usart_if p_usart, s_serial_mdw_data_timestamp *data_timestamp)
{
	uint8_t uart_buffer = uart_buffer_from_UART(p_usart);
	timestamp_t timestamp;
	uint8_t result_pop = tstp_buf_pop(&serial_mdw_buffer[uart_buffer].timestamp_buff, &timestamp);
	if(result_pop == TB_SUCCESS)
	{
		uint8_t *data = (uint8_t *)malloc(sizeof(uint8_t) * timestamp.length); // TODO : handle the free() of data

		result_pop = circ_bbuf_pop_bytes(&serial_mdw_buffer[uart_buffer].buffer_rx, timestamp.length, data);
		data_timestamp->data = data;
		data_timestamp->timestamp = timestamp.timestamp;
		data_timestamp->length = timestamp.length;
	}
	
	return result_pop;
}
#endif
	
uint8_t uart_buffer_from_UART(usart_if p_usart){
		
	uint8_t uart_buffer = UART0_pointer;
		
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
	
void serial_mdw_stdio_init(volatile void *usart, const usart_serial_options_t *opt){
	stdio_base = (void *)usart;
	ptr_put = (int (*)(void volatile*,char))&serial_mdw_send_byte;
	ptr_get = (void (*)(void volatile*,char*))&serial_mdw_read_byte;
		
	serial_mdw_init_interface((Usart *)usart,(usart_serial_options_t *)opt);
		
	// For AVR32 and SAM GCC
	// Specify that stdout and stdin should not be buffered.
	setbuf(stdout, NULL);
	setbuf(stdin, NULL);
	// Note: Already the case in IAR's Normal DLIB default configuration
	// and AVR GCC library:
	// - printf() emits one character at a time.
	// - getchar() requests only 1 byte to exit.

}
	
	/// @cond 0
	/**INDENT-OFF**/
	#ifdef __cplusplus
}
#endif
/**INDENT-ON**/
/// @endcond

