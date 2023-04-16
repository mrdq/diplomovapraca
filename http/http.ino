#include <SPI.h>
#include <WiFiNINA.h>
#include "arduino_secrets.h" 

char ssid[] = SECRET_SSID; // názov siete
char pass[] = SECRET_PASS; // heslo siete

int status = WL_IDLE_STATUS;

WiFiServer server(80);

//teplotný senzor
int tSenzorPin = A0;

//fotorezistor
const int pTransistor = A1;

void setup() {
  //Inicializujte sériový port a počkajte na otvorenie portu:
  Serial.begin(9600);
  pinMode(pTransistor, INPUT);
  pinMode(tSenzorPin, INPUT);

  while (!Serial) {;}

  // skontroluj modul WiFi:
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Komunikácia s WiFi modulom zlyhala!");
    // nepokračuj
    while (true);
  }

  String fv = WiFi.firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
    Serial.println("Prosím, aktualizujte si firmware");
  }

  // pokus o pripojenie k sieti WiFi:
  while (status != WL_CONNECTED) {
    Serial.print("Pokus o pripojenie do SSID: ");
    Serial.println(ssid);
    // Pripojte sa k sieti
    status = WiFi.begin(ssid, pass);

    // počkaj 5 sekúnd na pripojenie:
    delay(5000);
  }
  server.begin();
  // teraz si pripojení, vypíš stav:
  printWifiStatus();
}

void loop() {
  // sledovať prichádzajúcich klientov
  WiFiClient client = server.available();
  if (client) {
    Serial.println("Nový klient");
    // HTTP požiadavka končí prázdnym riadkom
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial.write(c);
        // ak ste sa dostali na koniec riadku 
        // (prijali ste znak nového riadka) a riadok je prázdny, 
        // požiadavka HTTP sa skončila, takže môžete poslať odpoveď
        if (c == '\n' && currentLineIsBlank) {
          // odoslať štandardnú hlavičku HTTP
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println("Connection: close");  // po dokončení odpovede sa spojenie uzavrie
          client.println("Refresh: 5");  // obnov stránku každých 5 sekúnd
          client.println();
          client.println("<!DOCTYPE HTML>");
          client.println("<html>");
          // premena nameraných hodnôt na teplotu v °C
          float teplota = analogRead(tSenzorPin);
          float voltaz = teplota * (5.0 / 1024);
          float teplotaC = (voltaz - 0.5) * 100 - 10;

          // načítanie nameraných hodnôt fototranzistorom
          int resistance;
          resistance = analogRead(pTransistor);

          client.println("Svetelnost: " +String(resistance)+"L");
          client.println("Teplota: " +String(teplotaC)+"C");

          client.println("</html>");
          break;
        }
        if (c == '\n') {
          // začínate nový riadok
          currentLineIsBlank = true;
        } else if (c != '\r') {
          // máte znak na aktuálnom riadku
          currentLineIsBlank = false;
        }
      }
    }
    // daj webovému prehliadaču čas na prijatie údajov
    delay(1);

    // uzavrieť spojenie:
    client.stop();
    Serial.println("Klient sa odpojil");
  }
}

void printWifiStatus() {
  // vypíš SSID siete, ku ktorej si pripojení:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // vypíš IP adresu dosky:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP adresa: ");
  Serial.println(ip);

  // vypíš silu signálu:
  long rssi = WiFi.RSSI();
  Serial.print("Sila signálu (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}
