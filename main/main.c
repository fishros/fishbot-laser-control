/*
 * @作者: 小鱼
 * @公众号: 鱼香ROS
 * @QQ交流群: 2642868461
 * @描述: file content
 */
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include "udp_client.h"
#include "tcp_client.h"
#include "mwifi.h"
#include "muart.h"

void app_main(void)
{
    wifi_init();
    wifi_set_as_sta("m","88888888");
    my_tcp_init();
    my_uart_init();
}
