/**
 * @author fishros (fishros@foxmail.com)
 * @brief  待补充
 * @version V1.0.0.220919
 * @date 2022-0919
 *
 * 版权所有：FishBot Open Source Organization
 *
 */
#ifndef FISHBOT_NVS_H__
#define FISHBOT_NVS_H__
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "nvs.h"
#include <string.h>
#include "esp_log.h"

#define NVS_DATA_UINT8_NONE -127
#define NVS_DATA_UINT8_CONFIG 1

void nvs_test(void);

int8_t nvs_read_string(char *string_name, char *data, const char *defaultvalue, size_t len);

/**
 * @brief 从数据库读取结构体数据
 *
 * @param struct_name
 * @param pStruct
 * @param length
 */
void nvs_read_struct(char *struct_name, void *pStruct, uint32_t length);

int8_t nvs_write_string(char *string_name, char *string_value);
void nvs_write_struct(char *struct_name, void *pStruct, uint32_t length);

/*写 */
void nvs_write_uint8(char *i8_name, int8_t i8);

/*读取int */
void nvs_read_uint8(char *i8_name, int8_t *i8);

#endif