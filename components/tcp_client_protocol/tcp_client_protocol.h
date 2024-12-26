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

#include "protocol.h"


/**
 * @brief 配置TCP客户端
 * 
 * @param ip 
 * @param port 
 * @return true 
 * @return false 
 */
bool tcp_client_config_init(char ip[16],uint32_t port);


/**
 * @brief 任务初始化
 *
 * @return true
 * @return false
 */
bool tcp_client_protocol_task_init(void);




#ifdef __cplusplus
}
#endif
