#include "DS3231M.h"

const uint8_t DS3231_REGISTER_CONTROL	= 0x0E;
const uint8_t DS3231_REGISTER_STATUS	= 0x0F;

const uint8_t DS3231_REGISTER_SECONDS	= 0x00;
const uint8_t DS3231_REGISTER_MINUTES	= 0x01;
const uint8_t DS3231_REGISTER_HOUR		= 0x02;
const uint8_t DS3231_REGISTER_DAY		= 0x03;
const uint8_t DS3231_REGISTER_DATE		= 0x04;
const uint8_t DS3231_REGISTER_MONTH		= 0x05;
const uint8_t DS3231_REGISTER_YEAR		= 0x06;
#define DS3231_REGISTER_DATETIME_LENGTH 7

static uint8_t bcd2bin (uint8_t val) { return val - 6 * (val >> 4); }
static uint8_t bin2bcd (uint8_t val) { return val + 6 * (val / 10); }

void DS3231M_init(uint8_t address){
	
	const twihs_options_t opt = {
		.master_clk = sysclk_get_peripheral_hz(),
		.speed = TWIHS_CLK
		};
		
	if (twihs_master_init(TWIHS0, &opt) != TWIHS_SUCCESS) {
		puts("-E-\tTWI master initialization failed.\r");
		// TODO : capture errors
	}
	if(twihs_probe(TWIHS0, address) != TWIHS_SUCCESS){
		// TODO : capture errors
	}
}

void DS3231M_setTime(rtc_ds3231m *ds3231m){
	
	uint8_t status_reg = 0;
	
	uint8_t buffer[DS3231_REGISTER_DATETIME_LENGTH] = {
		bin2bcd(ds3231m->second),
		bin2bcd(ds3231m->minute),
		bin2bcd(ds3231m->hour),
		bin2bcd(ds3231m->day_of_week),
		bin2bcd(ds3231m->date),
		bin2bcd(ds3231m->month),
		bin2bcd(ds3231m->year - 2000)} ;
	
	twihs_packet_t packet_tx = {
		.chip = ds3231m->address,
		.addr[0] = DS3231_REGISTER_SECONDS,
		.addr_length = 1,
		.buffer = (uint8_t *)buffer,
		.length = DS3231_REGISTER_DATETIME_LENGTH
	};
	
	if (twihs_master_write(TWIHS0, &packet_tx) != TWIHS_SUCCESS) {
		puts("-E-\tTWI master write packet failed.\r");
		// TODO : capture errors
	}
	
	twihs_packet_t packet_rx = {
		.chip = ds3231m->address,
		.addr[0] = DS3231_REGISTER_STATUS,
		.addr_length = 1,
		.buffer = &status_reg,
		.length = 1
	};
		
	if (twihs_master_read(TWIHS0, &packet_rx) != TWIHS_SUCCESS) {
		puts("-E-\tTWI master read packet failed.\r");
		// TODO : capture errors
	}
	
	status_reg &= ~0x80;
	
	packet_tx.addr[0] = DS3231_REGISTER_STATUS;
	packet_tx.buffer = &status_reg;
	packet_tx.length = 1;
	
	if (twihs_master_write(TWIHS0, &packet_tx) != TWIHS_SUCCESS) {
		puts("-E-\tTWI master write packet failed.\r");
		// TODO : capture errors
	}
	
	
}

void DS3231M_getTime(rtc_ds3231m *ds3231m){
	static uint8_t buffer[DS3231_REGISTER_DATETIME_LENGTH] = {0,0,0,0,0,0,0};
	
	twihs_packet_t packet_rx = {
		.chip = ds3231m->address,
		.addr[0] = DS3231_REGISTER_SECONDS,
		.addr_length = 1,
		.buffer = (uint8_t *)buffer,
		.length = DS3231_REGISTER_DATETIME_LENGTH
	};
	
	if (twihs_master_read(TWIHS0, &packet_rx) != TWIHS_SUCCESS) {
		puts("-E-\tTWI master read packet failed.\r");
		// TODO : capture errors
	}
	
	ds3231m->second = bcd2bin(buffer[DS3231_REGISTER_SECONDS]);
	ds3231m->minute = bcd2bin(buffer[DS3231_REGISTER_MINUTES]);
	ds3231m->hour = bcd2bin(buffer[DS3231_REGISTER_HOUR]);
	ds3231m->day_of_week = bcd2bin(buffer[DS3231_REGISTER_DAY]);
	ds3231m->date = bcd2bin(buffer[DS3231_REGISTER_DATE]);
	ds3231m->month = bcd2bin(buffer[DS3231_REGISTER_MONTH]);
	ds3231m->year = bcd2bin(buffer[DS3231_REGISTER_YEAR]);
		
}

// 

uint64_t convert_dateTime_to_unixms(rtc_ds3231m *ds3231m){
	uint16_t y;
    uint8_t m;
    uint8_t d;
    uint64_t unix_timestamp;
 
    // Year
    y = ds3231m->year;
    // Month of year
    m = ds3231m->month;
    // Day of month
    d = ds3231m->date;
 
    // January and February are counted as months 13 and 14 of the previous year
    if(m <= 2)
    {
       m += 12;
       y -= 1;
    }
 
    // Convert years to days
    unix_timestamp = (365 * y) + (y / 4) - (y / 100) + (y / 400);
    // Convert months to days
    unix_timestamp += (30 * m) + (3 * (m + 1) / 5) + d;
    // Unix time starts on January 1st, 1970
    unix_timestamp -= 719561;
    // Convert days to seconds
    unix_timestamp *= 86400;
    // Add hours, minutes and seconds
    unix_timestamp += (3600 * ds3231m->hour) + (60 * ds3231m->minute) + ds3231m->second;
	
	// Convert to ms
	unix_timestamp = unix_timestamp*1000;
 
    // Return Unix time
  return unix_timestamp;
}

void convert_unixms_to_dateTime(uint64_t unix_timestamp_ms, rtc_ds3231m *ds3231m){
	uint32_t a;
	uint32_t b;
	uint32_t c;
	uint32_t d;
	uint32_t e;
	uint32_t f;
	    
	//Negative Unix time values are not supported
	if(unix_timestamp_ms < 1){
		unix_timestamp_ms = 0;
	}
	    
	//Retrieve hours, minutes and seconds
	uint64_t unix_timestamp = unix_timestamp_ms /1000;
	ds3231m->second = unix_timestamp % 60;
	unix_timestamp /= 60;
	ds3231m->minute = unix_timestamp % 60;
	unix_timestamp /= 60;
	ds3231m->hour = unix_timestamp % 24;
	unix_timestamp /= 24;
	    
	//Convert Unix time to date
	a = (uint32_t) ((4 * unix_timestamp + 102032) / 146097 + 15);
	b = (uint32_t) (unix_timestamp + 2442113 + a - (a / 4));
	c = (20 * b - 2442) / 7305;
	d = b - 365 * c - (c / 4);
	e = d * 1000 / 30601;
	f = d - e * 30 - e * 601 / 1000;
	    
	//January and February are counted as months 13 and 14 of the previous year
	if(e <= 13)
	{
		c -= 4716;
		e -= 1;
	}
	else
	{
		c -= 4715;
		e -= 13;
	}
	    
	//Retrieve year, month and day
	ds3231m->year = c;
	ds3231m->month = e;
	ds3231m->date = f;
}




