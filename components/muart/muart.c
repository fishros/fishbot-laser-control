/*
 * @作者: 小鱼
 * @公众号: 鱼香ROS
 * @QQ交流群: 2642868461
 * @描述: file content
 */
#include"muart.h"

#define ECHO_UART_PORT_NUM      0
#define ECHO_UART_BAUD_RATE     115200
#define ECHO_TASK_STACK_SIZE    4086

#define BUF_SIZE (512)

static void uart_init()
{
    uart_config_t uart_config = {
        .baud_rate = ECHO_UART_BAUD_RATE,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
    };
    int intr_alloc_flags = 0;
    ESP_ERROR_CHECK(uart_driver_install(ECHO_UART_PORT_NUM, BUF_SIZE * 2, 0, 0, NULL, intr_alloc_flags));
    ESP_ERROR_CHECK(uart_param_config(ECHO_UART_PORT_NUM, &uart_config));
}


void my_uart_send(char* data,uint8_t len)
{
    uart_write_bytes(ECHO_UART_PORT_NUM,data,len);
}


static void uart_task(void *arg)
{
    uart_init();
    uint8_t *data = (uint8_t *) malloc(BUF_SIZE);
    uint16_t read_len = 0;
    send_data("uart start with baut 115200",27);
    vTaskDelay(pdMS_TO_TICKS(100));
    uart_write_bytes(ECHO_UART_PORT_NUM,"stoplds$", 8);
    vTaskDelay(pdMS_TO_TICKS(1000));
    vTaskDelay(pdMS_TO_TICKS(1000));
    uart_write_bytes(ECHO_UART_PORT_NUM,"startlds$",9);
    vTaskDelay(pdMS_TO_TICKS(1000));
    while (1) {
        read_len = uart_read_bytes(ECHO_UART_PORT_NUM, data, BUF_SIZE, 8 / portTICK_RATE_MS);
        // if (read_len<22) continue;
        send_data((char*)data,read_len);
        vTaskDelay(2 / portTICK_PERIOD_MS);
        esp_task_wdt_reset();
    }
    // uart_write_bytes(ECHO_UART_PORT_NUM,"stoplds$", 8);
}


void my_uart_init()
{
    xTaskCreate(uart_task, "my_uart_init", ECHO_TASK_STACK_SIZE, NULL, 10, NULL);
}
