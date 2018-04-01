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

typedef struct rtc_struct {
	uint8_t address;
	uint32_t year;
	uint8_t month;
	uint8_t day;
	uint8_t date;
	uint8_t hour;
	uint8_t minute;
	uint8_t second;

} rtc_struct_t;

typedef rtc_struct_t rtc_ds3231m;
 
/*
   ---------------------------------------
   -------------- Functions --------------
   ---------------------------------------
*/

extern void DS3231M_init(uint8_t address);
extern void DS3231M_setTime(rtc_ds3231m *ds3231m);
extern void DS3231M_getTime(rtc_ds3231m *ds3231m);

#endif /* DS3231M_H_ */