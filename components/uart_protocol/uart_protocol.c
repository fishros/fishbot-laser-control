/**
 * @author fishros (fishros@foxmail.com)
 * @brief  待补充
 * @version V1.0.0.220919
 * @date 2022-09-19
 *
 * 版权所有：FishBot Open Source Organization
 *
 */
#include "uart_protocol.h"

static bool is_uart_init_ = false;

void uart_init(uint32_t baudrate)
{
    uart_config_t uart_config = {
        .baud_rate = baudrate,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
    };
    int intr_alloc_flags = 0;
    // is_uart_init_ = true;
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
    rx_bytes_len = uart_read_bytes(UART_PROTOC_NUM, protocol_package_->data,
                                   RX_BUF_SIZE, 6 / portTICK_PERIOD_MS);
    protocol_package_->size = rx_bytes_len;
    vTaskDelay(2 / portTICK_PERIOD_MS);

#ifdef DEBUG_FISHBOT
    // print_frame_to_hex((uint8_t *)"rxraw",
    //                    (uint8_t *)protocol_package_->data, rx_bytes_len);
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

bool uart_protocol_task_init(uint32_t baudrate)
{
    // uart_init(baudrate);
    // if (!is_uart_init_) {
    //     uart_init(baudrate);
    // } else {
    uart_set_baudrate(UART_PROTOC_NUM, baudrate);
    // }
    is_uart_init_ = true;
    return true;
}
