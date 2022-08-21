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
static bool is_uart_init_ = false;
static char frame_buffer_tx_[TX_BUF_SIZE];
static char frame_buffer_rx_[RX_BUF_SIZE];
static protocol_package_t frame_pack_tx_;
static protocol_package_t frame_pack_rx_;

static char ip_[16] = {"192.168.0.108"};
static uint32_t port_ = 3334;

static const char *TAG = "tcp_client";
int sock = -1;
uint8_t send_error = 0;

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
bool tcp_client_protocol_init(xQueueHandle *data_uart_rx_queue, xQueueHandle *data_socket_rx_queue)
{
    data_uart_rx_queue_ = data_uart_rx_queue;
    data_socket_rx_queue_ = data_socket_rx_queue;
    return true;
}

static void tcp_client_tx_task(void *parameters)
{
    static int16_t tx_bytes_len = 0;
    int err;
    while (true)
    {
        if (is_uart_init_ == false)
        {
            vTaskDelay(20);
            continue;
        }
        if (xQueueReceive(*data_socket_rx_queue_, &frame_pack_tx_, 5) == pdTRUE)
        {
            err = send(sock, (char *)frame_pack_tx_.data, frame_pack_tx_.size, 0);
            if (errno == 12)
                continue;
        }
    }
}

static void tcp_client_rx_task(void *parameters)
{
    char addr_str[128];
    int addr_family = 0;
    int ip_protocol = 0;
    int rx_bytes_len = 0;

    while (1)
    {
        // define setting
        struct sockaddr_in destAddr;
        destAddr.sin_addr.s_addr = inet_addr(ip_);
        destAddr.sin_family = AF_INET;
        destAddr.sin_port = htons(port_);
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
        ESP_LOGI(TAG, "Socket created, connecting to %s:%d", ip_, port_);
        int err = connect(sock, (struct sockaddr *)&destAddr, sizeof(destAddr));
        if (err != 0)
        {
            ESP_LOGE(TAG, "Socket unable to connect: errno %d", errno);
            sock = -1;
            vTaskDelay(2000 / portTICK_PERIOD_MS);
            continue;
        }
        send(sock, "Successfully connected", 22, 0);
        send_error = 0;
        while (1)
        {
            rx_bytes_len = recv(sock, frame_pack_rx_.data, RX_BUF_SIZE - 1, 0);
            if (rx_bytes_len > 0)
            {
                frame_pack_rx_.size = rx_bytes_len;
                xQueueSend(*data_uart_rx_queue_, &frame_pack_rx_, 2 / portTICK_RATE_MS);
            }
            if (send_error)
            {
                shutdown(sock, 0);
                close(sock);
                break;
            }
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

/**
 * @brief 任务初始化
 *
 * @return true
 * @return false
 */
bool tcp_client_protocol_task_init(void)
{
    xTaskCreate(tcp_client_tx_task, "tcp_client_tx_task", 1024 * 2, NULL, 7,
                NULL);
    xTaskCreate(tcp_client_rx_task, "tcp_client_rx_task", 1024 * 2, NULL, 7,
                NULL);
    return true;
}
