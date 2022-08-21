#include "key.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "driver/gpio.h"

#include "esp_log.h"
#include "esp_system.h"

#include "nvs.h"
#include <time.h>

#define FISHBOT_MODULE "KEY"
static const char *TAG = "KEY";

#define GPIO_INPUT_IO 0

static xQueueHandle gpio_evt_queue = NULL;

static void gpio_isr_handler(void *arg)
{
    uint32_t gpio_num = (uint32_t)arg;
    xQueueSendFromISR(gpio_evt_queue, &gpio_num, NULL);
}

static void gpio_task_example(void *arg)
{
    uint32_t io_num;
    time_t now, last_time;
    time(&now);
    time(&last_time);
    static uint8_t count = 0;
    for (;;)
    {
        if (xQueueReceive(gpio_evt_queue, &io_num, portMAX_DELAY))
        {
            printf("GPIO[%d] intr, val: %d\n", io_num, gpio_get_level(io_num));
            if (gpio_get_level(io_num) == 1)
                count++;
            time(&now);
            if (now - last_time > 2)
            {
                last_time = now;
                count = 0;
            }

            if (count > 2)
            {
                nvs_write_uint8("is_smart", 0);
                esp_restart();
            }
        }
    }
}

void key_init(void)
{
    gpio_config_t io_conf;
    io_conf.intr_type = GPIO_INTR_POSEDGE;
    // bit mask of the pins, use GPIO4/5 here
    io_conf.pin_bit_mask = (1ULL << GPIO_INPUT_IO);
    // set as input mode
    io_conf.mode = GPIO_MODE_INPUT;

    io_conf.pull_up_en = 1;
    gpio_config(&io_conf);

    // change gpio intrrupt type for one pin
    gpio_set_intr_type(GPIO_INPUT_IO, GPIO_INTR_POSEDGE);

    // create a queue to handle gpio event from isr
    gpio_evt_queue = xQueueCreate(5, sizeof(uint32_t));

    xTaskCreate(gpio_task_example, "gpio_task", 1024, NULL, 4, NULL);
    // install gpio isr service
    gpio_install_isr_service(0);

    // hook isr handler for specific gpio pin
    gpio_isr_handler_add(GPIO_INPUT_IO, gpio_isr_handler, (void *)GPIO_INPUT_IO);
}
