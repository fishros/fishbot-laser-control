/**
 * @author fishros (fishros@foxmail.com)
 * @brief  待补充
 * @version V1.0.0.220919
 * @date 2022-0919
 *
 * 版权所有：FishBot Open Source Organization
 *
 */
#ifndef UART_PROTOCOL_H_
#define UART_PROTOCOL_H_

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_system.h"
#include "esp_log.h"
#include "driver/uart.h"
#include "string.h"
#include "driver/gpio.h"

#include "protocol.h"

#define UART_LOG_NUM UART_NUM_0
#define UART_PROTOC_NUM UART_NUM_0

#define TX_BUF_SIZE 1024 // 串口缓存帧的大小
#define RX_BUF_SIZE 1024 // 串口缓存帧的大小

/**
 * @brief 串口初始化
 *
 */
void uart_init(uint32_t baudrate);

/**
 * @brief 串口协议初始化
 *
 * @param data_uart_rx_queue 串口接收队列
 * @param data_socket_rx_queue 网络数据接收队列
 * @return true
 * @return false
 */
bool uart_protocol_init(xQueueHandle *data_uart_rx_queue, xQueueHandle *data_socket_rx_queue);

/**
 * @brief 串口任务初始化
 *
 * @return true
 * @return false
 */
bool uart_protocol_task_init(uint32_t baudrate);

/**
 * @brief 初始化串口数据接收任务
 *
 * @return true
 * @return false
 */
bool uart_protocol_recv_task_init(void);

#endif // UART_PROTOCOL_H_