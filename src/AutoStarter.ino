#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <cstring>

#include "funcs.h"
#include "page.h"

const char* ssid = "AP";
const char* password = "$-10family01-$";
const char* host = "autostarter";
const long utcOffsetInSeconds = 5 * 3600 + 30 * 60; // +5:30 offset in seconds
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);
ESP8266WebServer server(80);
bool updateDone = false;

void handleRoot() {
  server.setContentLength(strlen(webpageCode)+1);
  server.send(200, "text/html", webpageCode);
}

void handleData() {
  String data = readFile();
  server.setContentLength(data.length());
  server.send(200, "application/json", readFile());
}

void handleReset() {
  writeFile(load());
  server.send(200, "text/plain", "OK");
}


void updateScheduleStatus() {
  if (server.arg("status") != "") {
    Serial.println(server.arg("status"));
    updateStatus(server.arg("status"));
    updateDone = true;
  } else if (server.arg("op") != "") {
    updateSchedule(server.arg("op"), server.arg("start"), server.arg("end"));
    updateDone = true;
  }
}

void setupWiFi() {
  WiFi.begin(ssid, password);
  Serial.print("Connecting to ");
  Serial.println(ssid);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void setup() {
  Serial.begin(115200);
  setupWiFi();

  server.on("/", handleRoot);
  server.on("/data", handleData);
  server.on("/reset", handleReset);

  LittleFS.begin();
  if (!LittleFS.exists(fname)) {
    Serial.println("File Not Found");
  }
  server.begin();
  Serial.println("HTTP server started");
  MDNS.begin(host);
  Serial.println("mDNS responder started. You can access the server at http://" + String(host) + ".local/");

  timeClient.begin();
  timeClient.update(); // Force an immediate time update
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
}

void loop() {
  server.handleClient();
  MDNS.update();
  
  timeClient.update();
  String formattedTime = formatTime(timeClient);
  checkScheduleAndControlLED(formattedTime);

  if (!updateDone) {
    updateScheduleStatus();
  }else{
  // Reset the flag to false at the end of the loop
  updateDone = false;
  }
}
