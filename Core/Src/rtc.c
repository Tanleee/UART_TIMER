#include "main.h"

void show_time_date_itm(void) {
	RTC_DateTypeDef rtc_date;
	RTC_TimeTypeDef rtc_time;

	memset(&rtc_date, 0, sizeof(rtc_date));
	memset(&rtc_time, 0, sizeof(rtc_time));

	HAL_RTC_GetTime(&hrtc, &rtc_time, RTC_FORMAT_BIN);
	HAL_RTC_GetDate(&hrtc, &rtc_date, RTC_FORMAT_BIN);

	printf("%02d:%02d:%02d", rtc_time.Hours, rtc_time.Minutes,
			rtc_time.Seconds);
	printf("\t%02d-%02d-%4d\n", rtc_date.Month, rtc_date.Date,
			2000 + rtc_date.Year);
}

void show_time_date(void) {
// Using static because we save pointer in q_print when this function done it's stack will be disappear
// if not use static pointer in queue item will point to unknow
	static char showTime[40];
	static char showDate[40];

	static char *showTimePtr = showTime;
	static char *showDatePtr = showDate;

	RTC_TimeTypeDef time;
	RTC_DateTypeDef date;

	// clear all old data
	memset(&showTime, 0, sizeof(showTime));
	memset(&showDate, 0, sizeof(showDate));

	HAL_RTC_GetTime(&hrtc, &time, RTC_FORMAT_BIN);
	HAL_RTC_GetDate(&hrtc, &date, RTC_FORMAT_BIN);

	//display time format: hh:mm:ss [AM/PM]
	snprintf(showTime, sizeof(showTime), "%s \t %02d:%02d:%02d ",
			"Current time & date: ", time.Hours, time.Minutes, time.Seconds);
	xQueueSend(q_print, &showTimePtr, portMAX_DELAY);

	//display date format: date-month-year
	snprintf(showDate, sizeof(showDate), "\t %02d-%02d-%4d \n", date.Date,
			date.Month, date.Year + 2000);
	xQueueSend(q_print, &showDatePtr, portMAX_DELAY);
}

void rtc_configure_time(RTC_TimeTypeDef *time) {

	// Because user only input hour/min/sec so we need to fill other field before set it to RTC

	time->TimeFormat = RTC_HOURFORMAT12_AM;	// ignore in 24h
	time->DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
	time->StoreOperation = RTC_STOREOPERATION_RESET;

	HAL_RTC_SetTime(&hrtc, time, RTC_FORMAT_BIN);
}

void rtc_configure_date(RTC_DateTypeDef *date) {
	HAL_RTC_SetDate(&hrtc, date, RTC_FORMAT_BIN);
}

int validate_rtc_information(RTC_TimeTypeDef *time, RTC_DateTypeDef *date) {

	// attributes inside time and date are uint type so it not negative -> no need to check negative
	if (time) {
		if (time->Hours > 23 || time->Minutes > 59 || time->Seconds > 59) {
			return 0;
		}
	}

	if (date) {
		if ((date->WeekDay > 7 || date->WeekDay == 0)
				|| (date->Month > 12 || date->Month == 0)
				|| (date->Date == 0 || date->Date > 31)
				|| (date->Month == 2 && date->Date > 29) || date->Year > 99) {
			return 0;
		}
	}

	return 1;
}
