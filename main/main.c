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

/**
 *
 *
 * config,wifi,m,88888888
 * config,proto,udp_client,192.168.1.1,3347
 * config,proto,udp_server,0.0.0.0,3347
 * restart
 *
 */
#define SSID_LEN 33
#define PASSWORD_LEN 65
#define BUFF_SIZE 512

static xQueueHandle data_uart_rx_queue_;
static xQueueHandle data_socket_rx_queue_;
static char ssid[SSID_LEN];
static char password[PASSWORD_LEN];
static char host_ip[16] = {"192.168.0.108"};
static uint32_t port = 3347;

#include <stdio.h>
#include <string.h>

void command_restart()
{
    esp_restart();
}

void command_config_wifi_sta(char *ssid, char *password)
{
    char rssid[SSID_LEN];
    char rpassword[PASSWORD_LEN];
    sprintf(rssid, "%s", ssid);
    sprintf(rpassword, "%s", password);
    // nvs write ssid password
    printf("config wifi %s, %s\n", ssid, password);
    nvs_write_string("ssid", (char *)rssid);
    nvs_write_string("password", (char *)rpassword);
    nvs_write_uint8("is_smart", 1);
}

#define UDP_IP_LEN 20
#define UDP_PORT_LEN 8
void command_config_protocol_udp_client(char *ip, char *port)
{
    // write ip and port and checkout mode
    char rip[UDP_IP_LEN];
    char rport[UDP_PORT_LEN];
    sprintf(rip, "%s", ip);
    sprintf(rport, "%s", port);
    // nvs write ssid password
    // nvs_read_string("udp_ip", rip, UDP_IP_LEN);
    // nvs_read_string("udp_port", rport, UDP_PORT_LEN);
}

void parse_command(uint8_t count, char result[][32])
{
    printf("======================\n");
    for (uint8_t index = 0; index < count; index++)
    {
        printf("%s\n", result[index]);
    }
    if (strcmp(result[0], "restart") == 0)
    {
        command_restart();
    }

    if (strcmp(result[0], "config") == 0)
    {
        if (strcmp(result[1], "wifi") == 0)
        {
            command_config_wifi_sta(result[2], result[3]);
        }
        if (strcmp(result[1], "proto") == 0)
        {
            if (strcmp(result[2], "udp_client") == 0)
            {
                command_config_protocol_udp_client(result[3], result[4]);
            }
        }
    }
}

uint8_t split_str(char *line, char result[][32])
{
    uint16_t index = 0;
    uint16_t count = 0;
    uint16_t temp_index = 0;
    for (index = 0; line[index] != '\0'; index++)
    {
        if (line[index] == ',')
        {
            result[count++][temp_index++] = '\0';
            temp_index = 0;
            continue;
        }
        result[count][temp_index++] = line[index];
    }
    result[count][temp_index++] = '\0';
    return count + 1;
}

void config_from_stdin(void)
{
    char line[512];
    char result[10][32];
    int count = 0;
    printf("wait for command:\n");
    while (count < 128)
    {
        int c = fgetc(stdin);
        if (c == '\n')
        {
            line[count] = '\0';
            break;
        }
        else if (c > 0 && c < 127)
        {
            line[count] = c;
            ++count;
        }
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
    count = split_str(line, result);
    parse_command(count, result);
}

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
            config_from_stdin();
        }
    }
    else
    {
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
