/**
 * @author fishros (fishros@foxmail.com)
 * @brief  UDP客户端协议实现
 * @version V1.0.0.220919
 * @date 2022-09-19
 *
 * 版权所有：FishBot Open Source Organization
 *
 */
#include "udp_client_protocol.h"

#define FISHBOT_MODULE "UDP_CLIENT"

#define TX_BUF_SIZE 512 // 缓存帧的大小
#define RX_BUF_SIZE 512 // 缓存帧的大小

static bool is_udp_client_init_ = false;

static char ip_[16] = {"192.168.0.108"};
static uint32_t port_ = 3334;

static const char *TAG = "udp_client";

int udp_sock = -1;
char udp_addr_str[128];
int udp_addr_family = 0;
int udp_ip_protocol = 0;
struct sockaddr_in udp_destAddr;

static void connect_to_udp_server()
{
    if (udp_sock != -1)
    {
        ESP_LOGE(TAG, "Shutting down socket and restarting...");
        shutdown(udp_sock, 0);
        close(udp_sock);
    }
    // define setting
    udp_destAddr.sin_addr.s_addr = inet_addr(ip_);
    udp_destAddr.sin_family = AF_INET;
    udp_destAddr.sin_port = htons(port_);
    udp_addr_family = AF_INET;
    udp_ip_protocol = IPPROTO_IP;
    inet_ntoa_r(udp_destAddr.sin_addr, udp_addr_str, sizeof(udp_addr_str) - 1);
    // create socket
    udp_sock = socket(udp_addr_family, SOCK_DGRAM, udp_ip_protocol);

    if (udp_sock < 0)
    {
        ESP_LOGE(TAG, "Unable to create socket: errno %d", errno);
        return;
    }
    ESP_LOGI(TAG, "Socket created, connecting to %s:%d", ip_, port_);
    is_udp_client_init_ = true;
}

int16_t udp_client_tx_data(protocol_package_t *protocol_package_)
{
    static int16_t tx_bytes_len;
    if (is_udp_client_init_ == false)
    {
        vTaskDelay(20 / portTICK_PERIOD_MS);
        return -1;
    }
    tx_bytes_len = sendto(udp_sock, (char *)protocol_package_->data, protocol_package_->size, 0,
                          (struct sockaddr *)&udp_destAddr, sizeof(udp_destAddr));
#ifdef DEBUG_FISHBOT
    printf("send %s to server.. %d \n", protocol_package_->data,
           protocol_package_->size);
    printf("send len=%d \n", tx_bytes_len);
#endif
    if (tx_bytes_len < 0)
    {
        is_udp_client_init_ = false;
    }
    return tx_bytes_len;
}

int16_t udp_client_rx_data(protocol_package_t *protocol_package_)
{
    static int16_t rx_bytes_len = 0;
    struct sockaddr_in sourceAddr;
    socklen_t socklen = sizeof(sourceAddr);
    
    if (is_udp_client_init_ == false)
    {
        vTaskDelay(10 / portTICK_PERIOD_MS);
        return -1;
    }
    rx_bytes_len = recvfrom(udp_sock, protocol_package_->data, RX_BUF_SIZE - 1, 0,
                            (struct sockaddr *)&sourceAddr, &socklen);
    if (rx_bytes_len > 0)
    {
        protocol_package_->size = rx_bytes_len;
    }
    return rx_bytes_len;
}

static void udp_client_reconnect_task(void *parameters)
{
    while (true)
    {
        if (is_udp_client_init_ == false)
        {
            connect_to_udp_server();
            vTaskDelay(500 / portTICK_PERIOD_MS);
            continue;
        }
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

/**
 * @brief 任务初始化
 *
 * @return true
 * @return false
 */
bool udp_client_protocol_task_init(void)
{
    xTaskCreate(udp_client_reconnect_task, "udp_client_reconnect_task", 1024 * 2, NULL, 6,
                NULL);
    return true;
}

bool udp_client_config_init(char ip[16], uint32_t port)
{
    sprintf(ip_, "%s", ip);
    port_ = port;
    return true;
} 