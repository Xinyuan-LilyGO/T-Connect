/*
 * @Description:
            Product factory original testing
        Connect the board using the Uart0 serial port,
    Open the computer window to view detailed output information,
    Press the BOOT button to switch output mode,When the transmission fails,
    the board lights up red, and pressing the BOOT button can reset the status
 * @Author: LILYGO_L
 * @Date: 2024-07-10 10:18:44
 * @LastEditTime: 2025-06-21 09:48:44
 * @License: GPL 3.0
 */

#include "Arduino.h"
#include "WiFi.h"
#include <HTTPClient.h>
#include "pin_config.h"
#include "FastLED.h"
#include <HardwareSerial.h>
#include "driver/twai.h"

#define WIFI_SSID "xinyuandianzi"
#define WIFI_PASSWORD "AA15994823428"
// #define WIFI_SSID "LilyGo-AABB"
// #define WIFI_PASSWORD "xinyuandianzi"

#define WIFI_CONNECT_WAIT_MAX 5000

#define NUM_LEDS 4
#define DATA_PIN APA102_DATA
#define CLOCK_PIN APA102_CLOCK

#define TRANSMIT_RATE_MS 1000
#define POLLING_RATE_MS 1000

enum Device_Running_State
{
    DEVICE_RUNNING_NULL,
    RS485_RUNNING,
    CAN_RUNNING,
};

enum Device_Communication_Status
{
    DEVICE_STATUS_NULL,
    RS485_DEVICE_1,
    RS485_DEVICE_2,
    RS485_DEVICE_3,
    RS485_DEVICE_4,
    CAN_DEVICE_1,
    CAN_DEVICE_2,
    CAN_DEVICE_3,
    CAN_DEVICE_4,
};

enum Device_Communication_Direction
{
    DEVICE_TX,
    DEVICE_RX,
    DEVICE_FAIL,
};

const char *fileDownloadUrl = "http://music.163.com/song/media/outer/url?id=26122999.mp3";

static bool Wifi_Connection_Flag = false;
static bool IO_Level = 0;

static size_t CycleTime = 0;
static size_t CycleTime_2 = 0;
static size_t CycleTime_3 = 0;

static char DATA1[100] = {0};

bool Device_Change_Flag = 0;
uint8_t Device_Running = Device_Running_State::DEVICE_RUNNING_NULL;
uint8_t Device_Communication_Status_Light = Device_Communication_Status::DEVICE_STATUS_NULL;
uint8_t Device_Device_Number = 0;

size_t RS485_Count = 0;

static uint32_t APA102_RGB_Data[] = {CRGB::Red, CRGB::Green, CRGB::Blue, CRGB::Black};

HardwareSerial SerialPort(2); // use UART2

CRGB leds[NUM_LEDS];

