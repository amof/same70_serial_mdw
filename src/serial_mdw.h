/************************************************************************
Title:    Serial Middleware Library
Author:   Julien Delvaux
Software: Atmel Studio 7
Hardware: SAME70Q21
License:  GNU General Public License 3
Usage:    see Doxygen manual

LICENSE:
	Copyright (C) 2018 Julien Delvaux

	This program is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 3 of the License, or
	any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

    
************************************************************************/

/** 
 *  @file
 *  @defgroup serial_mdw Serial Middleware Library
 *  @code #include <serial_mdw.h> @endcode
 * 
 *  @brief This library provides abstraction for manipulation of serial library provided by Atmel.
 *	@date 2019-04-25
 *
 *  @author Julien Delvaux <delvaux.ju@gmail.com>
 */



#ifndef SERIAL_MDW_H_
#define SERIAL_MDW_H_

/*
   +========================================+
				Includes						
   +========================================+
*/

#if defined(TEST)
#	include <stdint.h>
#else
#	include "compiler.h"
#endif

#include "status_codes.h"
#include "uart_serial.h"
#include "circular-byte-buffer.h"
#include "timestamp-buffer.h"

/*
   +========================================+
				Defines						
   +========================================+
*/

/**Activate the use of timestamp. Creation of the buffer and activation of dedicated functions.*/ 
#define SERIAL_MDW_TIMESTAMP_ACTIVATED

#define SERIAL_MDW_BUFFER_SIZE				512
#define SERIAL_MDW_BUFFER_TIMESTAMP_SIZE	20

typedef enum {
	TIMESTAMP_USED,
	TIMESTAMP_NOT_USED
} UART_timestamp_t;

#ifdef SERIAL_MDW_TIMESTAMP_ACTIVATED
typedef struct serial_mdw_data_timestamp_t {
	uint8_t *data;
	uint32_t length;
	uint64_t timestamp;
} serial_mdw_data_timestamp_t;
#endif

/*
   +========================================+
				Global Variables						
   +========================================+
*/

extern volatile uint64_t unix_timestamp_ms;

/*
   +========================================+
				Functions declaration						
   +========================================+
*/

#if !defined (TEST)
/**
* Initialize the UART/USART with parameters. It does not activate interrupt by default.
* @ingroup serial_mdw
* @param p_usart[in] : UARTx/USARTx pointer
* @param opt[in] : parameters
* @param activate_rx_interrupt[in] : flag to automatically activate rx interruption 
*/
extern void serial_mdw_init_interface(usart_if p_usart, const usart_serial_options_t *opt, uint8_t activate_rx_interrupt);
/**
* Enable interrupts
* @ingroup serial_mdw
* @param p_usart[in] : UARTx/USARTx pointer
*/
extern void serial_mdw_enable_usart_rx_irq(usart_if p_usart);
/**
* Disable interrupts
* @ingroup serial_mdw
* @param mask p_usart[in] : UARTx/USARTx pointer
*/
extern void serial_mdw_disable_usart_rx_irq(usart_if p_usart);
/**
* Send a byte through the given UART/USART
* @ingroup serial_mdw
* @param p_usart[in] : UARTx/USARTx pointer
* @param c[in] : byte to send
*/
extern uint8_t serial_mdw_send_byte(usart_if p_usart, const uint8_t data);
/**
* Send multiples bytes through the given UART/USART
* @ingroup serial_mdw
* @param p_usart[in] : UARTx/USARTx pointer
* @param p_buff[in] : pointer to the buffer of bytes
* @param ulsize[in] : size of the buffer of bytes
*/
extern uint8_t serial_mdw_send_bytes(usart_if p_usart, const uint8_t *p_buff, uint32_t ulsize);
/**
* Check if UART/USART has some data in it to be read
* @ingroup serial_mdw
* @return uint8_t : byte and LSB is UART0, MSB is USART2, mask to be used to determine wich UART/USART has data
*/
extern uint8_t serial_mdw_available(void);
/**
* Return the number of available bytes for the designed UART/USART
* @ingroup serial_mdw
* @param p_usart[in] : UARTx/USARTx pointer
* @return uint32_t : number of bytes that can be read from the buffer
*/
extern uint32_t serial_mdw_available_bytes(usart_if p_usart);
/**
* Read the byte in the given UART/USART
* @ingroup serial_mdw
* @param p_usart[in] : UARTx/USARTx pointer
* @param data[out] : pointer to the data
* @return uint8_t : status of the read
*/
extern uint8_t serial_mdw_read_byte(usart_if p_usart, uint8_t *data);
/**
* Read multiple bytes in the given UART/USART
* @ingroup serial_mdw
* @param p_usart[in] : UARTx/USARTx pointer
* @param p_buff[in] : pointer to the buffer of bytes
* @param ulsize[in] : size of the buffer of bytes that has to be read
* @return uint8_t : status of the read
*/
extern uint8_t serial_mdw_read_bytes(usart_if p_usart, uint8_t *p_buff, uint32_t ulsize);

#if defined(SERIAL_MDW_TIMESTAMP_ACTIVATED)
/**
 * Set the termination character for the p_usart given. Used to raised a flag when data received from serial link is equal to the one given here.
 * @ingroup serial_mdw
 * @param p_usart 
 * @param data 
 */
extern void serial_mdw_set_compare_trigger(usart_if p_usart, const uint8_t data);
/**
* Return the number of data timestamped for the designed UART/USART
* @ingroup serial_mdw
* @param p_usart[in] : UARTx/USARTx pointer
* @return uint32_t : number of data that can be read from the buffer
*/
extern uint32_t serial_mdw_timestamp_available(usart_if p_usart);
/**
* Read the a frame received
* @ingroup serial_mdw
* @param p_usart[in] : UARTx/USARTx pointer
* @param data_timestamp[out] : pointer to a data_timestamp structure
* @return uint8_t : status of the read
*/
extern uint8_t serial_mdw_timestamp_read(usart_if p_usart, serial_mdw_data_timestamp_t *data_timestamp);
#endif

// Allow to use CMock to mock this library by removing 'extern' keyword
#elif defined (TEST)
void serial_mdw_init_interface(usart_if p_usart, const usart_serial_options_t *opt, uint8_t activate_rx_interrupt) ;
void serial_mdw_enable_usart_rx_irq(usart_if p_usart);
void serial_mdw_disable_usart_rx_irq(usart_if p_usart);
uint8_t serial_mdw_send_byte(usart_if p_usart, const uint8_t data);
uint8_t serial_mdw_send_bytes(usart_if p_usart, const uint8_t *p_buff, uint32_t ulsize);
uint8_t serial_mdw_available(void);
uint32_t serial_mdw_available_bytes(usart_if p_usart);
uint8_t serial_mdw_read_byte(usart_if p_usart, uint8_t *data);
uint8_t serial_mdw_read_bytes(usart_if p_usart, uint8_t *p_buff, uint32_t ulsize);

#if defined(SERIAL_MDW_TIMESTAMP_ACTIVATED)
uint32_t serial_mdw_timestamp_available(usart_if p_usart);
uint8_t serial_mdw_timestamp_read(usart_if p_usart, serial_mdw_data_timestamp_t *data_timestamp);
#endif
#endif

#endif /* SERIAL_MDW_H_ */