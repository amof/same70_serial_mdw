#include "logger.h"

static log_level_t logger_log_level = LOG_DEBUG;
static const char *level_names[] = {
	"TRACE", "DEBUG", "INFO", "WARN", "ERROR", "FATAL"};

void logger_init(log_level_t log_level)
{
    logger_log_level = log_level;
    #if defined(SERIAL_LOG)
        const usart_serial_options_t serial_option = {
            .baudrate = 115200ul,
            .charlength = US_MR_CHRL_8_BIT,
            .paritytype = US_MR_PAR_NO,
            .stopbits = US_MR_NBSTOP_1_BIT
        };
        serial_mdw_stdio_init(SERIAL_LOG_ID, &serial_option);
        serial_mdw_set_logInterface(SERIAL_LOG_ID);
    #endif
}

void logger_set_log_level(log_level_t log_level)
{
	logger_log_level = log_level;
}

void log_buffer(const char *text, uint8_t *p_buff, uint8_t length)
{
    char buffer [LOGGER_MESSAGE_MAX_LENGTH] ={0};
    char *put = buffer;

    put += snprintf(put, LOGGER_MESSAGE_MAX_LENGTH, "%s <", text);

    for(uint8_t i=0; i<length; i++){
        put += snprintf(put, sizeof buffer - (put - buffer), "%u:", *p_buff);
		p_buff++;
	}

	put += snprintf(put, LOGGER_MESSAGE_MAX_LENGTH, "\b>");

	#if defined(SERIAL_LOG)
        if(length >= 0) serial_mdw_send_bytes(SERIAL_LOG_ID, buffer, (uint32_t)length);
		delay_ms(DELAY_TO_PRINT);
    #elif defined(CONSOLE_LOG)
        printf("%s\n",buffer, length);
    #endif
}

void log_log(log_level_t level, const char *file, uint32_t line, const char *fmt, ...)
{
	if (level >= logger_log_level )
	{
		int length = 0;
		char buffer [LOGGER_MESSAGE_MAX_LENGTH] ={0};
		va_list args;

		va_start(args, fmt);
		length = vsnprintf(buffer, LOGGER_MESSAGE_MAX_LENGTH, fmt, args);
		va_end (args);
		
		#if defined(ADVANCED_LOG)
			char output_message[LOGGER_MESSAGE_MAX_LENGTH]={0};
			int length_advanced_log = 0;
			length_advanced_log = snprintf(output_message, LOGGER_MESSAGE_MAX_LENGTH, "%-5s %s:%lu: ", level_names[level], file, line,fmt, args);
			strcat(output_message, buffer);
			if(length_advanced_log >= 0) length += length_advanced_log;
			memcpy(buffer, output_message, length);
		#endif

        #if defined(SERIAL_LOG)
			if(length >= 0) serial_mdw_send_bytes(SERIAL_LOG_ID, buffer, (uint32_t)length);
		    delay_ms(DELAY_TO_PRINT);
		#elif defined(CONSOLE_LOG)
			printf("%s\n",buffer, length);
        #endif
	}
	 
}