/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.h
 * @brief          : Header for main.c file.
 *                   This file contains the common defines of the application.
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2026 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include <string.h>
#include <stdio.h>
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */
typedef struct {
	uint8_t payload[10];	// store data bytes to command
	uint8_t len;			// command length
} command_t;

typedef enum {
	sMainMenu = 0,
	sLedEffect,
	sRtcMenu,
	sRtcTimeConfig,
	sRtcDateConfig,
	sRtcReport
} state_t;

extern TaskHandle_t handle_menu_task, handle_led_task, handle_rtc_task,
		handle_print_task, handle_cmd_task;

extern QueueHandle_t q_data;
extern QueueHandle_t q_print;

extern TimerHandle_t handle_led_timer[4];
extern TimerHandle_t rtc_timer;

extern state_t curr_state;

extern UART_HandleTypeDef huart2;
extern RTC_HandleTypeDef hrtc;
/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */
void menu_task(void *pvParamters);
void led_task(void *pvParamters);
void rtc_task(void *pvParamters);
void print_task(void *pvParamters);
void cmd_handler_task(void *pvParamters);

void led_effect(uint8_t n);
void led_effect_stop(void);

void led_effect1(void);
void led_effect2(void);
void led_effect3(void);
void led_effect4(void);

void show_time_date_itm(void);
void show_time_date(void);
void rtc_configure_time(RTC_TimeTypeDef *time);
void rtc_configure_date(RTC_DateTypeDef *date);
int validate_rtc_information(RTC_TimeTypeDef *time, RTC_DateTypeDef *date);

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define LED1_Pin GPIO_PIN_0
#define LED1_GPIO_Port GPIOB
#define LED2_Pin GPIO_PIN_1
#define LED2_GPIO_Port GPIOB
#define LED3_Pin GPIO_PIN_2
#define LED3_GPIO_Port GPIOB
#define LED4_Pin GPIO_PIN_10
#define LED4_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
