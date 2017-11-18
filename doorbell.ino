#include <Adafruit_LiquidCrystal.h>
#include <ArduinoJson.h>

const int but1 = 0, but2 = 1, but3 = 2;
const int rs = 15, en = 13, d4 = 12, d5 = 14, d6 = 2, d7 = 16;

Adafruit_LiquidCrystal lcd(rs, en, d4, d5, d6, d7);


void setup() {
  // put your setup code here, to run once:
  pinMode(but1, INPUT);
  pinMode(but2, INPUT);
  pinMode(but3, INPUT);

  lcd.begin(16, 2);
  
}

void loop() {
  // put your main code here, to run repeatedly:
  lcd.clear();
  lcd.print("Hello World");
  delay(100);
}
