#include <Arduino.h>
#include <DHT.h>
#include <WiFiManager.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

WiFiManager wm;
WiFiClient espClient;
PubSubClient client(espClient);

const char* ssid = "ESP32";  


// capteur temperature
#define DHTPIN 13
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// capteur humiditer
const int sensor_pin = 32 ;
int moisture ,sensor_analog ; 

// MQTT Broker
const char *mqtt_broker = "broker.emqx.io";
const int mqtt_port = 1883;

// Topic dynamique
String Code;
String Dynamic_topic_envoie;
String Dynamic_topic_recois;

int user_id= 0;

void callback(char* topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");


  String messageTemp;
  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();

  String topicCode = String(topic);

  if (topicCode.startsWith("Ynov/VHT/idClient/")) {
 // Extraire le code ESP depuis le topic
        String codeFromTopic = topicCode.substring(String("Ynov/VHT/idClient/").length());
        Serial.println("Appairage demandé pour le code ESP : " + codeFromTopic);

        // Parse JSON pour récupérer user_id
    DynamicJsonDocument doc(200);
    DeserializationError error = deserializeJson(doc, messageTemp);
    if (error) {
        Serial.println("Erreur JSON: " + String(error.c_str()));
        return;
    }

    user_id = doc["user_id"];
    Serial.println("user_id reçu : " + String(user_id));

    // Mettre a jour les topic pour enovyer les données
    String mac = WiFi.macAddress();
    mac.replace(":","");
    Code = mac.substring(6);
    Dynamic_topic_envoie = "Ynov/VHT/" + String(user_id) + "/" + Code ;
    Dynamic_topic_recois = "Ynov/VHT/" + String(user_id) + "/" + Code + "/cmd";

    //Envoyer la mec de l'esp au site
    String reponseTopic = "Ynov/VHT/idClient/" + String(user_id) + "/mac";
    String reponseJson =  "{ \"mac\": \"" + mac + "\" }";
    client.publish(reponseTopic.c_str(), reponseJson.c_str());
    Serial.println("Réponse envoyée sur : " + reponseTopic);
  }
}
void setup() {

  WiFi.mode(WIFI_STA); 

  // put your setup code here, to run once:
  Serial.begin(115200);

  //Initialise le capteur DHT11
  dht.begin();

  // Accéder a la mac du client pour ce connecter
  String mac = WiFi.macAddress();
  mac.replace(":", "");
  Code = mac.substring(6);

 // Définir un HTML simple pour la page captive
  String html = "<!DOCTYPE html><html><head><title>Bienvenue</title></head><body>";
  html += "<h1>Bienvenue !</h1>";
  html += "<p>CODE À ENREGISTRER SUR LE SITE :</p>";
  html += "<h2>" + Code + "</h2>";
  html += "</body></html>";
  WiFiManagerParameter infoText(html.c_str());
  wm.addParameter(&infoText);


  Dynamic_topic_envoie = "Ynov/VHT/" + mac ;
  Serial.println(Dynamic_topic_envoie);

  if(!wm.autoConnect(ssid))  // Test d'auto-connexion
    Serial.println("Erreur de connexion.");  // Si pas de connexion = Erreur
  else
    Serial.println("Connexion etablie !");  // Si connexion = OK

  Dynamic_topic_envoie = "Ynov/VHT/" + Code ;
  Dynamic_topic_recois = "Ynov/VHT/" + Code + "/cmd";

  Serial.println("Topic Publish : " + Dynamic_topic_envoie);
  Serial.println("Topic Subscribe : " + Dynamic_topic_recois);

  //connecting to a mqtt broker
  client.setServer(mqtt_broker, mqtt_port);
  client.setCallback(callback);
  client.subscribe("Ynov/VHT/idClient/+");
  Serial.println("Subscribe to pairing: Ynov/VHT/idClient/+");

  //Reconnexion MQTT automatique
  while (!client.connected()) {
        String client_id = "esp32-client-" + WiFi.macAddress();
        Serial.printf("The client %s connects to the public MQTT broker\n", client_id.c_str());
        if (client.connect(client_id.c_str())) {
          Serial.println("Connecté !");
          client.subscribe("Ynov/VHT/idClient/+");
          Serial.println("Connection au parrainage : Ynov/VHT/idClient/X");
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
    moisture = ( 100 - ( (sensor_analog/4095.00) * 100 ) );
    
    // afficher dans le terminal
    Serial.println("Humidite du sol = " + String(moisture)+ " %");

  // Publier sur le MQTT
  String payload( "{ \"Celsius\": " + String(tempString) + ", \"Humidité\": " + String(humString) + ", \"Sol\": " + String(moisture) + " }" );  
  client.publish(Dynamic_topic_envoie.c_str(), payload.c_str());

   // Publier seulement si user_id a été reçu
    if (user_id > 0) {
        String topicToPublish = Dynamic_topic_envoie;
        
        client.publish(topicToPublish.c_str(), payload.c_str());
        Serial.println("Données publiées sur : " + topicToPublish);
    } else {
        Serial.println("En attente de user_id pour publier...");
    }
  
  // délais de 5 secondes
  delay(5000);
}


