#include <Arduino.h>
#include <DHT.h>
#include <WiFiManager.h>

WiFiManager wm;   
const char* ssid = "ESP32";  
const char* password = "motdepasse"; 

// capteur temperature
#define DHTPIN 13
#define DHTTYPE DHT11

// capteur humiditer
//const int sensorPin = 14 ;

// put function declarations here:
DHT dht(DHTPIN, DHTTYPE);

void setup() {

  WiFi.mode(WIFI_STA); 

  // put your setup code here, to run once:
  Serial.begin(9600);

  //Initialise le capteur DHT11
  dht.begin();

  if(!wm.autoConnect(ssid, password))  // Test d'auto-connexion
    Serial.println("Erreur de connexion.");  // Si pas de connexion = Erreur
  else
    Serial.println("Connexion etablie !");  // Si connexion = OK

}

void loop() {
  // recupere la temperature et l'humidite 
  Serial.println("Temperature = " + String(dht.readTemperature())+ " degrés");
  Serial.println("Humidite = " + String(dht.readHumidity())+" %");

  // recupere l'humiditer du sol 
  //int soilHumidity = analogRead(sensorPin);
  //Serial.println("Humidite du sol = " + String(soilHumidity));
  // délais de 10 secondes
  delay(10000);
}

