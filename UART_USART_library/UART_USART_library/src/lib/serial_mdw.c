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

/**
 * \brief Configure USART to work in RS232 mode.
 *
 * \note By default, the transmitter and receiver aren't enabled.
 *
 * \param p_usart Pointer to a USART instance.
 * \param p_usart_opt Pointer to sam_usart_opt_t instance.
 * \param ul_mck USART module input clock frequency.
 *
 * \retval 0 on success.
 * \retval 1 on failure.
 */

uint8_t arrayString[20];
uint8_t head=0;
uint8_t tail=0;

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
 * \param p_usart   Base address of the USART instance.
 * \param c       Character to write.
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

void serial_mdw_sendData(usart_if p_usart,const uint8_t *p_buff, uint32_t ulsize){
	if(UART0 == (Uart*)p_usart || UART1 == (Uart*)p_usart || UART2 == (Uart*)p_usart || UART3 == (Uart*)p_usart || UART4 == (Uart*)p_usart ){
		while(ulsize > 0) {
			if(0 == usart_write((Uart*)p_usart, *p_buff)){
				uart_enable_interrupt((Uart*)p_usart, UART_IER_TXRDY | UART_IER_TXEMPTY);
				ulsize--;
				p_buff++;
			}
		}
	}
	else if(USART0 == (Usart*)p_usart || USART1 == (Usart*)p_usart || USART2 == (Usart*)p_usart ){
		while(ulsize > 0) {
			if(0 == usart_write((Uart*)p_usart, *p_buff)){
				usart_enable_interrupt((Uart*)p_usart, UART_IER_TXRDY | UART_IER_TXEMPTY);
				ulsize--;
				p_buff++;
			}
		}
	}
	
}

void UART4_Handler(void)
{
	uint32_t ul_status;
	uint8_t uc_char;

	/* Read USART status. */
	ul_status = uart_get_status(UART4);

	/*transmit interrupt rises*/
	if(ul_status & (UART_IER_TXRDY | UART_IER_TXEMPTY)) {
		
		uart_disable_interrupt(UART4, (UART_IER_TXRDY | UART_IER_TXEMPTY));
	}
}
void USART0_Handler(void)
{
	uint32_t ul_status;
	uint8_t uc_char;

	/* Read USART status. */
	ul_status = usart_get_status(USART0);

	/*transmit interrupt rises*/
	if(ul_status & (US_IER_TXRDY | US_IER_TXEMPTY)) {
		usart_disable_interrupt(USART0, (US_IER_TXRDY | US_IER_TXEMPTY));
	}
}

/// @cond 0
/**INDENT-OFF**/
#ifdef __cplusplus
}
#endif
/**INDENT-ON**/
/// @endcond

