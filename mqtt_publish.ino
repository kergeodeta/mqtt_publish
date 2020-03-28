#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#define DHT_TYP DHT11
#define DHT_PIN 2

const char* ssid  = "";
const char* pass  = "";
const char* id    = "";
// Refresh rate in seconds
const int refreshRate = 5;

char* tTemp = "livingroom/temp";
char* tHum  = "livingroom/hum";

DHT dht(DHT_PIN, DHT_TYP);
IPAddress broker(192,168,1,108);
WiFiClient wifiClient;
PubSubClient pubSubClient(wifiClient);

void setup() {
  Serial.begin(9600);
  dht.begin();

  setup_wifi();
  pubSubClient.setServer(broker, 1883);
}

void setup_wifi() {
  WiFi.begin(ssid, pass);
  Serial.print("Connect to WiFi network");
  while(WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.print("\nWiFi connection established. IP: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  if(!pubSubClient.connected()) {
    reconnect();
  }
  pubSubClient.loop();
  publish(tTemp, dht.readTemperature());
  publish(tHum, dht.readHumidity());

  delay(refreshRate * 1000);
}

void publish(char* topic, float data) {
  char payload[10];
  dtostrf(data, 2, 0, payload);
  if(payload != "nan") {
    pubSubClient.publish(topic, payload);
  }
}

void reconnect() {
  Serial.print("Try to connect MQTT broker");
  while(!pubSubClient.connected()) {
    Serial.print(".");

    if(pubSubClient.connect(id)) {
      Serial.println("\nConnected to MQTT broker");
    } else {
      delay(1000);
    }
  }
}
