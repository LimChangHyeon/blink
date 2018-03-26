#include <ESP8266WiFi.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <ArduinoJson.h>


// PROGRAM_TYPE 0 => thingspeak & aws
// PROGRAM_TYPE 1 => ifttt
#define PROGRAM_TYPE 0

#if PROGRAM_TYPE == 0
#define LOOP_DELAY (60 * 1000)
#elif PROGRAM_TYPE == 1
#define LOOP_DELAY (5 * 1000)
#endif

#define ONE_WIRE_BUS 0

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature tpSensor(&oneWire);

const char *SSID = "spslab1";
const char *PWD ="sps123456";

void setup()
{
  Serial.begin(115200);
  Serial.println("\r\n");
  Serial.println("program type : " + PROGRAM_TYPE);
  connectWifi();
}

void loop()
{
  float temp;
  unsigned long prevTime;
  unsigned long elapsedTime;
  tpSensor.requestTemperatures();
  temp = tpSensor.getTempCByIndex(0);
  Serial.print("Temperature : ");
  Serial.println(temp);

  prevTime = (unsigned long)millis();

#if PROGRAM_TYPE == 0
  sendTemperatureAWS(temp);
  sendTemperatureTS(temp);
#elif PROGRAM_TYPE == 1
  sendTemperatureIFTTT(temp);
#endif

  elapsedTime = (unsigned long)millis() - prevTime;

  if(elapsedTime <= (unsigned long)LOOP_DELAY)
  {
    delay(LOOP_DELAY - elapsedTime);
  }
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

void sendTemperatureTS(float temp)
{
  static const String _TS_apiKey = "6BAGZ5AM53XR2VL4";
  static const char *_TS_server = "api.thingspeak.com";
  static const int _TS_PORT = 80;
  
  WiFiClient client;

  if(client.connect(_TS_server, _TS_PORT))
  {
    String postStr = _TS_apiKey;
    postStr += "&field1=";
    postStr += String(temp);
    postStr += "\r\n\r\n";
   
    client.print("POST /update HTTP/1.1\n");
    client.print("Host: api.thingspeak.com\n");
    client.print("Connection: close\n");
    client.print("X-THINGSPEAKAPIKEY: " + _TS_apiKey + "\n");
    client.print("Content-Type: application/x-www-form-urlencoded\n");
    client.print("Content-Length: ");
    client.print(postStr.length());
    client.print("\n\n");
    client.print(postStr);

    client.stop();
  }
  else
  {
    Serial.println("connect failed");
  }
}

void sendTemperatureAWS(float temp)
{
  static const char *_AWS_server = "ec2-13-125-232-83.ap-northeast-2.compute.amazonaws.com";
  static const int _AWS_PORT = 15234;
  
  WiFiClient client;

  if(client.connect(_AWS_server, _AWS_PORT))
  {
    String getStr = "/log?device=101&unit=1&type=T&seq=1&value=";
    getStr += String(temp);
   
    client.print("GET ");
    client.print(getStr);
    client.print(" HTTP/1.1\r\n");
    client.print("Host: ");
    client.print(_AWS_server);
    client.print("\r\nConnection: close\r\n\r\n");

    client.stop();
  }
  else
  {
    Serial.println("connect failed");
  }
}

void sendTemperatureIFTTT(float temp)
{
  static const char *_IFTTT_event = "hot_temp";
  static const char *_IFTTT_apiKey = "dV6vhAY4RezWR7LzHkNXH9";
  static const char *_IFTTT_server = "maker.ifttt.com";
  static const int _IFTTT_PORT = 80;
  WiFiClient client;

  if(client.connect(_IFTTT_server, _IFTTT_PORT))
  {
    String postStr;
    DynamicJsonBuffer jsonBuffer;
    JsonObject &jobject = jsonBuffer.createObject();
    jobject["value1"] = String(temp);
    jobject.printTo(postStr);
    client.print("POST /trigger/");
    client.print(_IFTTT_event);
    client.print("/with/key/");
    client.print(_IFTTT_apiKey);
    client.println(" HTTP/1.1");
    client.println("Host: maker.ifttt.com");
    client.println("Content-Type: application/json");
    client.println("Accept: text/plain");
    client.print("Content-Length: ");
    client.print(postStr.length());
    client.print("\n\n");
    client.print(postStr);

    client.stop();
  }
  else
  {
    Serial.println("connect failed");
  }
}
