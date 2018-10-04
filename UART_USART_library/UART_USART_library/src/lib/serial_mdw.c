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
	
	typedef enum UART_status_definition{NOT_INITIALIZED, INITIALIZED, ERROR, OVERFLOW};
	
	typedef struct
	{
		uint8_t *buffer_rx;
		uint8_t *buffer_tx;
		// TODO : how to handle timestamp ?
		UART_status_definition status;
		
	}s_serial_mdw_buffer;

	s_serial_mdw_buffer serial_mdw_buffer[number_of_uart];
	enum UART_pointers {UART0_pointer, UART1_pointer, UART2_pointer, UART3_pointer, UART4_pointer, USART0_pointer, USART1_pointer, USART2_pointer};
	
	static usart_if logger_uart = NULL;
	
	uint8_t uart_buffer_from_UART(usart_if p_usart);
	
void serial_mdw_init()
{
	// Initialization of buffers
	CIRC_BBUF_DEF(BUFFER_UART0_RX, SERIAL_MDW_BUFFER_SIZE);
	CIRC_BBUF_DEF(BUFFER_UART1_RX, SERIAL_MDW_BUFFER_SIZE);
	CIRC_BBUF_DEF(BUFFER_UART2_RX, SERIAL_MDW_BUFFER_SIZE);
	CIRC_BBUF_DEF(BUFFER_UART3_RX, SERIAL_MDW_BUFFER_SIZE);
	CIRC_BBUF_DEF(BUFFER_UART4_RX, SERIAL_MDW_BUFFER_SIZE);
	CIRC_BBUF_DEF(BUFFER_USART0_RX, SERIAL_MDW_BUFFER_SIZE);
	CIRC_BBUF_DEF(BUFFER_USART1_RX, SERIAL_MDW_BUFFER_SIZE);
	CIRC_BBUF_DEF(BUFFER_USART2_RX, SERIAL_MDW_BUFFER_SIZE);
	
	CIRC_BBUF_DEF(BUFFER_UART0_TX, SERIAL_MDW_BUFFER_SIZE);
	CIRC_BBUF_DEF(BUFFER_UART1_TX, SERIAL_MDW_BUFFER_SIZE);
	CIRC_BBUF_DEF(BUFFER_UART2_TX, SERIAL_MDW_BUFFER_SIZE);
	CIRC_BBUF_DEF(BUFFER_UART3_TX, SERIAL_MDW_BUFFER_SIZE);
	CIRC_BBUF_DEF(BUFFER_UART4_TX, SERIAL_MDW_BUFFER_SIZE);
	CIRC_BBUF_DEF(BUFFER_USART0_TX, SERIAL_MDW_BUFFER_SIZE);
	CIRC_BBUF_DEF(BUFFER_USART1_TX, SERIAL_MDW_BUFFER_SIZE);
	CIRC_BBUF_DEF(BUFFER_USART2_TX, SERIAL_MDW_BUFFER_SIZE);
	
	// Map the buffers to UART/USART
	serial_mdw_buffer[UART0_pointer].buffer_rx = BUFFER_UART0_RX;
	serial_mdw_buffer[UART0_pointer].buffer_tx = BUFFER_UART0_TX;
	serial_mdw_buffer[UART1_pointer].buffer_rx = BUFFER_UART1_RX;
	serial_mdw_buffer[UART1_pointer].buffer_tx = BUFFER_UART1_TX;
	serial_mdw_buffer[UART2_pointer].buffer_rx = BUFFER_UART2_RX;
	serial_mdw_buffer[UART2_pointer].buffer_tx = BUFFER_UART2_TX;
	serial_mdw_buffer[UART3_pointer].buffer_rx = BUFFER_UART3_RX;
	serial_mdw_buffer[UART3_pointer].buffer_tx = BUFFER_UART3_TX;
	serial_mdw_buffer[UART4_pointer].buffer_rx = BUFFER_UART4_RX;
	serial_mdw_buffer[UART4_pointer].buffer_tx = BUFFER_UART4_TX;
	
	serial_mdw_buffer[USART0_pointer].buffer_rx = BUFFER_USART0_RX;
	serial_mdw_buffer[USART0_pointer].buffer_tx = BUFFER_USART0_TX;
	serial_mdw_buffer[USART1_pointer].buffer_rx = BUFFER_USART1_RX;
	serial_mdw_buffer[USART1_pointer].buffer_tx = BUFFER_USART1_TX;
	serial_mdw_buffer[USART2_pointer].buffer_rx = BUFFER_USART2_RX;
	serial_mdw_buffer[USART2_pointer].buffer_tx = BUFFER_USART2_TX;
	
	// Set status of UART/USART
	for(uint8_t i=0; i<number_of_uart; i++){
		serial_mdw_buffer[i].status = NOT_INITIALIZED;
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
		usart_enable_interrupt(USART0, UART_IER_RXRDY);
		serial_mdw_buffer[uart_buffer].status = INITIALIZED;
		SRL_MDW_DEBUGF("USART0 initialized");
	}
	else if(USART1 == (Usart*)p_usart){
		sysclk_enable_peripheral_clock(ID_USART1);
		usart_init_rs232(USART1, &usart_settings, sysclk_get_peripheral_hz());
		usart_enable_rx(USART1);
		NVIC_ClearPendingIRQ(USART1_IRQn);
		NVIC_EnableIRQ(USART1_IRQn);
		usart_enable_interrupt(USART1, UART_IER_RXRDY);
		serial_mdw_buffer[uart_buffer].status = INITIALIZED;
		SRL_MDW_DEBUGF("USART1 initialized");
	}
	else if(USART2 == (Usart*)p_usart){
		sysclk_enable_peripheral_clock(ID_USART2);
		usart_init_rs232(USART2, &usart_settings, sysclk_get_peripheral_hz());
		usart_enable_rx(USART2);
		NVIC_ClearPendingIRQ(USART2_IRQn);
		NVIC_EnableIRQ(USART2_IRQn);
		usart_enable_interrupt(USART2, UART_IER_RXRDY);
		serial_mdw_buffer[uart_buffer].status = INITIALIZED;
		SRL_MDW_DEBUGF("USART2 initialized");
	}
	else{
		SRL_MDW_DEBUGF("Error");
		serial_mdw_buffer[uart_buffer].status = ERROR;
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
uint8_t serial_mdw_putchar(usart_if p_usart, const uint8_t c)
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
uint8_t serial_mdw_sendData(usart_if p_usart, const uint8_t *p_buff, uint32_t ulsize){
		
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
	
void UART0_Handler(void)
{
	uint8_t uc_char;
	uint16_t tmphead;
	uint16_t tmptail;
	uint32_t ul_status;

	/* Read USART status. */
	ul_status = uart_get_status(UART0);
		
	/*transmit interrupt rises*/
	if(ul_status & (UART_IER_TXRDY | UART_IER_TXEMPTY)) {
		if (!circ_bbuf_is_empty(&serial_mdw_buffer[UART0_pointer].buffer_tx)) {
			circ_bbuf_pop(&serial_mdw_buffer[UART0_pointer].buffer_tx, &uc_char);
			uart_write(UART0, uc_char);
			}else{
			uart_disable_interrupt(UART0, (UART_IER_TXRDY | UART_IER_TXEMPTY));
			uart_disable_tx(UART0);
		}
	}
	/*receive interrupt rises*/
	if (ul_status & UART_SR_RXRDY ) {
		if(circ_bbuf_is_empty(&serial_mdw_buffer[UART0_pointer].buffer_tx))	
		tmphead = ( UART_buffer_pointers[UART0_pointer][UART_RxHead] + 1) & 0xFF;
			
		if ( tmphead != UART_buffer_pointers[UART0_pointer][UART_RxTail] ) {
			// store received data in buffer
			uart_read(UART0, &uc_char);
			UART_RxBuf[UART0_pointer][tmphead] = uc_char;
			// store new index
			UART_buffer_pointers[UART0_pointer][UART_RxHead] = tmphead;
		}

			
	}
}
	
void UART1_Handler(void)
{
	uint8_t uc_char;
	uint16_t tmphead;
	uint16_t tmptail;
	uint32_t ul_status;

	/* Read USART status. */
	ul_status = uart_get_status(UART1);
		
	/*transmit interrupt rises*/
	if(ul_status & (UART_IER_TXRDY | UART_IER_TXEMPTY)) {
		if ( UART_buffer_pointers[UART1_pointer][UART_TxHead] != UART_buffer_pointers[UART1_pointer][UART_TxTail]) {
			tmptail = (UART_buffer_pointers[UART1_pointer][UART_TxTail] + 1) & 0xFF;
			uart_write(UART1, UART_TxBuf[UART1_pointer][tmptail]);
			UART_buffer_pointers[UART1_pointer][UART_TxTail] = tmptail;
			}else{
			uart_disable_interrupt(UART1, (UART_IER_TXRDY | UART_IER_TXEMPTY));
			uart_disable_tx(UART1);
		}
	}
	/*receive interrupt rises*/
	if (ul_status & UART_SR_RXRDY ) {
			
		tmphead = ( UART_buffer_pointers[UART1_pointer][UART_RxHead] + 1) & 0xFF;
			
		if ( tmphead != UART_buffer_pointers[UART1_pointer][UART_RxTail] ) {
			// store received data in buffer
			uart_read(UART1, &uc_char);
			UART_RxBuf[UART1_pointer][tmphead] = uc_char;
			// store new index
			UART_buffer_pointers[UART1_pointer][UART_RxHead] = tmphead;
		}

			
	}
}
	
void UART2_Handler(void)
{
	uint8_t uc_char;
	uint16_t tmphead;
	uint16_t tmptail;
	uint32_t ul_status;

	/* Read USART status. */
	ul_status = uart_get_status(UART2);
		
	/*transmit interrupt rises*/
	if(ul_status & (UART_IER_TXRDY | UART_IER_TXEMPTY)) {
		if ( UART_buffer_pointers[UART2_pointer][UART_TxHead] != UART_buffer_pointers[UART2_pointer][UART_TxTail]) {
			tmptail = (UART_buffer_pointers[UART2_pointer][UART_TxTail] + 1) & 0xFF;
			uart_write(UART2, UART_TxBuf[UART2_pointer][tmptail]);
			UART_buffer_pointers[UART2_pointer][UART_TxTail] = tmptail;
			}else{
			uart_disable_interrupt(UART2, (UART_IER_TXRDY | UART_IER_TXEMPTY));
			uart_disable_tx(UART2);
		}
	}
	/*receive interrupt rises*/
	if (ul_status & UART_SR_RXRDY ) {
			
		tmphead = ( UART_buffer_pointers[UART2_pointer][UART_RxHead] + 1) & 0xFF;
			
		if ( tmphead != UART_buffer_pointers[UART2_pointer][UART_RxTail] ) {
			// store received data in buffer
			uart_read(UART2, &uc_char);
			UART_RxBuf[UART2_pointer][tmphead] = uc_char;
			// store new index
			UART_buffer_pointers[UART2_pointer][UART_RxHead] = tmphead;
		}

			
	}
}
	
void UART3_Handler(void)
{
	uint8_t uc_char;
	uint16_t tmphead;
	uint16_t tmptail;
	uint32_t ul_status;

	/* Read USART status. */
	ul_status = uart_get_status(UART3);
		
	/*transmit interrupt rises*/
	if(ul_status & (UART_IER_TXRDY | UART_IER_TXEMPTY)) {
		if ( UART_buffer_pointers[UART3_pointer][UART_TxHead] != UART_buffer_pointers[UART3_pointer][UART_TxTail]) {
			tmptail = (UART_buffer_pointers[UART3_pointer][UART_TxTail] + 1) & 0xFF;
			uart_write(UART3, UART_TxBuf[UART3_pointer][tmptail]);
			UART_buffer_pointers[UART3_pointer][UART_TxTail] = tmptail;
			}else{
			uart_disable_interrupt(UART3, (UART_IER_TXRDY | UART_IER_TXEMPTY));
			uart_disable_tx(UART3);
		}
	}
	/*receive interrupt rises*/
	if (ul_status & UART_SR_RXRDY ) {
			
		tmphead = ( UART_buffer_pointers[UART3_pointer][UART_RxHead] + 1) & 0xFF;
			
		if ( tmphead != UART_buffer_pointers[UART3_pointer][UART_RxTail] ) {
			// store received data in buffer
			uart_read(UART3, &uc_char);
			UART_RxBuf[UART3_pointer][tmphead] = uc_char;
			// store new index
			UART_buffer_pointers[UART3_pointer][UART_RxHead] = tmphead;
		}

			
	}
}
	
void UART4_Handler(void)
{
	uint8_t uc_char;
	uint16_t tmphead;
	uint16_t tmptail;
	uint32_t ul_status;

	/* Read USART status. */
	ul_status = uart_get_status(UART4);
		
	/*transmit interrupt rises*/
	if(ul_status & (UART_IER_TXRDY | UART_IER_TXEMPTY)) {
		if ( UART_buffer_pointers[UART4_pointer][UART_TxHead] != UART_buffer_pointers[UART4_pointer][UART_TxTail]) {
			tmptail = (UART_buffer_pointers[UART4_pointer][UART_TxTail] + 1) & 0xFF;
			uart_write(UART4, UART_TxBuf[UART4_pointer][tmptail]);
			UART_buffer_pointers[UART4_pointer][UART_TxTail] = tmptail;
			}else{
			uart_disable_interrupt(UART4, (UART_IER_TXRDY | UART_IER_TXEMPTY));
			uart_disable_tx(UART4);
		}
	}
	/*receive interrupt rises*/
	if (ul_status & US_CSR_RXRDY ) {
			
		tmphead = ( UART_buffer_pointers[UART4_pointer][UART_RxHead] + 1) & 0xFF;
			
		if ( tmphead != UART_buffer_pointers[UART4_pointer][UART_RxTail] ) {
			// store received data in buffer
			uart_read(UART4, &uc_char);
			UART_RxBuf[UART4_pointer][tmphead] = uc_char;
			// store new index
			UART_buffer_pointers[UART4_pointer][UART_RxHead] = tmphead;
		}

			
	}
}
	
void USART0_Handler(void)
{
		
	uint32_t uc_char;
	uint16_t tmphead;
	uint16_t tmptail;
	uint32_t ul_status;

	/* Read USART status. */
	ul_status = usart_get_status(USART0);
		
	/*transmit interrupt rises*/
	if(ul_status & (US_CSR_TXRDY | US_CSR_TXEMPTY)) {
		if ( UART_buffer_pointers[USART0_pointer][UART_TxHead] != UART_buffer_pointers[USART0_pointer][UART_TxTail]) {
			tmptail = (UART_buffer_pointers[USART0_pointer][UART_TxTail] + 1) & 0xFF;
			usart_write(USART0, UART_TxBuf[USART0_pointer][tmptail]);
			UART_buffer_pointers[USART0_pointer][UART_TxTail] = tmptail;
			}else{
			usart_disable_interrupt(USART0, (UART_IER_TXRDY | UART_IER_TXEMPTY));
			usart_disable_tx(USART0);
		}
	}
	/*receive interrupt rises*/
	if (ul_status & US_CSR_RXRDY ) {
			
		tmphead = ( UART_buffer_pointers[USART0_pointer][UART_RxHead] + 1) & 0xFF;
			
		if ( tmphead != UART_buffer_pointers[USART0_pointer][UART_RxTail] ) {
			// store received data in buffer
			usart_read(USART0, &uc_char);
			UART_RxBuf[USART0_pointer][tmphead] = uc_char;
			// store new index
			UART_buffer_pointers[USART0_pointer][UART_RxHead] = tmphead;
		}

			
	}
}
	
void USART1_Handler(void)
{
		
	uint32_t uc_char;
	uint16_t tmphead;
	uint16_t tmptail;
	uint32_t ul_status;

	/* Read USART status. */
	ul_status = usart_get_status(USART1);
		
	/*transmit interrupt rises*/
	if(ul_status & (US_CSR_TXRDY | US_CSR_TXEMPTY)) {
		if ( UART_buffer_pointers[USART1_pointer][UART_TxHead] != UART_buffer_pointers[USART1_pointer][UART_TxTail]) {
			tmptail = (UART_buffer_pointers[USART1_pointer][UART_TxTail] + 1) & 0xFF;
			usart_write(USART1, UART_TxBuf[USART1_pointer][tmptail]);
			UART_buffer_pointers[USART1_pointer][UART_TxTail] = tmptail;
			}else{
			usart_disable_interrupt(USART1, (UART_IER_TXRDY | UART_IER_TXEMPTY));
			usart_disable_tx(USART1);
		}
	}
	/*receive interrupt rises*/
	if (ul_status & US_CSR_RXRDY ) {
			
		tmphead = ( UART_buffer_pointers[USART1_pointer][UART_RxHead] + 1) & 0xFF;
			
		if ( tmphead != UART_buffer_pointers[USART1_pointer][UART_RxTail] ) {
			// store received data in buffer
			usart_read(USART1, &uc_char);
			UART_RxBuf[USART1_pointer][tmphead] = uc_char;
			// store new index
			UART_buffer_pointers[USART1_pointer][UART_RxHead] = tmphead;
		}

			
	}
}
	
void USART2_Handler(void)
{
		
	uint32_t uc_char;
	uint16_t tmphead;
	uint16_t tmptail;
	uint32_t ul_status;

	/* Read USART status. */
	ul_status = usart_get_status(USART2);
		
	/*transmit interrupt rises*/
	if(ul_status & (US_CSR_TXRDY | US_CSR_TXEMPTY)) {
		if ( UART_buffer_pointers[USART2_pointer][UART_TxHead] != UART_buffer_pointers[USART2_pointer][UART_TxTail]) {
			tmptail = (UART_buffer_pointers[USART2_pointer][UART_TxTail] + 1) & 0xFF;
			usart_write(USART2, UART_TxBuf[USART2_pointer][tmptail]);
			UART_buffer_pointers[USART2_pointer][UART_TxTail] = tmptail;
			}else{
			usart_disable_interrupt(USART2, (UART_IER_TXRDY | UART_IER_TXEMPTY));
			usart_disable_tx(USART2);
		}
	}
	/*receive interrupt rises*/
	if (ul_status & US_CSR_RXRDY ) {
			
		tmphead = ( UART_buffer_pointers[USART2_pointer][UART_RxHead] + 1) & 0xFF;
			
		if ( tmphead != UART_buffer_pointers[USART2_pointer][UART_RxTail] ) {
			// store received data in buffer
			usart_read(USART2, &uc_char);
			UART_RxBuf[USART2_pointer][tmphead] = uc_char;
			// store new index
			UART_buffer_pointers[USART2_pointer][UART_RxHead] = tmphead;
		}

			
	}
}
	
	
uint8_t serial_mdw_available(usart_if p_usart)
{
	uint8_t uart_buffer = uart_buffer_from_UART(p_usart);
	uint16_t temp = 256 + UART_buffer_pointers[uart_buffer][UART_RxHead] - UART_buffer_pointers[uart_buffer][UART_RxTail];
	return (uint8_t) (temp & 0xFF);
}
	
uint16_t serial_mdw_readChar(usart_if p_usart)	
{
	uint16_t data;
	uint8_t uart_buffer = uart_buffer_from_UART(p_usart);
		
		
	if ( UART_buffer_pointers[uart_buffer][UART_RxHead] == UART_buffer_pointers[uart_buffer][UART_RxTail] ) {
		return 0x0100;   // no data available 
	}

	// calculate store buffer index
	uint16_t tmptail = (UART_buffer_pointers[uart_buffer][UART_RxTail] + 1) & 0xFF;
	UART_buffer_pointers[uart_buffer][UART_RxTail] = tmptail;

	// get data from receive buffer 
	data = UART_RxBuf[uart_buffer][tmptail];

	return data;
		
}
	
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
	ptr_put = (int (*)(void volatile*,char))&serial_mdw_putchar;
	ptr_get = (void (*)(void volatile*,char*))&serial_mdw_readChar;
		
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

