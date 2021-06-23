# Proyecto Final: Servidor Web Esp32 con lector de tarjetas MicroSD y pantalla OLED

## Código:

```ruby
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
```

## Procedimiento

Empezamos estableciendo las diferentes librerías que utilizaremos en el código: 

- Tarjeta microSD
- WIFI
- Pantalla OLED
- Servidor Web

```ruby
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
```

Aquí definimos el tamaño que ocupa nuestra pantalla OLED con el *Width* y el *Height*, también establecemos el pin de reseteo.
También añadimos tanto el nombre del WIFI como su contraseña para que la conexión pueda suceder.
Finalmente creamos la función pantalla, donde añadimos las características de la misma.
También creamos el *WebServer* en el puerto 80.

```ruby
#define SCREEN_WIDTH 128  
#define SCREEN_HEIGHT 32  
#define OLED_RESET -1     

const char* ssid = "MIWIFI_2G_2jcE";  
const char* password = "9GLX7t3u";    

protocolo de comunicación I2C (&Wire)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

AsyncWebServer server(80);
```

En esta otra función se encuentra todo lo relacionado con la tarjeta SD y su lectura. Comenzamos realizando comprobaciones de detección de la tarjeta, si no se detecta saltan diferentes mensajes para advertir de ello y se detiene el programa, si se detecta tarjeta se pasa a la siguiente fase.
En la siguiente fase detectamos el tipo de tarjeta que es la microSD.
Continuamos comprobando el tamaño que tiene la tarjeta de espacio disponible para mostrarlo por pantalla.
Por último utilizamos todos los datos de la pantalla OLED para mostrar por pantalla los valores de que tipo de tarjeta es y su espacio disponible, para que sea todo visible. Además de la adición de estas variables para mostrarlas por pantalla, también encontramos una definición del tamaño de texto, el color y la posición en la que se encontrara todo.

```ruby
void initSDCard(){

  if(!SD.begin()){
    Serial.println("Falló en el montaje de la tarjeta");
    return;
  }

  uint8_t cardType = SD.cardType();

  if(cardType == CARD_NONE){
    Serial.println("No hay ninguna tarjeta SD conectada!!");
    return;
  }

  Serial.print("Tipo de tarjeta SD: ");
  if(cardType == CARD_MMC){
    Serial.println("MMC");
  } else if(cardType == CARD_SD){
    Serial.println("SDSC");
  } else if(cardType == CARD_SDHC){
    Serial.println("SDHC");
  } else {
    Serial.println("Desconocida");
  }

  uint64_t cardSize = SD.cardSize() / (1024 * 1024);
  Serial.printf("Tamaño de la tarjeta SD: %lluMB\n", cardSize);

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    Serial.println(F("SSD1306 falló en la asignación"));
    for(;;);
  }

  delay(2000);
  display.clearDisplay();

  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 10);

  if(!SD.begin()){
    display.println("Falló en el montaje de la tarjeta");
    return;
  }

  if(cardType == CARD_NONE){
    display.println("No hay ninguna tarjeta SD conectada!!");
    return;
  }

  display.print("Tipo SD: ");
  if(cardType == CARD_MMC){
    display.println("MMC");
  } else if(cardType == CARD_SD){
    display.println("SDSC");
  } else if(cardType == CARD_SDHC){
    display.println("SDHC");
  } else {
    display.println("Desconocida");
  }

  display.printf("Tamanyo SD: %lluMB\n", cardSize); 

  display.display();
}
```

Aquí creamos la función para que el apartado WIFI funcione correctamente. En este iniciamos la conexión con el rúter, utilizando tanto el nombre del mismo como la contraseña y, seguidamente, intentamos establecer conexión.

```ruby
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
```

Pasadas todas las funciones nos encontramos con el `void setup()`, en este mismo llamamos a todas las funciones para su uso. Llamamos a la función Wifi para poder conectarnos al mismo y también llamamos a la función `initSDCard()` para poder conectar también la tarjeta microSD. Por último leemos de dentro de la tarjeta SD los archivos index.html y text/html, estos archivos irán incluidos dentro de la tarjeta microSD y los leeremos para así crear la página html donde introduciremos el texto que queramos.
Finalmente iniciamos el *server*.

```ruby
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
```

Por último tenemos el `void loop()`, en el cual no desarrollamos nada, debido a que sería innecesario

```ruby
void loop() {
  
}
```

## Fotos del montaje

![foto1](https://i.ibb.co/Wz9qP1W/Whats-App-Image-2021-06-23-at-14-42-09.jpg)
![foto2](https://i.ibb.co/KjcB0L1/Whats-App-Image-2021-06-23-at-14-42-09-2.jpg)
![foto3](https://i.ibb.co/Z1bB7X2/Whats-App-Image-2021-06-23-at-14-42-09-1.jpg)
![foto4](https://i.ibb.co/xqFNwHS/Whats-App-Image-2021-06-23-at-14-42-09-3.jpg)