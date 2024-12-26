/**
 * @brief LED驱动，状态指示灯
 * @author 小鱼 (fishros@foxmail.com)
 * @version V1.0.0
 * @date 2022-07-03
 * @copyright 版权所有：FishBot Open Source Organization
 */
#ifndef __LED_H__
#define __LED_H__

/* freertos includes */
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

/* esp includes */
#include "driver/gpio.h"
#include "esp_log.h"

#define LED_POL_POS 0
#define LED_POL_NEG 1

typedef struct {
  uint8_t id;
  uint8_t led_pin;
  uint8_t led_polarity;
} led_config_t;

/**
 * @brief Set the led delay object
 * 
 * @param led_delay 
 */
void led_set_delay(uint16_t led_delay);

/**
 * @brief 初始化led
 *
 */
bool led_init(void);

/**
 * @brief LED flash
 * 
 */
void led_flash();

/**
 * @brief led闪烁测试
 *
 * @return true
 * @return false
 */
bool led_test(void);

/**
 * @brief 设置led灯的状态
 *
 * @param led
 * @param value
 */
void led_set(uint8_t id, bool value);

/**
 * @brief led任务
 *
 * @param param 任务参数
 */
void led_task_init();

/**
 * @brief 快捷函数，设置蓝灯状态
 *
 */
#define led_set_blue(VALUE) led_set(LED_BLUE, VALUE)

#endif  // __LED_H__
