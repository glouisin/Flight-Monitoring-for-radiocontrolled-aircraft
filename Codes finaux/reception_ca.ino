// À lire: a été codé à l'aide de l'ia mais le fonctionnement reste compréhensible: cette fois ci, on ne traite que les trames valides, car certaines trames reçues étaient corrompues
#include <SoftwareSerial.h>
#include <LiquidCrystal.h>

LiquidCrystal lcd(12, 11, 6, 4, 3, 2);
SoftwareSerial Radio(5, 7);  // RX, TX

String buffer = "";
unsigned long previousMillis = 0;
int displayPage = 0;
String hm = "--", te = "--", lt = "--", lg = "--", at = "--", v = "--", st = "--", h = "--";
bool newData = false;

unsigned long lastValidDataTime = 0;  // Dernière trame valide reçue
const unsigned long timeout = 20000;  // 20 secondes timeout

void setup() {
  lcd.begin(16, 2);
  lcd.setCursor(0,0);
  lcd.print("CA POUR PLANEUR");
  lcd.setCursor(0,1);
  lcd.print("Groupe 27, V.1.0");
  delay(4000);
  Serial.begin(9600);
  Radio.begin(9600);
  Serial.println("Attente données...");
  lastValidDataTime = millis();  // Initialiser à maintenant
}

void loop() {
  // Lecture non bloquante du port série logiciel
  while (Radio.available()) {
    char c = Radio.read();

    // Ignorer le retour chariot
    if (c == '\r') continue;

    if (c == '\n') {
      Serial.println("TRAME RECUE");
      Serial.println("=== TRAME BRUTE ===");
      Serial.println(buffer);  // Affiche la trame brute reçue

      if (isValidData(buffer)) {
        parseData(buffer);       // Extraction des valeurs

        Serial.println("====Données reçues du planeur====");
        Serial.print("Humidité: "); Serial.print(hm); Serial.println(" %");
        Serial.print("Temperature (C): "); Serial.println(te);
        Serial.print("Latitude: "); Serial.println(lt);
        Serial.print("Longitude: "); Serial.println(lg);
        Serial.print("Altitude: "); Serial.print(at); Serial.println(" m");
        Serial.print("Vitesse: "); Serial.print(v); Serial.println(" km/h");
        Serial.print("Sattelites: "); Serial.println(st);
        Serial.print("Heure de réception (UTC): "); Serial.println(h);

        newData = true;
        lastValidDataTime = millis();  // Mise à jour du timer
      } else {
        Serial.println("Trame ignorée (incomplète ou invalide)");
      }
      buffer = "";  // Réinitialise le buffer pour la prochaine trame
    } else {
      buffer += c;
    }
  }

  // Affichage cyclique sur LCD toutes les 3 secondes
  if (millis() - previousMillis > 3000) {
    previousMillis = millis();

    if (millis() - lastValidDataTime > timeout) {
      // Plus de données valides depuis trop longtemps
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Attente de");
      lcd.setCursor(0, 1);
      lcd.print("donnees...");
      newData = false;
    } else {
      if (newData) {
        lcd.clear();
        newData = false;
      }
//Attention, laisser les espaces car cela permet de 'nettoyer' l'écran car les caractères ne s'effacent pas automatiquement 
      switch (displayPage) {
        case 0:
          lcd.setCursor(0, 0); lcd.print("HM:" + hm + " %        ");
          lcd.setCursor(0, 1); lcd.print("TEMP:" + te + " dC       ");
          break;
        case 1:
          lcd.setCursor(0, 0); lcd.print("LAT:" + lt + " dN       ");
          lcd.setCursor(0, 1); lcd.print("LNG:" + lg + " dE       ");
          break;
        case 2:
          lcd.setCursor(0, 0); lcd.print("ALT:" + at + " m       ");
          lcd.setCursor(0, 1); lcd.print("SPD:" + v + " km/h    ");
          break;
        case 3:
          lcd.setCursor(0, 0); lcd.print("SAT:" + st + "            ");
          lcd.setCursor(0, 1); lcd.print("TIME:" + h + "     ");
          break;
      }

      displayPage = (displayPage + 1) % 4;
    }
  }
}
//permet de supprimer les trames invalide (ex: "Viiiiiteesse:", peut causer des bug de parsing)
bool isValidData(String data) {
  return data.indexOf("Humidite:") != -1 &&
         data.indexOf("Temp (C):") != -1 &&
         data.indexOf("Lat:") != -1 &&
         data.indexOf("Lng:") != -1 &&
         data.indexOf("Altitude:") != -1 &&
         data.indexOf("Vitesse:") != -1 &&
         data.indexOf("Sat:") != -1 &&
         data.indexOf("Heure:") != -1;
}
//traite la trame, on enregistre chque donnée importante dans la variable éponyme
void parseData(String data) {
  hm = extractValue(data, "Humidite:");
  te = extractValue(data, "Temp (C):");
  lt = extractValue(data, "Lat:");
  lg = extractValue(data, "Lng:");
  at = extractValue(data, "Altitude:");
  v  = extractValue(data, "Vitesse:");
  st = extractValue(data, "Sat:");
  h  = extractValue(data, "Heure:");
}
//on lit la valeur aprés un label (ex: "LAT:") jusqu'à la prochaine virgule
String extractValue(String data, String label) {
  int start = data.indexOf(label);
  if (start == -1) return "";

  start += label.length();

  // Cherche la prochaine virgule ou saut de ligne ou fin de chaîne
  int end = data.indexOf(',', start);
  if (end == -1) end = data.indexOf('\n', start);
  if (end == -1) end = data.length();

  String val = data.substring(start, end);
  val.trim();
  return val;
}
