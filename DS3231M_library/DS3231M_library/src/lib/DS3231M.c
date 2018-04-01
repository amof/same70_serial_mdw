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
		bin2bcd(ds3231m->day),
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
	ds3231m->day = bcd2bin(buffer[DS3231_REGISTER_DAY]);
	ds3231m->date = bcd2bin(buffer[DS3231_REGISTER_DATE]);
	ds3231m->month = bcd2bin(buffer[DS3231_REGISTER_MONTH]);
	ds3231m->year = bcd2bin(buffer[DS3231_REGISTER_YEAR]);
		
}




