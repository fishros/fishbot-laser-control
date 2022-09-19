/**
 * @author fishros (fishros@foxmail.com)
 * @brief  待补充
 * @version V1.0.0.220919
 * @date 2022-0919
 *
 * 版权所有：FishBot Open Source Organization
 *
 */
#include "wificonfig.h"

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

void command_config_protocol_udp_client(char *ip, char *port)
{
    // write ip and port and checkout mode
    char rip[UDP_IP_LEN];
    char rport[UDP_PORT_LEN];
    sprintf(rip, "%s", ip);
    sprintf(rport, "%s", port);
    // nvs write ssid password
    nvs_write_string("udp_ip", rip);
    nvs_write_string("udp_port", rport);
    nvs_write_uint8("is_smart", 1);
}

void parse_command(uint8_t count, char result[][32])
{
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

void wificonfig_byuart(void)
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
