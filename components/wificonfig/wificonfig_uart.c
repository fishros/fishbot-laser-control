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

static char ssid[SSID_LEN];
static char password[PASSWORD_LEN];
static char server_ip[UDP_IP_LEN] = {"192.168.0.108"};
static char server_port[UDP_PORT_LEN] = {"3347"};
static char motor_speed[MOTOR_SPEED_LEN] = {"600"};

void command_restart()
{
    esp_restart();
}

void print_config()
{
    nvs_read_string("wifi_ssid", ssid, "fishbot", SSID_LEN);
    nvs_read_string("wifi_pswd", password, "12345678", PASSWORD_LEN);
    nvs_read_string("server_ip", server_ip, "192.168.4.1", UDP_IP_LEN);
    nvs_read_string("server_port", server_port, "8889", UDP_PORT_LEN);
    nvs_read_string("motor_speed", motor_speed, "600", MOTOR_SPEED_LEN);
    printf("$%s=%s\n", "wifi_ssid", ssid);
    printf("$%s=%s\n", "wifi_pswd", password);
    printf("$%s=%s\n", "server_ip", server_ip);
    printf("$%s=%s\n", "server_port", server_port);
    printf("$%s=%s\n", "motor_speed", motor_speed);
    printf("$board=laser_board\n");
}

void parse_command(uint8_t count, char result[][32])
{
    if (strcmp(result[0], "command") == 0)
    {
        if (strcmp(result[1], "restart") == 0)
        {
            command_restart();
        }
        else if (strcmp(result[1], "read_config") == 0)
        {
            print_config();
        }
        return;
    }
    else
    {
        nvs_write_string(result[0], result[1]);
        // nvs_write_uint8("is_smart", NVS_DATA_UINT8_CONFIG);
        printf("$result=ok\n");
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}

int8_t split_str(const char *line, char result[][32])
{
    if (line[0] != '$')
        return -1;
    uint16_t index = 0;
    uint16_t count = 0;
    uint16_t temp_index = 0;
    for (index = 1; line[index] != '\0'; index++)
    {
        if (line[index] == '=')
        {
            result[count++][temp_index++] = '\0';
            temp_index = 0;
            continue;
        }
        result[count][temp_index++] = line[index];
    }
    result[count][temp_index++] = '\0';
    if (count != 1)
    {
        return -1;
    }
    return count;
}

void wificonfig_byuart(void)
{
    char line[512];
    char result[10][32];
    int count = 0;
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
    if (count <= 0)
    {
        printf("$result=error parse\n");
        return;
    }
    parse_command(count, result);
}
