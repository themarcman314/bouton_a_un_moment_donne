#include "LittleFS.h"
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>

#ifndef APSSID
#define APSSID "fbi_van"
#define APPSK "password"
#endif

/* Set these to your desired credentials. */
const char *ssid = APSSID;
const char *password = APPSK;

ESP8266WebServer server(80);

/* Go to http://192.168.4.1 in a web browser */
void handleRoot() {
  server.send(200, "text/html", "\
  <h1>Coucou</h1>\
  <a href=\"/toggle\"><button>Toggle LED</button></a>\
  </button>");
}

void handleled()
{
  if(digitalRead(LED_BUILTIN) == HIGH)
    digitalWrite(LED_BUILTIN, LOW);
  else digitalWrite(LED_BUILTIN, HIGH);

  server.send(200, "text/html", "\
  <h1>Hello this is the toogle led page</h1>\
  <a href=\"/toggle\"><button>Toggle LED</button></a>\
  </button>");
}



void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  Serial.begin(115200);

  SPIFFSConfig cfg;
  cfg.setAutoFormat(false);
  SPIFFS.setConfig(cfg);
  if(LittleFS.begin() == true)
      Serial.print("\r\nFS mounted\r\n");
  else Serial.print("Problem mounting FS!\r\n");

  if(LittleFS.exists("/secret_file.txt") == true)
    Serial.println("File exists");
  else Serial.println("File does not exist");

  File f = LittleFS.open("/secret_file.txt", "r");
  if (!f) {
    Serial.println("file open failed");
  }
  else Serial.println("File opened");

  Serial.println("File Content:");
  while(f.available()){
    Serial.write(f.read());
  }
  f.close();
  
  LittleFS.end();

  Serial.print("\r\nConfiguring access point...\r\n");
  /* You can remove the password parameter if you want the AP to be open. */
  WiFi.softAP(ssid/*, password*/);

  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
  server.on("/", handleRoot);
  server.on("/toggle", handleled);
  server.begin();
  Serial.println("HTTP server started");

}

void loop() {
  server.handleClient();
}
