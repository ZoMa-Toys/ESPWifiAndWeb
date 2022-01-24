# ESPWifiAndWeb

This is just a small lib for ESP32 and ESP8266. It can create wifi/webserial/websocketClient/OTA  connections.

platformio.ini build_flags:
```
build_flags = 
	-D STASSID=\"WIFI SSID\"
	-D STAPSK=\"WIFI PW\"
	-D THISMDNS=\"LOCAL DNS NAME OF ESP\"
	-D WSPATH=\"WEBSOCKET PATH\"
	-D WSHOST=\"WEBSOCKET HOST\"
	-D WSPORT=WEBSOCKETPORT
	-D DBG=\"DEBUG MODE (Serial/webserial/webosket/none\"
```
Webserial and websocekt callbacks:
```
//Websocket:
void onDataReceived(String msg){
  StaticJsonDocument<2048>  messageJSON;
  DeserializationError error = deserializeJson(messageJSON, msg);
  if (error) {
      debugPrint("deserializeJson() failed: ");
      debugPrint(error.f_str());
  return;
  }
  // do something with the data
}

//Webserial
void recvMsg(uint8_t *data, size_t len){
  WebSerial.println("Received Data...");
  String d = "";
  for(int i=0; i < len; i++){
    d += char(data[i]);
  }
  // do something with d
}

```

main.cpp part:
```
void setup() {
  Serial.begin(115200);
  connectWifi();
  connectWS(onDataReceived);
  createWebSerial(recvMsg);
  createOTA();
}

void loop() {
  ArduinoOTA.handle();
  if (WiFi.status() != WL_CONNECTED){
    ESP.restart();
  }
  if(client.available()) {
    client.poll();
  }
  else{
    client.connect(websockets_server_host, websockets_server_port, websockets_server_path);
  }
```

platformio.ini lib_deps:
ESP8266:
```
lib_deps = 
	bblanchon/ArduinoJson@^6.18.5
	gilmaimon/ArduinoWebsockets@^0.5.3
	ayushsharma82/WebSerial@^1.3.0
	me-no-dev/ESPAsyncTCP@^1.2.2
	me-no-dev/ESP Async WebServer@^1.2.3
```
ESP32:
```
lib_deps = 
	bblanchon/ArduinoJson@^6.18.5
	gilmaimon/ArduinoWebsockets@^0.5.3
	ayushsharma82/WebSerial@^1.3.0
	me-no-dev/AsyncTCP@^1.1.1
	me-no-dev/ESP Async WebServer@^1.2.3
```

## Adding to project as submodule:
execute the following command
```
git submodule add https://github.com/GuBee33/ESPWifiAndWeb.git lib/WifiAndWeb
```


