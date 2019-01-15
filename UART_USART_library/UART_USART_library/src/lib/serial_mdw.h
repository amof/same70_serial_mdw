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

#include <stdio.h>
#include "compiler.h"
#include "sysclk.h"
#include "uart.h"
#include "usart.h"
#include "status_codes.h"
#include "uart_serial.h"
#include "utils/circular-byte-buffer.h"
#include "utils/timestamp-buffer.h"

typedef struct s_serial_mdw_data_timestamp_t
{
	uint8_t *data;
	uint32_t length;
	uint64_t timestamp;
}s_serial_mdw_data_timestamp;

/*
   ---------------------------------------
   --------------- Includes --------------
   ---------------------------------------
*/

#define number_of_uart 8

#define SERIAL_MDW_BUFFER_SIZE 256
#define SERIAL_MDW_BUFFER_TIMESTAMP_SIZE 20

static const uint8_t char_to_compare_for_timestamp = 69;
static volatile uint64_t unix_timestamp_ms = 0UL;

//#define ACTIVATE_TIMESTAMP_RECORDING
/*
   ---------------------------------------
   ---------- Debugging options ----------
   ---------------------------------------
*/

//#define SRL_MDW_DEBUG

/* Debug facilities. SRL_MDW_DEBUG must be defined to read output */
#ifdef SRL_MDW_DEBUG
#define SRL_MDW_DEBUGF(message) {printf("%s\r\n",message);}
#else
#define SRL_MDW_DEBUGF(message)
#endif

extern volatile void *volatile stdio_base;
//! Pointer to the external low level write function.
extern int (*ptr_put)(void volatile*, char);

//! Pointer to the external low level read function.
extern void (*ptr_get)(void volatile*, char*);

/*
   ---------------------------------------
   -------------- Functions --------------
   ---------------------------------------
*/

// Init
extern void serial_mdw_init(void);
extern void serial_mdw_init_interface(usart_if p_usart, const usart_serial_options_t *opt) ;

// Send functions
extern uint8_t serial_mdw_send_byte(usart_if p_usart, const uint8_t c);
extern uint8_t serial_mdw_send_bytes(usart_if p_usart, const uint8_t *p_buff, uint32_t ulsize);

// Basic read function
extern uint32_t serial_mdw_available_bytes(usart_if p_usart);
extern uint8_t serial_mdw_read_byte(usart_if p_usart, uint8_t *data);
extern uint8_t serial_mdw_read_bytes(usart_if p_usart, uint8_t *data, uint32_t ulsize);

// Basic read function with timestamp
#ifdef ACTIVATE_TIMESTAMP_RECORDING
extern void serial_mdw_tmstp_available(uint8_t *buffer);
extern uint32_t serial_mdw_tmstp_available_bytes(usart_if p_usart);
extern uint8_t serial_mdw_tmstp_read(usart_if p_usart, s_serial_mdw_data_timestamp *data_timestamp);
#endif

// Init logging
extern void serial_mdw_stdio_init(volatile void *usart, const usart_serial_options_t *opt);
extern void serial_mdw_set_logInterface(usart_if p_usart);

#endif /* SERIAL_MDW_H_ */