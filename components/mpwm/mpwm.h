/**
 * @brief PWM引脚,用于控制电机
 * @author 小鱼 (fishros@foxmail.com)
 * @version V1.0.0
 * @date 2022-10-15
 * @copyright 版权所有：FishBot Open Source Organization
 */
#ifndef __PWM_H__
#define __PWM_H__

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "esp_log.h"
#include "esp_system.h"
#include "esp_err.h"

#include "esp8266/gpio_register.h"
#include "esp8266/pin_mux_register.h"

#include "driver/pwm.h"

#define PWM_0_OUT_IO_NUM 4
#define PWM_1_OUT_IO_NUM 5


/**
 * @brief 初始化pwm
 *
 */
bool mpwm_init(void);


void pwm_set_percent(uint8_t io,uint16_t percent);


#endif  // __LED_H__