void Set_Device_Communication_Status_Light(uint8_t status, uint8_t set, uint32_t delay_ms)
{
    switch (status)
    {
    case Device_Communication_Direction::DEVICE_TX:
        switch (set)
        {
        case Device_Communication_Status::RS485_DEVICE_1:
            leds[1] = CRGB::Blue;
            leds[2] = CRGB::Blue;
            leds[3] = CRGB::Blue;
            FastLED.show();
            delay(delay_ms);
            leds[1] = CRGB::Black;
            leds[2] = CRGB::Black;
            leds[3] = CRGB::Black;
            FastLED.show();
            break;
        case Device_Communication_Status::RS485_DEVICE_2:
            leds[0] = CRGB::Blue;
            leds[2] = CRGB::Blue;
            leds[3] = CRGB::Blue;
            FastLED.show();
            delay(delay_ms);
            leds[0] = CRGB::Black;
            leds[2] = CRGB::Black;
            leds[3] = CRGB::Black;
            FastLED.show();
            break;
        case Device_Communication_Status::RS485_DEVICE_3:
            leds[0] = CRGB::Blue;
            leds[1] = CRGB::Blue;
            leds[3] = CRGB::Blue;
            FastLED.show();
            delay(delay_ms);
            leds[0] = CRGB::Black;
            leds[1] = CRGB::Black;
            leds[3] = CRGB::Black;
            FastLED.show();
            break;
        case Device_Communication_Status::RS485_DEVICE_4:
            leds[0] = CRGB::Blue;
            leds[1] = CRGB::Blue;
            leds[2] = CRGB::Blue;
            FastLED.show();
            delay(delay_ms);
            leds[0] = CRGB::Black;
            leds[1] = CRGB::Black;
            leds[2] = CRGB::Black;
            FastLED.show();
            break;
        case Device_Communication_Status::CAN_DEVICE_1:
            leds[1] = CRGB::Blue;
            leds[2] = CRGB::Blue;
            leds[3] = CRGB::Blue;
            FastLED.show();
            delay(delay_ms);
            leds[1] = CRGB::Black;
            leds[2] = CRGB::Black;
            leds[3] = CRGB::Black;
            FastLED.show();
            break;
        case Device_Communication_Status::CAN_DEVICE_2:
            leds[0] = CRGB::Blue;
            leds[2] = CRGB::Blue;
            leds[3] = CRGB::Blue;
            FastLED.show();
            delay(delay_ms);
            leds[0] = CRGB::Black;
            leds[2] = CRGB::Black;
            leds[3] = CRGB::Black;
            FastLED.show();
            break;
        case Device_Communication_Status::CAN_DEVICE_3:
            leds[0] = CRGB::Blue;
            leds[1] = CRGB::Blue;
            leds[3] = CRGB::Blue;
            FastLED.show();
            delay(delay_ms);
            leds[0] = CRGB::Black;
            leds[1] = CRGB::Black;
            leds[3] = CRGB::Black;
            FastLED.show();
            break;
        case Device_Communication_Status::CAN_DEVICE_4:
            leds[0] = CRGB::Blue;
            leds[1] = CRGB::Blue;
            leds[2] = CRGB::Blue;
            FastLED.show();
            delay(delay_ms);
            leds[0] = CRGB::Black;
            leds[1] = CRGB::Black;
            leds[2] = CRGB::Black;
            FastLED.show();
            break;

        default:
            break;
        }
        break;
    case Device_Communication_Direction::DEVICE_RX:
        switch (set)
        {
        case Device_Communication_Status::RS485_DEVICE_1:
            leds[1] = CRGB::Yellow;
            leds[2] = CRGB::Yellow;
            leds[3] = CRGB::Yellow;
            FastLED.show();
            delay(delay_ms);
            leds[1] = CRGB::Black;
            leds[2] = CRGB::Black;
            leds[3] = CRGB::Black;
            FastLED.show();
            break;
        case Device_Communication_Status::RS485_DEVICE_2:
            leds[0] = CRGB::Yellow;
            leds[2] = CRGB::Yellow;
            leds[3] = CRGB::Yellow;
            FastLED.show();
            delay(delay_ms);
            leds[0] = CRGB::Black;
            leds[2] = CRGB::Black;
            leds[3] = CRGB::Black;
            FastLED.show();
            break;
        case Device_Communication_Status::RS485_DEVICE_3:
            leds[0] = CRGB::Yellow;
            leds[1] = CRGB::Yellow;
            leds[3] = CRGB::Yellow;
            FastLED.show();
            delay(delay_ms);
            leds[0] = CRGB::Black;
            leds[1] = CRGB::Black;
            leds[3] = CRGB::Black;
            FastLED.show();
            break;
        case Device_Communication_Status::RS485_DEVICE_4:
            leds[0] = CRGB::Yellow;
            leds[1] = CRGB::Yellow;
            leds[2] = CRGB::Yellow;
            FastLED.show();
            delay(delay_ms);
            leds[0] = CRGB::Black;
            leds[1] = CRGB::Black;
            leds[2] = CRGB::Black;
            FastLED.show();
            break;
        case Device_Communication_Status::CAN_DEVICE_1:
            leds[1] = CRGB::Yellow;
            leds[2] = CRGB::Yellow;
            leds[3] = CRGB::Yellow;
            FastLED.show();
            delay(delay_ms);
            leds[1] = CRGB::Black;
            leds[2] = CRGB::Black;
            leds[3] = CRGB::Black;
            FastLED.show();
            break;
        case Device_Communication_Status::CAN_DEVICE_2:
            leds[0] = CRGB::Yellow;
            leds[2] = CRGB::Yellow;
            leds[3] = CRGB::Yellow;
            FastLED.show();
            delay(delay_ms);
            leds[0] = CRGB::Black;
            leds[2] = CRGB::Black;
            leds[3] = CRGB::Black;
            FastLED.show();
            break;
        case Device_Communication_Status::CAN_DEVICE_3:
            leds[0] = CRGB::Yellow;
            leds[1] = CRGB::Yellow;
            leds[3] = CRGB::Yellow;
            FastLED.show();
            delay(delay_ms);
            leds[0] = CRGB::Black;
            leds[1] = CRGB::Black;
            leds[3] = CRGB::Black;
            FastLED.show();
            break;
        case Device_Communication_Status::CAN_DEVICE_4:
            leds[0] = CRGB::Yellow;
            leds[1] = CRGB::Yellow;
            leds[2] = CRGB::Yellow;
            FastLED.show();
            delay(delay_ms);
            leds[0] = CRGB::Black;
            leds[1] = CRGB::Black;
            leds[2] = CRGB::Black;
            FastLED.show();
            break;

        default:
            break;
        }
        break;
    case Device_Communication_Direction::DEVICE_FAIL:
        switch (set)
        {
        case Device_Communication_Status::RS485_DEVICE_1:
            leds[1] = CRGB::Red;
            leds[2] = CRGB::Red;
            leds[3] = CRGB::Red;
            FastLED.show();
            break;
        case Device_Communication_Status::RS485_DEVICE_2:
            leds[0] = CRGB::Red;
            leds[2] = CRGB::Red;
            leds[3] = CRGB::Red;
            FastLED.show();
            break;
        case Device_Communication_Status::RS485_DEVICE_3:
            leds[0] = CRGB::Red;
            leds[1] = CRGB::Red;
            leds[3] = CRGB::Red;
            FastLED.show();
            break;
        case Device_Communication_Status::RS485_DEVICE_4:
            leds[0] = CRGB::Red;
            leds[1] = CRGB::Red;
            leds[2] = CRGB::Red;
            FastLED.show();
            break;
        case Device_Communication_Status::CAN_DEVICE_1:
            leds[1] = CRGB::Red;
            leds[2] = CRGB::Red;
            leds[3] = CRGB::Red;
            FastLED.show();
            break;
        case Device_Communication_Status::CAN_DEVICE_2:
            leds[0] = CRGB::Red;
            leds[2] = CRGB::Red;
            leds[3] = CRGB::Red;
            FastLED.show();
            break;
        case Device_Communication_Status::CAN_DEVICE_3:
            leds[0] = CRGB::Red;
            leds[1] = CRGB::Red;
            leds[3] = CRGB::Red;
            FastLED.show();
            break;
        case Device_Communication_Status::CAN_DEVICE_4:
            leds[0] = CRGB::Red;
            leds[1] = CRGB::Red;
            leds[2] = CRGB::Red;
            FastLED.show();
            break;

        default:
            break;
        }
        break;

    default:
        break;
    }
}

