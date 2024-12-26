/**
 * @author fishros (fishros@foxmail.com)
 * @brief  待补充
 * @version V1.0.0.220919
 * @date 2022-0919
 *
 * 版权所有：FishBot Open Source Organization
 *
 */
#ifndef WIFICONFIG_H_
#define WIFICONFIG_H_
#include <string.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "tcpip_adapter.h"
#include "esp_smartconfig.h"
#include "smartconfig_ack.h"
#include <stdio.h>
#include <string.h>
#include "oled.h"
#include "nvs.h"
#include "driver/uart.h"

#define SSID_LEN 32
#define PASSWORD_LEN 32
#define BUFF_SIZE 512
#define UDP_IP_LEN 32
#define UDP_PORT_LEN 8
#define MOTOR_SPEED_LEN 8
#define LASER_BAUDRATE_LEN 8

/**
 * @brief SmartConfig
 * 
 */
void wificonfig_start(void);

/**
 * config,wifi,m,88888888
 * config,proto,udp_client,192.168.1.1,3347
 * config,proto,udp_server,0.0.0.0,3347
 * restart
 * 
 */
void wificonfig_byuart(void);

void print_config();

#endif // WIFICONFIG_H_