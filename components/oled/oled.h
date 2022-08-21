#ifndef __OLED_H__
#define __OLED_H__

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "esp_log.h"
#include "esp_system.h"
#include "esp_err.h"

#include "driver/i2c.h"

//显示结构体
typedef struct
{    
    uint8_t x;
    uint8_t y;
    uint8_t row[128];
}oled_show_queue_t;

void oled_init(void);

void oled_delete(void);

void oled_write(uint8_t reg_address, uint8_t data);

void oled_writes(uint8_t reg_address, uint8_t *data,uint8_t data_len);

void oled_setxy(uint8_t x, uint8_t y);

void oled_ascii(uint8_t x,uint8_t y,char* str);

#endif