void Wifi_STA_Test(void)
{
    String text;
    int wifi_num = 0;

    Serial.printf("\nScanning wifi");
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    delay(100);

    wifi_num = WiFi.scanNetworks();
    if (wifi_num == 0)
    {
        text = "\nWiFi scan complete !\nNo wifi discovered.\n";
    }
    else
    {
        text = "\nWiFi scan complete !\n";
        text += wifi_num;
        text += " wifi discovered.\n\n";

        for (int i = 0; i < wifi_num; i++)
        {
            text += (i + 1);
            text += ": ";
            text += WiFi.SSID(i);
            text += " (";
            text += WiFi.RSSI(i);
            text += ")";
            text += (WiFi.encryptionType(i) == WIFI_AUTH_OPEN) ? " \n" : "*\n";
            delay(10);
        }
    }

    Serial.println(text);

    delay(3000);
    text.clear();

    text = "Connecting to ";
    Serial.print("Connecting to ");
    text += WIFI_SSID;
    text += "\n";

    Serial.print(WIFI_SSID);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    uint32_t last_tick = millis();

    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.print(".");
        text += ".";
        delay(100);

        if (millis() - last_tick > WIFI_CONNECT_WAIT_MAX)
        {
            Wifi_Connection_Flag = false;
            break;
        }
        else
        {
            Wifi_Connection_Flag = true;
        }
    }

    if (Wifi_Connection_Flag == true)
    {
        text += "\nThe connection was successful ! \nTakes ";
        Serial.print("\nThe connection was successful ! \nTakes ");

        text += millis() - last_tick;
        Serial.print(millis() - last_tick);

        text += " ms\n";
        Serial.println(" ms\n");
    }
    else
    {
        Serial.printf("\nWifi test error!\n");
    }
}

