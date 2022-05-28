
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27,20,4);


unsigned long startMillis;  //some global variables available anywhere in the program
unsigned long currentMillis;
const int period = 1000 ;  //the value is a number of milliseconds
const byte ledPin = 13;    //using the built in LED
float current = 0.5;
float c_current = 0.5;
float totalCoulumbs = 0.0;



void setup() {
  lcd.init();
  lcd.backlight();
  lcd.setCursor(3,0);
  lcd.print("Starting soc measurement");
  Serial.begin(115200);  //start Serial in case we need to print debugging info
  pinMode(ledPin, OUTPUT);
  startMillis = millis();  //initial start time
  lcd.clear();

}

void loop() {
  currentMillis = millis();  //get the current "time" (actually the number of milliseconds since the program started)
  if (currentMillis - startMillis >= period)  //test whether the period has elapsed
  {
    lcd.setCursor(0,2);
    lcd.print(millis());
    lcd.setCursor(0,3);
    lcd.print("current :");
    lcd.setCursor(11,3);
    lcd.print(current);
    totalCoulumbs = totalCoulumbs + current;
    Serial.print("Total Coulumbs = ");
    lcd.setCursor(0,0);
    lcd.print("total columbs:");
    lcd.setCursor(14,0);
    lcd.print(totalCoulumbs);
    Serial.println(totalCoulumbs);
    lcd.setCursor(0,1);
    lcd.print("total Ah :");
    lcd.setCursor(10,1);
    lcd.print(totalCoulumbs/3600.0);
    Serial.print("Total Ah = ");
    Serial.println(totalCoulumbs/3600.0);
    digitalWrite(ledPin, !digitalRead(ledPin));  //if so, change the state of the LED.  Uses a neat trick to change the state
    startMillis = currentMillis;  //IMPORTANT to save the start time of the current LED state.
  }

}
