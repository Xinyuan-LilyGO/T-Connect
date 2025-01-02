/*
 * @Description: 
            RS485 Test
        RS485 module mutual communication transceiver program
    You can view detailed output information in the computer window,
    Press the BOOT button to switch output channels
 * @Author: LILYGO_L
 * @Date: 2024-02-26 17:17:40
 * @LastEditTime: 2024-12-18 10:23:54
 * @License: GPL 3.0
 */

#include <Arduino.h>
#include <HardwareSerial.h>
#include <Wire.h>
#include "pin_config.h"
#include "FastLED.h"

// How many leds in your strip?
#define NUM_LEDS 4
#define DATA_PIN APA102_DATA
#define CLOCK_PIN APA102_CLOCK

static size_t CycleTime = 0;

bool RS485_Change_Flag = 0;
uint8_t RS485_Device_Number = 0;
size_t RS485_Count = 0;

HardwareSerial SerialPort(2); // use UART2
static char DATA1 = 0;

// Define the array of leds
CRGB leds[NUM_LEDS];

void setup()
{
    Serial.begin(115200);
    Serial.println("Ciallo");

    FastLED.addLeds<APA102, DATA_PIN, CLOCK_PIN, BGR>(leds, NUM_LEDS);
    FastLED.setBrightness(20);

    // 初始化串口，并重新定义引脚
    // 参数包括串行通信的波特率、串行模式、使用的 RX 引脚和 TX 引脚。
    // SerialPort.begin(115200, SERIAL_8N1, RS485A_RX, RS485A_TX);

    leds[0] = CRGB::Black;
    leds[1] = CRGB::Black;
    leds[2] = CRGB::Black;
    leds[3] = CRGB::Black;
    FastLED.show();
}

void loop()
{
    if (millis() > CycleTime)
    {
        RS485_Count++;
        SerialPort.printf("%d", RS485_Count);
        CycleTime = millis() + 1000;
    }

    if (digitalRead(KEY_BOOT) == 0)
    {
        RS485_Change_Flag = true;
    }

    if (RS485_Change_Flag == true)
    {
        delay(300);
        RS485_Device_Number++;
        switch (RS485_Device_Number)
        {
        case 1:
            SerialPort.end();
            SerialPort.begin(115200, SERIAL_8N1, RS485_RX_1, RS485_TX_1);
            leds[0] = CRGB::Red;
            leds[1] = CRGB::Black;
            leds[2] = CRGB::Black;
            leds[3] = CRGB::Black;
            FastLED.show();
            break;
        case 2:
            SerialPort.end();
            SerialPort.begin(115200, SERIAL_8N1, RS485_RX_2, RS485_TX_2);
            leds[0] = CRGB::Black;
            leds[1] = CRGB::Red;
            leds[2] = CRGB::Black;
            leds[3] = CRGB::Black;
            FastLED.show();
            break;
        case 3:
            SerialPort.end();
            SerialPort.begin(115200, SERIAL_8N1, RS485_RX_3, RS485_TX_3);
            leds[0] = CRGB::Black;
            leds[1] = CRGB::Black;
            leds[2] = CRGB::Red;
            leds[3] = CRGB::Black;
            FastLED.show();
            break;
        case 4:
            SerialPort.end();
            SerialPort.begin(115200, SERIAL_8N1, RS485_RX_4, RS485_TX_4);
            leds[0] = CRGB::Black;
            leds[1] = CRGB::Black;
            leds[2] = CRGB::Black;
            leds[3] = CRGB::Red;
            FastLED.show();
            break;

        default:
            break;
        }

        if (RS485_Device_Number > 3)
        {
            RS485_Device_Number = 0;
        }

        RS485_Change_Flag = false;
    }

    while (SerialPort.available() > 0)
    {
        DATA1 = SerialPort.read();
        Serial.print(DATA1);
    }
}
