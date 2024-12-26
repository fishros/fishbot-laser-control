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
static char laser_baud_str[MOTOR_SPEED_LEN] = {"115200"};

static uint32_t udp_port = 3347;
static char motor_speed_str[MOTOR_SPEED_LEN] = {"600"};
static uint32_t motor_speed = 600;
static uint32_t laser_baud = 115200;
static mwifi_status_t current_status = WIFI_INIT;

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
            if (tcp_client_tx_data(&uart_rx_package_) < 0)
            {
                oled_update_with_wifi_status(WIFI_PING_TIMEOUT);
            }
            else
            {
                oled_update_with_wifi_status(WIFI_RUNNING);
            }
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

static void periodic_test_task(void *parameters)
{
    while (true)
    {
        if (current_status == WIFI_PING_TIMEOUT || current_status == WIFI_GOT_IP)
        {
            uart_rx_package_.size = sprintf((char *)uart_rx_package_.data, "hello fishros~\r\n");
            if (tcp_client_tx_data(&uart_rx_package_) < 0)
            {
                oled_update_with_wifi_status(WIFI_PING_TIMEOUT);
            }
            else
            {
                oled_update_with_wifi_status(WIFI_RUNNING);
            }
        }
        vTaskDelay(1000 / portTICK_PERIOD_MS); // Delay for 1 second
        esp_task_wdt_reset();
    }
    vTaskDelete(NULL);
}

void app_main(void)
{
    oled_init();
    uart_init(115200);
    mpwm_init();
    key_init();
    led_init();
    print_config();
    // pwm_set_percent(0, 0);
    // pwm_set_percent(1, 0);
    led_set_delay(1000);

    nvs_read_string("motor_speed", motor_speed_str, "600", MOTOR_SPEED_LEN);
    motor_speed = atoi(motor_speed_str);
    pwm_set_percent(0, motor_speed);
    pwm_set_percent(1, motor_speed);

    /*read config*/
    nvs_read_string("wifi_ssid", ssid, "fishbot", SSID_LEN);
    nvs_read_string("wifi_pswd", password, "12345678", PASSWORD_LEN);
    nvs_read_string("server_ip", udp_ip, "192.168.4.1", UDP_IP_LEN);
    nvs_read_string("server_port", udp_port_str, "8889", UDP_PORT_LEN);
    nvs_read_string("laser_baud", laser_baud_str, "115200", UDP_PORT_LEN);
    udp_port = atoi(udp_port_str);
    oled_update_with_wifi_status(WIFI_WAITING_FOR_CONNECTION);
    printf("read config ssid=%s,pswd=%s,udp_ip=%s,udp_port=%s,port=%d", ssid,
           password, udp_ip, udp_port_str, udp_port);

    /*wifi config*/
    wifi_init();
    wifi_set_as_sta(ssid, password);
    tcp_client_config_init(udp_ip, udp_port);
    tcp_client_protocol_task_init();

    // 启动时接收串口数据，看数据量大小，数据量大则为运行模式，无数据则为配置模式
    // uart_rx_package_.size = uart_read_bytes(UART_NUM_0, uart_rx_package_.data, RX_BUF_SIZE, 1000 / portTICK_PERIOD_MS);
    int sum_recv_data = 0;
    for (int i = 0; i < 10; i++)
    {
        if (uart_rx_data(&uart_rx_package_))
        {
            sum_recv_data += uart_rx_package_.size;
        }
    }
    if (sum_recv_data < 10)
    {
        oled_ascii(0, 1, "mode:flash&config");
        printf("$status=ready\n");
        xTaskCreate(periodic_test_task, "periodic_test_task", 4096, NULL, 5, NULL);
        while (true)
        {
            wificonfig_byuart();
            led_flash(); // 立即生效电机速度
            nvs_read_string("motor_speed", motor_speed_str, "600", MOTOR_SPEED_LEN);
            motor_speed = atoi(motor_speed_str);
        }
    }

    oled_ascii(0, 1, "mode:laser2wifi");
    pwm_set_percent(0, motor_speed);
    pwm_set_percent(1, motor_speed);
    led_task_init();
    led_set_delay(500);

    /*uart init*/
    laser_baud = atol(laser_baud_str);
    uart_protocol_task_init(laser_baud);

    /*tcp_client config*/

    xTaskCreate(data_uart_rx_data, "data_uart_rx_data", 4096 * 2, NULL, 4,
                NULL);
    xTaskCreate(data_tcp_rx_data, "data_tcp_rx_data", 4096 * 2, NULL, 4,
                NULL);
}

void oled_update_with_wifi_startup()
{
    static char temp_str[64];
    // wifi info
    // oled_ascii(0, 2, "wifi:wait connect");
    // wifi name
    sprintf(temp_str, "ssid:%s", ssid);
    oled_ascii(0, 3, temp_str);
    // wifi password
    sprintf(temp_str, "pswd:%s", password);
    oled_ascii(0, 4, temp_str);
    // local ip
    sprintf(temp_str, "ip:");
    oled_ascii(0, 5, temp_str);
    // udp server_ip
    sprintf(temp_str, "s_ip:%s", udp_ip);
    oled_ascii(0, 6, temp_str);
    // s_port
    sprintf(temp_str, "s_port:%s", udp_port_str);
    oled_ascii(0, 7, temp_str);
}

void oled_update_with_wifi_status(mwifi_status_t status)
{

    static char temp_str[64];

    if (status == WIFI_PING_TIMEOUT)
    {
        if (current_status == WIFI_WAITING_FOR_CONNECTION || current_status == WIFI_NOT_FOUND || current_status == WIFI_WRONG_PASSWORD || current_status == WIFI_WRONG_PASSWORD)
        {
            return;
        }
    }
    if (current_status != status)
    {
        current_status = status;
        switch (current_status)
        {
        case WIFI_WAITING_FOR_CONNECTION:
            /* code */
            oled_ascii(0, 2, "wifi:wait connect  ");
            oled_update_with_wifi_startup();
            break;
        case WIFI_NOT_FOUND:
            oled_update_with_wifi_startup();
            oled_ascii(0, 2, "wifi:wifi not found");
            break;
        case WIFI_WRONG_PASSWORD:
            oled_update_with_wifi_startup();
            oled_ascii(0, 2, "wifi:password error");
            break;
        case WIFI_GOT_IP:
        case WIFI_PING_TIMEOUT:
            oled_ascii(0, 2, "wifi:ping timeput  ");
            break;
        case WIFI_RUNNING:
            oled_ascii(0, 2, "wifi:running       ");
            break;
        default:
            break;
        }
    }
}
