// #include "LittleFS.h"
#include <Arduino.h>
#include "AudioFileSourceSPIFFS.h"
#include "AudioGeneratorMP3.h"
#include "AudioOutputI2SNoDAC.h"


#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <DNSServer.h>
#include "pages.h"
#include "style.h"

// set max space for flash
// FLASH_MAP_SETUP_CONFIG(FLASH_MAP_MAX_FS)


#ifndef APSSID
#define APSSID "fbi_van"
#define APPSK "password"
#endif

static const char TEXT_PLAIN[] PROGMEM = "text/plain";
File uploadFile;

AudioGeneratorMP3 *mp3;
AudioFileSourceSPIFFS *file;
AudioOutputI2S *out;

const byte DNS_PORT = 53;
DNSServer dnsServer;

/* Set these to your desired credentials. */
const char *ssid = APSSID;
const char *password = APPSK;

ESP8266WebServer server(80);

void handleRoot() {
  server.send(200, "text/html", index_html);
}

void handleLed() {
  digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
  handleRoot();
}

void handleCaptivePortal() {
  String header = "HTTP/1.1 302 Found\r\nLocation: http://";
  header += WiFi.softAPIP().toString();
  header += "/\r\n\r\n";
  server.sendContent(header);
  server.client().stop(); // End the current connection

  digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN)); // Toggle LED state
}

void replyOK() {
  server.send(200, FPSTR(TEXT_PLAIN), "");
}
void replyServerError(String msg) {
  Serial.println(msg);
  server.send(500, FPSTR(TEXT_PLAIN), msg + "\r\n");
}

void replyBadRequest(String msg) {
  Serial.println(msg);
  server.send(400, FPSTR(TEXT_PLAIN), msg + "\r\n");
}

void handleFileUpload() {
  handleRoot();
  HTTPUpload& upload = server.upload();

  Serial.println("Upload status :" + String(upload.status));
  if (upload.status == UPLOAD_FILE_START) {
    String filename = upload.filename;
    if (!filename.startsWith("/")) { filename = "/" + filename; }
    Serial.println("Preparing to upload : " + filename);
    uploadFile = SPIFFS.open(filename, "w+");
    if (!uploadFile) {Serial.println("Create failed");}
    else{
      Serial.println("File opened");
      Serial.println("Upload starting, filename : " + String(filename));
    }
  }
  else if (upload.status == UPLOAD_FILE_WRITE) {
    Serial.println("Writing");
    size_t bytesWritten = uploadFile.write(upload.buf, upload.currentSize);
    Serial.println("Bytes written :" + String(bytesWritten));
    if (bytesWritten != upload.currentSize) { 
      Serial.println("Something went wrong");
      return replyServerError(F("WRITE FAILED")); }
    }
  else if (upload.status == UPLOAD_FILE_END) {
    Serial.println("File Content:");
    uploadFile.seek(0,SeekSet);
    while(uploadFile.available()){
      Serial.write(uploadFile.read());
    }
    uploadFile.close();
    Serial.println(String("Upload: END, Size: ") + upload.totalSize);
  }
}

void handleFileList() {
  Dir dir = SPIFFS.openDir("/");

  String file_info = "<html><body>";
  file_info += "<h1>Files</h1>";
  file_info += "<table border='1'>";
  file_info += "<tr><th>File Name</th><th>Size (bytes)</th></tr>";

  while (dir.next()) {
    String file_name = dir.fileName();
    size_t file_size = dir.fileSize();
    file_info += "<tr><td>" + file_name + "</td><td>" + String(file_size) + "</td></tr>";
  }
  file_info += "</table>";
  file_info += "</body></html>";

  server.send(200, "text/html", file_info);
}

void handleImageRequest() {
  if (SPIFFS.exists("/foyer.jpg")) {
    File file = SPIFFS.open("/foyer.jpg", "r");
    server.streamFile(file, "foyer/jpg");
    file.close();
  } 
  else {server.send(404, "text/plain", "File not found");}
}

void handleMusicSelection() {
  handleRoot();
  Serial.println("In audio cb");
  
  if(SPIFFS.exists("/jamonit.mp3")) {
    Serial.println("File exists");
  }
  else {Serial.println("File does not exist :(");}
  file = new AudioFileSourceSPIFFS("/jamonit.mp3");
  out = new AudioOutputI2S();
  mp3 = new AudioGeneratorMP3();
  mp3->begin(file, out);
  
  while(1) {
    if (mp3->isRunning()) {
      if (!mp3->loop()) mp3->stop(); 
    }
  }
}

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  Serial.begin(115200);
  delay(5000);  // allow serial port to be opened

  SPIFFSConfig cfg;
  cfg.setAutoFormat(false);
  SPIFFS.setConfig(cfg);
  if(SPIFFS.begin() == true)
      Serial.print("\r\n\nFS mounted\r\n");
  else Serial.print("\r\n\nProblem mounting FS!\r\n");

  if(SPIFFS.exists("/secret_file.txt") == true)
    Serial.println("File exists");
  else Serial.println("File does not exist");

  File f = SPIFFS.open("/secret_file.txt", "r");
  if (!f) {
    Serial.println("file open failed");
  }
  else Serial.println("File opened");

  Serial.println("File Content:");
  while(f.available()){
    Serial.write(f.read());
  }

  IPAddress local_IP(192,168,1,1);
  IPAddress gateway(192,168,1,1);
  IPAddress subnet(255,255,255,0);
  WiFi.softAPConfig(local_IP, gateway, subnet);
   
  WiFi.softAP(ssid/*, password*/);
  Serial.print("\r\nConfiguring access point...\r\n");

  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);

  server.on("/", handleRoot);
  server.on("/toggle", handleLed);
  server.on("/select", handleMusicSelection);
  server.on("/style.css", [] {
    server.send(200, "text/css", css_file);
  });
  server.on("/foyer.jpg", HTTP_GET, handleImageRequest);


  server.onNotFound(handleRoot);  // Redirect all other URLs to the root handler
  
  server.on("/edit", HTTP_POST, replyOK, handleFileUpload);
  server.on("/list", HTTP_GET, handleFileList);

  dnsServer.start(DNS_PORT, "*", local_IP);

  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  dnsServer.processNextRequest();
  server.handleClient();
}
