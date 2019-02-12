#define UNITY_LONG_WIDTH 64

#include "unity.h"
#include "mock_twihs.h"
#include "logger.h"
#include "DS3231M.h"

void setUp(void)
{

}

void tearDown(void)
{

}

void test_init(void)
{
    // Expected calls
    twihs_probe_ExpectAnyArgsAndReturn(TWIHS_SUCCESS);

    // Test init
    uint8_t result = DS3231M_init(DS3231_DEFAULT_ADDRESS);
    TEST_ASSERT_EQUAL_UINT8(TWIHS_SUCCESS, result);
}

void test_get_time(void)
{
    uint8_t buffer[]= {
        0x39, // seconds
        0x18, // minutes
        0x05, // hours
        0x01, // day of the week
        0x12, // date
        0x02, // month
        0x19  // year
        };
    twihs_packet_t packet_rx = {
    .buffer = (uint8_t *)buffer,
    };
    ds3231m_t ds3231m;

    // Expected calls
    twihs_master_read_ExpectAnyArgsAndReturn(TWIHS_SUCCESS);
    twihs_master_read_ReturnThruPtr_p_packet(&packet_rx);

    // Execute function
    DS3231M_getTime(&ds3231m);

    // Verify that return was correct
    TEST_ASSERT_EQUAL_UINT8(39, ds3231m.second);
    TEST_ASSERT_EQUAL_UINT8(18, ds3231m.minute);
    TEST_ASSERT_EQUAL_UINT8(5, ds3231m.hour);
    TEST_ASSERT_EQUAL_UINT8(1, ds3231m.day_of_week);
    TEST_ASSERT_EQUAL_UINT8(12, ds3231m.date);
    TEST_ASSERT_EQUAL_UINT8(2, ds3231m.month);
    TEST_ASSERT_EQUAL_UINT16(2019, ds3231m.year);
}

void test_set_time(void)
{
    ds3231m_t ds3231m = {
        .second = 39,
        .minute = 18,
        .hour = 5,
        .day_of_week = 1,
        .date = 12,
        .month = 2,
        .year = 2019
    };

    // Expected calls
    twihs_master_write_ExpectAnyArgsAndReturn(TWIHS_SUCCESS);
    twihs_master_read_ExpectAnyArgsAndReturn(TWIHS_SUCCESS);
    twihs_master_write_ExpectAnyArgsAndReturn(TWIHS_SUCCESS);

    // Execute function
    DS3231M_setTime(&ds3231m);

}

void test_conversion()
{
    ds3231m_t ds3231m = {
        .second = 39,
        .minute = 18,
        .hour = 5,
        .day_of_week = 1,
        .date = 12,
        .month = 2,
        .year = 2019
    };
    ds3231m_t ds3231m_received = {0};

    uint64_t timestamp = convert_dateTime_to_unixms(&ds3231m);
    TEST_ASSERT_EQUAL_UINT64(1549948719000, timestamp);

    convert_unixms_to_dateTime(timestamp, &ds3231m_received);
    TEST_ASSERT_EQUAL_UINT8(ds3231m.second, ds3231m_received.second);
    TEST_ASSERT_EQUAL_UINT8(ds3231m.minute, ds3231m_received.minute);
    TEST_ASSERT_EQUAL_UINT8(ds3231m.hour, ds3231m_received.hour);
    TEST_ASSERT_EQUAL_UINT8(ds3231m.date, ds3231m_received.date);
    TEST_ASSERT_EQUAL_UINT8(ds3231m.month, ds3231m_received.month);
    TEST_ASSERT_EQUAL_UINT16(ds3231m.year, ds3231m_received.year);
}