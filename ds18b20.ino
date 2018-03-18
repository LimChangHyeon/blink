#include <ESP8266WiFi.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <ArduinoJson.h>


// PROGRAM_TYPE 0 => thingspeak
// PROGRAM_TYPE 1 => ifttt
#define PROGRAM_TYPE 1

#if PROGRAM_TYPE == 0
#define LOOP_DELAY (60 * 1000)
#elif PROGRAM_TYPE == 1
#define LOOP_DELAY (5 * 1000)
#endif

#define ONE_WIRE_BUS 0

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature tpSensor(&oneWire);

#if PROGRAM_TYPE == 0
String apiKey = "6BAGZ5AM53XR2VL4";
const char *server = "api.thingspeak.com";
const int _PORT = 80;
#elif PROGRAM_TYPE == 1
const char *event = "hot_temp";
const char *apiKey = "dV6vhAY4RezWR7LzHkNXH9";
const char *server = "maker.ifttt.com";
const int _PORT = 80;
#endif

const char *SSID = "spslab1";
const char *PWD ="sps123456";

void setup()
{
  Serial.begin(115200);
  Serial.println("\r\n");
  Serial.println("program type : " + PROGRAM_TYPE);
  Serial.print("server : ");
  Serial.println(server);
  connectWifi();
}

void loop()
{
  float temp;
  tpSensor.requestTemperatures();
  temp = tpSensor.getTempCByIndex(0);
  Serial.print("Temperature : ");
  Serial.println(temp);

  sendTemperature(temp);

  delay(LOOP_DELAY);
}

void connectWifi()
{
  Serial.print("try to connect wifi");
  WiFi.begin(SSID, PWD);
  while(WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.print(".");
  }

  Serial.println("connect success");
}

void sendTemperature(float temp)
{
  WiFiClient client;

  if(client.connect(server, _PORT))
  {
#if PROGRAM_TYPE == 0
    String postStr = apiKey;
    postStr += "&field1=";
    postStr += String(temp);
    postStr += "\r\n\r\n";
   
    client.print("POST /update HTTP/1.1\n");
    client.print("Host: api.thingspeak.com\n");
    client.print("Connection: close\n");
    client.print("X-THINGSPEAKAPIKEY: " + apiKey + "\n");
    client.print("Content-Type: application/x-www-form-urlencoded\n");
    client.print("Content-Length: ");
    client.print(postStr.length());
    client.print("\n\n");
    client.print(postStr);
    
#elif PROGRAM_TYPE == 1
    String postStr;
    DynamicJsonBuffer jsonBuffer;
    JsonObject &jobject = jsonBuffer.createObject();
    jobject["value1"] = String(temp);
    jobject.printTo(postStr);
    client.print("POST /trigger/");
    client.print(event);
    client.print("/with/key/");
    client.print(apiKey);
    client.println(" HTTP/1.1");
    client.println("Host: maker.ifttt.com");
    client.println("Content-Type: application/json");
    client.println("Accept: text/plain");
    client.print("Content-Length: ");
    client.print(postStr.length());
    client.print("\n\n");
    client.print(postStr);
#endif

    client.stop();
  }
  else
  {
    Serial.println("connect failed");
  }
}

