/*
 * @作者: 小鱼
 * @公众号: 鱼香ROS
 * @QQ交流群: 2642868461
 * @描述: file content
 */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif


#define CONFIG_EXAMPLE_IPV4         y
#define PORT                        8848
#define KEEPALIVE_IDLE              5
#define KEEPALIVE_INTERVAL          5
#define KEEPALIVE_COUNT             3

#include <string.h>
#include <sys/param.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_netif.h"

#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include <lwip/netdb.h>

#include <string.h>


void my_udp_init();
void send_data(char *data,uint8_t len);

void print_hex(char *buffer, int len);
uint8_t calc_checksum(char *p_data, int32_t data_len);

#ifdef __cplusplus
}
#endif
