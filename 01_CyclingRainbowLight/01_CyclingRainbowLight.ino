//  Tinypico Helper library
#include <TinyPICO.h>
TinyPICO tp = TinyPICO();

//  Haptic Controller Library
#include <Wire.h>
#include "Adafruit_DRV2605.h"
Adafruit_DRV2605 drv;

//  Protopie Communication
#include <string.h>

struct MessageValue {
  String message;
  String value;
};

struct MessageValue getMessage(String inputtedStr) {
  struct MessageValue result;

  char charArr[50];
  inputtedStr.toCharArray(charArr, 50);
  char* ptr = strtok(charArr, "||");
  result.message = String(ptr);
  ptr = strtok(NULL, "||");

  if (ptr == NULL) {
    result.value = String("");
    return result;
  }

  result.value = String(ptr);

  return result;
}

struct MessageValue receivedData;

bool firstTime = true;

void setup() {

  Serial.begin(115200);
  Serial.setTimeout(10);

  Serial.println("DRV2605 Available");
  if (! drv.begin()) {
    Serial.println("NO DRV2605 PANIC PANIC");
    while (1) delay(10);
  }
  drv.selectLibrary(1);
  drv.setMode(DRV2605_MODE_INTTRIG);
}

void loop() {

  while (Serial.available() > 0) {
    String receivedString = Serial.readStringUntil('\0');
    receivedData = getMessage(receivedString);
  }

  if (receivedData.message.equals("Start") && firstTime) {
    startFlockySynchro();
    firstTime=false;
  }

  if (!firstTime) {
    tp.DotStar_SetPixelColor(0xFFFFFF);

    if (receivedData.message.equals("Finish")) {
    finalHype();
    firstTime=true;
  }
  }
}

void startFlockySynchro() {
  
  tp.DotStar_SetPower(true);
  tp.DotStar_CycleColor(25);

  // set the haptics to play
  drv.setWaveform(0, 64);  // play effect 64 − Transition Hum 1 – 100%
  drv.setWaveform(1, 0);       // end waveform
  drv.go();
  //drv.setWaveform(0, 70);  // play effect 70 − Transition Ramp Down Long Smooth 1 – 100 to 0%
  //drv.setWaveform(1, 0);       // end waveform
  //drv.go();

  // wait a bit
  delay(5000);
}

void finalHype() {
  tp.DotStar_SetPower(false);
}
