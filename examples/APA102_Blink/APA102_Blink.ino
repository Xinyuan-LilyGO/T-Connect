/*
 * @Description: 
            APA102
        All lights will be lit in sequence of red, green, and blue lights
 * @Author: LILYGO_L
 * @Date: 2024-02-26 16:32:52
 * @LastEditTime: 2024-07-10 10:14:13
 * @License: GPL 3.0
 */

#include "FastLED.h"
#include "pin_config.h"

#define NUM_LEDS 4
#define DATA_PIN APA102_DATA
#define CLOCK_PIN APA102_CLOCK

static uint32_t APA102_RGB_Data[] = {CRGB::Red, CRGB::Green, CRGB::Blue, CRGB::Black};

CRGB leds[NUM_LEDS];

void setup()
{
    FastLED.addLeds<APA102, DATA_PIN, CLOCK_PIN, BGR>(leds, NUM_LEDS);
    FastLED.setBrightness(20);
}

void loop()
{
    for (int i = 0; i < 4; i++)
    {
        leds[0] = APA102_RGB_Data[i];
        FastLED.show();

        leds[1] = APA102_RGB_Data[i];
        FastLED.show();

        leds[2] = APA102_RGB_Data[i];
        FastLED.show();

        leds[3] = APA102_RGB_Data[i];
        FastLED.show();
        delay(1000);
    }
}
