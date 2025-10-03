#include <Arduino.h>
#include <SPI.h>
#include <U8g2lib.h>
#include <esp_now.h>
#include <WiFi.h>

#define HallInputPin   4
int turns = 0;
int count = 0; 
int i = 0;
bool TurnsMultiple = false;
void IRAM_ATTR Increment_turns()
{
  turns++;
  count++;
  if (count == 10) {
    TurnsMultiple = true;
    count = 0;
  }

 

}
//d4:8c:49:e4:f0:78
// MAC Address of responder - edit as required
uint8_t broadcastAddress[] = {0xd4, 0x8c, 0x49, 0xe4, 0xf0, 0x78};

typedef struct struct_message {
  
  int a;
  
} struct_message; 

struct_message myData;

 // Peer info
esp_now_peer_info_t peerInfo;

 // Callback function called when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}
/* Constructor */
U8G2_SH1106_72X40_WISE_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE, 6, 5);
int enableUTF8Print(turns);
/* u8g2.begin() is required and will sent the setup/init sequence to the display */
void setup(void) {

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
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  
  // Add peer        
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }
 {
  pinMode(HallInputPin, INPUT);
  attachInterrupt(HallInputPin, Increment_turns, RISING);
}
  u8g2.begin();
 
}

/* draw something on the display with the `firstPage()`/`nextPage()` loop*/
void loop(void) {
  u8g2.firstPage();
  do {
    u8g2.setFont(u8g2_font_courB18_tr);
    u8g2.setCursor(0, 18);
    u8g2.print(turns,0);
    u8g2.setFont(u8g2_font_courB14_tr);
    u8g2.drawStr(10,40,"Turns");
  } while ( u8g2.nextPage() );

  if (TurnsMultiple == true)
  
  
  // Format structured data
  myData.a = turns;
  
  
  // Send message via ESP-NOW
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));
  // Invert the boolean value
  TurnsMultiple = !TurnsMultiple;

  if (result == ESP_OK) {
    Serial.println("Sending confirmed");
  }
  else {
    Serial.println("Sending error");
  }
  
}