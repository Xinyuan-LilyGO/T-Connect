# 1 "C:\\Users\\XK\\AppData\\Local\\Temp\\tmpmf_xjbbn"
#include <Arduino.h>
# 1 "D:/Information/VSCode/GitHub/T-Connect/examples/RS232_2/RS232_2.ino"
# 10 "D:/Information/VSCode/GitHub/T-Connect/examples/RS232_2/RS232_2.ino"
#include <Arduino.h>
#include "pin_config.h"
#include <HardwareSerial.h>

static size_t CycleTime = 0;
static uint8_t Uart_Buf[105] = {0};

static uint32_t Uart_Count = 0;

static uint8_t Uart_Data[] = {

    0x0A,


    0B00000000,
    0B00000000,
    0B00000000,
    0B00000000,


    0xAA,
    0xAA,
    0xAA,
    0xAA,
    0xAA,
    0xAA,
    0xAA,
    0xAA,
    0xAA,
    0xAA,
    0xAA,
    0xAA,
    0xAA,
    0xAA,
    0xAA,
    0xAA,
    0xAA,
    0xAA,
    0xAA,
    0xAA,
    0xAA,
    0xAA,
    0xAA,
    0xAA,
    0xAA,
    0xAA,
    0xAA,
    0xAA,
    0xAA,
    0xAA,
    0xAA,
    0xAA,
    0xAA,
    0xAA,
    0xAA,
    0xAA,
    0xAA,
    0xAA,
    0xAA,
    0xAA,
    0xAA,
    0xAA,
    0xAA,
    0xAA,
    0xAA,
    0xAA,
    0xAA,
    0xAA,
    0xAA,
    0xAA,
    0xAA,
    0xAA,
    0xAA,
    0xAA,
    0xAA,
    0xAA,
    0xAA,
    0xAA,
    0xAA,
    0xAA,
    0xAA,
    0xAA,
    0xAA,
    0xAA,
    0xAA,
    0xAA,
    0xAA,
    0xAA,
    0xAA,
    0xAA,
    0xAA,
    0xAA,
    0xAA,
    0xAA,
    0xAA,
    0xAA,
    0xAA,
    0xAA,
    0xAA,
    0xAA,
    0xAA,
    0xAA,
    0xAA,
    0xAA,
    0xAA,
    0xAA,
    0xAA,
    0xAA,
    0xAA,
    0xAA,
    0xAA,
    0xAA,
    0xAA,
    0xAA,
    0xAA,
    0xAA,
    0xAA,
    0xAA,
    0xAA,
    0xAA,
};
bool Uart_Check_Dynamic_Data(uint8_t *uart_data, uint32_t check_data);
bool Uart_Check_Static_Data(uint8_t *uart_data);
void setup();
void loop();
#line 132 "D:/Information/VSCode/GitHub/T-Connect/examples/RS232_2/RS232_2.ino"
bool Uart_Check_Dynamic_Data(uint8_t *uart_data, uint32_t check_data)
{
    uint32_t temp;

    temp = (uint32_t)uart_data[1] << 24 | (uint32_t)uart_data[2] << 16 |
           (uint32_t)uart_data[3] << 8 | (uint32_t)uart_data[4];

    if (temp == check_data)
    {
        return true;
    }

    return false;
}

bool Uart_Check_Static_Data(uint8_t *uart_data)
{
    if (memcmp(&uart_data[5], &Uart_Data[5], 100) == 0)
    {
        return true;
    }
    return false;
}

void setup()
{
    Serial.begin(115200);
    Serial.println("Ciallo");



    Serial1.begin(115200, SERIAL_8N1, RS485_RX_4, RS485_TX_4);

    Serial.println("RS232 is preparing");
    delay(1000);
    Serial.println("5");
    delay(1000);
    Serial.println("4");
    delay(1000);
    Serial.println("3");
    delay(1000);
    Serial.println("2");
    delay(1000);
    Serial.println("1");
    delay(1000);
    Serial.println("RS232 preparation completed");

    while (Serial1.available() > 0)
    {
        Serial1.read();
    }
}

