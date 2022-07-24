#include <WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <string.h>
#define PIR 16

// Replace the next variables with your SSID/Password combination
const char* ssid = "Song Quynh";
const char* password = "songquynh25042112";
char temp[5];
// Add your MQTT Broker IP address, example:
const char* mqtt_server = "192.168.1.41";
unsigned long timeNow = 0;
int buff = 0;
int counter = 0;
WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;

void setup_wifi() {
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}


void callback(char* topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;
  
  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    //messageTemp += (char)message[i];
  }
  Serial.println();

}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP32Client")) {
      Serial.println("connected");
      // Subscribe
      client.subscribe("test");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(1000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(PIR, INPUT);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

}

void loop() 
{
  buff = digitalRead(PIR);
  
  if(buff)
  {
    if(counter == 0)
    {
      counter = 1;
    }
    else if (counter == 1)
    {
      counter = 0;
    }
    sprintf(temp,"%d",counter);
    if (!client.connected()) 
    {
      reconnect();
    }
    client.loop();
    long now = millis();
    if (now - lastMsg > 100) 
    {
      lastMsg = now;
      client.publish("Sensor/PIR2", temp);
    }
  }

  while(buff != 0)
  {
    buff = digitalRead(PIR);
  }
}