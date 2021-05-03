/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>





#include "cJSON.h"
#ifdef ATM_ENABLED
    #include "at_api.h"
#endif

char g_product_key[IOTX_PRODUCT_KEY_LEN + 1]       = "a16OKk6dTya";
/* setup your productSecret !!! */
char g_product_secret[IOTX_PRODUCT_SECRET_LEN + 1] = "";
/* setup your deviceName !!! */
char g_device_name[IOTX_DEVICE_NAME_LEN + 1]       = "";
/* setup your deviceSecret !!! */
char g_device_secret[IOTX_DEVICE_SECRET_LEN + 1]   = "";

#define EXAMPLE_TRACE(...)                                          \
    do {                                                            \
        HAL_Printf("\033[1;32;40m%s.%d: ", __func__, __LINE__);     \
        HAL_Printf(__VA_ARGS__);                                    \
        HAL_Printf("\033[0m\r\n");                                  \
    } while (0)
#define EXAMPLE_MASTER_DEVID            (0)
#define EXAMPLE_YIELD_TIMEOUT_MS        (200)

typedef struct {
    int master_devid;
    int cloud_connected;
    int master_initialized;
} user_example_ctx_t;
static user_example_ctx_t g_user_example_ctx;

/**
 * @brief 上报属性ParkingState到云端
 * @param devid 由调用IOT_Linkkit_Open返回的设备标示符
 * @param value 属性ParkingState的值，数据类型uint8_t
 * @return 消息id:(>=1), 上报失败: <0
 */
int32_t app_post_property_ParkingState(uint32_t devid, uint8_t value);

/**
 * @brief 上报属性RSSI到云端
 * @param devid 由调用IOT_Linkkit_Open返回的设备标示符
 * @param value 属性RSSI的值，数据类型int32_t
 * @return 消息id:(>=1), 上报失败: <0
 */
int32_t app_post_property_RSSI(uint32_t devid, int32_t value);

/**
 * @brief 上报事件ParkingChangeNotification到云端
 * @param devid 由调用IOT_Linkkit_Open返回的设备标示符
 * @param value 事件ParkingChangeNotification的值，数据类型uint8_t
 * @return 消息id:(>=1), 上报失败: <0
 */
int32_t app_post_event_ParkingChangeNotification(uint32_t devid, uint8_t value);

/**
 * @brief 上报事件HeartbeatNotification到云端
 * @param devid 由调用IOT_Linkkit_Open返回的设备标示符
 * @param value 事件HeartbeatNotification的值，数据类型uint8_t
 * @return 消息id:(>=1), 上报失败: <0
 */
int32_t app_post_event_HeartbeatNotification(uint32_t devid, uint8_t value);

/**
 * @brief 上报事件HeartbeatNotification到云端
 * @param devid 由调用IOT_Linkkit_Open返回的设备标示符
 * @param value 事件HeartbeatNotification的值，数据类型double
 * @return 消息id:(>=1), 上报失败: <0
 */
int32_t app_post_event_HeartbeatNotification(uint32_t devid, double value);

/**
 * @brief 上报事件Error到云端
 * @param devid 由调用IOT_Linkkit_Open返回的设备标示符
 * @param value 事件Error的值，数据类型uint32_t
 * @return 消息id:(>=1), 上报失败: <0
 */
int32_t app_post_event_Error(uint32_t devid, uint32_t value);


/**
 * @brief 上报属性ParkingState到云端
 * @param devid 由调用IOT_Linkkit_Open返回的设备标示符
 * @param value 属性ParkingState的值，数据类型uint8_t
 * @return 消息id:(>=1), 上报失败: <0
 */
int app_post_property_ParkingState(uint32_t devid, uint8_t value)
{
    int32_t res = STATE_USER_INPUT_BASE;
    char property_payload[64] = {0};

    res = HAL_Snprintf(property_payload, sizeof(property_payload), "{\"ParkingState\": %d}", value);
    if (res < 0) {
        return STATE_SYS_DEPEND_SNPRINTF;
    }

    res = IOT_Linkkit_Report(devid, ITM_MSG_POST_PROPERTY,
                            (uint8_t *)property_payload, strlen(property_payload));
    return res;
}

/**
 * @brief 上报属性RSSI到云端
 * @param devid 由调用IOT_Linkkit_Open返回的设备标示符
 * @param value 属性RSSI的值，数据类型int32_t
 * @return 消息id:(>=1), 上报失败: <0
 */
int app_post_property_RSSI(uint32_t devid, int32_t value)
{
    int32_t res = STATE_USER_INPUT_BASE;
    char property_payload[64] = {0};

    res = HAL_Snprintf(property_payload, sizeof(property_payload), "{\"RSSI\": %d}", value);
    if (res < 0) {
        return STATE_SYS_DEPEND_SNPRINTF;
    }

    res = IOT_Linkkit_Report(devid, ITM_MSG_POST_PROPERTY,
                            (uint8_t *)property_payload, strlen(property_payload));
    return res;
}

