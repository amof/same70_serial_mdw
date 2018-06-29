/************************************************************************
Title:    Logger Library
Author:   Julien Delvaux based on rxi library: https://github.com/rxi/log.c/
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
 *  @defgroup Logger Library
 *  @code #include <logger.h> @endcode
 * 
 *  @brief
 *
 *
 *  @author Julien Delvaux <delvaux.ju@gmail.com>
 */



#ifndef LOGGER_H_
#define LOGGER_H_

#include "serial_mdw.h"

enum { LOG_TRACE, LOG_DEBUG, LOG_INFO, LOG_WARN, LOG_ERROR, LOG_FATAL };

/*
   ---------------------------------------
   ---------- Logger options ----------
   ---------------------------------------
*/

// Define on which interface you want to output the traces
#define SERIAL_LOG
//#define ETHERNET_LOG TO IMPLEMENT

// Define the level of logging
#define LOG_LEVEL LOG_DEBUG

// Define if you want to use a basic output format or a more verbose one
//#define BASIC_LOG
#define ADVANCED_LOG

/*
   ---------------------------------------
   ---------- Serial Logger ----------
   ---------------------------------------
*/

#define SERIAL_LOG_ID	USART1

/*
   ---------------------------------------
   --------- Debugging functions ---------
   ---------------------------------------
*/

#define log_trace(...) log_log(LOG_TRACE, __FILE__, __LINE__, __VA_ARGS__)
#define log_debug(...) log_log(LOG_DEBUG, __FILE__, __LINE__, __VA_ARGS__)
#define log_info(...)  log_log(LOG_INFO,  __FILE__, __LINE__, __VA_ARGS__)
#define log_warn(...)  log_log(LOG_WARN,  __FILE__, __LINE__, __VA_ARGS__)
#define log_error(...) log_log(LOG_ERROR, __FILE__, __LINE__, __VA_ARGS__)
#define log_fatal(...) log_log(LOG_FATAL, __FILE__, __LINE__, __VA_ARGS__)


extern void logger_init(void);

extern void logger_set_level(uint8_t level);

extern void log_buffer(const uint8_t *text, uint8_t *p_buff, uint8_t length);

extern void log_log(uint8_t level, const char *file, uint32_t line, const char *fmt, ...);


#endif /* LOGGER_H_ */