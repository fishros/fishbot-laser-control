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
#include "mpwm.h"

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
    oled_init();
    mpwm_init();
    key_init();
    led_init();
    pwm_set_percent(0, 600);
    pwm_set_percent(1, 600);
    print_config();
    int8_t is_config_wifi;
    nvs_read_uint8("is_smart", &is_config_wifi);
    if (is_config_wifi == NVS_DATA_UINT8_NONE || is_config_wifi == 0)
    {
        oled_ascii(0, 2, "MODE   :CONFIG MODE");
        oled_ascii(0, 3, "STATUS :Wait config");
        printf("$status=ready\n");
        while (true)
        {
            wificonfig_byuart();
            led_flash();
        }
    }
    oled_ascii(0, 2, "MODE:RUN MODE");
    oled_ascii(0, 3, "WIFI:Wait connect");

    led_task_init();
    /*read config*/
    nvs_read_string("wifi_ssid", ssid, "fishbot", SSID_LEN);
    nvs_read_string("wifi_pswd", password, "12345678", PASSWORD_LEN);
    nvs_read_string("server_ip", udp_ip, "192.168.4.1", UDP_IP_LEN);
    nvs_read_string("server_port", udp_port_str, "8889", UDP_PORT_LEN);
    udp_port = atoi(udp_port_str);
    printf("read config ssid=%s,pswd=%s,udp_ip=%s,udp_port=%s,port=%d", ssid,
           password, udp_ip, udp_port_str, udp_port);
    // 显示相关
    char oled_ssid[128];
    sprintf(oled_ssid, "SSID:%s", ssid);
    oled_ascii(0, 4, oled_ssid);

    char oled_pswd[128];
    sprintf(oled_pswd, "PSWD:%s", password);
    oled_ascii(0, 5, oled_pswd);

    char oled_sip[128];
    sprintf(oled_sip, "S_IP:%s", udp_ip);
    oled_ascii(0, 6, oled_sip);

    char oled_sport[128];
    sprintf(oled_sport, "PORT:%s", udp_port_str);
    oled_ascii(0, 7, oled_sport);

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
