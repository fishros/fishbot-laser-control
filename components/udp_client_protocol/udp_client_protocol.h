/**
 * @author fishros (fishros@foxmail.com)
 * @brief  UDP客户端协议
 * @version V1.0.0.220919
 * @date 2022-09-19
 *
 * 版权所有：FishBot Open Source Organization
 *
 */
#ifndef UDP_CLIENT_PROTOCOL_H_
#define UDP_CLIENT_PROTOCOL_H_

#ifdef __cplusplus
extern "C" {
#endif

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
 * @brief 配置UDP客户端
 * 
 * @param ip 服务器IP地址
 * @param port 服务器端口
 * @return true 配置成功
 * @return false 配置失败
 */
bool udp_client_config_init(char ip[16], uint32_t port);

/**
 * @brief UDP客户端任务初始化
 *
 * @return true 初始化成功
 * @return false 初始化失败
 */
bool udp_client_protocol_task_init(void);

/**
 * @brief 发送数据到UDP服务器
 * 
 * @param protocol_package_ 协议包
 * @return int16_t 发送的字节数，-1表示失败
 */
int16_t udp_client_tx_data(protocol_package_t *protocol_package_);

/**
 * @brief 从UDP服务器接收数据
 * 
 * @param protocol_package_ 协议包
 * @return int16_t 接收的字节数，-1表示失败
 */
int16_t udp_client_rx_data(protocol_package_t *protocol_package_);

#ifdef __cplusplus
}
#endif

#endif // UDP_CLIENT_PROTOCOL_H_ 