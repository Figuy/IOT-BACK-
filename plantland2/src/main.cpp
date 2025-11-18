#include <Arduino.h>
#include <DHT.h>
#include <WiFiManager.h>
#include <PubSubClient.h>

WiFiManager wm;
WiFiClient espClient;
PubSubClient client(espClient);

const char* ssid = "ESP32";  
const char* password = "motdepasse"; 

// capteur temperature
#define DHTPIN 13
#define DHTTYPE DHT11

// capteur humiditer
const int sensor_pin = 32 ;
int _moisture,sensor_analog ; 

// MQTT Broker
const char *mqtt_broker = "broker.emqx.io";
const char *topic = "Ynov/VHT/1/1";
const int mqtt_port = 1883;


// put function declarations here:
DHT dht(DHTPIN, DHTTYPE);

void setup() {

  WiFi.mode(WIFI_STA); 

  // put your setup code here, to run once:
  Serial.begin(115200);

  //Initialise le capteur DHT11
  dht.begin();

  if(!wm.autoConnect(ssid, password))  // Test d'auto-connexion
    Serial.println("Erreur de connexion.");  // Si pas de connexion = Erreur
  else
    Serial.println("Connexion etablie !");  // Si connexion = OK

  //connecting to a mqtt broker
  client.setServer(mqtt_broker, mqtt_port);
  //Reconnexion MQTT automatique
  while (!client.connected()) {
        String client_id = "esp32-client-";
        client_id += String(WiFi.macAddress());
        Serial.printf("The client %s connects to the public MQTT broker\n", client_id.c_str());
        if (client.connect(client_id.c_str())) {
          Serial.println("Connecté !");
        } else {
            Serial.print("failed with state ");
            Serial.print(client.state());
            delay(2000);
        }
  }
}

void loop() {

  if(!client.connected()) {
        // Reconnexion MQTT 
   }
    client.loop();

    // Donner pour la temperature 
    float temperature = dht.readTemperature();   
     // Convert the value to a char array
    char tempString[8];
    dtostrf(temperature, 1, 2, tempString);


  // afficher dans le terminal
  Serial.println("Temperature = " + String(dht.readTemperature())+ " degrés");

  // Donner pour l'humiditer
    float humiditer = dht.readHumidity();   
    // Convert the value to a char array
    char humString[8];
    dtostrf(humiditer, 1, 2, humString);

    Serial.println("Humidite = " + String(dht.readHumidity())+" %");

    // recupere l'humiditer du sol 

    sensor_analog = analogRead(sensor_pin);
    _moisture = ( 100 - ( (sensor_analog/4095.00) * 100 ) );
    
    // afficher dans le terminal
    Serial.println("Humidite du sol = " + String(_moisture));

  // Publier sur le MQTT
  String payload( "{ \"Celsius\": " + String(tempString) + ", \"Humidité\": " + String(humString) + ", \"Sol\": " + String(_moisture) + " }" );  
  client.publish("Ynov/VHT/1/1", payload.c_str());

  
 
  // délais de 10 secondes
  delay(10000);
}

