#define latchPin 10
#define clockPin 9
#define dataPin 8
#define RIGHT true
#define LEFT false

#define rotaryKey 7
#define rotaryS2 6
#define rotaryS1 5

//‎A4 (SDA), A5 (SCL)

#include <DS3231.h>
#include "Rotary.hpp"

DS3231 rtc(SDA, SCL);
Time t;
int sekunden;
Rotary rotary(rotaryKey, rotaryS1, rotaryS2);

const byte values[]=
  {
    0b01111011, // 0
    0b01100000, // 1
    0b01011101, // 2
    0b01110101, // 3
    0b01100110, // 4
    0b00110111, // 5
    0b00111111, // 6
    0b01100001, // 7
    0b01111111, // 8
    0b01110111,  // 9
    0b10000000  //dp
  };

void setTimeEncoder()
{
  int state = 0;
  int hourBig, hourSmall, minuteBig, minuteSmall = 0;

  Serial.println("Setting time...");
  for(int i = 0; i< 6; i++)
  {
  shiftOut(dataPin, clockPin, MSBFIRST, values[10]);
  }
  digitalWrite(latchPin, LOW);
  digitalWrite(latchPin, HIGH);
  

  while(state < 4)
  {
    switch(state)
    {
      case(0):
      {
        if(rotary.getTurn() == LEFT)
        {
          hourBig--;        
        }
        else if(rotary.getTurn() == RIGHT)
        {
          hourBig++;        
        }
        if(hourBig>2)
          hourBig = 0;

        if(hourBig < 0)
          hourBig = 2;
        break;
      }
      case(1):
      {
        if(rotary.getTurn() == LEFT)
        {
          hourSmall--;        
        }
        else if(rotary.getTurn() == RIGHT)
        {
          hourSmall++;        
        }

        if(hourBig == 2 && hourSmall> 3)
          hourSmall = 0;

        else if(hourSmall > 9)
          hourSmall = 0;

        else if(hourSmall < 0)
          hourSmall = 9;
        
        break;
      }
      case(2):
      {
         if(rotary.getTurn() == LEFT)
        {
          minuteBig--;        
        }
        else if(rotary.getTurn() == RIGHT)
        {
          minuteBig++;        
        }

        if(minuteBig > 5)
          minuteBig = 0;

        if(minuteBig < 0)
          minuteBig = 5;

        break;
      }
      case(3):
      {
         if(rotary.getTurn() == LEFT)
        {
          minuteSmall--;        
        }
        else if(rotary.getTurn() == RIGHT)
        {
          minuteSmall++;        
        }

        if(minuteSmall > 9)
          minuteSmall = 0;

        if(minuteSmall < 0)
          minuteSmall = 9;
        break;
      }
    }
    
    if(rotary.buttonPressed())
    {
      state++;
      delay(1000);
    }
    renderTimeDigit(hourBig * 10 + hourSmall, minuteBig * 10 + minuteSmall, 0);
  }
  rtc.setTime(hourBig * 10 + hourSmall, minuteBig * 10 + minuteSmall, 0);
}

void renderTimeDigit(int hours, int minutes, int seconds)
{
  byte zeit[6];

  zeit[0] = hours/10; // hours big
  zeit[1] = hours%10; // hours small

  zeit[2] = minutes/10; // minutes big
  zeit[3] = minutes%10; // minutes small

  zeit[4] = seconds/10; // seconds big
  zeit[5] = seconds%10; // seconds small
  
  for(int i = 5; i>=0; i--)
  {
    shiftOut(dataPin, clockPin, MSBFIRST, values[zeit[i]]);
  }
    digitalWrite(latchPin, LOW);
    digitalWrite(latchPin, HIGH);
  
}

void testDigit()
{
  for(int i = 0; i<=9; i++)
  {
    renderTimeDigit(i*10 + i, i*10 + i, i*10 + i);
    delay(1000);
  }
}

void setup()
{
    Serial.begin(9600);
    
    pinMode(latchPin, OUTPUT);
    pinMode(clockPin, OUTPUT);
    pinMode(dataPin, OUTPUT);
    pinMode(rotaryKey, INPUT);
    pinMode(rotaryS1, INPUT);
    pinMode(rotaryS2, INPUT);
    rtc.begin();
}

void loop()
{
    /*shiftOut(dataPin, clockPin, MSBFIRST, values[i]);
    Serial.print("i: ");
    Serial.print(i);
    Serial.print(" ");
    Serial.println(values[i], BIN);
    digitalWrite(latchPin, LOW);
    digitalWrite(latchPin, HIGH);*/

     sekunden = t.sec;

    t = rtc.getTime();
   /* Serial.print(t.hour);
    Serial.print(":");
    Serial.print(t.min);
    Serial.print(":");
    Serial.print(t.sec);
    Serial.println();*/
    
    if(sekunden != t.sec )
    {
      renderTimeDigit(t.hour, t.min, t.sec);
    }

    //if(rotary.buttonPressed())
    {
      //testDigit();
      //setTimeEncoder();
    }

     if(rotary.buttonPressed())
     {
       int keyT = rotary.keyTime();
       if(keyT> 0)
      Serial.println(keyT);
       if(keyT >= 2000)
       {
        setTimeEncoder();
       }else
       {
             testDigit();
        }
     }
}

