#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoJson.h>
#include <Arduino.h>

// Store SSID and password of your preferred network
const char *SSID = "Digisol";
const char *PWD = "1234567890";

// Store value from hall effect sensor
float hall_val = 0;

WebServer server(80);

StaticJsonDocument<250> jsonDocument;
char buffer[250];

void connectToWifi() {
  Serial.print("Connecting to");
  Serial.println(SSID);
  WiFi.begin(SSID,PWD);

  while(WiFi.status() != WL_CONNECTED){
   Serial.print(".");
   delay(500); 
  }
  
  Serial.print("Connected. IP: ");
  Serial.println(WiFi.localIP());
}

void setup_routing(){
  server.on("/hall",getHallVal);

  // Start server
  server.begin();
}

void getHallVal() {
  Serial.println("Get hall value");
  create_json("magnetic field",hall_val,"T");
  server.send(200,"application/json",buffer);
}

void create_json(String tag,float value,String unit){
  jsonDocument.clear();
  jsonDocument["type"] = tag;
  jsonDocument["value"] = value;
  jsonDocument["unit"] = unit;
  serializeJson(jsonDocument,buffer);
}

void read_sensor_data(void * parameter) {
  for(;;) {
    hall_val = hallRead();

    vTaskDelay(6000 / portTICK_PERIOD_MS);
  }
}

void setup_task() {
  xTaskCreate(read_sensor_data,"Read sensor data",1000,NULL,1,NULL);
}

void setup(){
  Serial.begin(115200);

  connectToWifi();
  setup_task();
  setup_routing();
}

void loop() {
  server.handleClient();
}
