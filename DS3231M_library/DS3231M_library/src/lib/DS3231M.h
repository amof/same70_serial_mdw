/*
 * DS3231M.h
 *
 * Created: 01-04-18 09:52:17
 *  Author: Julien
 */ 

#ifndef DS3231M_H_
#define DS3231M_H_


#include <stdio.h>
#include "asf.h"
#include "status_codes.h"


/*
   ---------------------------------------
   ----------- TWI CONFIGURATION----------
   ---------------------------------------
*/
/** TWI Bus Clock 400kHz */
#define TWIHS_CLK 400000
/*
   ---------------------------------------
   --------------- DS3231M ---------------
   ---------------------------------------
*/
#define DS3231_DEFAULT_ADDRESS 0x68

typedef struct rtc_ds3231m_struct {
	uint8_t address;
	uint16_t year; // Year
	uint8_t month; // Month
	uint8_t day_of_week; // Day Of The Week
	uint8_t date; // Date of day in the month
	uint8_t hour; // Hour
	uint8_t minute; //Minute 
	uint8_t second; //Second
} rtc_ds3231m;
 
/*
   ---------------------------------------
   -------------- Functions --------------
   ---------------------------------------
*/

extern void DS3231M_init(uint8_t address);
extern void DS3231M_setTime(rtc_ds3231m *ds3231m);
extern void DS3231M_getTime(rtc_ds3231m *ds3231m);

extern uint64_t convert_dateTime_to_unixms(rtc_ds3231m *ds3231m);
extern void convert_unixms_to_dateTime(uint64_t unix_timestamp_ms, rtc_ds3231m *ds3231m);

#endif /* DS3231M_H_ */