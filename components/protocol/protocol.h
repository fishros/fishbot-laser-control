#ifndef PROTOCOL_H_
#define PROTOCOL_H_
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#define RX_TX_PACKET_SIZE 1024

typedef struct
{
    uint8_t size;
    uint8_t data[RX_TX_PACKET_SIZE];
}  __attribute__((packed)) protocol_package_t;

void print_frame_to_hex(uint8_t *title, uint8_t *buffer, uint16_t size);

/**
 * @brief TCP客户端发送数据
 * 
 * @param protocol_package_ 
 * @return int16_t 
 */
int16_t tcp_client_tx_data(protocol_package_t *protocol_package_);

/**
 * @brief TCP客户端接收数据
 * 
 * @param protocol_package_ 
 * @return int16_t 
 */
int16_t tcp_client_rx_data(protocol_package_t *protocol_package_);

/**
 * @brief 串口接收数据
 * 
 * @param protocol_package_ 
 * @return int16_t 
 */
int16_t uart_rx_data(protocol_package_t *protocol_package_);

/**
 * @brief 串口发送数据
 * 
 * @param protocol_package_ 
 * @return int16_t 
 */
int16_t uart_tx_data(protocol_package_t *protocol_package_);


#endif
