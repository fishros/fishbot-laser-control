
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

#define TX_BUF_SIZE 512 // 串口缓存帧的大小
#define RX_BUF_SIZE 512 // 串口缓存帧的大小

static xQueueHandle *data_uart_rx_queue_;
static xQueueHandle *data_socket_rx_queue_;
static bool is_uart_init_ = false;
static char frame_buffer_tx_[TX_BUF_SIZE];
static char frame_buffer_rx_[RX_BUF_SIZE];
static protocol_package_t frame_pack_tx_;
static protocol_package_t frame_pack_rx_;

void uart_init(void)
{
    const uart_config_t uart_config = {
        .baud_rate = 76800,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
    };
    int intr_alloc_flags = 0;
    ESP_ERROR_CHECK(uart_driver_install(UART_PROTOC_NUM, TX_BUF_SIZE * 2, 0, 0, NULL, intr_alloc_flags));
    ESP_ERROR_CHECK(uart_param_config(UART_PROTOC_NUM, &uart_config));
}

bool uart_protocol_init(xQueueHandle *data_uart_rx_queue, xQueueHandle *data_socket_rx_queue)
{
    data_uart_rx_queue_ = data_uart_rx_queue;
    data_socket_rx_queue_ = data_socket_rx_queue;
    // uart hardware init
    uart_init();
    is_uart_init_ = true;
    return is_uart_init_;
}

static void uart_tx_task(void *pvParameters)
{
    static int16_t tx_bytes_len = 0;
    while (true)
    {
        if (is_uart_init_ == false)
        {
            vTaskDelay(20);
            continue;
        }
        if (xQueueReceive(*data_socket_rx_queue_, &frame_pack_tx_, 5) == pdTRUE)
        {
            tx_bytes_len = uart_write_bytes(UART_PROTOC_NUM, (char *)frame_pack_tx_.data,
                                            frame_pack_tx_.size);
            if (tx_bytes_len <= 0)
            {
                // TODO() Add Error LOG!
            }
#ifdef DEBUG_UART
            print_hex();
#endif
        }
    }
}

static void uart_rx_task(void *pvParameters)
{
    static uint16_t i = 0;
    static uint16_t rx_index = 0;
    static int16_t frame_start_index = -1;
    static int16_t frame_end_index = -1;
    uint16_t rx_bytes_len;
    while (true)
    {
        if (is_uart_init_ == false)
        {
            vTaskDelay(20);
            continue;
        }
        rx_bytes_len = uart_read_bytes(UART_PROTOC_NUM, frame_pack_rx_.data,
                                       RX_BUF_SIZE, 10 / portTICK_RATE_MS);
        if (rx_bytes_len <= 0)
        {
            continue;
        }
#ifdef DEBUG_FISHBOT
        print_frame_to_hex((uint8_t *)"rxraw",
                           (uint8_t *)frame_buffer_rx_ + rx_index, rx_bytes_len);
        printf("rx_index=%d,rx_bytes_len=%d\n", rx_index, rx_bytes_len);
#endif
        frame_pack_rx_.size = rx_bytes_len;
        xQueueSend(*data_uart_rx_queue_, &frame_pack_rx_, 2 / portTICK_RATE_MS);
    }
}

bool uart_protocol_task_init(void)
{
    xTaskCreate(uart_rx_task, "uart_rx_task", 1024 * 2, NULL, 5,
                NULL); //接收任务
    xTaskCreate(uart_tx_task, "uart_tx_task", 1024 * 2, NULL, 4,
                NULL); //发送任务
    return true;
}

bool uart_protocol_recv_task_init(void)
{
    xTaskCreate(uart_rx_task, "uart_rx_task", 1024 * 2, NULL, 5,
                NULL); //接收任务
    return true;
}