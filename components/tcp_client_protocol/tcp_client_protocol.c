/**
 * @author fishros (fishros@foxmail.com)
 * @brief  待补充
 * @version V1.0.0.220919
 * @date 2022-09-19
 *
 * 版权所有：FishBot Open Source Organization
 *
 */
#include "tcp_client_protocol.h"

#define FISHBOT_MODULE "TCP_CLIENT"

#define TX_BUF_SIZE 512 // 缓存帧的大小
#define RX_BUF_SIZE 512 // 缓存帧的大小

static bool is_tcp_client_init_ = false;

static char ip_[16] = {"192.168.0.108"};
static uint32_t port_ = 3334;

static const char *TAG = "tcp_client";


int tcp_sock = -1;
char tcp_addr_str[128];
int tcp_addr_family = 0;
int tcp_ip_protocol = 0;
struct sockaddr_in tcp_destAddr;

static void connect_to_tcp_server()
{
    if (tcp_sock != -1)
    {
        ESP_LOGE(TAG, "Shutting down socket and restarting...");
        shutdown(tcp_sock, 0);
        close(tcp_sock);
    }
    // define setting
    tcp_destAddr.sin_addr.s_addr = inet_addr(ip_);
    tcp_destAddr.sin_family = AF_INET;
    tcp_destAddr.sin_port = htons(port_);
    tcp_addr_family = AF_INET;
    tcp_ip_protocol = IPPROTO_IP;
    inet_ntoa_r(tcp_destAddr.sin_addr, tcp_addr_str, sizeof(tcp_addr_str) - 1);
    // create socket
    tcp_sock = socket(tcp_addr_family, SOCK_STREAM, tcp_ip_protocol);

    if (tcp_sock < 0)
    {
        ESP_LOGE(TAG, "Unable to create socket: errno %d", errno);
        return;
    }
    int flag = 1;
    setsockopt(tcp_sock, IPPROTO_TCP, TCP_NODELAY, &flag, sizeof(flag));
    ESP_LOGI(TAG, "Socket created, connecting to %s:%d", ip_, port_);
    int err = connect(tcp_sock, (struct sockaddr *)&tcp_destAddr, sizeof(tcp_destAddr));
    if (err != 0)
    {
        ESP_LOGE(TAG, "Socket unable to connect: errno %d", errno);
        close(tcp_sock);
        return;
    }
    is_tcp_client_init_ = true;
}

int16_t tcp_client_tx_data(protocol_package_t *protocol_package_)
{
    static int16_t tx_bytes_len;
    if (is_tcp_client_init_ == false)
    {
        vTaskDelay(20 / portTICK_PERIOD_MS);
        return -1;
    }
    tx_bytes_len = send(tcp_sock, (char *)protocol_package_->data, protocol_package_->size, 0);
#ifdef DEBUG_FISHBOT
    printf("send %s to server.. %d \n", protocol_package_->data,
           protocol_package_->size);
    printf("send len=%d \n", tx_bytes_len);
#endif
    if (tx_bytes_len < 0)
    {
        is_tcp_client_init_ = false;
    }
    // printf("send len1=%d \n", tx_bytes_len);
    return tx_bytes_len;
}

int16_t tcp_client_rx_data(protocol_package_t *protocol_package_)
{
    static int16_t rx_bytes_len = 0;
    if (is_tcp_client_init_ == false)
    {
        vTaskDelay(10 / portTICK_PERIOD_MS);
        return -1;
    }
    rx_bytes_len =
        recv(tcp_sock, protocol_package_->data, RX_BUF_SIZE - 1, 0);
    if (rx_bytes_len > 0)
    {
        protocol_package_->size = rx_bytes_len;
    }
    return rx_bytes_len;
}

static void tcp_client_reconnect_task(void *parameters)
{
    while (true)
    {
        if (is_tcp_client_init_ == false)
        {
            connect_to_tcp_server();
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
bool tcp_client_protocol_task_init(void)
{
    xTaskCreate(tcp_client_reconnect_task, "tcp_client_reconnect_task", 1024 * 2, NULL, 6,
                NULL);
    return true;
}

bool tcp_client_config_init(char ip[16], uint32_t port)
{
    sprintf(ip_, "%s", ip);
    port_ = port;
    return true;
}
