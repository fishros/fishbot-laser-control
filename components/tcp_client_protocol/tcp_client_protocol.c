/*
 * @作者: 小鱼
 * @公众号: 鱼香ROS
 * @QQ交流群: 2642868461
 * @描述: file content
 */
#include "tcp_client_protocol.h"

#define FISHBOT_MODULE "TCP_CLIENT"

#define TX_BUF_SIZE 512 // 缓存帧的大小
#define RX_BUF_SIZE 512 // 缓存帧的大小

static xQueueHandle *data_uart_rx_queue_;
static xQueueHandle *data_socket_rx_queue_;
static bool is_tcp_client_init_ = false;
static char frame_buffer_tx_[TX_BUF_SIZE];
static char frame_buffer_rx_[RX_BUF_SIZE];
static protocol_package_t frame_pack_tx_;
static protocol_package_t frame_pack_rx_;

static char ip_[16] = {"192.168.0.108"};
static uint32_t port_ = 3334;

static const char *TAG = "tcp_client";

int sock = -1;
char addr_str[128];
int addr_family = 0;
int ip_protocol = 0;
struct sockaddr_in destAddr;

bool tcp_client_config_init(char ip[16], uint32_t port)
{
    sprintf(ip_, "%s", ip);
    port_ = port;
    return true;
}

/**
 * @brief 协议初始化
 *
 * @param data_uart_rx_queue 串口接收队列
 * @param data_socket_rx_queue 网络数据接收队列
 * @return true
 * @return false
 */
bool tcp_client_protocol_init(xQueueHandle *data_uart_rx_queue,
                              xQueueHandle *data_socket_rx_queue)
{
    data_uart_rx_queue_ = data_uart_rx_queue;
    data_socket_rx_queue_ = data_socket_rx_queue;
    return true;
}

static void connect_to_tcp_server()
{
    if (sock != -1)
    {
        ESP_LOGE(TAG, "Shutting down socket and restarting...");
        shutdown(sock, 0);
        close(sock);
    }
    // define setting
    destAddr.sin_addr.s_addr = inet_addr(ip_);
    destAddr.sin_family = AF_INET;
    destAddr.sin_port = htons(port_);
    addr_family = AF_INET;
    ip_protocol = IPPROTO_IP;
    inet_ntoa_r(destAddr.sin_addr, addr_str, sizeof(addr_str) - 1);
    // create socket
    sock = socket(addr_family, SOCK_STREAM, ip_protocol);
    ESP_LOGE(TAG, "1");
    if (sock < 0)
    {
        ESP_LOGE(TAG, "Unable to create socket: errno %d", errno);
        return;
    }
    ESP_LOGE(TAG, "2");
    ESP_LOGI(TAG, "Socket created, connecting to %s:%d", ip_, port_);
    int err = connect(sock, (struct sockaddr *)&destAddr, sizeof(destAddr));
    if (err != 0)
    {
        ESP_LOGE(TAG, "Socket unable to connect: errno %d", errno);
        close(sock);
        return;
    }
    ESP_LOGE(TAG, "3");
    is_tcp_client_init_ = true;
}

static void tcp_client_tx_task(void *parameters)
{
    static int16_t tx_bytes_len = 0;
    int len;
    while (true)
    {
        if (is_tcp_client_init_ == false)
        {
            ESP_LOGE(TAG, "4");
            connect_to_tcp_server();
            vTaskDelay(1000 / portTICK_PERIOD_MS);
            continue;
        }
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        ESP_LOGE(TAG, "6");
        if (xQueueReceive(*data_uart_rx_queue_, &frame_pack_tx_, 5) == pdTRUE)
        {
            printf("send %s to server.. %d \n", frame_pack_tx_.data,
                   frame_pack_tx_.size);
            len = send(sock, (char *)frame_pack_tx_.data, frame_pack_tx_.size, 0);
            printf("send len=%d \n", len);
            if (len < 0)
            {
                is_tcp_client_init_ = false;
                continue;
            }
        }
    }
}

static void tcp_client_rx_task(void *parameters)
{
    int rx_bytes_len = 0;
    while (true)
    {
        if (is_tcp_client_init_ == false)
        {
            ESP_LOGE(TAG, "5");
            vTaskDelay(500 / portTICK_PERIOD_MS);
            continue;
        }
        rx_bytes_len =
            recv(sock, frame_pack_rx_.data, RX_BUF_SIZE - 1, 10 / portTICK_RATE_MS);
        if (rx_bytes_len > 0)
        {
            frame_pack_rx_.size = rx_bytes_len;
            xQueueSend(*data_uart_rx_queue_, &frame_pack_rx_, 2 / portTICK_RATE_MS);
        }
    }
    vTaskDelete(NULL);
}

/**
 * @brief 任务初始化
 *
 * @return true
 * @return false
 */
bool tcp_client_protocol_task_init(void)
{
    xTaskCreate(tcp_client_tx_task, "tcp_client_tx_task", 1024 * 2, NULL, 5,
                NULL);
    xTaskCreate(tcp_client_rx_task, "tcp_client_rx_task", 1024 * 2, NULL, 5,
                NULL);
    return true;
}
