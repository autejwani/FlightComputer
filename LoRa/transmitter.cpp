#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <LoRa.h>


#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_BMP085.h>


// ================= LoRa Pins =================
#define SCK   5
#define MISO  19
#define MOSI  27
#define SS    18
#define RST   14
#define DIO0  26


#define FREQ 915E6


// ================= OLED I2C ==================
#define OLED_SDA 4
#define OLED_SCL 15
#define OLED_RST 16
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64


// ================= BMP I2C ===================
#define BMP_SDA 21
#define BMP_SCL 22


// ================= I2C Objects ===============
TwoWire I2C_BMP = TwoWire(1);


// ================= Objects ===================
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RST);
Adafruit_BMP085 bmp;


// ================= System Status =============
enum SystemStatus {
 SYS_OK = 0,
 SYS_BMP_FAIL,
 SYS_LORA_FAIL
};


SystemStatus systemStatus = SYS_OK;


// ================= Timing ====================
unsigned long lastTx = 0;
const unsigned long txInterval = 10000;


// ================= Packet ====================
uint8_t packet[11];


void setup() {
 Serial.begin(115200);


 // ===== OLED RESET =====
 pinMode(OLED_RST, OUTPUT);
 digitalWrite(OLED_RST, LOW);
 delay(20);
 digitalWrite(OLED_RST, HIGH);


 // ===== OLED I2C INIT =====
 Wire.begin(OLED_SDA, OLED_SCL);


 // ===== BMP I2C INIT =====
 I2C_BMP.begin(BMP_SDA, BMP_SCL);
 I2C_BMP.setClock(100000); // BMP-safe speed


 // ===== OLED INIT =====
 if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
   while (1);
 }


 display.clearDisplay();
 display.setTextSize(1);
 display.setTextColor(SSD1306_WHITE);
 display.setCursor(0, 0);
 display.println("LORA TX INIT");
 display.display();


 // ===== BMP INIT (ON Wire1) =====
 if (!bmp.begin(BMP085_STANDARD, &I2C_BMP)) {
   systemStatus = SYS_BMP_FAIL;
 }


 // ===== LORA INIT =====
 SPI.begin(SCK, MISO, MOSI, SS);
 LoRa.setPins(SS, RST, DIO0);


 if (!LoRa.begin(FREQ)) {
   systemStatus = SYS_LORA_FAIL;
 }


 // ===== STATUS DISPLAY =====
 display.clearDisplay();
 display.setCursor(0, 0);
 display.println("INIT STATUS:");


 if (systemStatus == SYS_OK)
   display.println("SYS OK");
 else if (systemStatus == SYS_BMP_FAIL)
   display.println("BMP FAIL");
 else if (systemStatus == SYS_LORA_FAIL)
   display.println("LORA FAIL");


 display.display();
 delay(2000);
}


void loop() {
 unsigned long now = millis();
 if (now - lastTx < txInterval) return;
 lastTx = now;


 display.clearDisplay();
 display.setCursor(0, 0);
 display.println("LORA TX");


 if (systemStatus != SYS_OK) {
   display.setCursor(0, 16);
   display.println("SYSTEM ERROR");
   display.display();
   return;
 }


 // ===== Read Sensors (BMP on Wire1) =====
 float temp_c = bmp.readTemperature();
 int32_t pressure = bmp.readPressure();
 int16_t temp_fixed = (int16_t)(temp_c * 100.0f);
 uint32_t timestamp = millis();


 // ===== Build Packet =====
 packet[0] = 10;


 packet[1]  = (timestamp >> 24) & 0xFF;
 packet[2]  = (timestamp >> 16) & 0xFF;
 packet[3]  = (timestamp >> 8)  & 0xFF;
 packet[4]  = timestamp & 0xFF;


 packet[5]  = (temp_fixed >> 8) & 0xFF;
 packet[6]  = temp_fixed & 0xFF;


 packet[7]  = (pressure >> 24) & 0xFF;
 packet[8]  = (pressure >> 16) & 0xFF;
 packet[9]  = (pressure >> 8)  & 0xFF;
 packet[10] = pressure & 0xFF;


 // ===== Transmit =====
 LoRa.beginPacket();
 LoRa.write(packet, sizeof(packet));
 LoRa.endPacket();


 // ===== OLED DISPLAY =====
 display.setCursor(0, 16);
 display.print("T: ");
 display.print(temp_c, 2);
 display.println(" C");


 display.setCursor(0, 28);
 display.print("P: ");
 display.print(pressure);
 display.println(" Pa");


 display.setCursor(0, 40);
 display.print("t(ms): ");
 display.println(timestamp);


 display.display();
}
