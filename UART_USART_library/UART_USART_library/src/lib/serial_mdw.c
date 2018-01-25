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
			/* Configure UART */
			uart_init((Uart*)p_usart, &uart_settings);
		}
		else if(UART1 == (Uart*)p_usart){
			sysclk_enable_peripheral_clock(ID_UART1);
			/* Configure UART */
			uart_init((Uart*)p_usart, &uart_settings);
		}
		else if(UART2 == (Uart*)p_usart){
			sysclk_enable_peripheral_clock(ID_UART2);
			/* Configure UART */
			uart_init((Uart*)p_usart, &uart_settings);
		}
		else if(UART3 == (Uart*)p_usart){
			sysclk_enable_peripheral_clock(ID_UART3);
			/* Configure UART */
			uart_init((Uart*)p_usart, &uart_settings);
		}
		else if(UART4 == (Uart*)p_usart){
			sysclk_enable_peripheral_clock(ID_UART4);
			/* Configure UART */
			uart_init((Uart*)p_usart, &uart_settings);
		}
		else if(USART0 == (Usart*)p_usart){
			sysclk_enable_peripheral_clock(ID_USART0);
			/* Configure UART */
			usart_init_rs232((Usart*)p_usart, &usart_settings,
			sysclk_get_peripheral_hz());
		}
		else if(USART1 == (Usart*)p_usart){
			sysclk_enable_peripheral_clock(ID_USART1);
			/* Configure UART */
			usart_init_rs232((Usart*)p_usart, &usart_settings,
			sysclk_get_peripheral_hz());
		}
		else if(USART2 == (Usart*)p_usart){
			sysclk_enable_peripheral_clock(ID_USART2);
			/* Configure UART */
			usart_init_rs232((Usart*)p_usart, &usart_settings,
			sysclk_get_peripheral_hz());
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
	void serial_mdw_sendData(usart_if p_usart,const uint8_t *p_buff, uint32_t ulsize){
		
		uint8_t uart_buffer = UART0_buffer;
		uint16_t tmphead;
		
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
		
		uart_enable_interrupt((Uart*)p_usart, UART_IER_TXRDY | UART_IER_TXEMPTY);
		
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

			}
		}
		/*receive interrupt rises*/
		if (ul_status & UART_SR_RXRDY) {
			
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
				uart_disable_interrupt(USART0, (UART_IER_TXRDY | UART_IER_TXEMPTY));

			}
		}
	}
	
	uint8_t uart4_available(void)
	{
		return (255 + UART_buffer_pointers[UART4_buffer][UART_RxHead] - UART_buffer_pointers[UART4_buffer][UART_RxTail]) & 0xFF;
	}
	
	uint8_t uart4_getc()	
	{
		uint8_t tmptail;
		uint8_t data;
		
		
		/*if ( UART_buffer_pointers[UART4_buffer][UART_RxHead] == UART_buffer_pointers[UART4_buffer][UART_RxTail] ) {
			return 0x0100;   // no data available 
		}*/

		// calculate store buffer index
		tmptail = (UART_buffer_pointers[UART4_buffer][UART_RxTail] + 1) & 0xFF;
		UART_buffer_pointers[UART4_buffer][UART_RxTail] = tmptail;

		// get data from receive buffer 
		data = UART_RxBuf[UART4_buffer][tmptail];

		return data;
		
	}
	
	
	/// @cond 0
	/**INDENT-OFF**/
	#ifdef __cplusplus
}
#endif
/**INDENT-ON**/
/// @endcond

