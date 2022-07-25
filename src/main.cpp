#include <WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <string.h>
#define PIR 16

/*****************************************Private Variable***************************************************************/
//Wifi Config Variables
const char* ssid = "Song Quynh";
const char* password = "songquynh25042112";

//Interrupt Variables
portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;

//Sensor Read Variables
char temp[5];
int counter = 1;
bool sensorState = 0;
// MQTT Conection Variables
const char* mqtt_server = "192.168.1.41";
unsigned long timeNow = 0;
WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;
/***********************************************Private Macro*******************************************************************/

void IRAM_ATTR SensoHIGH();
void reconnect(); 
void setup_wifi();
void callback(char* topic, byte* message, unsigned int length);
/********************************************************************************************************************************/
void setup() {
  Serial.begin(115200);
  pinMode(PIR, INPUT);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  attachInterrupt(digitalPinToInterrupt(PIR), SensoHIGH, RISING);
}

void loop() 
{
  if(sensorState)
  {
    if(counter == 1)
    {
      counter = 0;
    }
    else if (counter == 0)
    {
      counter = 1;
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
      client.publish("Sensor/PIR1", temp);
    }
    sensorState = 0;
  }
  
}

/***********************************************Private Function*******************************************************************/
void IRAM_ATTR SensoHIGH()
{
  portENTER_CRITICAL_ISR(&mux);     //Phần quan trọng cần được bảo vệ khỏi mọi truy cập đồng thời để thay đổi nó
  sensorState = 1;
  portEXIT_CRITICAL_ISR(&mux);    //Cho phép tiếp tục chạy các Task khác
}

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
    if (client.connect("Sensor1")) {
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