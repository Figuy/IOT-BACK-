### ESP32 IoT Plant Monitor – 
Description

Ce projet permet de lire et publier les données d’un capteur de température, d’humidité de l’air et d’humidité du sol  via MQTT.

L’ESP32 utilise WiFiManager pour la configuration WiFi sans hardcoder le SSID, et permet un appairage via MQTT avec un site web ou une application pour associer le device à un utilisateur.

Matériel requis :

- ESP32 (n’importe quel modèle avec ADC et WiFi)
- Capteur DHT11 (température + humidité)
- Capteur d’humidité du sol (analogique)
- Câbles de connexion
- Ordinateur pour flasher l’ESP32

Librairies Arduino nécessaires :

- DHT sensor library
- Adafruit Unified Sensor
- WiFiManager
- PubSubClient


Installation et utilisation :

1. Flasher l’ESP32

Installer les librairies nécessaires via Arduino IDE ou PlatformIO

Connecter l’ESP32 via USB

Compiler et téléverser le code

2. Configuration WiFi

Lancer le moniteur série (115200 baud)

Connecter le téléphone ou ordinateur au WiFi créé par l’ESP32 (SSID par défaut : ESP32)

Ouvrir la page WiFiManager → noter le code affiché pour l’appairage

3. Test MQTT

Abonner un client MQTT au topic Ynov/VHT/<Code>/data

Vérifier que les données du capteur arrivent toutes les 10 secondes

Pour l’appairage, publier un userId sur Ynov/VHT/idClient/<Code>

Vérifier que l’ESP répond sur Ynov/VHT/idClient/<userId>/mac
