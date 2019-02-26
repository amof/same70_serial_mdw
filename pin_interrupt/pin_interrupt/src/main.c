#include <asf.h>
#include "conf_example.h"

void pin_edge_handler(const uint32_t id, const uint32_t index);

int main(void)
{

	sysclk_init();
	board_init();
	ioport_init();

	delay_init(sysclk_get_cpu_hz());

	/* Set output direction on the given LED IOPORTs */
	ioport_set_port_dir(EXAMPLE_LED_PORT, EXAMPLE_LED_MASK,
			IOPORT_DIR_OUTPUT);

	/* Set direction and pullup on the given button IOPORT */
	ioport_set_port_dir(EXAMPLE_BUTTON_PORT, EXAMPLE_BUTTON_MASK,
			IOPORT_DIR_INPUT);
	ioport_set_port_mode(EXAMPLE_BUTTON_PORT, EXAMPLE_BUTTON_MASK,
			IOPORT_MODE_PULLUP);

	/* Set LED IOPORTs high */
	ioport_set_port_level(EXAMPLE_LED_PORT, EXAMPLE_LED_MASK,
			IOPORT_PIN_LEVEL_HIGH);
			
	// Configure interrupt
	
	pio_handler_set(PIOA, ID_PIOA, EXAMPLE_BUTTON_PIO, PIO_IT_FALL_EDGE, pin_edge_handler);
	pio_enable_interrupt(PIOA, EXAMPLE_BUTTON_PIO);
	
	NVIC_EnableIRQ(PIOA_IRQn);
	

	while (true) {
	}
}

void pin_edge_handler(const uint32_t id, const uint32_t index)
{
	if ((id == ID_PIOA) && (index == EXAMPLE_BUTTON_PIO)){
		if (pio_get(PIOA, PIO_TYPE_PIO_INPUT, PIO_PA16))
		{
			ioport_toggle_port_level(EXAMPLE_LED_PORT, EXAMPLE_LED_MASK);
		}
	}
}
