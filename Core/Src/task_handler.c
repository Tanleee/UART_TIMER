#include "main.h"

// PFP
void process_command(command_t*);
int extract_command(command_t*);

const char *msg_inv = "--- Invalid option ---\n";

void menu_task(void *pvParamters) {
	const char *msg_menu = "_______________\n"
			"|     Menu     |\n"
			"_______________\n"
			"0. Led effect\n"
			"1. Date and time\n"
			"2. Exit\n"
			"Enter your choice here: ";

	uint32_t cmd_addr;	// address of pointer cmd
	command_t *cmd;		// pointer to cmd

	uint8_t option;

	while (1) {
		xQueueSend(q_print, &msg_menu, portMAX_DELAY);

		xTaskNotifyWait(0, 0, &cmd_addr, portMAX_DELAY);
		cmd = (command_t*) cmd_addr;

		if (cmd->len == 1) {
			option = cmd->payload[0] - 48;

			switch (option) {
			case 0:
				curr_state = sLedEffect;
				xTaskNotify(handle_led_task, 0, eNoAction);
				break;
			case 1:
				curr_state = sRtcMenu;
				xTaskNotify(handle_rtc_task, 0, eNoAction);
				break;
			case 2:
				curr_state = sMainMenu;
				xTaskNotify(handle_menu_task, 0, eNoAction);
				break;
			default:
				xQueueSend(q_print, &msg_inv, portMAX_DELAY);
				continue;
			}

		} else {
			// invalid entry
			xQueueSend(q_print, &msg_inv, portMAX_DELAY);
			continue;
		}

		// Wait to run again when another task notify
		xTaskNotifyWait(0, 0, NULL, portMAX_DELAY);
	}
}

void led_effect(uint8_t);

void led_task(void *pvParamters) {
	const char *msg_led = "_______________\n"
			"   LED effect  \n"
			"_______________\n"
			"None. Turn off all led\n"
			"e1.Effect 1\n"
			"e2.Effect 2\n"
			"e3.Effect 3\n"
			"e4.Effect 4\n"
			"Enter your choice here: ";

	uint32_t cmd_addr;
	command_t *cmd;

	while (1) {
		xTaskNotifyWait(0, 0, NULL, portMAX_DELAY);

		xQueueSend(q_print, &msg_led, portMAX_DELAY);//  save address not a full string - print led menu

		xTaskNotifyWait(0, 0, &cmd_addr, portMAX_DELAY);// wait for command of the user
		cmd = (command_t*) cmd_addr;

		if (cmd->len == 2 || cmd->len == 4) {
			if (!strcmp((char*) cmd->payload, "None")) {
				led_effect_stop();
			} else if (!strcmp((char*) cmd->payload, "e1")) {
				led_effect(1);
			} else if (!strcmp((char*) cmd->payload, "e2")) {
				led_effect(2);
			} else if (!strcmp((char*) cmd->payload, "e3")) {
				led_effect(3);
			} else if (!strcmp((char*) cmd->payload, "e4")) {
				led_effect(4);
			} else {
				xQueueSend(q_print, &msg_inv, portMAX_DELAY);
			}
		} else {
			xQueueSend(q_print, &msg_inv, portMAX_DELAY);
		}

		// after selected effect go back to main menu (that reasonable cause we don't have any exit option in led menu)
		curr_state = sMainMenu;

		xTaskNotify(handle_menu_task, 0, eNoAction);
	}
}

int getNumber(command_t *cmd) {
	uint8_t number;

	if (cmd->len == 1) {
		number = cmd->payload[0] - 48;
	} else {
		number = (cmd->payload[0] - 48) * 10 + cmd->payload[1] - 48;
	}

	return number;
}

