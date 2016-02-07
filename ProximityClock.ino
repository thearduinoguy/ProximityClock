#include <VCNL4000.h>
#include "LedControl.h"
#include <Wire.h>
#include <Time.h>
#include <DS1307RTC.h>
#include <TinyGPS++.h>
#include <SoftwareSerial.h>

static const int RXPin = 3, TXPin = 4;
static const uint32_t GPSBaud = 9600;
int counter = 0;

VCNL4000 proximitySensor;
tmElements_t tm;

// The TinyGPS++ object
TinyGPSPlus gps;
int countDown = 1000;

// The serial connection to the GPS device
SoftwareSerial ss(RXPin, TXPin);

/*
 pin 12: DataIn.   pin 11: CLK.   pin 10: LOAD.   Number of MAX72XX.
 */
LedControl lc=LedControl(12,11,10,1);

void setup() {
  Serial.begin(115200);
  ss.begin(GPSBaud);

  lc.shutdown(0,false);
  /* Set the brightness to a medium values */
  lc.setIntensity(0,9);
  /* and clear the display */
  lc.clearDisplay(0);
  Serial.println("GPS Clock by Mike McRoberts");
  proximitySensor.begin();
}

void showTime() {

    if (RTC.read(tm)) {
      uint8_t hour = tm.Hour;
      uint8_t minute = tm.Minute;
      uint8_t second = tm.Second;
      lc.setDigit(0,0,hour/10,false);
      lc.setDigit(0,1,hour%10,false);
      // flash the seperator every second
      if (second%2) {
        lc.setDigit(0,1,hour%10,true);
      }
      lc.setDigit(0,2,minute/10,false);
      lc.setDigit(0,3,minute%10,false);
    }
    countDown-=1;
}

void getGpsTime()
{
  uint8_t  hour, minute, second;
   
  while (ss.available())
      if (gps.encode(ss.read())) {  
         if (gps.time.isValid()) {
            hour = gps.time.hour();
            minute = gps.time.minute();
            second = gps.time.second();
            tm.Hour = hour;
            tm.Minute = minute;
            tm.Second = second;
            //tm.Day = day;
            //tm.Month = month;
            //tm.Year = year;
            RTC.write(tm);
            Serial.print("TIME SYNCED: ");
            char timeDateStr[64];
            sprintf(timeDateStr, "%02d:%02d:%02d    ",hour, minute, second);
            Serial.println(timeDateStr);    
            }
            else {
                Serial.println("INVALID TIME");
            }
      }
}

void loop() {  
  uint16_t distance = proximitySensor.readProximity();
  
  getGpsTime();
  
  if (distance>2150) { countDown=250; }
  if (countDown>0) showTime();
  else { 
    countDown=0;
     lc.clearDisplay(0);
  }
  
}
