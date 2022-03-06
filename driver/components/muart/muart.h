/*
 * @作者: 小鱼
 * @公众号: 鱼香ROS
 * @QQ交流群: 2642868461
 * @描述: file content
 */
#pragma once
#ifdef __cplusplus
extern "C" {
#endif
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/uart.h"
#include "driver/gpio.h"
#include "tcp_client.h"


typedef struct 
{
    uint8_t START;
    uint8_t TARGET;
    uint8_t CODE;
	uint8_t data_len;
    short sacc[3]; 
    short sgy[3]; 
    short sangle[3];
    uint8_t sum;
    uint8_t END;
}__attribute__ ((packed)) imu_t;


void my_uart_init();
void my_uart_send(char* data,uint8_t len);

#ifdef __cplusplus
}
#endif