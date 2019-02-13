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
 *  @defgroup Serial Middleware Library
 *  @code #include <serial_mdw.h> @endcode
 * 
 *  @brief
 *
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
#include "utils/circular-byte-buffer.h"
#include "utils/timestamp-buffer.h"

/*
   +========================================+
				Defines						
   +========================================+
*/

#define SERIAL_MDW_TIMESTAMP_ACTIVATED

#define SERIAL_MDW_BUFFER_SIZE 256
#define SERIAL_MDW_BUFFER_TIMESTAMP_SIZE 20

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

static const uint8_t char_to_compare_for_timestamp = 'E';
static volatile uint64_t unix_timestamp_ms = 0UL;

/*
   +========================================+
				Functions declaration						
   +========================================+
*/

#if !defined (TEST)
/**
* Initialize the UART/USART with parameters
* @param p_usart : UARTx/USARTx
* @param opt : parameters
* @param activate_timestamp : activate timestamp, only works if SERIAL_MDW_TIMESTAMP_ACTIVATED is defined
* @return none
*/
extern void serial_mdw_init_interface(usart_if p_usart, const usart_serial_options_t *opt, UART_timestamp_t activate_timestamp) ;
/**
* Send a byte through the given UART/USART
* @param p_usart : UART/USART
* @param c : byte to send
* @return none
*/
extern uint8_t serial_mdw_send_byte(usart_if p_usart, const uint8_t data);
/**
* Send multiples bytes through the given UART/USART
* @param p_usart : UARTx/USARTx
* @param p_buff : pointer to the buffer of bytes
* @param ulsize : size of the buffer of bytes
* @return none
*/
extern uint8_t serial_mdw_send_bytes(usart_if p_usart, const uint8_t *p_buff, uint32_t ulsize);
/**
* Check if UART/USART has some data in it to be read
* @param none
* @return byte and LSB is UART0, MSB is USART2, mask to be used to determine wich UART/USART has data
*/
extern uint8_t serial_mdw_available(void);
/**
* Return the number of available bytes for the designed UART/USART
* @param p_usart : UARTx/USARTx
* @return number of bytes that can be read from the buffer
*/
extern uint32_t serial_mdw_available_bytes(usart_if p_usart);
/**
* Read the byte in the given UART/USART
* @param p_usart : UARTx/USARTx
* @param data : pointer to the data
* @return status of the read
*/
extern uint8_t serial_mdw_read_byte(usart_if p_usart, uint8_t *data);
/**
* Read the byte in the given UART/USART
* @param p_usart : UARTx/USARTx
* @param p_buff : pointer to the buffer of bytes
* @param ulsize : size of the buffer of bytes that has to be read
* @return status of the read
*/
extern uint8_t serial_mdw_read_bytes(usart_if p_usart, uint8_t *p_buff, uint32_t ulsize);

#if defined(SERIAL_MDW_TIMESTAMP_ACTIVATED)
/**
* Return the number of data timestamped for the designed UART/USART
* @param p_usart : UARTx/USARTx
* @return number of data that can be read from the buffer
*/
extern uint32_t serial_mdw_timestamp_available(usart_if p_usart);
/**
* Read the a frame received
* @param p_usart : UARTx/USARTx
* @param data_timestamp : pointer to a data_timestamp structure
* @return status of the read
*/
extern uint8_t serial_mdw_timestamp_read(usart_if p_usart, serial_mdw_data_timestamp_t *data_timestamp);
#endif

// Allow to use CMock to mock this library by removing 'extern' keyword
#elif defined (TEST)
void serial_mdw_init_interface(usart_if p_usart, const usart_serial_options_t *opt, UART_timestamp_t activate_timestamp) ;
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