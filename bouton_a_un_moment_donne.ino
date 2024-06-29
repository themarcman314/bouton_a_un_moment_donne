#include "LittleFS.h"
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <DNSServer.h>


#ifndef APSSID
#define APSSID "fbi_van"
#define APPSK "password"
#endif

const byte DNS_PORT = 53;
DNSServer dnsServer;

String responseHTML = ""
  "<!DOCTYPE html><html><head><title>CaptivePortal</title></head><body>"
  "<h1>Hello World!</h1><p>This is a captive portal example. All requests will "
  "be redirected here.</p></body></html>";


/* Set these to your desired credentials. */
const char *ssid = APSSID;
const char *password = APPSK;

ESP8266WebServer server(80);

/* Go to http://192.168.4.1 in a web browser */
void handleRoot() {
  server.send(200, "text/html", "\
  <h1>Hello</h1>\
  <a href=\"/toggle\"><button>Toggle LED</button></a>\
  </button>");
}

void handleled() {
  if(digitalRead(LED_BUILTIN) == HIGH)
    digitalWrite(LED_BUILTIN, LOW);
  else digitalWrite(LED_BUILTIN, HIGH);

  server.send(200, "text/html", "\
  <h1>Hello this is the toogle led page</h1>\
  <a href=\"/toggle\"><button>Toggle LED</button></a>\
  </button>");
}

void handleCaptivePortal() {
  String header = "HTTP/1.1 302 Found\r\nLocation: http://";
  header += WiFi.softAPIP().toString();
  header += "/\r\n\r\n";
  server.sendContent(header);
  server.client().stop(); // End the current connection

  // Change the state of the LED
  digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN)); // Toggle LED state
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


  IPAddress local_IP(192,168,1,1);
  IPAddress gateway(192,168,1,1);
  IPAddress subnet(255,255,255,0);
  WiFi.softAPConfig(local_IP, gateway, subnet);
  
  /* You can remove the password parameter if you want the AP to be open. */
 
  WiFi.softAP(ssid/*, password*/);
  Serial.print("\r\nConfiguring access point...\r\n");

  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);


  server.on("/", handleRoot);
  server.on("/toggle", handleled);
  server.onNotFound(handleRoot);  // Redirect all other URLs to the root handler

  // // Common captive portal detection URLs
  // server.on("/generate_204", handleCaptivePortal); // Android
  // server.on("/fwlink", handleCaptivePortal); // Windows
  // server.on("/hotspot-detect.html", handleCaptivePortal); // Apple
  // server.onNotFound(handleCaptivePortal);

  String responseHTML = ""
  "<!DOCTYPE html><html><head><title>CaptivePortal</title></head><body>"
  "<h1>Hello World!</h1><p>This is a captive portal example. All requests will "
  "be redirected here.</p></body></html>";
  

  dnsServer.start(DNS_PORT, "*", local_IP);

  server.begin();
  Serial.println("HTTP server started");

}

void loop() {
  dnsServer.processNextRequest();
  server.handleClient();
}
