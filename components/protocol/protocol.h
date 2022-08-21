#ifndef PROTOCOL_H_
#define PROTOCOL_H_
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#define RX_TX_PACKET_SIZE 512

typedef struct
{
    uint8_t size;
    uint8_t data[RX_TX_PACKET_SIZE];
}  __attribute__((packed)) protocol_package_t;

#endif
