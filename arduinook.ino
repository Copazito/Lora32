#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <SPI.h>
#include <LoRa.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_ADDR 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// Pines LoRa
#define SCK     5
#define MISO    19
#define MOSI    27
#define SS      18
#define RST     14
#define DIO0    26
#define LORA_BAND 915E6

void setup() {
  Serial.begin(115200);
  while(!Serial);

  // OLED
  if(!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
    Serial.println("OLED no detectada");
    while(true);
  }
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 20);
  display.println("Transmisor");
  display.display();

  // LoRa
  SPI.begin(SCK, MISO, MOSI, SS);
  LoRa.setPins(SS, RST, DIO0);
  if(!LoRa.begin(LORA_BAND)) {
    display.clearDisplay();
    display.setCursor(0, 20);
    display.println("Error LoRa");
    display.display();
    Serial.println("Error inicializando LoRa");
    while(true);
  }

  display.clearDisplay();
  display.setCursor(0, 20);
  display.println("LoRa listo");
  display.display();
}

void loop() {
  // Enviar mensaje "Hola"
  LoRa.beginPacket();
  LoRa.print("Hola");
  LoRa.endPacket();
  Serial.println("Mensaje enviado: Hola");

  display.clearDisplay();
  display.setCursor(0, 20);
  display.println("Enviado: Hola");
  display.display();

  // Esperar confirmación del receptor por 2 segundos
  long start = millis();
  bool recibido = false;
  while(millis() - start < 2000) {
    int packetSize = LoRa.parsePacket();
    if(packetSize) {
      String msg = "";
      while(LoRa.available()) {
        msg += (char)LoRa.read();
      }
      if(msg == "OK") {
        recibido = true;
        Serial.println("Receptor confirmó: OK");
        display.clearDisplay();
        display.setCursor(0, 20);
        display.println("Receptor OK");
        display.display();
        break;
      }
    }
  }

  if(!recibido) {
    Serial.println("No se recibió confirmación");
    display.clearDisplay();
    display.setCursor(0, 20);
    display.println("Sin confirmación");
    display.display();
  }

  delay(3000); // espera 3 segundos antes del próximo envío
}