/*
 * HTTPS Secured Client POST Request
 * Copyright (c) 2019, circuits4you.com
 * All rights reserved.
 * https://circuits4you.com 
 * Connects to WiFi HotSpot. */

#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h> 
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include <Arduino_JSON.h>

/* Set these to your desired credentials. */
//const char *ssid = "test";  //ENTER YOUR WIFI SETTINGS
//const char *password = "testtest";
const char *ssid = "x";  //ENTER YOUR WIFI SETTINGS
const char *password = "x";

const char *device = "1";
const char *user = "test";
const char *pw = "rNW)dW,^X8fHz_bb";

//Link to read data from https://jsonplaceholder.typicode.com/comments?postId=7
//Web/Server address to read/write from 
const char *host = "arduino.al-shantir.com";
const int httpsPort = 443;  //HTTPS= 443 and HTTP = 80

//fotorezistor
const int pTransistor = A0;

//SHA1 finger print of certificate use web browser to view and copy
const char fingerprint[] PROGMEM = "B4 21 9E C3 37 6F 33 22 9C 32 41 2F 1A 58 46 98 C0 7B 91 AE";
//=======================================================================
//                    Power on setup
//=======================================================================

void setup() {
  delay(1000);
  Serial.begin(115200);
  WiFi.mode(WIFI_OFF);        //Prevents reconnection issue (taking too long to connect)
  delay(1000);
  WiFi.mode(WIFI_STA);        //Only Station No AP, This line hides the viewing of ESP as wifi hotspot
  
}

//=======================================================================
//                    Main Program Loop
//=======================================================================
void loop() {

  WiFi.begin(ssid, password);     //Connect to your WiFi router
  Serial.println("");

  Serial.print("Connecting");
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  //If connection successful show IP address in serial monitor
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());  //IP address assigned to your ESP

  WiFiClientSecure httpsClient;    //Declare object of class WiFiClient

  Serial.println(host);

  Serial.printf("Using fingerprint '%s'\n", fingerprint);
  httpsClient.setFingerprint(fingerprint);
  httpsClient.setTimeout(15000); // 15 Seconds
  delay(1000);
  
  Serial.println("HTTPS Connecting");
  int r=0; //retry counter
  while((!httpsClient.connect(host, httpsPort)) && (r < 30)){
      delay(100);
      Serial.print(".");
      r++;
  }
  if(r==30) {
    Serial.println("Connection failed");
  }
  else {
    Serial.println("Connected to host\r\n");

    //httpsClient.print(String("POST /") +" HTTP/1.1\r\n" + "Host: " + host + "\r\nConnection: close\r\n\r\n");


    //String PostData = printJSON();
    //String PostData = "data=123";

    int resistance;
    resistance = analogRead(pTransistor);

    String PostData = "data=" + String(resistance) + "&user=" + String(user) + "&pw=" + String(pw) + "&device=" + String(device); 

    httpsClient.print(String("POST / HTTP/1.1\r\n") + "Host: " + host  + "\r\nUser-Agent: Arduino/1.0\r\n" + "Connection: close\r\n" + "Content-Type: application/x-www-form-urlencoded\r\n" + "Content-Length: " + PostData.length() + "\r\n\r\n" + PostData);

    Serial.println("=====Vypis=====");
    Serial.println(String("POST / HTTP/1.1\r\n") + "Host: " + host  + "\r\nUser-Agent: Arduino/1.0\r\n" + "Connection: close\r\n" + "Content-Type: application/x-www-form-urlencoded\r\n" + "Content-Length: " + PostData.length() + "\r\n\r\n" + PostData);
    Serial.println("=====Vypis=====");
  }

    while (httpsClient.connected()) {
    String line = httpsClient.readStringUntil('\n');
    if (line == "\r") {
      Serial.println("headers received");
      break;
    }
  }

  Serial.println("reply was:");
  String line;
  while(httpsClient.available()){        
    line = httpsClient.readStringUntil('\n');  //Read Line by Line
    Serial.println(line); //Print response
  }
  Serial.println("==========");
  Serial.println("closing connection");
  Serial.println(" ");
  
  delay(900000);  //POST Data at every 15 minutes
}

/*String printJSON() {

  JSONVar myArray;

  // načítanie nameraných hodnôt fototranzistorom
  int resistance;
  resistance = analogRead(pTransistor);

  myArray[0] = "data:";
  myArray[1] = resistance;

  //JSON.stringify(myVar) can be used to convert the JSONVar to a String
  String jsonString = JSON.stringify(myArray);

  for (int i = 0; i < myArray.length(); i++) {
    JSONVar value = myArray[i];

  }
  return jsonString;
}*/