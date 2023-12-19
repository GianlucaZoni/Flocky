/*
  
  ESP-NOW Transmit
  
*/

#include <esp_now.h>
#include <WiFi.h>

bool helpabroout = false;
bool LeftBehind = false;

// MAC Address of Responders
uint8_t broadcastAddressAlpha[] = {0xD4, 0xD4, 0xDA, 0x83, 0xE1, 0x10};
uint8_t broadcastAddressBravo[] = {0xD4, 0xD4, 0xDA, 0x83, 0xE1, 0x90};

// Define a data structure
typedef struct struct_message {
  char a[7];
  bool b;
  bool c;
} struct_message;

// Create a structured object
struct_message myData;

// Peer info
esp_now_peer_info_t peerInfo;

// Callback function called when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

void setup() {
  
  // Set up Serial Monitor
  Serial.begin(115200);
 
  // Set ESP32 as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Initilize ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Register the send callback
  esp_now_register_send_cb(OnDataSent);
  
  // Register peer
  memcpy(peerInfo.peer_addr, broadcastAddressAlpha, 6);
  memcpy(peerInfo.peer_addr, broadcastAddressBravo, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  
  // Add peer        
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }
}

void loop() {

  LeftBehind = !LeftBehind;
  
  // Format structured data
  strcpy(myData.a, "Flocky1");
  myData.b = helpabroout;
  myData.c = LeftBehind;
  
  // Send message via ESP-NOW
  esp_err_t result = esp_now_send(broadcastAddressAlpha, (uint8_t *) &myData, sizeof(myData));
  esp_err_t result = esp_now_send(broadcastAddressBravo, (uint8_t *) &myData, sizeof(myData));
   
  if (result == ESP_OK) {
    Serial.println("Sending confirmed");
  }
  else {
    Serial.println("Sending error");
  }
  delay(2000);
}