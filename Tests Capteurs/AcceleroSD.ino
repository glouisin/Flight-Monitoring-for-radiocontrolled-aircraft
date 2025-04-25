#include <Wire.h>
#include <Adafruit_LIS3DH.h>
#include <SPI.h>
#include <SD.h>

Adafruit_LIS3DH lis = Adafruit_LIS3DH();
File myFile;

float time = 0;
bool enregistrementTermine = false;

void setup() {
  Serial.begin(9600);
  while (!Serial); // attendre le port série

  Serial.print("Initialisation carte SD...");
  if (!SD.begin(4)) {
    Serial.println("Échec !");
    while (1);
  }
  Serial.println("Carte SD OK.");

  if (!lis.begin(0x19)) {
    Serial.println("Capteur LIS3DH non detecte !");
    while (1);
  }
  Serial.println("Capteur detecte.");
  lis.setRange(LIS3DH_RANGE_4_G);

  // Création nom de fichier (acc1.txt, acc2.txt, etc.)
  int index = 1;
  String nomFichier;
  do {
    nomFichier = "acc" + String(index) + ".txt";
    index++;
  } while (SD.exists(nomFichier));

  myFile = SD.open(nomFichier.c_str(), FILE_WRITE);
  if (!myFile) {
    Serial.println("Erreur ouverture fichier !");
    while (1);
  }

  Serial.print("Enregistrement dans : ");
  Serial.println(nomFichier);
  Serial.println("Temps (s), X (m/s2), Y (m/s2), Z (m/s2)");
  myFile.println("Temps (s), X (m/s2), Y (m/s2), Z (m/s2)");
}

void loop() {
  if (!enregistrementTermine && myFile) {
    sensors_event_t event;
    lis.getEvent(&event);
    time += 0.0075;

    // Affichage série
    Serial.print(time, 4);
    Serial.print(", ");
    Serial.print(event.acceleration.x);
    Serial.print(", ");
    Serial.print(event.acceleration.y);
    Serial.print(", ");
    Serial.println(event.acceleration.z);

    // Écriture fichier
    myFile.print(time, 4);
    myFile.print(", ");
    myFile.print(event.acceleration.x);
    myFile.print(", ");
    myFile.print(event.acceleration.y);
    myFile.print(", ");
    myFile.println(event.acceleration.z);
    myFile.flush();

    if (time >= 10.0) {
      myFile.close();
      Serial.println(">>> Enregistrement terminé et fichier fermé !");
      enregistrementTermine = true;
    }

    delay(1);
  }
}