void WIFI_STA_Test_Loop(void)
{
    if (Wifi_Connection_Flag == true)
    {
        // 初始化HTTP客户端
        HTTPClient http;
        http.begin(fileDownloadUrl);
        // 获取重定向的URL
        const char *headerKeys[] = {"Location"};
        http.collectHeaders(headerKeys, 1);

        // 记录下载开始时间
        size_t startTime = millis();
        // 无用时间
        size_t uselessTime = 0;

        // 发起GET请求
        int httpCode = http.GET();

        while (httpCode == HTTP_CODE_MOVED_PERMANENTLY || httpCode == HTTP_CODE_FOUND)
        {
            String newUrl = http.header("Location");
            Serial.printf("Redirecting to: %s\n", newUrl.c_str());
            http.end(); // 关闭旧的HTTP连接

            // 使用新的URL重新发起GET请求
            http.begin(newUrl);
            httpCode = http.GET();
        }

        if (httpCode == HTTP_CODE_OK)
        {
            // 获取文件大小
            size_t fileSize = http.getSize();
            Serial.printf("Starting file download...\n");
            Serial.printf("file size: %f MB\n", fileSize / 1024.0 / 1024.0);

            // 读取HTTP响应
            WiFiClient *stream = http.getStreamPtr();

            size_t temp_count_s = 0;
            size_t temp_fileSize = fileSize;
            uint8_t *buf_1 = (uint8_t *)heap_caps_malloc(100 * 1024, MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
            // uint8_t buf_1[4096] = {0};
            CycleTime = millis() + 1000; // 开始计时
            bool temp_count_flag = true;
            while (http.connected() && (temp_fileSize > 0 || temp_fileSize == -1))
            {
                // 获取可用数据的大小
                size_t availableSize = stream->available();
                if (availableSize)
                {
                    temp_fileSize -= stream->read(buf_1, min(availableSize, (size_t)(100 * 1024)));

                    if (millis() > CycleTime)
                    {
                        size_t temp_time_1 = millis();
                        temp_count_s++;
                        Serial.printf("Download speed: %f KB/s\n", ((fileSize - temp_fileSize) / 1024.0) / temp_count_s);
                        Serial.printf("Remaining file size: %f MB\n\n", temp_fileSize / 1024.0 / 1024.0);

                        CycleTime = millis() + 1000;
                        size_t temp_time_2 = millis();

                        uselessTime = uselessTime + (temp_time_2 - temp_time_1);
                    }
                }
                // delay(1);

                if (temp_count_s > 30)
                {
                    temp_count_flag = false;
                    break;
                }
            }

            // 关闭HTTP客户端
            http.end();

            // 记录下载结束时间并计算总花费时间
            size_t endTime = millis();

            if (temp_count_flag == true)
            {
                Serial.printf("Download completed!\n");
                Serial.printf("Total download time: %f s\n", (endTime - startTime - uselessTime) / 1000.0);
                Serial.printf("Average download speed: %f KB/s\n", (fileSize / 1024.0) / ((endTime - startTime - uselessTime) / 1000.0));
            }
            else
            {
                Serial.printf("Download incomplete!\n");
                Serial.printf("Download time: %f s\n", (endTime - startTime - uselessTime) / 1000.0);
                Serial.printf("Average download speed: %f KB/s\n", ((fileSize - temp_fileSize) / 1024.0) / ((endTime - startTime - uselessTime) / 1000.0));
            }
        }
        else
        {
            Serial.printf("Failed to download\n");
            Serial.printf("Error httpCode: %d \n", httpCode);
        }
    }
    else
    {
        Serial.print("Not connected to the network");
    }
    delay(1000);
}

bool CAN_Device_Initialization(uint8_t tx, uint8_t rx)
{
    // Initialize configuration structures using macro initializers
    twai_general_config_t g_config = TWAI_GENERAL_CONFIG_DEFAULT((gpio_num_t)tx, (gpio_num_t)rx, TWAI_MODE_NORMAL);
    twai_timing_config_t t_config = TWAI_TIMING_CONFIG_1MBITS();
    twai_filter_config_t f_config = TWAI_FILTER_CONFIG_ACCEPT_ALL();

    // Install TWAI driver
    if (twai_driver_install(&g_config, &t_config, &f_config) == ESP_OK)
    {
        Serial.println("Driver installed");
    }
    else
    {
        Serial.println("Failed to install driver");
        return false;
    }

    // Start TWAI driver
    if (twai_start() == ESP_OK)
    {
        Serial.println("Driver started");
    }
    else
    {
        Serial.println("Failed to start driver");
        return false;
    }

    // 配置
    uint32_t alerts_to_enable = TWAI_ALERT_TX_IDLE | TWAI_ALERT_TX_SUCCESS |
                                TWAI_ALERT_TX_FAILED | TWAI_ALERT_ERR_PASS |
                                TWAI_ALERT_BUS_ERROR | TWAI_ALERT_RX_DATA |
                                TWAI_ALERT_RX_QUEUE_FULL;

    if (twai_reconfigure_alerts(alerts_to_enable, NULL) == ESP_OK)
    {
        Serial.println("CAN Alerts reconfigured");
    }
    else
    {
        Serial.println("Failed to reconfigure alerts");
        return false;
    }
    return true;
}

void Twai_Send_Message()
{
    // Configure message to transmit
    twai_message_t message;
    message.identifier = 0xF1;
    // message.data_length_code = 1;
    // message.data[0] = CAN_Count++;
    message.data_length_code = 8;
    message.data[0] = 1;
    message.data[1] = 2;
    message.data[2] = 3;
    message.data[3] = 4;
    message.data[4] = 5;
    message.data[5] = 6;
    message.data[6] = 7;
    message.data[7] = 8;

    // Queue message for transmission
    if (twai_transmit(&message, pdMS_TO_TICKS(1000)) == ESP_OK)
    {
        printf("Message queued for transmission\n");
    }
    else
    {
        printf("Failed to queue message for transmission\n");
    }
}

void Twai_Receive_Message(twai_message_t &message)
{
    // Process received message
    if (message.extd)
    {
        Serial.println("Message is in Extended Format");
    }
    else
    {
        Serial.println("Message is in Standard Format");
    }
    Serial.printf("ID: 0x%X\n", message.identifier);
    if (!(message.rtr))
    {
        for (int i = 0; i < message.data_length_code; i++)
        {
            Serial.printf("Data [%d] = %d\n", i, message.data[i]);
        }
        Serial.println("");
    }
}

void Task1(void *pvParameters)
{
    // 在这里可以添加一些代码，这样的话这个任务执行时会先执行一次这里的内容
    // 当然后面进入while循环之后不会再执行这部分了
    while (1)
    {
        if (digitalRead(KEY_BOOT) == 0)
        {
            Device_Change_Flag = true;
        }
    }
}

void setup()
{
    Serial.begin(115200);
    Serial.println("Ciallo");

    // pinMode(GPIO_NUM_2, OUTPUT);
    // pinMode(GPIO_NUM_41, OUTPUT);
    // pinMode(GPIO_NUM_39, OUTPUT);
    // pinMode(GPIO_NUM_37, OUTPUT);
    // pinMode(GPIO_NUM_35, OUTPUT);
    // pinMode(GPIO_NUM_48, OUTPUT);
    // pinMode(GPIO_NUM_21, OUTPUT);
    // pinMode(GPIO_NUM_13, OUTPUT);
    // pinMode(GPIO_NUM_11, OUTPUT);
    // pinMode(GPIO_NUM_1, OUTPUT);
    // pinMode(GPIO_NUM_42, OUTPUT);
    // pinMode(GPIO_NUM_40, OUTPUT);
    // pinMode(GPIO_NUM_38, OUTPUT);
    // pinMode(GPIO_NUM_36, OUTPUT);
    // pinMode(GPIO_NUM_45, OUTPUT);
    // pinMode(GPIO_NUM_47, OUTPUT);
    // pinMode(GPIO_NUM_14, OUTPUT);
    // pinMode(GPIO_NUM_12, OUTPUT);
    // pinMode(GPIO_NUM_46, OUTPUT);

    // digitalWrite(GPIO_NUM_2, IO_Level);
    // digitalWrite(GPIO_NUM_41, IO_Level);
    // digitalWrite(GPIO_NUM_39, IO_Level);
    // digitalWrite(GPIO_NUM_37, IO_Level);
    // digitalWrite(GPIO_NUM_35, IO_Level);
    // digitalWrite(GPIO_NUM_48, IO_Level);
    // digitalWrite(GPIO_NUM_21, IO_Level);
    // digitalWrite(GPIO_NUM_13, IO_Level);
    // digitalWrite(GPIO_NUM_11, IO_Level);
    // digitalWrite(GPIO_NUM_1, IO_Level);
    // digitalWrite(GPIO_NUM_42, IO_Level);
    // digitalWrite(GPIO_NUM_40, IO_Level);
    // digitalWrite(GPIO_NUM_38, IO_Level);
    // digitalWrite(GPIO_NUM_36, IO_Level);
    // digitalWrite(GPIO_NUM_45, IO_Level);
    // digitalWrite(GPIO_NUM_47, IO_Level);
    // digitalWrite(GPIO_NUM_14, IO_Level);
    // digitalWrite(GPIO_NUM_12, IO_Level);
    // digitalWrite(GPIO_NUM_46, IO_Level);

    FastLED.addLeds<APA102, DATA_PIN, CLOCK_PIN, BGR>(leds, NUM_LEDS);
    FastLED.setBrightness(20);

    leds[0] = CRGB::Red;
    leds[1] = CRGB::Red;
    leds[2] = CRGB::Red;
    leds[3] = CRGB::Red;
    FastLED.show();
    delay(1000);

    leds[0] = CRGB::Green;
    leds[1] = CRGB::Green;
    leds[2] = CRGB::Green;
    leds[3] = CRGB::Green;
    FastLED.show();
    delay(1000);

    leds[0] = CRGB::Blue;
    leds[1] = CRGB::Blue;
    leds[2] = CRGB::Blue;
    leds[3] = CRGB::Blue;
    FastLED.show();
    delay(1000);

    leds[0] = CRGB::White;
    leds[1] = CRGB::White;
    leds[2] = CRGB::White;
    leds[3] = CRGB::White;
    FastLED.show();
    delay(1000);

    Wifi_STA_Test();
    WIFI_STA_Test_Loop();

    if (Wifi_Connection_Flag == true)
    {
        for (int i = 0; i < 3; i++)
        {
            leds[0] = CRGB::Green;
            leds[1] = CRGB::Green;
            leds[2] = CRGB::Green;
            leds[3] = CRGB::Green;
            FastLED.show();
            delay(500);
            leds[0] = CRGB::Black;
            leds[1] = CRGB::Black;
            leds[2] = CRGB::Black;
            leds[3] = CRGB::Black;
            FastLED.show();
            delay(500);
        }
    }
    else
    {
        for (int i = 0; i < 3; i++)
        {
            leds[0] = CRGB::Red;
            leds[1] = CRGB::Red;
            leds[2] = CRGB::Red;
            leds[3] = CRGB::Red;
            FastLED.show();
            delay(500);
            leds[0] = CRGB::Black;
            leds[1] = CRGB::Black;
            leds[2] = CRGB::Black;
            leds[3] = CRGB::Black;
            FastLED.show();
            delay(500);
        }
    }

    // for (uint8_t i = 0; i < 3; i++)
    // {
    //     leds[0] = APA102_RGB_Data[i];
    //     FastLED.show();
    //     delay(50);
    //     leds[1] = APA102_RGB_Data[i];
    //     FastLED.show();
    //     delay(50);
    //     leds[2] = APA102_RGB_Data[i];
    //     FastLED.show();
    //     delay(50);
    //     leds[3] = APA102_RGB_Data[i];
    //     FastLED.show();
    //     delay(50);

    //     leds[0] = CRGB::Black;
    //     FastLED.show();
    //     delay(50);
    //     leds[1] = CRGB::Black;
    //     FastLED.show();
    //     delay(50);
    //     leds[2] = CRGB::Black;
    //     FastLED.show();
    //     delay(50);
    //     leds[3] = CRGB::Black;
    //     FastLED.show();
    //     delay(50);
    // }

    xTaskCreatePinnedToCore(Task1, "Task1", 10000, NULL, 1, NULL, 1);

    delay(1000);
}

void loop()
{
    if (Device_Change_Flag == true)
    {
        delay(300);

        SerialPort.end();
        twai_stop();
        twai_driver_uninstall();

        Device_Device_Number++;
        switch (Device_Device_Number)
        {
        case 1:
            SerialPort.begin(115200, SERIAL_8N1, RS485_RX_1, RS485_TX_1);
            Device_Running = Device_Running_State::RS485_RUNNING;
            Device_Communication_Status_Light = Device_Communication_Status::RS485_DEVICE_1;
            leds[0] = CRGB::Red;
            leds[1] = CRGB::Black;
            leds[2] = CRGB::Black;
            leds[3] = CRGB::Black;
            FastLED.show();
            break;
        case 2:
            SerialPort.begin(115200, SERIAL_8N1, RS485_RX_2, RS485_TX_2);
            Device_Running = Device_Running_State::RS485_RUNNING;
            Device_Communication_Status_Light = Device_Communication_Status::RS485_DEVICE_2;
            leds[0] = CRGB::Black;
            leds[1] = CRGB::Red;
            leds[2] = CRGB::Black;
            leds[3] = CRGB::Black;
            FastLED.show();
            break;
        case 3:
            SerialPort.begin(115200, SERIAL_8N1, RS485_RX_3, RS485_TX_3);
            Device_Running = Device_Running_State::RS485_RUNNING;
            Device_Communication_Status_Light = Device_Communication_Status::RS485_DEVICE_3;
            leds[0] = CRGB::Black;
            leds[1] = CRGB::Black;
            leds[2] = CRGB::Red;
            leds[3] = CRGB::Black;
            FastLED.show();
            break;
        case 4:
            SerialPort.begin(115200, SERIAL_8N1, RS485_RX_4, RS485_TX_4);
            Device_Running = Device_Running_State::RS485_RUNNING;
            Device_Communication_Status_Light = Device_Communication_Status::RS485_DEVICE_4;
            leds[0] = CRGB::Black;
            leds[1] = CRGB::Black;
            leds[2] = CRGB::Black;
            leds[3] = CRGB::Red;
            FastLED.show();
            break;
        case 5:
            if (CAN_Device_Initialization(CAN_TX_1, CAN_RX_1) == true)
            {
                Device_Running = Device_Running_State::CAN_RUNNING;
                Device_Communication_Status_Light = Device_Communication_Status::CAN_DEVICE_1;
            }
            else
            {
                Device_Running = Device_Running_State::DEVICE_RUNNING_NULL;
                Device_Communication_Status_Light = Device_Communication_Status::DEVICE_STATUS_NULL;
            }

            leds[0] = CRGB::Green;
            leds[1] = CRGB::Black;
            leds[2] = CRGB::Black;
            leds[3] = CRGB::Black;
            FastLED.show();
            break;
        case 6:
            if (CAN_Device_Initialization(CAN_TX_2, CAN_RX_2) == true)
            {
                Device_Running = Device_Running_State::CAN_RUNNING;
                Device_Communication_Status_Light = Device_Communication_Status::CAN_DEVICE_2;
            }
            else
            {
                Device_Running = Device_Running_State::DEVICE_RUNNING_NULL;
                Device_Communication_Status_Light = Device_Communication_Status::DEVICE_STATUS_NULL;
            }

            leds[0] = CRGB::Black;
            leds[1] = CRGB::Green;
            leds[2] = CRGB::Black;
            leds[3] = CRGB::Black;
            FastLED.show();
            break;
        case 7:
            if (CAN_Device_Initialization(CAN_TX_3, CAN_RX_3) == true)
            {
                Device_Running = Device_Running_State::CAN_RUNNING;
                Device_Communication_Status_Light = Device_Communication_Status::CAN_DEVICE_3;
            }
            else
            {
                Device_Running = Device_Running_State::DEVICE_RUNNING_NULL;
                Device_Communication_Status_Light = Device_Communication_Status::DEVICE_STATUS_NULL;
            }

            leds[0] = CRGB::Black;
            leds[1] = CRGB::Black;
            leds[2] = CRGB::Green;
            leds[3] = CRGB::Black;
            FastLED.show();
            break;
        case 8:
            if (CAN_Device_Initialization(CAN_TX_4, CAN_RX_4) == true)
            {
                Device_Running = Device_Running_State::CAN_RUNNING;
                Device_Communication_Status_Light = Device_Communication_Status::CAN_DEVICE_4;
            }
            else
            {
                Device_Running = Device_Running_State::DEVICE_RUNNING_NULL;
                Device_Communication_Status_Light = Device_Communication_Status::DEVICE_STATUS_NULL;
            }

            leds[0] = CRGB::Black;
            leds[1] = CRGB::Black;
            leds[2] = CRGB::Black;
            leds[3] = CRGB::Green;
            FastLED.show();
            break;
        case 9:
            Device_Running = Device_Running_State::DEVICE_RUNNING_NULL;
            Device_Communication_Status_Light = Device_Communication_Status::DEVICE_STATUS_NULL;
            leds[0] = CRGB::Black;
            leds[1] = CRGB::Black;
            leds[2] = CRGB::Black;
            leds[3] = CRGB::Black;
            FastLED.show();
            break;

        default:
            break;
        }

        if (Device_Device_Number > 8)
        {
            Device_Device_Number = 0;
        }

        Device_Change_Flag = false;
    }

    if (millis() > CycleTime)
    {
        // digitalWrite(GPIO_NUM_2, IO_Level);
        // digitalWrite(GPIO_NUM_41, IO_Level);
        // digitalWrite(GPIO_NUM_39, IO_Level);
        // digitalWrite(GPIO_NUM_37, IO_Level);
        // digitalWrite(GPIO_NUM_35, IO_Level);
        // digitalWrite(GPIO_NUM_48, IO_Level);
        // digitalWrite(GPIO_NUM_21, IO_Level);
        // digitalWrite(GPIO_NUM_13, IO_Level);
        // digitalWrite(GPIO_NUM_11, IO_Level);
        // digitalWrite(GPIO_NUM_1, IO_Level);
        // digitalWrite(GPIO_NUM_42, IO_Level);
        // digitalWrite(GPIO_NUM_40, IO_Level);
        // digitalWrite(GPIO_NUM_38, IO_Level);
        // digitalWrite(GPIO_NUM_36, IO_Level);
        // digitalWrite(GPIO_NUM_45, IO_Level);
        // digitalWrite(GPIO_NUM_47, IO_Level);
        // digitalWrite(GPIO_NUM_14, IO_Level);
        // digitalWrite(GPIO_NUM_12, IO_Level);
        // digitalWrite(GPIO_NUM_46, IO_Level);
        // IO_Level = !IO_Level;

        CycleTime = millis() + 1000; // 1000ms
    }

    if (Device_Running == Device_Running_State::RS485_RUNNING)
    {
        if (millis() > CycleTime_2)
        {
            RS485_Count++;
            SerialPort.printf("%d", RS485_Count);
            Set_Device_Communication_Status_Light(Device_Communication_Direction::DEVICE_TX,
                                                  Device_Communication_Status_Light, 500);
            CycleTime_2 = millis() + 3000;
        }

        if (SerialPort.available() > 0)
        {
            memset(DATA1, 0, sizeof(DATA1));
            SerialPort.read(DATA1, sizeof(DATA1));

            if (DATA1[0] != '\0')
            {
                Serial.print(DATA1);
                Set_Device_Communication_Status_Light(Device_Communication_Direction::DEVICE_RX,
                                                      Device_Communication_Status_Light, 500);
            }
        }
    }

    if (Device_Running == Device_Running_State::CAN_RUNNING)
    {
        // 通信报警检测
        uint32_t alerts_triggered;
        twai_read_alerts(&alerts_triggered, pdMS_TO_TICKS(POLLING_RATE_MS));
        // 总线状态信息
        twai_status_info_t twai_status_info;
        twai_get_status_info(&twai_status_info);

        switch (alerts_triggered)
        {
        case TWAI_ALERT_ERR_PASS:
            Serial.println("\nAlert: TWAI controller has become error passive.");
            delay(1000);
            break;
        case TWAI_ALERT_BUS_ERROR:
            Serial.println("\nAlert: A (Bit, Stuff, CRC, Form, ACK) error has occurred on the bus.");
            Serial.printf("Bus error count: %d\n", twai_status_info.bus_error_count);
            delay(1000);
            break;
        case TWAI_ALERT_TX_FAILED:
            Serial.println("\nAlert: The Transmission failed.");
            Serial.printf("TX buffered: %d\t", twai_status_info.msgs_to_tx);
            Serial.printf("TX error: %d\t", twai_status_info.tx_error_counter);
            Serial.printf("TX failed: %d\n", twai_status_info.tx_failed_count);
            delay(1000);
            break;
        case TWAI_ALERT_TX_SUCCESS:
            Serial.println("\nAlert: The Transmission was successful.");
            Serial.printf("TX buffered: %d\t", twai_status_info.msgs_to_tx);
            break;
        case TWAI_ALERT_RX_QUEUE_FULL:
            Serial.println("\nAlert: The RX queue is full causing a received frame to be lost.");
            Serial.printf("RX buffered: %d\t", twai_status_info.msgs_to_rx);
            Serial.printf("RX missed: %d\t", twai_status_info.rx_missed_count);
            Serial.printf("RX overrun %d\n", twai_status_info.rx_overrun_count);
            delay(1000);
            break;

        default:
            break;
        }

        switch (twai_status_info.state)
        {
        case TWAI_STATE_RUNNING:
            Serial.println("\nTWAI_STATE_RUNNING");
            break;
        case TWAI_STATE_BUS_OFF:
            Serial.println("\nTWAI_STATE_BUS_OFF");
            twai_initiate_recovery();
            Set_Device_Communication_Status_Light(Device_Communication_Direction::DEVICE_FAIL,
                                                  Device_Communication_Status_Light, 500);
            while (1)
            {
                if (Device_Change_Flag == true)
                {
                    Device_Change_Flag = false;
                    break;
                }
                Serial.println("\nTWAI_STATE_BUS_OFF");
                delay(1000);
            }
            delay(1000);
            break;
        case TWAI_STATE_STOPPED:
            Serial.println("\nTWAI_STATE_STOPPED");
            twai_start();
            Set_Device_Communication_Status_Light(Device_Communication_Direction::DEVICE_FAIL,
                                                  Device_Communication_Status_Light, 500);
            while (1)
            {
                if (Device_Change_Flag == true)
                {
                    Device_Change_Flag = false;
                    break;
                }
                Serial.println("\nTWAI_STATE_BUS_OFF");
                delay(1000);
            }
            delay(1000);
            break;
        case TWAI_STATE_RECOVERING:
            Serial.println("\nTWAI_STATE_RECOVERING");
            delay(1000);
            break;

        default:
            break;
        }

        // 如果TWAI有信息接收到
        if (alerts_triggered & TWAI_ALERT_RX_DATA)
        {
            twai_message_t rx_buf;
            while (twai_receive(&rx_buf, 0) == ESP_OK)
            {
                Twai_Receive_Message(rx_buf);
                Set_Device_Communication_Status_Light(Device_Communication_Direction::DEVICE_RX,
                                                      Device_Communication_Status_Light, 500);
            }
        }

        if (millis() > CycleTime_3)
        {
            Twai_Send_Message();
            Set_Device_Communication_Status_Light(Device_Communication_Direction::DEVICE_TX,
                                                  Device_Communication_Status_Light, 500);
            CycleTime_3 = millis() + 3000;
        }
    }
}
