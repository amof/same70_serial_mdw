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
	
	#define number_of_uart 8
	#define number_of_uart_buf_point 4

	const enum UART_pointers {UART_RxHead, UART_RxTail, UART_TxHead, UART_TxTail};
	const enum UART_buffers {UART0_buffer, UART1_buffer, UART2_buffer, UART3_buffer, UART4_buffer, USART0_buffer, USART1_buffer, USART2_buffer};

	static volatile uint8_t UART_buffer_pointers[number_of_uart][number_of_uart_buf_point];
	static volatile uint8_t UART_RxBuf[number_of_uart][255];
	static volatile uint8_t UART_TxBuf[number_of_uart][255];
	
	uint8_t uart_buffer_from_UART(usart_if p_usart);

	
	/**
	* \brief Initialize the serial mdw.
	*
	* \param p_usart	Base address of the USART instance.
	* \param *opt		Options for the configuration of the UART instance.
	*
	* \return None
	*/
	void serial_mdw_init(usart_if p_usart,
	usart_serial_options_t *opt)
	{
		sam_uart_opt_t uart_settings;
		uart_settings.ul_mck = sysclk_get_peripheral_hz();
		uart_settings.ul_baudrate = opt->baudrate;
		uart_settings.ul_mode = opt->paritytype;

		sam_usart_opt_t usart_settings;
		usart_settings.baudrate = opt->baudrate;
		usart_settings.char_length = opt->charlength;
		usart_settings.parity_type = opt->paritytype;
		usart_settings.stop_bits= opt->stopbits;
		usart_settings.channel_mode= US_MR_CHMODE_NORMAL;
		
		for(uint8_t i=0;i<number_of_uart;i++){
			for (uint8_t j=0;j<number_of_uart_buf_point;j++)
			{
				UART_buffer_pointers[i][j] = 0;
			}
		}
		
		if(UART0 == (Uart*)p_usart){
			sysclk_enable_peripheral_clock(ID_UART0);
			uart_init((Uart*)p_usart, &uart_settings);
			NVIC_ClearPendingIRQ(UART0_IRQn);
			NVIC_EnableIRQ(UART0_IRQn);
			uart_enable_interrupt(UART0, UART_IER_RXRDY);
		}
		else if(UART1 == (Uart*)p_usart){
			sysclk_enable_peripheral_clock(ID_UART1);
			uart_init((Uart*)p_usart, &uart_settings);
			NVIC_ClearPendingIRQ(UART1_IRQn);
			NVIC_EnableIRQ(UART1_IRQn);
			uart_enable_interrupt(UART1, UART_IER_RXRDY);
		}
		else if(UART2 == (Uart*)p_usart){
			sysclk_enable_peripheral_clock(ID_UART2);
			uart_init((Uart*)p_usart, &uart_settings);
			NVIC_ClearPendingIRQ(UART2_IRQn);
			NVIC_EnableIRQ(UART2_IRQn);
			uart_enable_interrupt(UART2, UART_IER_RXRDY);
		}
		else if(UART3 == (Uart*)p_usart){
			sysclk_enable_peripheral_clock(ID_UART3);
			uart_init((Uart*)p_usart, &uart_settings);
			NVIC_ClearPendingIRQ(UART3_IRQn);
			NVIC_EnableIRQ(UART3_IRQn);
			uart_enable_interrupt(UART3, UART_IER_RXRDY);
		}
		else if(UART4 == (Uart*)p_usart){
			sysclk_enable_peripheral_clock(ID_UART4);
			uart_init((Uart*)p_usart, &uart_settings);
			NVIC_ClearPendingIRQ(UART4_IRQn);
			NVIC_EnableIRQ(UART4_IRQn);
			uart_enable_interrupt(UART4, UART_IER_RXRDY);
		}
		else if(USART0 == (Usart*)p_usart){
			sysclk_enable_peripheral_clock(ID_USART0);
			usart_init_rs232((Usart*)p_usart, &usart_settings, sysclk_get_peripheral_hz());
			usart_enable_rx(USART0);
			NVIC_ClearPendingIRQ(USART0_IRQn);
			NVIC_EnableIRQ(USART0_IRQn);
			usart_enable_interrupt(USART0, UART_IER_RXRDY);
		}
		else if(USART1 == (Usart*)p_usart){
			sysclk_enable_peripheral_clock(ID_USART1);
			usart_init_rs232((Usart*)p_usart, &usart_settings, sysclk_get_peripheral_hz());
			usart_enable_rx(USART1);
			NVIC_ClearPendingIRQ(USART1_IRQn);
			NVIC_EnableIRQ(USART1_IRQn);
			usart_enable_interrupt(USART1, UART_IER_RXRDY);
		}
		else if(USART2 == (Usart*)p_usart){
			sysclk_enable_peripheral_clock(ID_USART2);
			usart_init_rs232((Usart*)p_usart, &usart_settings, sysclk_get_peripheral_hz());
			usart_enable_rx(USART2);
			NVIC_ClearPendingIRQ(USART2_IRQn);
			NVIC_EnableIRQ(USART2_IRQn);
			usart_enable_interrupt(USART2, UART_IER_RXRDY);
		}

	}

	/**
	* \brief Sends a character with the USART.
	*
	* \param p_usart	Base address of the USART instance.
	* \param c			Character to write.
	*
	* \return Status.
	*   \retval 1  The character was written.
	*   \retval 0  The function timed out before the USART transmitter became
	* ready to send.
	*/
	int serial_mdw_putchar(usart_if p_usart, const uint8_t c)
	{
		if(UART0 == (Uart*)p_usart || UART1 == (Uart*)p_usart || UART2 == (Uart*)p_usart || UART3 == (Uart*)p_usart || UART4 == (Uart*)p_usart ){
			while (uart_write((Uart*)p_usart, c)!=0);
			return 1;
		}
		return 0;
	}

	/**
	* \brief Send multiples characters across the desired usart.
	*
	* \param p_usart	Base address of the USART instance.
	* \param			.
	*
	* \return None
	*/
	void serial_mdw_sendData(usart_if p_usart, const uint8_t *p_buff, uint32_t ulsize){
		
		uint8_t uart_buffer = uart_buffer_from_UART(p_usart);
		uint16_t tmphead = 0;
		
		
		for (uint8_t i=0; i<ulsize;i++)
		{
			tmphead = (UART_buffer_pointers[uart_buffer][UART_TxHead] + 1) & 0xFF;
			if (tmphead != UART_buffer_pointers[uart_buffer][UART_TxTail]){
				//there is room in buffer
				UART_TxBuf[uart_buffer][tmphead] = *p_buff;
				UART_buffer_pointers[uart_buffer][UART_TxHead] = tmphead;
				p_buff++;
			}
		}
		
		if(UART0 == (Uart*)p_usart || UART1 == (Uart*)p_usart || UART2 == (Uart*)p_usart || UART3 == (Uart*)p_usart || UART4 == (Uart*)p_usart ){
			uart_enable_tx((Uart*)p_usart);
			uart_enable_interrupt((Uart*)p_usart, UART_IER_TXRDY | UART_IER_TXEMPTY);
		}
		else if(USART0 == (Usart*)p_usart || USART1 == (Usart*)p_usart || USART2 == (Usart*)p_usart ){
			usart_enable_tx((Uart*)p_usart);
			usart_enable_interrupt((Uart*)p_usart, UART_IER_TXRDY | UART_IER_TXEMPTY);
		}
			
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
			if ( UART_buffer_pointers[UART0_buffer][UART_TxHead] != UART_buffer_pointers[UART0_buffer][UART_TxTail]) {
				tmptail = (UART_buffer_pointers[UART0_buffer][UART_TxTail] + 1) & 0xFF;
				usart_write(UART0, UART_TxBuf[UART0_buffer][tmptail]);
				UART_buffer_pointers[UART0_buffer][UART_TxTail] = tmptail;
				}else{
				uart_disable_interrupt(UART0, (UART_IER_TXRDY | UART_IER_TXEMPTY));
				uart_disable_tx(UART0);
			}
		}
		/*receive interrupt rises*/
		if (ul_status & UART_SR_RXRDY ) {
			
			tmphead = ( UART_buffer_pointers[UART0_buffer][UART_RxHead] + 1) & 0xFF;
			
			if ( tmphead != UART_buffer_pointers[UART0_buffer][UART_RxTail] ) {
				// store received data in buffer
				uart_read(UART0, &uc_char);
				UART_RxBuf[UART0_buffer][tmphead] = uc_char;
				// store new index
				UART_buffer_pointers[UART0_buffer][UART_RxHead] = tmphead;
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
			if ( UART_buffer_pointers[UART1_buffer][UART_TxHead] != UART_buffer_pointers[UART1_buffer][UART_TxTail]) {
				tmptail = (UART_buffer_pointers[UART1_buffer][UART_TxTail] + 1) & 0xFF;
				usart_write(UART1, UART_TxBuf[UART1_buffer][tmptail]);
				UART_buffer_pointers[UART1_buffer][UART_TxTail] = tmptail;
				}else{
				uart_disable_interrupt(UART1, (UART_IER_TXRDY | UART_IER_TXEMPTY));
				uart_disable_tx(UART1);
			}
		}
		/*receive interrupt rises*/
		if (ul_status & UART_SR_RXRDY ) {
			
			tmphead = ( UART_buffer_pointers[UART1_buffer][UART_RxHead] + 1) & 0xFF;
			
			if ( tmphead != UART_buffer_pointers[UART1_buffer][UART_RxTail] ) {
				// store received data in buffer
				uart_read(UART1, &uc_char);
				UART_RxBuf[UART1_buffer][tmphead] = uc_char;
				// store new index
				UART_buffer_pointers[UART1_buffer][UART_RxHead] = tmphead;
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
			if ( UART_buffer_pointers[UART2_buffer][UART_TxHead] != UART_buffer_pointers[UART2_buffer][UART_TxTail]) {
				tmptail = (UART_buffer_pointers[UART2_buffer][UART_TxTail] + 1) & 0xFF;
				usart_write(UART2, UART_TxBuf[UART2_buffer][tmptail]);
				UART_buffer_pointers[UART2_buffer][UART_TxTail] = tmptail;
				}else{
				uart_disable_interrupt(UART2, (UART_IER_TXRDY | UART_IER_TXEMPTY));
				uart_disable_tx(UART2);
			}
		}
		/*receive interrupt rises*/
		if (ul_status & UART_SR_RXRDY ) {
			
			tmphead = ( UART_buffer_pointers[UART2_buffer][UART_RxHead] + 1) & 0xFF;
			
			if ( tmphead != UART_buffer_pointers[UART2_buffer][UART_RxTail] ) {
				// store received data in buffer
				uart_read(UART2, &uc_char);
				UART_RxBuf[UART2_buffer][tmphead] = uc_char;
				// store new index
				UART_buffer_pointers[UART2_buffer][UART_RxHead] = tmphead;
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
			if ( UART_buffer_pointers[UART3_buffer][UART_TxHead] != UART_buffer_pointers[UART3_buffer][UART_TxTail]) {
				tmptail = (UART_buffer_pointers[UART3_buffer][UART_TxTail] + 1) & 0xFF;
				usart_write(UART3, UART_TxBuf[UART3_buffer][tmptail]);
				UART_buffer_pointers[UART3_buffer][UART_TxTail] = tmptail;
				}else{
				uart_disable_interrupt(UART3, (UART_IER_TXRDY | UART_IER_TXEMPTY));
				uart_disable_tx(UART3);
			}
		}
		/*receive interrupt rises*/
		if (ul_status & UART_SR_RXRDY ) {
			
			tmphead = ( UART_buffer_pointers[UART3_buffer][UART_RxHead] + 1) & 0xFF;
			
			if ( tmphead != UART_buffer_pointers[UART3_buffer][UART_RxTail] ) {
				// store received data in buffer
				uart_read(UART3, &uc_char);
				UART_RxBuf[UART3_buffer][tmphead] = uc_char;
				// store new index
				UART_buffer_pointers[UART3_buffer][UART_RxHead] = tmphead;
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
			if ( UART_buffer_pointers[UART4_buffer][UART_TxHead] != UART_buffer_pointers[UART4_buffer][UART_TxTail]) {	
				tmptail = (UART_buffer_pointers[UART4_buffer][UART_TxTail] + 1) & 0xFF;			
				usart_write(UART4, UART_TxBuf[UART4_buffer][tmptail]);
				UART_buffer_pointers[UART4_buffer][UART_TxTail] = tmptail;
			}else{
				uart_disable_interrupt(UART4, (UART_IER_TXRDY | UART_IER_TXEMPTY));
				uart_disable_tx(UART4);
			}
		}
		/*receive interrupt rises*/
		if (ul_status & UART_SR_RXRDY ) {
			
			tmphead = ( UART_buffer_pointers[UART4_buffer][UART_RxHead] + 1) & 0xFF;
			
			if ( tmphead != UART_buffer_pointers[UART4_buffer][UART_RxTail] ) {
				// store received data in buffer 
				uart_read(UART4, &uc_char); 
				UART_RxBuf[UART4_buffer][tmphead] = uc_char;
				// store new index 
				UART_buffer_pointers[UART4_buffer][UART_RxHead] = tmphead;
			}

			
		}
	}
	
	void USART0_Handler(void)
	{
		
		uint8_t uc_char;
		uint16_t tmphead;
		uint16_t tmptail;
		uint32_t ul_status;

		/* Read USART status. */
		ul_status = uart_get_status(USART0);
		
		/*transmit interrupt rises*/
		if(ul_status & (UART_IER_TXRDY | UART_IER_TXEMPTY)) {
			if ( UART_buffer_pointers[USART0_buffer][UART_TxHead] != UART_buffer_pointers[USART0_buffer][UART_TxTail]) {
				tmptail = (UART_buffer_pointers[USART0_buffer][UART_TxTail] + 1) & 0xFF;
				usart_write(USART0, UART_TxBuf[USART0_buffer][tmptail]);
				UART_buffer_pointers[USART0_buffer][UART_TxTail] = tmptail;
				}else{
				usart_disable_interrupt(USART0, (UART_IER_TXRDY | UART_IER_TXEMPTY));
				usart_disable_tx(USART0);
			}
		}
		/*receive interrupt rises*/
		if (ul_status & UART_SR_RXRDY ) {
			
			tmphead = ( UART_buffer_pointers[USART0_buffer][UART_RxHead] + 1) & 0xFF;
			
			if ( tmphead != UART_buffer_pointers[USART0_buffer][UART_RxTail] ) {
				// store received data in buffer
				usart_read(USART0, &uc_char);
				UART_RxBuf[USART0_buffer][tmphead] = uc_char;
				// store new index
				UART_buffer_pointers[USART0_buffer][UART_RxHead] = tmphead;
			}

			
		}
	}
	
	void USART1_Handler(void)
	{
			
		uint8_t uc_char;
		uint16_t tmphead;
		uint16_t tmptail;
		uint32_t ul_status;

		/* Read USART status. */
		ul_status = uart_get_status(USART1);
			
		/*transmit interrupt rises*/
		if(ul_status & (UART_IER_TXRDY | UART_IER_TXEMPTY)) {
			if ( UART_buffer_pointers[USART1_buffer][UART_TxHead] != UART_buffer_pointers[USART1_buffer][UART_TxTail]) {
				tmptail = (UART_buffer_pointers[USART1_buffer][UART_TxTail] + 1) & 0xFF;
				usart_write(USART1, UART_TxBuf[USART1_buffer][tmptail]);
				UART_buffer_pointers[USART1_buffer][UART_TxTail] = tmptail;
				}else{
				usart_disable_interrupt(USART1, (UART_IER_TXRDY | UART_IER_TXEMPTY));
				usart_disable_tx(USART1);
			}
		}
		/*receive interrupt rises*/
		if (ul_status & UART_SR_RXRDY ) {
				
			tmphead = ( UART_buffer_pointers[USART1_buffer][UART_RxHead] + 1) & 0xFF;
				
			if ( tmphead != UART_buffer_pointers[USART1_buffer][UART_RxTail] ) {
				// store received data in buffer
				usart_read(USART1, &uc_char);
				UART_RxBuf[USART1_buffer][tmphead] = uc_char;
				// store new index
				UART_buffer_pointers[USART1_buffer][UART_RxHead] = tmphead;
			}

				
		}
	}
		
	void USART2_Handler(void)
	{
				
		uint8_t uc_char;
		uint16_t tmphead;
		uint16_t tmptail;
		uint32_t ul_status;

		/* Read USART status. */
		ul_status = uart_get_status(USART2);
				
		/*transmit interrupt rises*/
		if(ul_status & (UART_IER_TXRDY | UART_IER_TXEMPTY)) {
			if ( UART_buffer_pointers[USART2_buffer][UART_TxHead] != UART_buffer_pointers[USART2_buffer][UART_TxTail]) {
				tmptail = (UART_buffer_pointers[USART2_buffer][UART_TxTail] + 1) & 0xFF;
				usart_write(USART2, UART_TxBuf[USART2_buffer][tmptail]);
				UART_buffer_pointers[USART2_buffer][UART_TxTail] = tmptail;
				}else{
				usart_disable_interrupt(USART2, (UART_IER_TXRDY | UART_IER_TXEMPTY));
				usart_disable_tx(USART2);
			}
		}
		/*receive interrupt rises*/
		if (ul_status & UART_SR_RXRDY ) {
					
			tmphead = ( UART_buffer_pointers[USART2_buffer][UART_RxHead] + 1) & 0xFF;
					
			if ( tmphead != UART_buffer_pointers[USART2_buffer][UART_RxTail] ) {
				// store received data in buffer
				usart_read(USART2, &uc_char);
				UART_RxBuf[USART2_buffer][tmphead] = uc_char;
				// store new index
				UART_buffer_pointers[USART2_buffer][UART_RxHead] = tmphead;
			}

					
		}
	}
	
	
	uint8_t serial_mdw_available(usart_if p_usart)
	{
		uint8_t uart_buffer = uart_buffer_from_UART(p_usart);
		
		return (255 + UART_buffer_pointers[uart_buffer][UART_RxHead] - UART_buffer_pointers[uart_buffer][UART_RxTail]) & 0xFF;
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
		
		uint8_t uart_buffer = UART0_buffer;
		
		if(UART0 == (Uart*)p_usart){
			uart_buffer = UART0_buffer;
		}else if(UART1 == (Uart*)p_usart){
			uart_buffer = UART1_buffer;
		}else if(UART2 == (Uart*)p_usart){
			uart_buffer = UART2_buffer;
		}else if(UART3 == (Uart*)p_usart){
			uart_buffer = UART3_buffer;
		}else if(UART4 == (Uart*)p_usart){
			uart_buffer = UART4_buffer;
		}else if(USART0 == (Uart*)p_usart){
			uart_buffer = USART0_buffer;
		}else if(USART1 == (Uart*)p_usart){
			uart_buffer = USART1_buffer;
		}else if(USART2 == (Uart*)p_usart){
			uart_buffer = USART2_buffer;
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

