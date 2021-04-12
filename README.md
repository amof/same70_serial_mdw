
# Serial Library with Interruptions

This library has been developped to leverage interrupts with SAME70.

## Dependencies

* ASF (add Standard serial I/O, verify you have both UART/USART drivers added)
* C-utils (https://github.com/amof/c-utils)

## Configuration of the pins for SAME70 

In _ASF/sam/boards/YOUR_BOARD/YOUR_BOARD.h_, define your pins:

```c
/** USART0 pins definitions, PB0,PB1. */
#define USART0_RXD_GPIO   PIO_PB0_IDX
#define USART0_RXD_FLAGS  IOPORT_MODE_MUX_C
#define USART0_TXD_GPIO   PIO_PB1_IDX
#define USART0_TXD_FLAGS  IOPORT_MODE_MUX_C

/** USART1 pins definitions, PA21,PB4. */
#define USART1_RXD_GPIO   PIO_PA21_IDX
#define USART1_RXD_FLAGS  IOPORT_MODE_MUX_A
#define USART1_TXD_GPIO   PIO_PB4_IDX
#define USART1_TXD_FLAGS  IOPORT_MODE_MUX_D

/** USART2 pins definitions */
#define USART2_RXD_GPIO   PIO_PD15_IDX
#define USART2_RXD_FLAGS  IOPORT_MODE_MUX_B
#define USART2_TXD_GPIO   PIO_PD16_IDX
#define USART2_TXD_FLAGS  IOPORT_MODE_MUX_B

/** UART0 pins definitions */
#define UART0_RXD_GPIO   PIO_PA9_IDX
#define UART0_RXD_FLAGS  IOPORT_MODE_MUX_A
#define UART0_TXD_GPIO   PIO_PA10_IDX
#define UART0_TXD_FLAGS  IOPORT_MODE_MUX_A

/** UART1 pins definitions */
#define UART1_RXD_GPIO   PIO_PA5_IDX
#define UART1_RXD_FLAGS  IOPORT_MODE_MUX_C
#define UART1_TXD_GPIO   PIO_PA6_IDX
#define UART1_TXD_FLAGS  IOPORT_MODE_MUX_C

/** UART2 pins definitions */
#define UART2_RXD_GPIO   PIO_PD25_IDX
#define UART2_RXD_FLAGS  IOPORT_MODE_MUX_C
#define UART2_TXD_GPIO   PIO_PD26_IDX
#define UART2_TXD_FLAGS  IOPORT_MODE_MUX_C

/** UART3 pins definitions */
#define UART3_RXD_GPIO   PIO_PD28_IDX
#define UART3_RXD_FLAGS  IOPORT_MODE_MUX_A
#define UART3_TXD_GPIO   PIO_PD30_IDX
#define UART3_TXD_FLAGS  IOPORT_MODE_MUX_A

/** UART4 pins definitions */
#define UART4_RXD_GPIO   PIO_PD18_IDX
#define UART4_RXD_FLAGS  IOPORT_MODE_MUX_C
#define UART4_TXD_GPIO   PIO_PD19_IDX
#define UART4_TXD_FLAGS  IOPORT_MODE_MUX_C
```

In _ASF/sam/boards/YOUR_BOARD/init.c_, define your pin functions mapping:

```c
// Configure TX
ioport_set_pin_peripheral_mode(UART0_TXD_GPIO, UART0_TXD_FLAGS);	// UART0 UTXD0
ioport_set_pin_peripheral_mode(UART1_TXD_GPIO, UART1_TXD_FLAGS);	// UART1 UTXD1
ioport_set_pin_peripheral_mode(UART2_TXD_GPIO, UART2_TXD_FLAGS);	// UART2 UTXD2
ioport_set_pin_peripheral_mode(UART3_TXD_GPIO, UART3_TXD_FLAGS);	// UART3 UTXD3
ioport_set_pin_peripheral_mode(UART4_TXD_GPIO, UART4_TXD_FLAGS);	// UART4 UTXD4
ioport_set_pin_peripheral_mode(USART0_TXD_GPIO, USART0_TXD_FLAGS);	// USART0 TXD0
ioport_set_pin_peripheral_mode(USART1_TXD_GPIO, USART1_TXD_FLAGS);	// USART1 TXD1
MATRIX->CCFG_SYSIO |= CCFG_SYSIO_SYSIO4; // Added to configure USART1
ioport_set_pin_peripheral_mode(USART2_TXD_GPIO, USART2_TXD_FLAGS);	// USART2 TXD2


// Configure RX
ioport_set_pin_peripheral_mode(UART0_RXD_GPIO, UART0_RXD_FLAGS);
ioport_set_pin_peripheral_mode(UART1_RXD_GPIO, UART1_RXD_FLAGS);
ioport_set_pin_peripheral_mode(UART2_RXD_GPIO, UART2_RXD_FLAGS);
ioport_set_pin_peripheral_mode(UART3_RXD_GPIO, UART3_RXD_FLAGS);
ioport_set_pin_peripheral_mode(UART4_RXD_GPIO, UART4_RXD_FLAGS);
ioport_set_pin_peripheral_mode(USART0_RXD_GPIO, USART0_RXD_FLAGS);
ioport_set_pin_peripheral_mode(USART1_RXD_GPIO, USART1_RXD_FLAGS); 
ioport_set_pin_peripheral_mode(USART2_RXD_GPIO, USART2_RXD_FLAGS); 
```

