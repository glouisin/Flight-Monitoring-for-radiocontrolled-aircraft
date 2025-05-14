#include <SoftwareSerial.h>
SoftwareSerial Radio(4, 5); // RX, TX

void setup() {
  Serial.begin(9600);    // Pour debug
  Radio.begin(9600);     // Vers GAMMA62M

  delay(2000); // Laisse le module démarrer

  Serial.println("Envoi vers GAMMA62M...");
  Radio.println("Hello depuis Arduino 1 !");
}

void loop() {
  // Rien pour le moment
}
