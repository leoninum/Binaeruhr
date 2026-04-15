#include "Clock.hpp"
#include "Configuration.h"

#include <DS3231.h>
#include "Rotary.hpp"

#define latchPin 10
#define clockPin 9
#define dataPin 8

#define dataBin 11
#define clockBin 12
#define latchBin 13

#define rotaryKey 7
#define rotaryS2 6
#define rotaryS1 5

#define PAUSE_LED 4

#define NormalSyncSeconds 43200
#define NotSyncedSyncSeconds 300

DS3231 rtc(SDA, SCL);
Time t;

int syncSeconds = 0;
bool synced = false;

Clock clock(dataPin, clockPin, latchPin, dataBin, clockBin, latchBin, 2, 3);
RotaryEncoder rotary(rotaryS1, rotaryS2, rotaryKey);

#define DIMENSION 3
int greenTime[DIMENSION][4] = {
  {12, 12, 12, 13},
  {13, 3, 13, 28},
  {13, 32, 13, 33}
};

void setup()
{
    pinMode(PAUSE_LED, OUTPUT);

    Serial.begin(9600);
    Serial.println("Start...");

    rtc.begin();

    t = rtc.getTime();
    clock.setTime(t.hour, t.min, t.sec);
    clock.render(!synced);
}

void loop()
{
    static int lastSecond = -1;

    t = rtc.getTime();

    // Sync
    if(syncSeconds <= 0)
    {
        Serial.println("Versuche Sync...");

        int* result = clock.getESPTime([&rtc, &clock]() {});

        if(result == NULL)
        {
            Serial.println("Sync fehlgeschlagen");
            syncSeconds = NotSyncedSyncSeconds;
            synced = false;
        }
        else
        {
            rtc.setTime(result[0], result[1], result[2]);
            syncSeconds = NormalSyncSeconds;
            synced = true;
        }
    }

    // Nur bei neuer Sekunde
    if (t.sec != lastSecond)
    {
        lastSecond = t.sec;

        clock.setTime(t.hour, t.min, t.sec);
        clock.render(!synced);

        if (syncSeconds > 0)
            syncSeconds--;

        // Pause prüfen
        bool isBreak = false;

        int current = t.hour * 60 + t.min;

        for(int i = 0; i < DIMENSION; i++)
        {
            int start = greenTime[i][0] * 60 + greenTime[i][1];
            int end   = greenTime[i][2] * 60 + greenTime[i][3];

            if(current >= start && current <= end)
            {
                isBreak = true;
                break;
            }
        }

        digitalWrite(PAUSE_LED, isBreak ? HIGH : LOW);
    }

    // Rotary
    int taste = rotary.getTaste();

    if (taste == 1)
    {
        Serial.println("Click");
        rotary.setCount(0);
        clock.testClock();
    }
    else if (taste > 1)
    {
        Serial.println("Long Click");
        clock.setTimeEncoder(&rotary, &rtc);
    }
}
