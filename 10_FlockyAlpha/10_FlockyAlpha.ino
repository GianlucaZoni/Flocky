//  Tinypico Helper library
#include <TinyPICO.h>
TinyPICO tp = TinyPICO();

//  Haptic Controller Library
#include <Wire.h>
#include "Adafruit_DRV2605.h"
Adafruit_DRV2605 drv;

//  Protopie Communication
#include <string.h>

// Comms
#include <WiFi.h>
#include <esp_now.h>

typedef struct struct_message {
  bool a;
  bool b;
  bool c;
} struct_message;

struct_message myPacket;

bool firstTime = true;
int startUpTime=5000;
int startUpCounter = 0;
int prevStartUpCounter = 0;
int startUpTiming = 25;

// Callback function
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  // Get incoming data
  memcpy(&myPacket, incomingData, sizeof(myPacket));
  
  //Start
  if (myPacket.a) {
    startFlockySynchro();
  }
  //Faraway
  if (myPacket.b) {
    helpRequest();
  }
  //Ending
  if (myPacket.c) {
    finalHype();
  }

}

void setup() {

  // Set up Serial Monitor
  Serial.begin(115200);
  Serial.setTimeout(10);

  // Haptic Controller
  Serial.println("DRV2605 Available");
  if (! drv.begin()) {
    Serial.println("NO DRV2605 PANIC PANIC");
    while (1) delay(10);
  }
  drv.selectLibrary(1);
  drv.setMode(DRV2605_MODE_INTTRIG);

  // Start ESP32 in Station mode
  WiFi.mode(WIFI_STA);
 
  // Initalize ESP-NOW
  if (esp_now_init() != 0) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
   
  // Register callback function
  esp_now_register_recv_cb(OnDataRecv);

}

void loop() {
}

void startFlockySynchro() {

  tp.DotStar_SetPower(true);

  // set the haptics to play
  drv.setWaveform(0, 64);  // play effect 64 − Transition Hum 1 – 100%
  drv.setWaveform(1, 0);       // end waveform

  int colorRotation = 0;
  for (int i=0; i<startUpTime;i++) {
    startUpCounter++;
    int tempCounter=startUpCounter;
    if ((tempCounter-prevStartUpCounter)>=startUpTiming){
      //event
      colorRotation++;
      byte WheelPos = 255 - colorRotation;
      drv.go();
      if(WheelPos < 85)
        {
            tp.DotStar_SetPixelColor(255 - WheelPos * 3, 0, WheelPos * 3);
        }
        else if(WheelPos < 170)
        {
            WheelPos -= 85;
            tp.DotStar_SetPixelColor(0, WheelPos * 3, 255 - WheelPos * 3);
        }
        else
        {
            WheelPos -= 170;
            tp.DotStar_SetPixelColor(WheelPos * 3, 255 - WheelPos * 3, 0);
        }
        tp.DotStar_Show();
      prevStartUpCounter=tempCounter;
    }

  }

  tp.DotStar_SetPixelColor(0xFFFFFF);

}

void helpRequest() {

  // set the haptics to play
  drv.setWaveform(0, 64);  // play effect 64 − Transition Hum 1 – 100%
  drv.setWaveform(1, 0);       // end waveform

  drv.go();

  for (int b=255;b>0;b--) {
    tp.DotStar_SetBrightness(b);
    tp.DotStar_Show();
  }

  drv.go();

  for (int b=1;b<=255;b++) {
    tp.DotStar_SetBrightness(b);
    tp.DotStar_Show();
  }
  
}

void finalHype() {

  // set the haptics to play
  drv.setWaveform(0, 94);  // play effect 94 − Transition Ramp Down Long Smooth 1 – 50 to 0%
  drv.setWaveform(1, 0);       // end waveform

  int colorRotation = 0;
  for (int i=0; i<startUpTime;i++) {
    startUpCounter++;
    int tempCounter=startUpCounter;
    if ((tempCounter-prevStartUpCounter)>=startUpTiming){
      //event
      colorRotation++;
      byte WheelPos = 255 - colorRotation;
      drv.go();
      if(WheelPos < 85)
        {
            tp.DotStar_SetPixelColor(255 - WheelPos * 3, 0, WheelPos * 3);
        }
        else if(WheelPos < 170)
        {
            WheelPos -= 85;
            tp.DotStar_SetPixelColor(0, WheelPos * 3, 255 - WheelPos * 3);
        }
        else
        {
            WheelPos -= 170;
            tp.DotStar_SetPixelColor(WheelPos * 3, 255 - WheelPos * 3, 0);
        }
        tp.DotStar_Show();
      prevStartUpCounter=tempCounter;
    }

  }

  tp.DotStar_SetPower(false);

}