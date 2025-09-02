#include &lt;SPI.h&gt;
#include &lt;LoRa.h&gt;
#include &lt;Wire.h&gt;
#include &lt;Adafruit_GFX.h&gt;
#include &lt;Adafruit_SSD1306.h&gt;
// --- Configuración OLED ---
#define OLED_SDA 21
#define OLED_SCL 22
#define OLED_ADDR 0x3C
Adafruit_SSD1306 display(128, 64, &amp;Wire);
// --- Configuración LoRa ---
#define SCK   5
#define MISO 19
#define MOSI 27
#define SS   18
#define RST  23
#define DIO0 26
unsigned long lastReceived = 0;
void setup() {
  Serial.begin(115200);
  while (!Serial);
  // Iniciar pantalla
  Wire.begin(OLED_SDA, OLED_SCL);
  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
    Serial.println(&quot;Error al iniciar OLED&quot;);
    while (1);
  }
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  // Iniciar SPI y LoRa
  SPI.begin(SCK, MISO, MOSI, SS);
  LoRa.setPins(SS, RST, DIO0);
  if (!LoRa.begin(915E6)) {  // Cambiar a 868E6 según tu región
    Serial.println(&quot;Error al iniciar LoRa&quot;);
    display.println(&quot;LoRa ERROR&quot;);
    display.display();
    while (1);
  }

  Serial.println(&quot;LoRa Receptor iniciado!&quot;);
  display.setCursor(0, 0);
  display.println(&quot;LoRa Receptor OK&quot;);
  display.display();
}
void loop() {
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    String message = &quot;&quot;;
    while (LoRa.available()) {
      message += (char)LoRa.read();
    }
    Serial.print(&quot;Recibido: &quot;);
    Serial.println(message);
    // Mostrar en pantalla
    display.clearDisplay();
    display.setCursor(0, 0);
    display.setTextSize(1);
    display.print(&quot;Recibido:&quot;);
    display.setCursor(0, 20);
    display.setTextSize(2);
    display.print(message);
    display.display();
    lastReceived = millis();
  }
  // Si pasan más de 5 segundos sin recibir nada
  if (millis() - lastReceived &gt; 5000) {
    display.clearDisplay();
    display.setCursor(0, 0);
    display.setTextSize(1);
    display.println(&quot;ERROR:&quot;);
    display.setCursor(0, 20);
    display.setTextSize(2);
    display.println(&quot;SIN ENLACE&quot;);
    display.display();
  }
}