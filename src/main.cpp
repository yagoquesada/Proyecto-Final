/*
- Proyecto Final Procesadores Digitales
- Servidor Web Esp32 con lector de tarjetas MicroSD y pantalla OLED 
- Autores: Marc Sánchez Masiques, Yago Quesada Floriach y Arnau Bosque Gomez
*/

// Librerías
#include <Arduino.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "FS.h"
#include "SD.h"
#include "SPI.h" 

#define SCREEN_WIDTH 128  // ancho de la pantalla OLED  (en pixels)
#define SCREEN_HEIGHT 32  // alto de la pantall OLED    (en pixels)   
#define OLED_RESET -1     // pin de reinicio

const char* ssid = "MIWIFI_2G_2jcE";  // SSID de la red WIFI
const char* password = "9GLX7t3u";    // contraseña de la red WIFI

// Inicializar objeto display con el ancho y alto definidos anteriormente con el protocolo de comunicación I2C (&Wire)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Crear objeto AsyncWebServer en el puerto 80
AsyncWebServer server(80);

// Esta función inicializa la tarjeta microSD en los pines SPI predeterminados
void initSDCard(){
  
  // Mostrar por el terminal si hay una tarjeta SD conectada, y en ese caso el tipo de SD y el tamaño de esta

  if(!SD.begin()){
    Serial.println("Falló en el montaje de la tarjeta");
    return;
  }

  uint8_t cardType = SD.cardType();

  // Si no hay ninguna tarjeta SD conectada 
  if(cardType == CARD_NONE){
    Serial.println("No hay ninguna tarjeta SD conectada!!");
    return;
  }

  Serial.print("Tipo de tarjeta SD: ");
  if(cardType == CARD_MMC){
    Serial.println("MMC");          // Si hay una tarjeta tipo MMC
  } else if(cardType == CARD_SD){
    Serial.println("SDSC");         // Si hay una tarjeta tipo SDSC
  } else if(cardType == CARD_SDHC){
    Serial.println("SDHC");         // Si hay una tarjeta tipo SDHC
  } else {
    Serial.println("Desconocida");  // Si hay una tarjeta desconocida
  }

  // Mostrar tamaño de la tarjeta
  uint64_t cardSize = SD.cardSize() / (1024 * 1024);
  Serial.printf("Tamaño de la tarjeta SD: %lluMB\n", cardSize);

  // Mostrar por la pantalla OLED si hay una tarjeta SD conectada, y en ese caso el tipo de SD y el tamaño de esta

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    Serial.println(F("SSD1306 falló en la asignación"));
    for(;;);
  }

  delay(2000);
  display.clearDisplay();

  display.setTextSize(1);       // Establecer el tamaño del texto
  display.setTextColor(WHITE);  // Establecer el color del texto
  display.setCursor(0, 10);     // Establecer posición inicial del texto

  if(!SD.begin()){
    display.println("Falló en el montaje de la tarjeta");
    return;
  }

  // Si no hay ninguna tarjeta SD conectada
  if(cardType == CARD_NONE){
    display.println("No hay ninguna tarjeta SD conectada!!");
    return;
  }

  display.print("Tipo SD: ");
  if(cardType == CARD_MMC){
    display.println("MMC");         // Si hay una tarjeta tipo MMC
  } else if(cardType == CARD_SD){
    display.println("SDSC");        // Si hay una tarjeta tipo SDSC
  } else if(cardType == CARD_SDHC){
    display.println("SDHC");        // Si hay una tarjeta tipo SDHC
  } else {
    display.println("Desconocida"); // Si hay una tarjeta desconocida
  }

  // Mostrar tamaño de la tarjeta
  display.printf("Tamanyo SD: %lluMB\n", cardSize); 

  display.display();
}

// Esta función conecta el módulo ES32 con el WIFI 
void initWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Conectando al WIFI ..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }
  Serial.println(WiFi.localIP());
}


void setup() {
  Serial.begin(9600); 

  initWiFi();
  initSDCard();

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SD, "/index.html", "text/html");
  });

  server.serveStatic("/", SD, "/");

  server.begin();

}

void loop() {
  
}



