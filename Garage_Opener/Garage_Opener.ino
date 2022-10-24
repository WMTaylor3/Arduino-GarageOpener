#include <PubSubClient.h>
#include <ESP8266WiFi.h>
#include "Credentials.h"

#define CONTROL_PIN 2

WiFiClient wifiClient;
PubSubClient mqtt;

void mqttCallback(char* topic, byte* payload, unsigned int length) {
    Serial.print(F("Message arrived ["));
    Serial.print(topic);
    Serial.print(F("] "));
    for (int i = 0; i < length; i++) {
        char receivedChar = (char)payload[i];
        Serial.print(receivedChar);
    }
    Serial.println();
}

void setup() {
    pinMode(CONTROL_PIN, INPUT);
    
    Serial.begin(9600);

    setupWifi();

    setupMqtt();
}

void setupWifi() {
    Serial.print(F("Connecting to WiFi: "));
    Serial.println(WIFI_NAME);

    WiFi.begin(WIFI_NAME, WIFI_PASSWORD);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(F("."));
    }

    Serial.println(F(""));
    Serial.println(F("WiFi connected"));
    Serial.print(F("IP address: "));
    Serial.println(WiFi.localIP());
}

void setupMqtt() {
  mqtt.setClient(wifiClient);
  mqtt.setServer(MQTT_SERVER, MQTT_PORT);
  mqtt.setCallback(mqttCallback);
}

void reconnectMqtt() {
    while (!mqtt.connected()) {
        Serial.print(F("Attempting MQTT connection..."));
        if (mqtt.connect(MQTT_ID, MQTT_USER, MQTT_PASSWORD)) {
            Serial.println(F("Connected"));
        } else {
            Serial.print(F("Failed, rc="));
            Serial.print(mqtt.state());
            Serial.println(F(" try again in 5 seconds"));
            delay(5000);
        }
    }
}

void loop() {
    if(WiFi.status() != WL_CONNECTED) {
      setupWifi();
    }

    if (!mqtt.connected()) {
      reconnectMqtt();
    }

    mqtt.loop();

    if(digitalRead(2) == LOW){
        mqtt.publish(MQTT_TOPIC, "toggle");
        delay(100);
    }
}
