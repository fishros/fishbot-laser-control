/*
 * @作者: 小鱼
 * @公众号: 鱼香ROS
 * @QQ交流群: 2642868461
 * @描述: file content
 */
#include <stdio.h>

#include "esp_spi_flash.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "key.h"
#include "mwifi.h"
#include "nvs.h"
#include "oled.h"
#include "protocol.h"
#include "tcp_client_protocol.h"
#include "tcp_server.h"
#include "uart_protocol.h"
#include "wificonfig.h"
#include "esp_task_wdt.h"

static char ssid[SSID_LEN];
static char password[PASSWORD_LEN];
static char udp_ip[UDP_IP_LEN] = {"192.168.0.108"};
static char udp_port_str[UDP_PORT_LEN] = {"3347"};
static uint32_t udp_port = 3347;

static protocol_package_t uart_rx_package_;
static protocol_package_t tcp_client_rx_package_;

void app_main(void)
{
    key_init();
    oled_init();
    int8_t is_config_wifi;
    nvs_read_uint8("is_smart", &is_config_wifi);
    if (is_config_wifi == NVS_DATA_UINT8_NONE || is_config_wifi == 0)
    {
        oled_ascii(0, 2, "Please Config WIFI!");
        while (true)
        {
            wificonfig_byuart();
        }
    }
    /*read config*/
    nvs_read_string("ssid", ssid, SSID_LEN);
    nvs_read_string("password", password, PASSWORD_LEN);
    nvs_read_string("udp_ip", udp_ip, UDP_IP_LEN);
    nvs_read_string("udp_port", udp_port_str, UDP_PORT_LEN);
    udp_port = atoi(udp_port_str);
    printf("read config ssid=%s,pswd=%s,udp_ip=%s,udp_port=%s,port=%d", ssid,
           password, udp_ip, udp_port_str, udp_port);

    /*wifi config*/
    wifi_init();
    wifi_set_as_sta(ssid, password);
    /*uart init*/
    uart_protocol_task_init();
    /*tcp_client config*/
    tcp_client_config_init(udp_ip, udp_port);
    tcp_client_protocol_task_init();

    static int16_t uart_rx_len;
    static int16_t tcp_client_rx_len;
    while (true)
    {
        uart_rx_len = uart_rx_data(&uart_rx_package_);
        if (uart_rx_len > 0)
        {
            tcp_client_tx_data(&uart_rx_package_);
        }
        tcp_client_rx_len = tcp_client_rx_data(&tcp_client_rx_package_);
        if (tcp_client_rx_len > 0)
        {
            uart_tx_data(&tcp_client_rx_package_);
        }
        esp_task_wdt_reset();
    }
}
