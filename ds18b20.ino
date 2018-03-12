#include <ESP8266WiFi.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#define ONE_WIRE_BUS 0

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature tpSensor(&oneWire);

float prevTemp = 0;
String apiKey = "6BAGZ5AM53XR2VL4";
const char *server = "api.thingspeak.com";
const char *SSID = "spslab1";
const char *PWD ="sps123456";

void setup()
{
  Serial.begin(115200);
  Serial.println("\r\n");
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

  delay(60 * 1000);
}

void connectWifi()
{
  Serial.print("try to connect server");
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

  if(client.connect(server, 80))
  {
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

    client.stop();
  }
}

