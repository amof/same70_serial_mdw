/************************************************************************
Title:    Serial Middleware Library
Author:   Julien Delvaux
Software: Atmel Studio 7
Hardware: SAME70Q21
License:  GNU General Public License 3
Usage:    see Doxygen manual

LICENSE:
	Copyright (C) 2015 Julien Delvaux

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

#include "compiler.h"
#include "sysclk.h"
#include "uart.h"
#include "usart.h"
#include "status_codes.h"
#include "uart_serial.h"
#include "stdio_serial.h"

/*
   ---------------------------------------
   ---------- Debugging options ----------
   ---------------------------------------
*/

#define SRL_MDW_DEBUG

/* Debug facilities. SRL_MDW_DEBUG must be defined to read output */
#ifdef SRL_MDW_DEBUG
#define SRL_MDW_DEBUG_PLATFORM_DIAG(x)   {printf(x);printf("\r\n");}
#define SRL_MDW_DEBUG_PLATFORM_ASSERT(x) {printf("Assertion \"%s\" failed at line %d in %s\n", x, __LINE__, __FILE__); while(1);}
#define SRL_MDW_DEBUGF(message) SRL_MDW_DEBUG_PLATFORM_DIAG(message)
#define SRL_MDW_DEBUGA(buffer, length) srl_mdw_debug_buffer(buffer, length)
#else
#define SRL_MDW_DEBUG_PLATFORM_DIAG(x)   {;}
#define SRL_MDW_DEBUG_PLATFORM_ASSERT(x) {while (1);}
#define SRL_MDW_DEBUGF(message)
#endif


/*
   ---------------------------------------
   -------------- Functions --------------
   ---------------------------------------
*/

extern void serial_mdw_init(usart_if p_usart,usart_serial_options_t *opt) ;

extern int serial_mdw_putchar(usart_if p_usart, const uint8_t c);

extern void serial_mdw_sendData(usart_if p_usart,const uint8_t *p_buff, uint32_t ulsize);

extern uint8_t serial_mdw_available(usart_if p_usart);

extern uint16_t serial_mdw_readChar(usart_if p_usart);

extern void srl_mdw_debug_buffer(const uint8_t *p_buff, uint8_t length);

#endif /* SERIAL_MDW_H_ */