/**
 * @author fishros (fishros@foxmail.com)
 * @brief  主函数
 * @version V1.0.0.220919
 * @date 2022-09-19
 *
 * 版权所有：FishBot Open Source Organization
 *
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
#include "led.h"
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

static void data_uart_rx_data(void *parameters)
{
    static int16_t uart_rx_len;
    while (true)
    {
        uart_rx_len = uart_rx_data(&uart_rx_package_);
        if (uart_rx_len > 0)
        {
            tcp_client_tx_data(&uart_rx_package_);
            led_flash();
        }
        esp_task_wdt_reset();
    }
    vTaskDelete(NULL);
}

static void data_tcp_rx_data(void *parameters)
{
    static int16_t tcp_client_rx_len;
    while (true)
    {
        tcp_client_rx_len = tcp_client_rx_data(&tcp_client_rx_package_);
        if (tcp_client_rx_len > 0)
        {
            uart_tx_data(&tcp_client_rx_package_);
            led_flash();
        }
        vTaskDelay(2 / portTICK_PERIOD_MS);
        esp_task_wdt_reset();
    }
    vTaskDelete(NULL);
}

void app_main(void)
{
    key_init();
    led_init();
    oled_init();
    int8_t is_config_wifi;
    nvs_read_uint8("is_smart", &is_config_wifi);
    if (is_config_wifi == NVS_DATA_UINT8_NONE || is_config_wifi == 0)
    {
        oled_ascii(0, 2, "Please Config First!");
        while (true)
        {
            wificonfig_byuart();
            led_flash();
        }
    }
    led_task_init();
    /*read config*/
    nvs_read_string("ssid", ssid, SSID_LEN);
    nvs_read_string("password", password, PASSWORD_LEN);
    nvs_read_string("udp_ip", udp_ip, UDP_IP_LEN);
    nvs_read_string("udp_port", udp_port_str, UDP_PORT_LEN);
    udp_port = atoi(udp_port_str);
    printf("read config ssid=%s,pswd=%s,udp_ip=%s,udp_port=%s,port=%d", ssid,
           password, udp_ip, udp_port_str, udp_port);

    led_set_delay(500);
    
    /*wifi config*/
    wifi_init();
    wifi_set_as_sta(ssid, password);

    /*uart init*/
    uart_protocol_task_init();

    /*tcp_client config*/
    tcp_client_config_init(udp_ip, udp_port);
    tcp_client_protocol_task_init();
    xTaskCreate(data_uart_rx_data, "data_uart_rx_data", 4096 * 2, NULL, 4,
                NULL);
    xTaskCreate(data_tcp_rx_data, "data_tcp_rx_data", 4096 * 2, NULL, 4,
                NULL);
}
