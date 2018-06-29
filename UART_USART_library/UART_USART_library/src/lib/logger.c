#include "logger.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

static uint8_t log_level = LOG_TRACE;
static const char *level_names[] = {
	"TRACE", "DEBUG", "INFO", "WARN", "ERROR", "FATAL"};

void logger_init(void){
	const usart_serial_options_t serial_option = {
		.baudrate = 115200ul,
		.charlength = US_MR_CHRL_8_BIT,
		.paritytype = US_MR_PAR_NO,
		.stopbits = US_MR_NBSTOP_1_BIT
	};
	serial_mdw_stdio_init(SERIAL_LOG_ID, &serial_option);
	serial_mdw_set_logInterface(SERIAL_LOG_ID);
}

void logger_set_level(uint8_t level) {
	log_level = level;
}

void log_buffer(const uint8_t *text, uint8_t *p_buff, uint8_t length){
	printf("%s<", text);
	for(uint8_t i=0;i<length;i++){
		printf("%c:", *p_buff);
		p_buff++;
	}
	printf("\b>\r\n");
}

void log_log(uint8_t level, const char *file, uint32_t line, const char *fmt, ...) {
	if (level >= log_level )
	{
		#ifdef BASIC_LOG
			printf(fmt);
		#elif defined ADVANCED_LOG
			va_list args;
			printf("[%s] %-5s (%lu)", level_names[level], file, line);
			va_start(args, fmt);
			vfprintf(stdout,fmt, args);
			va_end(args);
		#endif
	}
	 
}