/*
 * @Description: 
            CAN Test
        Continuously sending and receiving information on the CAN bus module,
    You can view detailed output information in the computer window,
    Press the BOOT button to switch output channels
 * @Author: LILYGO_L
 * @Date: 2024-02-27 09:06:27
 * @LastEditTime: 2024-07-10 10:24:35
 * @License: GPL 3.0
 */

#include <Arduino.h>
#include "driver/twai.h"
#include "pin_config.h"
#include "FastLED.h"

// Intervall:
#define TRANSMIT_RATE_MS 1000
#define POLLING_RATE_MS 1000

#define NUM_LEDS 4
#define DATA_PIN APA102_DATA
#define CLOCK_PIN APA102_CLOCK

size_t CycleTime = 0;
size_t CAN_Count = 0;

bool CAN_Change_Flag = 0;
bool CAN_Device_Running = false;
uint8_t CAN_Device_Number = 0;

CRGB leds[NUM_LEDS];

bool CAN_Device_Initialization(uint8_t tx, uint8_t rx)
{
    // Initialize configuration structures using macro initializers
    twai_general_config_t g_config = TWAI_GENERAL_CONFIG_DEFAULT((gpio_num_t)tx, (gpio_num_t)rx, TWAI_MODE_NORMAL);
    twai_timing_config_t t_config = TWAI_TIMING_CONFIG_500KBITS();
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
    message.identifier = 0x0F1;
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
            CAN_Change_Flag = true;
        }
    }
}

void setup()
{
    Serial.begin(115200);

    FastLED.addLeds<APA102, DATA_PIN, CLOCK_PIN, BGR>(leds, NUM_LEDS);
    FastLED.setBrightness(20);

    leds[0] = CRGB::Black;
    leds[1] = CRGB::Black;
    leds[2] = CRGB::Black;
    leds[3] = CRGB::Black;
    FastLED.show();

    xTaskCreatePinnedToCore(Task1, "Task1", 10000, NULL, 1, NULL, 1);

    // CAN_Device_Initialization();
}

void loop()
{
    if (CAN_Change_Flag == true)
    {
        delay(300);
        CAN_Device_Number++;
        switch (CAN_Device_Number)
        {
        case 1:
            twai_stop();
            twai_driver_uninstall();
            if (CAN_Device_Initialization(CAN_TX_1, CAN_RX_1) == true)
            {
                CAN_Device_Running = true;
            }
            else
            {
                CAN_Device_Running = false;
            }

            leds[0] = CRGB::Green;
            leds[1] = CRGB::Black;
            leds[2] = CRGB::Black;
            leds[3] = CRGB::Black;
            FastLED.show();
            break;
        case 2:
            twai_stop();
            twai_driver_uninstall();
            if (CAN_Device_Initialization(CAN_TX_2, CAN_RX_2) == true)
            {
                CAN_Device_Running = true;
            }
            else
            {
                CAN_Device_Running = false;
            }
            leds[0] = CRGB::Black;
            leds[1] = CRGB::Green;
            leds[2] = CRGB::Black;
            leds[3] = CRGB::Black;
            FastLED.show();

            break;
        case 3:
            twai_stop();
            twai_driver_uninstall();
            if (CAN_Device_Initialization(CAN_TX_3, CAN_RX_3) == true)
            {
                CAN_Device_Running = true;
            }
            else
            {
                CAN_Device_Running = false;
            }
            leds[0] = CRGB::Black;
            leds[1] = CRGB::Black;
            leds[2] = CRGB::Green;
            leds[3] = CRGB::Black;
            FastLED.show();

            break;
        case 4:
            twai_stop();
            twai_driver_uninstall();
            if (CAN_Device_Initialization(CAN_TX_4, CAN_RX_4) == true)
            {
                CAN_Device_Running = true;
            }
            else
            {
                CAN_Device_Running = false;
            }

            leds[0] = CRGB::Black;
            leds[1] = CRGB::Black;
            leds[2] = CRGB::Black;
            leds[3] = CRGB::Green;
            FastLED.show();

            break;

        default:
            break;
        }

        if (CAN_Device_Number > 3)
        {
            CAN_Device_Number = 0;
        }

        CAN_Change_Flag = false;
    }

    if (CAN_Device_Running == true)
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
            delay(1000);
            break;
        case TWAI_STATE_STOPPED:
            Serial.println("\nTWAI_STATE_STOPPED");
            twai_start();
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
            }
        }

        if (millis() > CycleTime)
        {
            Twai_Send_Message();
            CycleTime = millis() + 1000; // 100ms
        }
    }
}
