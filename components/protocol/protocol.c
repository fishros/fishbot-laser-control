/**
 * @author fishros (fishros@foxmail.com)
 * @brief  待补充
 * @version V1.0.0.220919
 * @date 2022-09-19
 *
 * 版权所有：FishBot Open Source Organization
 *
 */
#include "protocol.h"

void print_frame_to_hex(uint8_t *title, uint8_t *buffer, uint16_t size)
{
    int i;
    printf("*************************start %s*******************************\n", title);
    for (i = 1; i <= size; i++)
    {
        printf("0x%02X ", buffer[i - 1]);
        if (i % 16 == 0)
        {
            printf("\n");
        }
    }
    printf("\n*************************end %s*******************************\n", title);
}
