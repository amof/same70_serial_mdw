#ifndef DS3231M_H_
#define DS3231M_H_

#if defined(TEST)
#  include <stdint.h>
#else
#  include "compiler.h" 
#endif


/*
   ---------------------------------------
   --------------- DS3231M ---------------
   ---------------------------------------
*/
#define DS3231_DEFAULT_ADDRESS 0x68

typedef struct ds3231m_t {
	uint8_t address;
	uint16_t year; // Year
	uint8_t month; // Month
	uint8_t day_of_week; // Day Of The Week
	uint8_t date; // Date of day in the month
	uint8_t hour; // Hour
	uint8_t minute; //Minute 
	uint8_t second; //Second
} ds3231m_t;
 
/*
   ---------------------------------------
   -------------- Functions --------------
   ---------------------------------------
*/

extern uint32_t DS3231M_init(uint8_t address);
extern uint32_t DS3231M_setTime(ds3231m_t *ds3231m);
extern uint32_t DS3231M_getTime(ds3231m_t *ds3231m);

extern uint64_t convert_dateTime_to_unixms(ds3231m_t *ds3231m);
extern void convert_unixms_to_dateTime(uint64_t unix_timestamp_ms, ds3231m_t *ds3231m);

#endif /* DS3231M_H_ */