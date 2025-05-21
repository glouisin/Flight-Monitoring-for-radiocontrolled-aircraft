#include <LiquidCrystal.h>

// RS, E, D4, D5, D6, D7
LiquidCrystal lcd(12, 11, 6, 4, 3, 2);

void setup() {
  lcd.begin(16, 2); // LCD 16 colonnes, 2 lignes
  lcd.print("bonjour");
}

void loop() {
  // Tu peux afficher des infos ici en continu
}
