#include <SoftwareSerial.h>
#include <SPI.h>
#include <SD.h>

SoftwareSerial SoftSerial(2, 3); // RX, TX
unsigned char buffer[64];        // Buffer de stockage
int count = 0;                   // Compteur de buffer

// Définitions pour la carte SD
const int chipSelect = 4;        // Broche CS pour la carte SD
File dataFile;                   // Fichier pour stocker les données

void setup() {
    Serial.begin(9600);          // Port série pour le PC
    SoftSerial.begin(9600);      // Port série pour le GPS
    
    // Initialisation de la carte SD
    Serial.print("Initialisation de la carte SD...");
    
    if (!SD.begin(chipSelect)) {
        Serial.println("Erreur d'initialisation de la carte SD!");
        return;
    }
    Serial.println("Carte SD initialisée.");
    
    // Création ou ouverture du fichier de données
    dataFile = SD.open("gpsdata.txt", FILE_WRITE);
    if (dataFile) {
        Serial.println("Fichier gpsdata.txt ouvert ou créé.");
        dataFile.println("=== Début des données GPS ===");
        dataFile.close();
    } else {
        Serial.println("Erreur lors de l'ouverture de gpsdata.txt!");
    }
}

void loop() {
    // Lecture des données GPS
    if (SoftSerial.available()) {  
        count = 0; // Réinitialisation du compteur

        while (SoftSerial.available() && count < 64) {  
            buffer[count++] = SoftSerial.read();  
        }

        // Envoi des données au PC
        Serial.write(buffer, count);
        
        // Enregistrement des données sur la carte SD
        dataFile = SD.open("gpsdata.txt", FILE_WRITE);
        if (dataFile) {
            dataFile.write(buffer, count);
            dataFile.close();
        } else {
            Serial.println("Erreur d'écriture sur la carte SD!");
        }
        
        clearBufferArray(); // Nettoyage du buffer
    }

    // Transmission PC vers GPS (optionnel)
    if (Serial.available()) {  
        SoftSerial.write(Serial.read());  
    }

    delay(10); // Petit délai pour éviter une surcharge CPU
}

// Nettoyage du buffer
void clearBufferArray() {
    memset(buffer, 0, sizeof(buffer));  
}