
/**
 * @file uart_protocol.c
 * @author cuizhongren (1326986768@qq.com)
 * @brief
 * @version 0.1
 * @date 2022-07-08
 *
 * 版权所有：FishBot Open Source Organization
 *
 */
#include "uart_protocol.h"

#define UART_LOG_NUM UART_NUM_0

#define UART_PROTOC_NUM UART_NUM_0

#define TX_BUF_SIZE 1024 // 串口缓存帧的大小
#define RX_BUF_SIZE 1024 // 串口缓存帧的大小

static bool is_uart_init_ = false;

void uart_init(void)
{
    uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
    };
    int intr_alloc_flags = 0;
    ESP_ERROR_CHECK(uart_driver_install(UART_PROTOC_NUM, TX_BUF_SIZE * 2, 0, 0, NULL, intr_alloc_flags));
    ESP_ERROR_CHECK(uart_param_config(UART_PROTOC_NUM, &uart_config));
}

/**
 * @brief 串口接收数据
 *
 * @param protocol_package_
 * @return int16_t
 */
int16_t uart_rx_data(protocol_package_t *protocol_package_)
{
    // static int length = 0;
    static int rx_bytes_len;
    static size_t length;
    // ESP_LOGI("UART", "47");
    uart_get_buffered_data_len(UART_PROTOC_NUM, &length);
    // ESP_LOGI("UART", "49,%d", length);
    if (length <= 0)
    {
        return -1;
    }
    rx_bytes_len = uart_read_bytes(UART_PROTOC_NUM, protocol_package_->data,
                                   length, 0);
    if(rx_bytes_len<=0)
    {
        return -1;
    }
    protocol_package_->size = rx_bytes_len;
#ifdef DEBUG_FISHBOT
    print_frame_to_hex((uint8_t *)"rxraw",
                       (uint8_t *)protocol_package_->data, rx_bytes_len);
    printf("rx_bytes_len=%d\n", rx_bytes_len);
#endif
    return rx_bytes_len;
}

/**
 * @brief 串口发送数据
 *
 * @param protocol_package_
 * @return int16_t
 */
int16_t uart_tx_data(protocol_package_t *protocol_package_)
{
    static int16_t tx_bytes_len = 0;
    tx_bytes_len = uart_write_bytes(UART_PROTOC_NUM, (char *)protocol_package_->data,
                                    protocol_package_->size);
    return tx_bytes_len;
}

bool uart_protocol_task_init(void)
{
    uart_init();
    is_uart_init_ = true;
    return true;
}
