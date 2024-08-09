// #include "LittleFS.h"
// #define HTTP_UPLOAD_BUFLEN 4096

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

void Playsong(void);


static const char TEXT_PLAIN[] PROGMEM = "text/plain";
File uploadFile;

AudioGeneratorMP3 *mp3;
AudioFileSourceSPIFFS *file;
AudioOutputI2S *out;


String selected_file;

const byte DNS_PORT = 53;
DNSServer dnsServer;

/* Set these to your desired credentials. */
const char *ssid = APSSID;
const char *password = APPSK;

ESP8266WebServer server(80);

void handleRoot() {

  FSInfo fs_info;
  SPIFFS.info(fs_info);

  // Calculate available space
  size_t totalBytes = fs_info.totalBytes;
  size_t usedBytes = fs_info.usedBytes;
  size_t freeKBytes = (totalBytes - usedBytes)*0.001;

  String main_page = index_html;

  // find available mp3 files
  Dir dir = SPIFFS.openDir("/");
  while (dir.next()) {
    String file_name = dir.fileName();
    if(file_name.endsWith(".mp3"))
      main_page += "<option value=\"" + file_name + "\">" + file_name + "</option>";
  }
  main_page += "</select><input type=\"submit\" value=\"Submit\" class=\"buttons\"></form>";
  main_page += "<p>Available space : ";
  main_page += String(freeKBytes);
  main_page += " KBytes</p>";

  main_page += R"=====(
  <h1>Delete</h1>
  <form id="del" action="/delete" method="post">
      <label for="fileDelete">Choose a file to delete :</label>
          <select name="file" id="fileDelete">
  )=====";

  // find available files
  dir = SPIFFS.openDir("/");
  while (dir.next()) {
    String file_name = dir.fileName();
    main_page += "<option value=\"" + file_name + "\">" + file_name + "</option>";
  }
  main_page += "</select><input type=\"submit\" value=\"Submit\" class=\"buttons\"></form>";
  main_page += scripts;
  main_page += "</body></html>";


  server.send(200, "text/html", main_page);
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
  HTTPUpload& upload = server.upload();

  // Serial.println("Upload status :" + String(upload.status));
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
    // Serial.println("Writing");
    size_t bytesWritten = uploadFile.write(upload.buf, upload.currentSize);
    // Serial.println("Bytes written :" + String(bytesWritten));
    if (bytesWritten != upload.currentSize) { 
      Serial.println("Something went wrong");
      return replyServerError(F("WRITE FAILED")); }
  }
  else if (upload.status == UPLOAD_FILE_END) {
    // Serial.println("File Content:");
    // uploadFile.seek(0,SeekSet);
    // while(uploadFile.available()){
    //   Serial.write(uploadFile.read());
    // }
    uploadFile.close();
    Serial.println(String("Upload: END, Size: ") + upload.totalSize);
    handleRoot();
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
    server.streamFile(file, "foyer.jpg");
    file.close();
  } 
  else {server.send(404, "text/plain", "File not found");}
}

void handleMusicSelection() {  
  selected_file = server.arg("file");
  Serial.println("Selected file : " + selected_file);
  handleRoot();
}

void handleMusicDeletion() {
  String selected_file = server.arg("file");
  Serial.println("Selected file : " + selected_file);

  const char *file_name = selected_file.c_str();

  if(SPIFFS.exists(file_name)) {
    Serial.println(selected_file + "File exists");
    SPIFFS.remove(file_name);
  }
  else Serial.println("File does not exist");
  handleRoot();
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

  if(SPIFFS.exists("/foyer.jpg") == true)
    Serial.println("File exists");
  else Serial.println("File does not exist");

  File f = SPIFFS.open("/foyer.jpg", "r");
  if (!f) {
    Serial.println("file open failed");
  }
  else Serial.println("File opened");

  // Serial.println("File Content:");
  // while(f.available()){
  //   Serial.write(f.read());
  // }

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
 
  server.on("/style.css", [] {
    server.send(200, "text/css", css_file);
  });
  server.on("/foyer.jpg", HTTP_GET, handleImageRequest);

  server.onNotFound(handleRedirect);  // Redirect all other URLs to the root handler
  
  server.on("/edit", HTTP_POST, replyOK, handleFileUpload);
  server.on("/select", handleMusicSelection);
  server.on("/redirect", handleRedirect);
  server.on("/delete", handleMusicDeletion);


  server.on("/list", HTTP_GET, handleFileList);

  dnsServer.start(DNS_PORT, "*", local_IP);

  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  dnsServer.processNextRequest();
  server.handleClient();
  if(analogRead(D7) == 0)
  {
    Playsong();
    delay(1000);
  }
}


void handleRedirect() {
  server.send(200, "text/html", redirect_page);
}

void Playsong(void)
{
  const char *file_name = selected_file.c_str();
  Serial.print(file_name);
  Serial.println();

  if(SPIFFS.exists(file_name)) {
    Serial.println(selected_file + "File exists");
  }
  else {Serial.println(selected_file + "File does not exist :(");}

  file = new AudioFileSourceSPIFFS(file_name);
  out = new AudioOutputI2S();
  // out->SetGain(analogRead(A0)/250.0); // scale 1023 (10 bit adc) down to 4
  mp3 = new AudioGeneratorMP3();
  mp3->begin(file, out);
  
  while(1) {
    if (mp3->isRunning()) {
      if (!mp3->loop()) mp3->stop(); 
    }
    else {
      break;
    }
  }
}
