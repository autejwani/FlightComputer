#include <SPI.h>
#include <LoRa.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>


// ================= LoRa Pins =================
#define SCK 5
#define MISO 19
#define MOSI 27
#define SS 18
#define RST 14
#define DIO0 26


#define FREQ 915E6


// ================= OLED ======================
#define OLED_SDA 4
#define OLED_SCL 15
#define OLED_RST 16
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64


Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RST);


enum SystemStatus {
 SYS_OK = 0,
 SYS_LORA_FAIL
};


SystemStatus systemStatus = SYS_OK;


// ===== Buffer for received packet =====
uint8_t packet[11];


void setup() {
 Serial.begin(115200);


 // OLED reset
 pinMode(OLED_RST, OUTPUT);
 digitalWrite(OLED_RST, LOW);
 delay(20);
 digitalWrite(OLED_RST, HIGH);


 Wire.begin(OLED_SDA, OLED_SCL);
 if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) while (1);


 display.clearDisplay();
 display.setTextSize(1);
 display.setTextColor(SSD1306_WHITE);
 display.setCursor(0,0);
 display.println("LORA RX INIT");
 display.display();


 // LoRa init
 SPI.begin(SCK, MISO, MOSI, SS);
 LoRa.setPins(SS, RST, DIO0);


 if (!LoRa.begin(FREQ)) {
   Serial.println("LoRa init failed");
   systemStatus = SYS_LORA_FAIL;
 }


 display.clearDisplay();
 display.setCursor(0,0);
 display.println("INIT STATUS:");
 if (systemStatus == SYS_OK)
   display.println("SYS OK");
 else
   display.println("LORA FAIL");
 display.display();


 Serial.println("Receiver ready");
}


void loop() {
 if (systemStatus != SYS_OK) return;


 int packetSize = LoRa.parsePacket(11);
 Serial.println(packetSize);
 if (packetSize > 0) {
   // Make sure packet has expected length
   if (packetSize != sizeof(packet)) {
     Serial.print("Unexpected packet size: ");
     Serial.println(packetSize);
     // flush this packet
     while (LoRa.available()) LoRa.read();
     return;
   }


   // Read the full packet
   for (int i = 0; i < packetSize; i++) {
     packet[i] = LoRa.read();
   }


   // ===== Parse packet =====
   uint32_t timestamp = ((uint32_t)packet[1] << 24) |
                        ((uint32_t)packet[2] << 16) |
                        ((uint32_t)packet[3] << 8)  |
                        (uint32_t)packet[4];


   int16_t temp_fixed = ((int16_t)packet[5] << 8) |
                        (int16_t)packet[6];
   float temperature = temp_fixed / 100.0f;


   int32_t pressure = ((int32_t)packet[7] << 24) |
                      ((int32_t)packet[8] << 16) |
                      ((int32_t)packet[9] << 8)  |
                      (int32_t)packet[10];


   // ===== Serial Output =====
   Serial.println("Received Packet:");
   Serial.print("Temp(C): ");
   Serial.println(temperature,2);
   Serial.print("Pressure(Pa): ");
   Serial.println(pressure);
   Serial.print("Timestamp(ms): ");
   Serial.println(timestamp);
   Serial.println("------------------");


   // ===== OLED Output =====
   display.clearDisplay();
   display.setCursor(0,0);
   display.println("LORA RX");
   display.setCursor(0,16);
   display.print("Temp: ");
   display.print(temperature,2);
   display.println(" C");
   display.setCursor(0,28);
   display.print("Pressure: ");
   display.print(pressure);
   display.println(" Pa");
   display.setCursor(0,40);
   display.print("Time(ms): ");
   display.println(timestamp);
   display.display();
 }


 delay(50);
}