/**
 * @brief 上报事件ParkingChangeNotification到云端
 * @param devid 由调用IOT_Linkkit_Open返回的设备标示符
 * @param value 事件ParkingChangeNotification的值，数据类型uint8_t
 * @return 消息id:(>=1), 上报失败: <0
 */
int app_post_event_ParkingChangeNotification(uint32_t devid, uint8_t value)
{
    int32_t res = STATE_USER_INPUT_BASE;
    char *event_id = "ParkingChangeNotification";
    char event_payload[64] = {0};

    res = HAL_Snprintf(event_payload, sizeof(event_payload), "{\"ParkingEvent\": %d}", value);
    if (res < 0) {
        return STATE_SYS_DEPEND_SNPRINTF;
    }

    res = IOT_Linkkit_TriggerEvent(EXAMPLE_MASTER_DEVID, event_id, strlen(event_id),
                                   event_payload, strlen(event_payload));
    return res;
}

/**
 * @brief 上报事件HeartbeatNotification到云端
 * @param devid 由调用IOT_Linkkit_Open返回的设备标示符
 * @param value 事件HeartbeatNotification的值，数据类型uint8_t
 * @return 消息id:(>=1), 上报失败: <0
 */
int app_post_event_HeartbeatNotification(uint32_t devid, uint8_t value)
{
    int32_t res = STATE_USER_INPUT_BASE;
    char *event_id = "HeartbeatNotification";
    char event_payload[64] = {0};

    res = HAL_Snprintf(event_payload, sizeof(event_payload), "{\"ParkingState\": %d}", value);
    if (res < 0) {
        return STATE_SYS_DEPEND_SNPRINTF;
    }

    res = IOT_Linkkit_TriggerEvent(EXAMPLE_MASTER_DEVID, event_id, strlen(event_id),
                                   event_payload, strlen(event_payload));
    return res;
}

/**
 * @brief 上报事件HeartbeatNotification到云端
 * @param devid 由调用IOT_Linkkit_Open返回的设备标示符
 * @param value 事件HeartbeatNotification的值，数据类型double
 * @return 消息id:(>=1), 上报失败: <0
 */
int app_post_event_HeartbeatNotification(uint32_t devid, double value)
{
    int32_t res = STATE_USER_INPUT_BASE;
    char *event_id = "HeartbeatNotification";
    char event_payload[384] = {0};

    res = HAL_Snprintf(event_payload, sizeof(event_payload), "{\"VoltageValue\": %f}", value);
    if (res < 0) {
        return STATE_SYS_DEPEND_SNPRINTF;
    }

    res = IOT_Linkkit_TriggerEvent(EXAMPLE_MASTER_DEVID, event_id, strlen(event_id),
                                   event_payload, strlen(event_payload));
    return res;
}

/**
 * @brief 上报事件Error到云端
 * @param devid 由调用IOT_Linkkit_Open返回的设备标示符
 * @param value 事件Error的值，数据类型uint32_t
 * @return 消息id:(>=1), 上报失败: <0
 */
int app_post_event_Error(uint32_t devid, uint32_t value)
{
    int32_t res = STATE_USER_INPUT_BASE;
    char *event_id = "Error";
    char event_payload[64] = {0};

    res = HAL_Snprintf(event_payload, sizeof(event_payload), "{\"ErrorCode\": %d}", value);
    if (res < 0) {
        return STATE_SYS_DEPEND_SNPRINTF;
    }

    res = IOT_Linkkit_TriggerEvent(EXAMPLE_MASTER_DEVID, event_id, strlen(event_id),
                                   event_payload, strlen(event_payload));
    return res;
}


/**
 * @brief 解析所有属性设置的值
 * @param request 指向属性设置请求payload的指针
 * @param request_len 属性设置请求的payload长度
 * @return 解析成功: 0, 解析失败: <0
 */
int32_t app_parse_property(const char *request, uint32_t request_len)
{

    cJSON *req = cJSON_Parse(request);
    if (req == NULL || !cJSON_IsObject(req)) {
        return STATE_DEV_MODEL_WRONG_JSON_FORMAT;
    }

    cJSON_Delete(req);
    return 0;
}

/** 建连成功事件回调 */
static int user_connected_event_handler(void)
{
    EXAMPLE_TRACE("Cloud Connected");
    g_user_example_ctx.cloud_connected = 1;

    return 0;
}

/** 断开连接事件回调 */
static int user_disconnected_event_handler(void)
{
    EXAMPLE_TRACE("Cloud Disconnected");
    g_user_example_ctx.cloud_connected = 0;

    return 0;
}

