/**
 * @brief LED驱动，状态指示灯
 * @author 小鱼 (fishros@foxmail.com)
 * @version V1.0.0
 * @date 2022-07-03
 * @copyright 版权所有：FishBot Open Source Organization
 */

/* fishbot compont includes */
#include "led.h"

#define FISHBOT_MODLUE "LED"
#define LED_BLUE 2
static bool is_init = false;

bool led_init(void)
{
  gpio_config_t io_conf;
  // disable interrupt
  io_conf.intr_type = GPIO_INTR_DISABLE;
  // bit mask of the pins that you want to set,e.g.GPIO18/19
  io_conf.pin_bit_mask = (1ULL << LED_BLUE);
  // disable pull-down mode
  io_conf.pull_down_en = 0;
  // disable pull-up mode
  io_conf.pull_up_en = 0;
  // set as output mode
  io_conf.mode = GPIO_MODE_OUTPUT;
  // configure GPIO with the given settings
  gpio_config(&io_conf);

  if (led_test())
  {
    ESP_LOGI(FISHBOT_MODLUE, "init success!");
    return true;
  }
  else
  {
    ESP_LOGE(FISHBOT_MODLUE, "init failed!");
    return false;
  }
}
bool led_test(void)
{
  led_set(LED_BLUE, 0);
  vTaskDelay(500 / portTICK_RATE_MS);
  led_set(LED_BLUE, 1);
  vTaskDelay(500 / portTICK_RATE_MS);
  led_set(LED_BLUE, 0);
  return true;
}

void led_set(uint8_t id, bool value)
{
  if (value)
  {
    gpio_set_level(id, 1);
  }
  else
  {
    gpio_set_level(id, 0);
  }
}

uint8_t status_led = 1;
void led_flash()
{
  status_led =! status_led;
  gpio_set_level(LED_BLUE, status_led);
}

static uint16_t led_delay_ = 1000;

void led_set_delay(uint16_t led_delay) { led_delay_ = led_delay; }

static void led_task(void *param)
{
  while (1)
  {
    led_set(LED_BLUE, 1);
    vTaskDelay(led_delay_ / portTICK_RATE_MS);
    led_set(LED_BLUE, 0);
    vTaskDelay(led_delay_ / portTICK_RATE_MS);
  }
}

void led_task_init(void)
{
  // led_task
  xTaskCreate(led_task, "led_task", 2 * 1024, NULL, 4, NULL);
}
