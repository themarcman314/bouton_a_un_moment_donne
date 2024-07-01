#include "LittleFS.h"
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <DNSServer.h>


#ifndef APSSID
#define APSSID "fbi_van"
#define APPSK "password"
#endif


static const char TEXT_PLAIN[] PROGMEM = "text/plain";
File uploadFile;

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

void replyOK() {
  server.send(200, FPSTR(TEXT_PLAIN), "");
}
void replyServerError(String msg) {
  Serial.println(msg);
  server.send(500, FPSTR(TEXT_PLAIN), msg + "\r\n");
}


void handleFileUpload() {
  HTTPUpload& upload = server.upload();
  if (upload.status == UPLOAD_FILE_START) {
    String filename = upload.filename;
    Serial.println("Upload starting, filename : " + String(filename));
  }
  else if (upload.status == UPLOAD_FILE_WRITE) {
    if (uploadFile) {
      size_t bytesWritten = uploadFile.write(upload.buf, upload.currentSize);
      if (bytesWritten != upload.currentSize) { return replyServerError(F("WRITE FAILED")); }
    }
    else if (upload.status == UPLOAD_FILE_END) {
    if (uploadFile) { uploadFile.close(); }
    Serial.println(String("Upload: END, Size: ") + upload.totalSize);
    }
  }

  // if (!fsOK) { return replyServerError(FPSTR(FS_INIT_ERROR)); }
  // if (server.uri() != "/edit") { return; }
  // HTTPUpload& upload = server.upload();
  // if (upload.status == UPLOAD_FILE_START) {
  //   String filename = upload.filename;
  //   // Make sure paths always start with "/"
  //   if (!filename.startsWith("/")) { filename = "/" + filename; }
  //   DBG_OUTPUT_PORT.println(String("handleFileUpload Name: ") + filename);
  //   uploadFile = fileSystem->open(filename, "w");
  //   if (!uploadFile) { return replyServerError(F("CREATE FAILED")); }
  //   DBG_OUTPUT_PORT.println(String("Upload: START, filename: ") + filename);
  // } else if (upload.status == UPLOAD_FILE_WRITE) {
  //   if (uploadFile) {
  //     size_t bytesWritten = uploadFile.write(upload.buf, upload.currentSize);
  //     if (bytesWritten != upload.currentSize) { return replyServerError(F("WRITE FAILED")); }
  //   }
  //   DBG_OUTPUT_PORT.println(String("Upload: WRITE, Bytes: ") + upload.currentSize);
  // } else if (upload.status == UPLOAD_FILE_END) {
  //   if (uploadFile) { uploadFile.close(); }
  //   DBG_OUTPUT_PORT.println(String("Upload: END, Size: ") + upload.totalSize);
  // }
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

  
  server.on("/edit", HTTP_POST, replyOK, handleFileUpload);
  // server.on("/edit", handleFileUpload);

  dnsServer.start(DNS_PORT, "*", local_IP);

  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  dnsServer.processNextRequest();
  server.handleClient();
}
