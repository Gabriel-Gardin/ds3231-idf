#include <stdio.h>
#include "ds3231/ds3231.h"
#include "esp_log.h"
#include "esp_sleep.h"
#include <time.h>
#include "driver/rtc_io.h"
#include "driver/gpio.h"

#define POWER_PIN 33
#define INT_PIN 32


void power_on()
{
    gpio_reset_pin(POWER_PIN); 
    rtc_gpio_hold_dis(POWER_PIN); //Disable hold functionality
    rtc_gpio_init(POWER_PIN);
    rtc_gpio_set_direction(POWER_PIN, GPIO_MODE_OUTPUT);
    rtc_gpio_set_level(POWER_PIN, 1);
    gpio_deep_sleep_hold_en();
}

void power_off()
{
    rtc_gpio_set_direction(POWER_PIN, GPIO_MODE_OUTPUT);
    gpio_pullup_en(INT_PIN);
    rtc_gpio_hold_en(INT_PIN);
    rtc_gpio_set_level(POWER_PIN, 0);
}

void start_deep_sleep()
{
    printf("Going to sleep. bye \n");
	esp_sleep_enable_ext0_wakeup(INT_PIN, 0);
    power_off();
	esp_deep_sleep_start();
}

void app_main(void)
{
    struct tm timeinfo;
    struct tm alarm_time;

    alarm_time.tm_min=36;
    //alarm_time.tm_hour=25;

    power_on();

    int16_t temp = 0;
    ds3231_t ds3231 = ds3231_create(I2C_NUM_0);
    ds3231_initialize(ds3231, NULL);
    ds3231_clearInt(ds3231);
    ds3231_setSquareWaveOutput(ds3231, DS3231_RATE_1HZ, 1);
    ds3231_setInterrupt(ds3231, 0, 1);
    ds3231_setAlarm2(ds3231, DS3231_ALARM2_M, &alarm_time);

    while(1){
        
        ds3231_get_temp_integer(ds3231, &temp);
        ds3231_getTime(ds3231, &timeinfo);

        // printf("timeinfo.tm_sec=%d",timeinfo.tm_sec);
        // printf("timeinfo.tm_min=%d",timeinfo.tm_min);
        // printf("timeinfo.tm_hour=%d",timeinfo.tm_hour);
        // printf("timeinfo.tm_wday=%d",timeinfo.tm_wday);
        // printf("timeinfo.tm_mday=%d",timeinfo.tm_mday);
        // printf("timeinfo.tm_mon=%d",timeinfo.tm_mon);
        // printf("timeinfo.tm_year=%d",timeinfo.tm_year);
        printf("HORA: %d:%d:%d", timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
        printf("\n\n");
        printf("Temperature: %d \n\n\n", temp);
        vTaskDelay(300);
        start_deep_sleep();
    }
}
