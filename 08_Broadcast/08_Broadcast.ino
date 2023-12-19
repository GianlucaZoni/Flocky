//  Tinypico Helper library
#include <TinyPICO.h>
TinyPICO tp = TinyPICO();

// Include Libraries
#include <WiFi.h>
#include <esp_now.h>

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

typedef struct struct_message {
  bool a;
  bool b;
  bool c;
} struct_message;

struct_message myPacket;

void formatMacAddress(const uint8_t *macAddr, char *buffer, int maxLength)
// Formats MAC Address
{
  snprintf(buffer, maxLength, "%02x:%02x:%02x:%02x:%02x:%02x", macAddr[0], macAddr[1], macAddr[2], macAddr[3], macAddr[4], macAddr[5]);
}

// Callback when Data Received
void receiveCallback(const uint8_t *macAddr, const uint8_t *data, int dataLen) {
  // maximum of 250 characters in the message + a null terminating byte
  char buffer[ESP_NOW_MAX_DATA_LEN + 1];
  int msgLen = min(ESP_NOW_MAX_DATA_LEN, dataLen);
  strncpy(buffer, (const char *)data, msgLen);

  // null terminated
  buffer[msgLen] = 0;

  // Format the MAC address
  char macStr[18];
  formatMacAddress(macAddr, macStr, 18);

  // Send Debug log message to the serial port
  Serial.printf("Received message from: %s - %s\n", macStr, buffer);

  /*// Check switch status
  if (strcmp("on", buffer) == 0)
  {
    ledOn = true;
  }
  else
  {
    ledOn = false;
  }
  digitalWrite(STATUS_LED, ledOn);*/
}

// Callback when Data Sent
void sentCallback(const uint8_t *macAddr, esp_now_send_status_t status) {
  char macStr[18];
  formatMacAddress(macAddr, macStr, 18);
  Serial.print("Last Packet Sent to: ");
  Serial.println(macStr);
  Serial.print("Last Packet Send Status: ");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

// Broadcast
void broadcast(struct_message &myPacket) {
  uint8_t broadcastAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
  esp_now_peer_info_t peerInfo = {};
  memcpy(&peerInfo.peer_addr, broadcastAddress, 6);
  if (!esp_now_is_peer_exist(broadcastAddress))
  {
    esp_now_add_peer(&peerInfo);
  }
  // Send Message
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &myPacket, sizeof(myPacket));

  // Results and Error Handling
  if (result == ESP_OK)
  {
    Serial.println("Broadcast message success");
  }
  else if (result == ESP_ERR_ESPNOW_NOT_INIT)
  {
    Serial.println("ESP-NOW not Init.");
  }
  else if (result == ESP_ERR_ESPNOW_ARG)
  {
    Serial.println("Invalid Argument");
  }
  else if (result == ESP_ERR_ESPNOW_INTERNAL)
  {
    Serial.println("Internal Error");
  }
  else if (result == ESP_ERR_ESPNOW_NO_MEM)
  {
    Serial.println("ESP_ERR_ESPNOW_NO_MEM");
  }
  else if (result == ESP_ERR_ESPNOW_NOT_FOUND)
  {
    Serial.println("Peer not found.");
  }
  else
  {
    Serial.println("Unknown error");
  }
}

bool firstTime = true;
bool StartUp = false;
bool Help = false;
bool Ending = false;

bool firstTime = true;
int startUpTime=5000;
int startUpCounter = 0;
int prevStartUpCounter = 0;
int startUpTiming = 25;

void setup() {

  // Set up Serial Monitor
  Serial.begin(115200);
  delay(250);

  // Set ESP32 in STA mode to begin with
  WiFi.mode(WIFI_STA);
  Serial.println("Broadcasting...");

  // Print MAC address
  /*
  Serial.print("MAC Address: ");
  Serial.println(WiFi.macAddress());
  */

  // Disconnect from WiFi
  WiFi.disconnect();

  // Initialize ESP-NOW
  if (esp_now_init() == ESP_OK)
  {
    Serial.println("ESP-NOW Init Successfull");
    esp_now_register_recv_cb(receiveCallback);
    esp_now_register_send_cb(sentCallback);
  }
  else
  {
    Serial.println("ESP-NOW Init Failed");
    delay(5000);
    ESP.restart();
  }

}

void loop() {

  while (Serial.available() > 0) {
    String receivedString = Serial.readStringUntil('\0');
    receivedData = getMessage(receivedString);
  }

  if (receivedData.message.equals("StartUp") && firstTime) {
    startFlockySynchro();
    StartUp=true;
    firstTime=false;
  }

  if (!firstTime) {
    tp.DotStar_SetPixelColor(0xFFFFFF);

    if (receivedData.message.equals("Ending")) {
    finalHype();
    firstTime=true;
    }
  }

  //HelpBro = !HelpBro;
  //LeftBehind = !LeftBehind;


  // Put Variables into Packet
  myPacket.a=StartUp;
  myPacket.b=Help;
  myPacket.c=LeftBehind;
  // Send!
  broadcast(myPacket);
  delay(500);
  StartUp=!StartUp;
}

void startFlockySynchro() {

  tp.DotStar_SetPower(true);

  // set the haptics to play
  /*
  drv.setWaveform(0, 64);  // play effect 64 − Transition Hum 1 – 100%
  drv.setWaveform(1, 0);       // end waveform
  */

  int colorRotation = 0;
  for (int i=0; i<startUpTime;i++) {
    startUpCounter++;
    int tempCounter=startUpCounter;
    if ((tempCounter-prevStartUpCounter)>=startUpTiming){
      //event
      colorRotation++;
      byte WheelPos = 255 - colorRotation;
      //drv.go();
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

}

void finalHype() {

  // set the haptics to play
  /*
  drv.setWaveform(0, 94);  // play effect 94 − Transition Ramp Down Long Smooth 1 – 50 to 0%
  drv.setWaveform(1, 0);       // end waveform
  */

  int colorRotation = 0;
  for (int i=0; i<startUpTime;i++) {
    startUpCounter++;
    int tempCounter=startUpCounter;
    if ((tempCounter-prevStartUpCounter)>=startUpTiming){
      //event
      colorRotation++;
      byte WheelPos = 255 - colorRotation;
      //drv.go();
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