void rtc_task(void *pvParamters) {
	const char *msg_rtc1 = "========================\n"
			"|         RTC          |\n"
			"========================\n";

	const char *msg_rtc2 = "Configure Time            ----> 0\n"
			"Configure Date            ----> 1\n"
			"Enable reporting          ----> 2\n"
			"Exit                      ----> 3\n"
			"Enter your choice here : ";

	const char *msg_rtc_hh = "Enter hour(1-12):";
	const char *msg_rtc_mm = "Enter minutes(0-59):";
	const char *msg_rtc_ss = "Enter seconds(0-59):";

	const char *msg_rtc_dd = "Enter date(1-31):";
	const char *msg_rtc_mo = "Enter month(1-12):";
	const char *msg_rtc_dow = "Enter day(1-7 sun:7):";
	const char *msg_rtc_yr = "Enter year(0-99):";

	const char *msg_conf = "Configuration successful\n";
	const char *msg_rtc_report = "Enable time&date reporting(y/n)?: ";

	uint32_t cmd_addr;
	command_t *cmd;

	static int rtc_state = 0;
//	int menu_code;

	RTC_TimeTypeDef time;
	RTC_DateTypeDef date;

#define HH_CONFIG 		0
#define MM_CONFIG 		1
#define SS_CONFIG 		2

#define DATE_CONFIG 	0
#define MONTH_CONFIG 	1
#define YEAR_CONFIG 	2
#define DAY_CONFIG 		3

	while (1) {
		xTaskNotifyWait(0, 0, NULL, portMAX_DELAY);	// waitig notify sent rtc_task

		xQueueSend(q_print, &msg_rtc1, portMAX_DELAY);
		show_time_date();
		xQueueSend(q_print, &msg_rtc2, portMAX_DELAY);

		while (curr_state != sMainMenu) {
			xTaskNotifyWait(0, 0, &cmd_addr, portMAX_DELAY);//waiting for user select in rtc menu
			cmd = (command_t*) cmd_addr;

			switch (curr_state) {
			case sRtcMenu: {
				if (cmd->len == 1) {
					uint8_t option = cmd->payload[0] - 48;

					switch (option) {
					case 0:
						curr_state = sRtcTimeConfig;
						xQueueSend(q_print, &msg_rtc_hh, portMAX_DELAY);
						break;
					case 1:
						curr_state = sRtcDateConfig;
						xQueueSend(q_print, &msg_rtc_dd, portMAX_DELAY);
						break;
					case 2:
						curr_state = sRtcReport;
						xQueueSend(q_print, &msg_rtc_report, portMAX_DELAY);
						break;
					case 3:
						curr_state = sMainMenu;
						break;
					default:
						curr_state = sMainMenu;
						xQueueSend(q_print, &msg_inv, portMAX_DELAY);
						break;
					}

				} else {
					curr_state = sMainMenu;
					xQueueSend(q_print, &msg_inv, portMAX_DELAY);
				}
				break;
			}
			case sRtcTimeConfig: {

				switch (rtc_state) {
				case HH_CONFIG: {
					time.Hours = getNumber(cmd);
					xQueueSend(q_print, &msg_rtc_mm, portMAX_DELAY);
					rtc_state = MM_CONFIG;
					break;
				}
				case MM_CONFIG: {
					time.Minutes = getNumber(cmd);
					xQueueSend(q_print, &msg_rtc_ss, portMAX_DELAY);
					rtc_state = SS_CONFIG;
					break;
				}
				case SS_CONFIG: {
					time.Seconds = getNumber(cmd);
					if (validate_rtc_information(&time, NULL)) {
						rtc_configure_time(&time);
						xQueueSend(q_print, &msg_conf, portMAX_DELAY);
						show_time_date();
					} else {
						xQueueSend(q_print, &msg_inv, portMAX_DELAY);
					}

					rtc_state = 0;
					curr_state = sMainMenu;
					break;
				}
				}

				break;
			}

			case sRtcDateConfig: {

				switch (rtc_state) {
				case DATE_CONFIG: {
					date.Date = getNumber(cmd);
					rtc_state = MONTH_CONFIG;
					xQueueSend(q_print, &msg_rtc_mo, portMAX_DELAY);
					break;
				}
				case MONTH_CONFIG: {
					date.Month = getNumber(cmd);
					rtc_state = DAY_CONFIG;
					xQueueSend(q_print, &msg_rtc_dow, portMAX_DELAY);
					break;
				}
				case DAY_CONFIG: {
					date.WeekDay = getNumber(cmd);
					rtc_state = YEAR_CONFIG;
					xQueueSend(q_print, &msg_rtc_yr, portMAX_DELAY);
					break;
				}
				case YEAR_CONFIG: {
					date.Year = getNumber(cmd);
					if (validate_rtc_information(NULL, &date)) {
						rtc_configure_date(&date);
						xQueueSend(q_print, &msg_conf, portMAX_DELAY);
						show_time_date();
					} else {
						xQueueSend(q_print, &msg_inv, portMAX_DELAY);
					}

					rtc_state = 0;
					curr_state = sMainMenu;
					break;
				}
				}

				break;
			}
			case sRtcReport: {
				if (cmd->len == 1) {
					if (cmd->payload[0] == 'y' || cmd->payload[0] == 'Y') {
						if (xTimerIsTimerActive(rtc_timer) == pdFALSE)
							xTimerStart(rtc_timer, portMAX_DELAY);
					} else if (cmd->payload[0] == 'n'
							|| cmd->payload[0] == 'N') {
						if (xTimerIsTimerActive(rtc_timer) != pdFALSE)
							xTimerStop(rtc_timer, portMAX_DELAY);
					} else {
						xQueueSend(q_print, &msg_inv, portMAX_DELAY);
					}
				} else {
					xQueueSend(q_print, &msg_inv, portMAX_DELAY);
				}

				curr_state = sMainMenu;
				break;
			}
			default:
				break;

			}

		} //while end

		xTaskNotify(handle_menu_task, 0, eNoAction);
	} //while super loop end
}

void print_task(void *pvParamters) {
	uint32_t *msg;

	while (1) {
		xQueueReceive(q_print, &msg, portMAX_DELAY);
		HAL_UART_Transmit(&huart2, (uint8_t*) msg, strlen((char*) msg),
		HAL_MAX_DELAY);
	}
}

void cmd_handler_task(void *pvParamters) {
	command_t cmd;

	while (1) {
		if (xTaskNotifyWait(0, 0, NULL, portMAX_DELAY) == pdTRUE) {
			process_command(&cmd); // we already get user data to q_data, now just take it to q_cmd
		}
	}
}

void process_command(command_t *cmd) {

	extract_command(cmd);

	switch (curr_state) {
	case sMainMenu:
		xTaskNotify(handle_menu_task, (uint32_t )cmd,
				eSetValueWithoutOverwrite);
		break;
	case sLedEffect:
		xTaskNotify(handle_led_task, (uint32_t )cmd, eSetValueWithoutOverwrite);
		break;
	case sRtcMenu:
	case sRtcTimeConfig:
	case sRtcDateConfig:
	case sRtcReport:
		xTaskNotify(handle_rtc_task, (uint32_t )cmd, eSetValueWithOverwrite);
		break;
	default:
		break;
	}
}

int extract_command(command_t *cmd) {
	uint8_t item;
	BaseType_t status;

	status = uxQueueMessagesWaiting(q_data);

	if (!status) {
		return -1;
	}

	uint8_t i = 0;
	do {
		// queue is not empty so no waiting
		status = xQueueReceive(q_data, &item, 0);
		if (status == pdTRUE)
			cmd->payload[i] = item;

		i++;
	} while (item != '\n');

	cmd->payload[i - 1] = '\0';	 // add null char
	cmd->len = i - 1;

	return 0;
}
