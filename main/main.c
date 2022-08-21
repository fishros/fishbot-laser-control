/*
 * @作者: 小鱼
 * @公众号: 鱼香ROS
 * @QQ交流群: 2642868461
 * @描述: file content
 */
#include <stdio.h>
#include "esp_system.h"
#include "esp_spi_flash.h"
#include "mwifi.h"
#include "oled.h"
#include "nvs.h"
#include "wificonfig.h"
#include "key.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "tcp_server.h"
#include "protocol.h"
#include "uart_protocol.h"
#include "tcp_client_protocol.h"

#define SSID_LEN 33
#define PASSWORD_LEN 65
#define BUFF_SIZE 512

static xQueueHandle data_uart_rx_queue_;
static xQueueHandle data_socket_rx_queue_;

void app_main(void)
{
    key_init();
    oled_init();
    int8_t is_config_wifi = 0;
    // nvs_write_uint8("is_smart", 0);
    nvs_read_uint8("is_smart", &is_config_wifi);
    if (is_config_wifi == 0)
    {
        oled_ascii(0, 2, "Please Config WIFI!");
        wificonfig_start();
    }
    else
    {
        char ssid[SSID_LEN];
        char password[PASSWORD_LEN];
        char host_ip[16] = {"192.168.0.108"};
        uint32_t port = 3347;
        nvs_read_string("ssid", ssid, SSID_LEN);
        nvs_read_string("password", password, PASSWORD_LEN);
        data_uart_rx_queue_ = xQueueCreate(5, sizeof(protocol_package_t));
        data_socket_rx_queue_ = xQueueCreate(5, sizeof(protocol_package_t));

        wifi_init();
        wifi_set_as_sta(ssid, password);

        uart_protocol_init(&data_uart_rx_queue_, &data_socket_rx_queue_);
        uart_protocol_task_init();

        tcp_client_config_init(host_ip, port);
        tcp_client_protocol_init(&data_uart_rx_queue_, &data_socket_rx_queue_);
        tcp_client_protocol_task_init();
    }
}
