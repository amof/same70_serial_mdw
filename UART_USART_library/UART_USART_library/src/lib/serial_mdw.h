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


#include <stdio.h>
#include "compiler.h"
#include "sysclk.h"
#include "uart.h"
#include "usart.h"
#include "status_codes.h"
#include "uart_serial.h"
#include "utils/circular-byte-buffer.h"
#include "utils/timestamp-buffer.h"

/*
   +========================================+
				Defines						
   +========================================+
*/

#define ACTIVATE_TIMESTAMP_RECORDING

#define SERIAL_MDW_BUFFER_SIZE 256
#define SERIAL_MDW_BUFFER_TIMESTAMP_SIZE 20

typedef struct serial_mdw_data_timestamp_t {
	uint8_t *data;
	uint32_t length;
	uint64_t timestamp;
} serial_mdw_data_timestamp_t;

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

/**
* Initialize the UART/USART with parameters
* @param p_usart : UARTx/USARTx
* @param opt : parameters
* @return none
*/
extern void serial_mdw_init_interface(usart_if p_usart, const usart_serial_options_t *opt) ;
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
* Return the number of available bytes for the designed UART/USART
* @param p_usart : UARTx/USARTx
* @return number of bytes that can be read in the buffer
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

// Basic read function with timestamp
#ifdef ACTIVATE_TIMESTAMP_RECORDING
extern void serial_mdw_tmstp_available(uint8_t *buffer);
extern uint32_t serial_mdw_tmstp_available_bytes(usart_if p_usart);
extern uint8_t serial_mdw_tmstp_read(usart_if p_usart, serial_mdw_data_timestamp_t *data_timestamp);
#endif

#endif /* SERIAL_MDW_H_ */