/* 设备初始化成功事件回调 */
static int user_initialized(const int devid)
{
    EXAMPLE_TRACE("Device Initialized");
    g_user_example_ctx.master_initialized = 1;

    return 0;
}

/** 事件回调：接收到云端回复属性上报应答 **/
static int user_report_reply_event_handler(const int devid, const int msgid, const int code, const char *reply,
        const int reply_len)
{
    EXAMPLE_TRACE("Message Post Reply Received, Message ID: %d, Code: %d, Reply: %.*s", msgid, code,
                  reply_len,
                  (reply == NULL) ? ("NULL") : (reply));
    return 0;
}

/** 事件回调：接收到云端回复的事件上报应答 **/
static int user_trigger_event_reply_event_handler(const int devid, const int msgid, const int code, const char *eventid,
        const int eventid_len, const char *message, const int message_len)
{
    EXAMPLE_TRACE("Trigger Event Reply Received, Message ID: %d, Code: %d, EventID: %.*s, Message: %.*s",
                  msgid, code,
                  eventid_len,
                  eventid, message_len, message);

    return 0;
}

/** 事件回调：接收到云端下发的属性设置 **/
static int user_property_set_event_handler(const int devid, const char *request, const int request_len)
{
    int res = 0;
    EXAMPLE_TRACE("Property Set Received, Request: %s", request);

    app_parse_property(request, request_len);

    res = IOT_Linkkit_Report(EXAMPLE_MASTER_DEVID, ITM_MSG_POST_PROPERTY,
                             (unsigned char *)request, request_len);
    EXAMPLE_TRACE("Post Property return: %d", res);

    return 0;
}

/** 事件回调：接收到云端下发的服务请求 **/
static int user_service_request_event_handler(const int devid, const char *serviceid, const int serviceid_len,
        const char *request, const int request_len,
        char **response, int *response_len)
{
    int add_result = 0;
    cJSON *root = NULL, *item_number_a = NULL, *item_number_b = NULL;
    const char *response_fmt = "{\"Result\": %d}";

    EXAMPLE_TRACE("Service Request Received, Service ID: %.*s, Payload: %s", serviceid_len, serviceid, request);

    /* Parse Root */
    root = cJSON_Parse(request);
    if (root == NULL || !cJSON_IsObject(root)) {
        EXAMPLE_TRACE("JSON Parse Error");
        return -1;
    }

    if (strlen("Operation_Service") == serviceid_len && memcmp("Operation_Service", serviceid, serviceid_len) == 0) {
        /* Parse NumberA */
        item_number_a = cJSON_GetObjectItem(root, "NumberA");
        if (item_number_a == NULL || !cJSON_IsNumber(item_number_a)) {
            cJSON_Delete(root);
            return -1;
        }
        EXAMPLE_TRACE("NumberA = %d", item_number_a->valueint);

        /* Parse NumberB */
        item_number_b = cJSON_GetObjectItem(root, "NumberB");
        if (item_number_b == NULL || !cJSON_IsNumber(item_number_b)) {
            cJSON_Delete(root);
            return -1;
        }
        EXAMPLE_TRACE("NumberB = %d", item_number_b->valueint);

        add_result = item_number_a->valueint + item_number_b->valueint;

        /* Send Service Response To Cloud */
        *response_len = strlen(response_fmt) + 10 + 1;
        *response = (char *)HAL_Malloc(*response_len);
        if (*response == NULL) {
            EXAMPLE_TRACE("Memory Not Enough");
            return -1;
        }
        memset(*response, 0, *response_len);
        HAL_Snprintf(*response, *response_len, response_fmt, add_result);
        *response_len = strlen(*response);
    }

    cJSON_Delete(root);
    return 0;
}

/** 事件回调：接收到云端回复的时间戳 **/
static int user_timestamp_reply_event_handler(const char *timestamp)
{
    EXAMPLE_TRACE("Current Timestamp: %s", timestamp);

    return 0;
}

/** FOTA事件回调处理 **/
static int user_fota_event_handler(int type, const char *version)
{
    char buffer[128] = {0};
    int buffer_length = 128;

    /* 0 - new firmware exist, query the new firmware */
    if (type == 0) {
        EXAMPLE_TRACE("New Firmware Version: %s", version);

        IOT_Linkkit_Query(EXAMPLE_MASTER_DEVID, ITM_MSG_QUERY_FOTA_DATA, (unsigned char *)buffer, buffer_length);
    }

    return 0;
}

/** 事件回调：接收到云端错误信息 **/
static int user_cloud_error_handler(const int code, const char *data, const char *detail)
{
    EXAMPLE_TRACE("code =%d ,data=%s, detail=%s", code, data, detail);
    return 0;
}

