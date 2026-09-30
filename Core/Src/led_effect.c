#include "main.h"

void toggle_all_leds(uint8_t n);

void led_effect_stop(void) {
	toggle_all_leds(0);

	for (uint8_t i = 0; i < 4; i++) {
		xTimerStop(handle_led_timer[i], portMAX_DELAY);
	}
}

// start an effect
void led_effect(uint8_t n) {
	led_effect_stop();
	xTimerStart(handle_led_timer[n - 1], portMAX_DELAY);
}

void toggle_all_leds(uint8_t flag) {
	HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, flag);
	HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, flag);
	HAL_GPIO_WritePin(LED3_GPIO_Port, LED3_Pin, flag);
	HAL_GPIO_WritePin(LED4_GPIO_Port, LED4_Pin, flag);
}

void turn_on_a_led(uint8_t position) {
	HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, position == 0);
	HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, position == 1);
	HAL_GPIO_WritePin(LED3_GPIO_Port, LED3_Pin, position == 2);
	HAL_GPIO_WritePin(LED4_GPIO_Port, LED4_Pin, position == 3);
}

void led_effect1(void) {
	static uint8_t flag = 1;
	toggle_all_leds(flag);
	flag ^= 1;
}

void led_effect2() {
	static uint8_t positionType = 0;	// 0 -> even and 1-> odd

	HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, positionType);
	HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, !positionType);
	HAL_GPIO_WritePin(LED3_GPIO_Port, LED3_Pin, positionType);
	HAL_GPIO_WritePin(LED4_GPIO_Port, LED4_Pin, !positionType);

	positionType ^= 1;
}

void led_effect3() {
	static uint8_t position = 0;
	turn_on_a_led(position);
	position = (position + 1) % 4;
}

void led_effect4() {
	static uint8_t position = 3;
	turn_on_a_led(position);
	position = (position + 3) % 4;
}
