/**
 * @author fishros (fishros@foxmail.com)
 * @brief  待补充
 * @version V1.0.0.220919
 * @date 2022-0919
 *
 * 版权所有：FishBot Open Source Organization
 *
 */
#ifndef __MWIFI_H__
#define __MWIFI_H__

#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"

#include "lwip/err.h"
#include "lwip/sys.h"

#include "oled.h"




/**
*   初始化连接到其他
*/
void wifi_set_as_sta(char* ssid,char* pswd);
/**
*   初始化wifi
*/
void wifi_init(void);

#endif