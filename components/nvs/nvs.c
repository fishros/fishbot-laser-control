/**
 * @author fishros (fishros@foxmail.com)
 * @brief  待补充
 * @version V1.0.0.220919
 * @date 2022-09-19
 *
 * 版权所有：FishBot Open Source Organization
 *
 */
#include "nvs.h"

static const char *TAG = "NVS_OPERATE";

// 数据库的表名
static const char *TB_SELF = "fishbot";

void nvs_read_struct(char *struct_name, void *pStruct, uint32_t length)
{
    nvs_handle mHandleNvsRead;
    esp_err_t err = nvs_open(TB_SELF, NVS_READWRITE, &mHandleNvsRead);
    memset(pStruct, 0x0, length);
    err = nvs_get_blob(mHandleNvsRead, struct_name, pStruct, &length);
    if (err == ESP_OK)
    {
        ESP_LOGI(TAG, "read struct %s success length=%d", struct_name, length);
    }
    else
    {
        ESP_LOGE(TAG, "read struct %s failed", struct_name);
    }
    nvs_close(mHandleNvsRead);
}

int8_t nvs_read_string(char *string_name, char *data, const char *defaultvalue, size_t len)
{
    nvs_handle mHandleNvsRead;
    esp_err_t err = nvs_open(TB_SELF, NVS_READWRITE, &mHandleNvsRead);
    err = nvs_get_str(mHandleNvsRead, string_name, data, &len);
    nvs_close(mHandleNvsRead);
    if (err == ESP_OK)
    {
        return 0;
    }
    else
    {
        sprintf(data, "%s", defaultvalue);
        return -1;
    }
}

void nvs_read_uint8(char *i8_name, int8_t *i8)
{
    nvs_handle mHandleNvsRead;
    esp_err_t err = nvs_open(TB_SELF, NVS_READWRITE, &mHandleNvsRead);
    // 读取 i8
    err = nvs_get_i8(mHandleNvsRead, i8_name, i8);
    if (err == ESP_OK)
    {
        // ESP_LOGI(TAG, "get %s nvs_i8 = %d ", i8_name, *i8);
    }
    else
    {
        // ESP_LOGI(TAG, "get nvs_i8 %s error", i8_name);
        *i8 = NVS_DATA_UINT8_NONE;
    }
    nvs_close(mHandleNvsRead);
}

void nvs_write_uint8(char *i8_name, int8_t i8)
{
    nvs_handle mHandleNvs;
    esp_err_t err = nvs_open(TB_SELF, NVS_READWRITE, &mHandleNvs);
    // 保存一个 int8_t
    err = nvs_set_i8(mHandleNvs, i8_name, i8);
    // if (err != ESP_OK)
    //     ESP_LOGE(TAG, "Save NVS %s  error !!", i8_name);
    // else
    //     ESP_LOGI(TAG, "Save NVS %s  i8 ok !! nvs_i8 = %d ", i8_name, i8);

    nvs_commit(mHandleNvs);
    nvs_close(mHandleNvs);
}

int8_t nvs_write_string(char *string_name, char *string_value)
{
    nvs_handle mHandleNvs;
    esp_err_t err = nvs_open(TB_SELF, NVS_READWRITE, &mHandleNvs);
    if (nvs_set_str(mHandleNvs, string_name, string_value) != ESP_OK)
        return 0;
    return -1;
    // ESP_LOGE(TAG, "Save NVS String Fail !!  ");
    // else
    // ESP_LOGI(TAG, "Save NVS String ok !! data : %s ", string_value);

    nvs_commit(mHandleNvs);
    nvs_close(mHandleNvs);
}

void nvs_write_struct(char *struct_name, void *pStruct, uint32_t length)
{
    nvs_handle mHandleNvs;
    esp_err_t err = nvs_open(TB_SELF, NVS_READWRITE, &mHandleNvs);

    if (nvs_set_blob(mHandleNvs, struct_name, pStruct, length) != ESP_OK)
        ESP_LOGE(TAG, "Save Struct  Fail !!  ");
    else
        ESP_LOGI(TAG, "Save Struct  ok !!  length=%d\n", length);

    nvs_commit(mHandleNvs);
    nvs_close(mHandleNvs);
}

void nvs_test(void)
{
    // nvs_write_string("ssid", "mweb");
    // nvs_read_string("ssid");
    // device d = {
    //     .id=1,
    //     .hard_id=1234568,
    //     .ssid="mweb",
    //     .pswd="88888888"
    // };
    // nvs_write_struct(&d);
    // nvs_read_struct(&d);
}
