#ifndef WiFIAndWeb_h
#define WiFIAndWeb_h


#if defined (ARDUINO_ARCH_ESP8266)
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ESPAsyncTCP.h>
#elif defined (ESP32)
#include <WiFi.h>
#include <ESPmDNS.h>
#include <AsyncTCP.h>
#else
#error Architecture unrecognized by this code.
#endif
#include <ArduinoWebsockets.h>
#include <ArduinoJson.h>
#include <WebSerial.h>
#include <ESPAsyncWebServer.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#ifndef STASSID
#define STASSID "SOMEWIFI"
#endif
#ifndef STAPSK
#define STAPSK  "SecretPW"
#endif

#ifndef WSHOST
#define WSHOST "RANDOMHOST"
#endif

#ifndef WSPORT
#define WSPORT 80
#endif

#ifndef DBG
#define DBG ""
#endif

#ifndef WSPATH
#define WSPATH "/ws"
#endif

#ifndef THISMDNS
#define THISMDNS "ESP"
#endif



using namespace websockets;
const char* ssid = STASSID; //Enter SSID
const char* password = STAPSK; //Enter Password
const char* websockets_server_host = WSHOST; //Enter server adress
const uint16_t websockets_server_port = WSPORT; // Enter server port
const char* websockets_server_path = WSPATH; //Enter server adress
String debug = DBG;
StaticJsonDocument<2048>  messageJSONToSend;


WebsocketsClient client;
AsyncWebServer server(80);

void debugPrint(String toprint){
    if (debug == "Serial"){
        Serial.println(toprint);
    }
    else if (debug == "webserial"){
        WebSerial.println(toprint);
    }
    else if (debug == "websocket"){
        StaticJsonDocument<2048> tp;
        tp["DEBUG"]=toprint;
        String message;
        serializeJson(tp,message);
        client.send(message);
    }
}

void sendJSON(){
    String toSend = "";
    serializeJson(messageJSONToSend,toSend);
    messageJSONToSend.clear();
    debugPrint("toSend: " + toSend);
    client.send(toSend);
    toSend = "";

}



void connectWifi(){
    WiFi.begin(ssid, password);

    // Wait some time to connect to wifi
    Serial.println("connecting to WiFi");
    while (WiFi.waitForConnectResult() != WL_CONNECTED) {
      Serial.println("Connection Failed! Rebooting...");
      delay(5000);
      ESP.restart();
    }
    Serial.print("Connected to ");
    Serial.println(ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    if (!MDNS.begin(THISMDNS)) {
        Serial.println("Error setting up MDNS responder!");
        while(1) {
            delay(1000);
        }
    }
    Serial.print("mDNS responder started:");
    Serial.println(THISMDNS);
}

void connectWS(){
  Serial.println("Connecting to server.");
  // try to connect to Websockets server
  bool connected = client.connect(websockets_server_host, websockets_server_port, websockets_server_path);
  if(connected) {
    Serial.print("Connected to ");
    Serial.println(websockets_server_host);
    client.send(WiFi.localIP().toString() + " connected");
  } else {
    Serial.println("Not Connected!");
  }
}



void createWebSerial(void (*recvMsg)(uint8_t *data, size_t len)){
  WebSerial.begin(&server);
  WebSerial.msgCallback(recvMsg);
  server.begin();
}

void createOTA(){
  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else { // U_FS
      type = "filesystem";
    }

    // NOTE: if updating FS this would be the place to unmount FS using FS.end()
    WebSerial.println("Start updating " + type);
  });
  ArduinoOTA.onEnd([]() {
    WebSerial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    WebSerial.println("Error: " + error);
    if (error == OTA_AUTH_ERROR) {
      WebSerial.println("Auth Failed");
    } else if (error == OTA_BEGIN_ERROR) {
      WebSerial.println("Begin Failed");
    } else if (error == OTA_CONNECT_ERROR) {
      WebSerial.println("Connect Failed");
    } else if (error == OTA_RECEIVE_ERROR) {
      WebSerial.println("Receive Failed");
    } else if (error == OTA_END_ERROR) {
      WebSerial.println("End Failed");
    }
  });
  ArduinoOTA.begin();
}




#endif 