void loop()
{
    if (digitalRead(0) == 0)
    {
        delay(300);

        Serial1.write(Uart_Data, 105);

        Uart_Count++;
    }

    while (Serial1.available() > 0)
    {
        delay(1000);
        Serial1.read(Uart_Buf, sizeof(Uart_Buf));

        if (Uart_Buf[0] == 0x0A)
        {
            if (Uart_Check_Dynamic_Data(Uart_Buf, Uart_Count) == false)
            {
                while (1)
                {
                    Serial.printf("Check Dynamic Data Failed\n");
                    Serial.printf("Check Data: %d\n", Uart_Count);
                    Serial.printf("Received Data: %d\n", (uint32_t)Uart_Buf[1] << 24 | (uint32_t)Uart_Buf[2] << 16 |
                                                              (uint32_t)Uart_Buf[3] << 8 | (uint32_t)Uart_Buf[4]);
                    Serial.printf("Received Buf[1]: %#X\n", Uart_Buf[1]);
                    Serial.printf("Received Buf[2]: %#X\n", Uart_Buf[2]);
                    Serial.printf("Received Buf[3]: %#X\n", Uart_Buf[3]);
                    Serial.printf("Received Buf[4]: %#X\n", Uart_Buf[4]);
                    delay(1000);
                }
            }
            else if (Uart_Check_Static_Data(Uart_Buf) == false)
            {
                Serial.printf("Check Static Data Failed\n");
                for (int i = 0; i < 100; i++)
                {
                    Serial.printf("Received Buf[%d]: %#X\n", i + 5, Uart_Buf[i + 5]);
                }
                delay(1000);
            }
            else
            {
                delay(500);

                Serial.printf("Check Data Successful\n");
                Serial.printf("Check Data: %d\n", Uart_Count);
                Serial.printf("Received Data: %d\n", (uint32_t)Uart_Buf[1] << 24 | (uint32_t)Uart_Buf[2] << 16 |
                                                          (uint32_t)Uart_Buf[3] << 8 | (uint32_t)Uart_Buf[4]);
                Serial.printf("Received Buf[1]: %#X\n", Uart_Buf[1]);
                Serial.printf("Received Buf[2]: %#X\n", Uart_Buf[2]);
                Serial.printf("Received Buf[3]: %#X\n", Uart_Buf[3]);
                Serial.printf("Received Buf[4]: %#X\n", Uart_Buf[4]);

                Serial.printf("Received Buf[105]: %#X\n", Uart_Buf[104]);

                Uart_Count++;

                Uart_Data[1] = Uart_Count >> 24;
                Uart_Data[2] = Uart_Count >> 16;
                Uart_Data[3] = Uart_Count >> 8;
                Uart_Data[4] = Uart_Count;

                delay(1000);
                Serial1.write(Uart_Data, 105);

                Uart_Count++;
            }
        }
        else
        {
            while (1)
            {
                Serial.printf("Check Header Failed\n");
                Serial.printf("Received Header: %#X\n", Uart_Buf[0]);
                Serial.printf("Received Data: %d\n", (uint32_t)Uart_Buf[1] << 24 | (uint32_t)Uart_Buf[2] << 16 |
                                                          (uint32_t)Uart_Buf[3] << 8 | (uint32_t)Uart_Buf[4]);
                Serial.printf("Received Buf[1]: %#X\n", Uart_Buf[1]);
                Serial.printf("Received Buf[2]: %#X\n", Uart_Buf[2]);
                Serial.printf("Received Buf[3]: %#X\n", Uart_Buf[3]);
                Serial.printf("Received Buf[4]: %#X\n", Uart_Buf[4]);
                delay(1000);
            }
        }
    }
}