#include <ArduinoMqttClient.h>
#include <WiFiNINA.h>
#include "arduino_secrets.h"

WiFiClient wifiClient;
MqttClient mqttClient(wifiClient);

char ssid[] = SECRET_SSID;
char pass[] = SECRET_PASS;

const char broker[] = "192.168.0.173";
int        port     = 1883;
const char topic[]  = "topic_svetlo";
const char topic2[]  = "topic_teplota";

//const char username[] = "";
//const char password[] = "";

const long interval = 60000;
unsigned long previousMillis = 0;

//senzor teploty
const int tSenzorPin = A0;

//fotorezistor
const int pTransistor = A1;
void setup() {
  //Inicializácia portu, nastavenie pinov senzorov a čakanie na otvorenie portu
  Serial.begin(9600);
  pinMode(pTransistor, INPUT);
  pinMode(tSenzorPin, INPUT);

  while (!Serial) {}

  // pokus o pripojenie sa do našej siete
  Serial.print("Pokus o pripojenie sa do siete: ");
  Serial.println(ssid);
  while (WiFi.begin(ssid, pass) != WL_CONNECTED) {
    // neúspešne, skús znova
    Serial.print(".");
    delay(5000);
  }

  //mqttClient.setUsernamePassword(username, password);

  Serial.println("Pripojený do siete");
  Serial.println();

  Serial.print("Pokus o pripojenie sa do MQTT brokera: ");
  Serial.println(broker);

  if (!mqttClient.connect(broker, port)) {
    Serial.print("MQTT pripojenie zlyhalo! Error kód = ");
    Serial.println(mqttClient.connectError());

    while (1);
  }

  Serial.println("Úspešné pripojenie do MQTT brokera!");
  Serial.println();
}
void loop() {
  // volanie poll(), ktorá zabraňuje odpojenie od brokera
  mqttClient.poll();

  // konvertovanie nameraných hodnôt tepelným senzorom do °C
  float teplota = analogRead(tSenzorPin);
  float voltaz = teplota * (5.0 / 1024);
  float teplotaC = (voltaz - 0.5) * 100 - 10;

  // načítanie nameraných hodnôt fototranzistorom
  int svetlost;
  svetlost = analogRead(pTransistor);

  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    // ukladá posledný čas, kedy naposledy odoslalo správu
    previousMillis = currentMillis;

    // výpis odoslaných správ pre tému na kontrolu v konzoli
    Serial.print("Sending message to topic: ");
    Serial.println("Svetlost");
    Serial.println(svetlost);

    Serial.print("Sending message to topic: ");
    Serial.println("Teplota");
    Serial.println(teplotaC);

    // odošle správu brokerovi
    mqttClient.beginMessage(topic);
    mqttClient.print(svetlost);
    mqttClient.endMessage();

    mqttClient.beginMessage(topic2);
    mqttClient.print(teplotaC);
    mqttClient.endMessage();

    Serial.println();
  }
}
