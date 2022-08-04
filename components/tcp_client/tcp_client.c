/*
 * @作者: 小鱼
 * @公众号: 鱼香ROS
 * @QQ交流群: 2642868461
 * @描述: file content
 */
#include "muart.h"
#include "tcp_client.h"

void print_hex(char *buffer, int len)
{
    int i;
    printf("******************start code**********************************\n");
    for (i = 1; i <= len; i++)
    {
        printf("0x%02X ", buffer[i - 1]);
        if (i % 16 == 0)
        {
            printf("\n");
        }
    }
    printf("\n");
    printf("********************end code************************************\n");
}

// #define HOST_IP_ADDR "10.55.23.25"
#define HOST_IP_ADDR "192.168.0.108"
#define PORT 3334

static const char *TAG = "tcp_client";

int sock = -1;
uint8_t send_error = 0;

void send_data(char *data, uint8_t len)
{
    if (sock > 0)
    {
        int err = send(sock,data,len, 0);
        if (err < 0 && send_error != 1 ) {
            ESP_LOGE(TAG, "Error occurred during sending: errno %d ,sock:%d ,send_error:%d", errno, sock,send_error);
            if(errno==12) return; 
            send_error = 1;
            if (send_error)
            {
                vTaskDelay(1000 / portTICK_PERIOD_MS);
                ESP_LOGE(TAG, "Delay wait sockt reconnect! sock:%d",sock);
            }
        }
    }
}


static void tcp_client_task(void *pvParameters)
{
    char rx_buffer[128];
    char host_ip[] = HOST_IP_ADDR;
    char addr_str[128];
    int addr_family = 0;
    int ip_protocol = 0;

    while (1)
    {
        // define setting
        struct sockaddr_in destAddr;
        destAddr.sin_addr.s_addr = inet_addr(HOST_IP_ADDR);
        destAddr.sin_family = AF_INET;
        destAddr.sin_port = htons(PORT);
        addr_family = AF_INET;
        ip_protocol = IPPROTO_IP;
        inet_ntoa_r(destAddr.sin_addr, addr_str, sizeof(addr_str) - 1);


        // create socket
        sock = socket(addr_family, SOCK_STREAM, ip_protocol);
        if (sock < 0)
        {
            ESP_LOGE(TAG, "Unable to create socket: errno %d", errno);
            break;
        }
        
        
        // socket connect
        ESP_LOGI(TAG, "Socket created, connecting to %s:%d", host_ip, PORT);
        int err = connect(sock, (struct sockaddr *)&destAddr, sizeof(destAddr));


        if (err != 0)
        {
            ESP_LOGE(TAG, "Socket unable to connect: errno %d", errno);
            sock = -1;
            vTaskDelay(2000 / portTICK_PERIOD_MS);
            continue;
        }

        send(sock,"Successfully connected",22,0);

        send_error = 0;
        while (1)
        {
            int len = recv(sock, rx_buffer, sizeof(rx_buffer) - 1, 0);
            if (len > 0)
            {
                my_uart_send(rx_buffer,len);
            }
            if(send_error){
                shutdown(sock, 0);
                close(sock);
                break;
            }
            vTaskDelay(512 / portTICK_PERIOD_MS);
        }
        if (sock != -1)
        {
            ESP_LOGE(TAG, "Shutting down socket and restarting...");
            shutdown(sock, 0);
            close(sock);
        }
    }
    vTaskDelete(NULL);
}

void my_tcp_init()
{
    xTaskCreate(tcp_client_task, "tcp_client", 4096, NULL, 5, NULL);
}