/** 事件回调：通过动态注册获取到DeviceSecret **/
static int dynreg_device_secret(const char *device_secret)
{
    EXAMPLE_TRACE("device secret: %s", device_secret);
    return 0;
}

/** 事件回调: SDK内部运行状态打印 **/
static int user_sdk_state_dump(int ev, const char *msg)
{
    printf("received state event, -0x%04x(%s)\n", -ev, msg);
    return 0;
}

/**
 * @brief main函数
 */
int main(int argc, char **argv)
{
    int res = 0;
    int cnt = 0;
    iotx_linkkit_dev_meta_info_t master_meta_info;
    int dynamic_register = 0, post_reply_need = 0;
    memset(&g_user_example_ctx, 0, sizeof(user_example_ctx_t));

#ifdef ATM_ENABLED
    if (IOT_ATM_Init() < 0) {
        EXAMPLE_TRACE("IOT_ATM_Init failed!\n");
        return -1;
    }
#endif

    memset(&master_meta_info, 0, sizeof(iotx_linkkit_dev_meta_info_t));
    memcpy(master_meta_info.product_key, g_product_key, strlen(g_product_key));
    memcpy(master_meta_info.product_secret, g_product_secret, strlen(g_product_secret));
    memcpy(master_meta_info.device_name, g_device_name, strlen(g_device_name));
    memcpy(master_meta_info.device_secret, g_device_secret, strlen(g_device_secret));

    IOT_SetLogLevel(IOT_LOG_DEBUG);

    /* 注册回调函数 */
    IOT_RegisterCallback(ITE_STATE_EVERYTHING, user_sdk_state_dump);
    IOT_RegisterCallback(ITE_CONNECT_SUCC, user_connected_event_handler);
    IOT_RegisterCallback(ITE_DISCONNECTED, user_disconnected_event_handler);
    IOT_RegisterCallback(ITE_SERVICE_REQUEST, user_service_request_event_handler);
    IOT_RegisterCallback(ITE_PROPERTY_SET, user_property_set_event_handler);
    IOT_RegisterCallback(ITE_REPORT_REPLY, user_report_reply_event_handler);
    IOT_RegisterCallback(ITE_TRIGGER_EVENT_REPLY, user_trigger_event_reply_event_handler);
    IOT_RegisterCallback(ITE_TIMESTAMP_REPLY, user_timestamp_reply_event_handler);
    IOT_RegisterCallback(ITE_INITIALIZE_COMPLETED, user_initialized);
    IOT_RegisterCallback(ITE_FOTA, user_fota_event_handler);
    IOT_RegisterCallback(ITE_CLOUD_ERROR, user_cloud_error_handler);
    IOT_RegisterCallback(ITE_DYNREG_DEVICE_SECRET, dynreg_device_secret);

    /* 选择上线方式（是否使用动态注册） */
    dynamic_register = 0;
    IOT_Ioctl(IOTX_IOCTL_SET_DYNAMIC_REGISTER, (void *)&dynamic_register);

    /* 是否需用属性、事件上报(设置)应答 */
    post_reply_need = 1;
    IOT_Ioctl(IOTX_IOCTL_RECV_EVENT_REPLY, (void *)&post_reply_need);

    do {
        g_user_example_ctx.master_devid = IOT_Linkkit_Open(IOTX_LINKKIT_DEV_TYPE_MASTER, &master_meta_info);
        if (g_user_example_ctx.master_devid >= 0) {
            break;
        }
        EXAMPLE_TRACE("IOT_Linkkit_Open failed! retry after %d ms\n", 2000);
        HAL_SleepMs(2000);
    } while (1);

    do {
        res = IOT_Linkkit_Connect(g_user_example_ctx.master_devid);
        if (res >= 0) {
            break;
        }
        EXAMPLE_TRACE("IOT_Linkkit_Connect failed! retry after %d ms\n", 5000);
        HAL_SleepMs(5000);
    } while (1);

    while (1) {
        IOT_Linkkit_Yield(EXAMPLE_YIELD_TIMEOUT_MS);

        /* Post Proprety Example */
        if ((cnt % 20) == 0) {
            app_post_property_ParkingState(EXAMPLE_MASTER_DEVID, 0);
            app_post_property_RSSI(EXAMPLE_MASTER_DEVID, -127);
            app_post_event_ParkingChangeNotification(EXAMPLE_MASTER_DEVID, 0);
            app_post_event_HeartbeatNotification(EXAMPLE_MASTER_DEVID, 0);
            app_post_event_HeartbeatNotification(EXAMPLE_MASTER_DEVID, 2.5);
            app_post_event_Error(EXAMPLE_MASTER_DEVID, 0);

        }

        cnt++;

        HAL_SleepMs(200);
    }

    IOT_Linkkit_Close(g_user_example_ctx.master_devid);
    IOT_SetLogLevel(IOT_LOG_NONE);

    return 0;
}
