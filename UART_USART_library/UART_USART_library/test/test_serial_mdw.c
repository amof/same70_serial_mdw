#define UNITY_LONG_WIDTH 64

#include "unity.h"
#include "logger.h"
#include "circular-byte-buffer.h"
#include "timestamp-buffer.h"
#include "serial_mdw.h"
#include "mock_uart.h"
#include "mock_usart.h"
#include "mock_pmc.h"

void setUp(void)
{

}

void tearDown(void)
{

}

void test_init(void)
{
    // Expected
    pmc_enable_periph_clk_ExpectAnyArgsAndReturn(0);
    uart_init_ExpectAnyArgsAndReturn(0);
    uart_enable_interrupt_ExpectAnyArgs();

    const usart_serial_options_t serial_option = {
    .baudrate = 115200ul,
    .charlength = US_MR_CHRL_8_BIT,
    .paritytype = US_MR_PAR_NO,
    .stopbits = US_MR_NBSTOP_1_BIT
	};
    serial_mdw_init_interface((usart_if)UART0, &serial_option, TIMESTAMP_USED);
}