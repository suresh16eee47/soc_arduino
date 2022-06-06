
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include "DHT.h"
#define DHTPIN 3
#define DHTTYPE DHT11
#include <SoftwareSerial.h>
SoftwareSerial espSerial(6, 7);
const int buzzer = 4;
DHT dht(DHTPIN, DHTTYPE);
LiquidCrystal_I2C lcd(0x27,20,4);

// voltage sensor variables
#define ANALOG_IN_PIN A0
float adc_voltage = 0.0000000;
float in_voltage = 0.0000000;
float R1 = 30000.0000000;
float R2 = 7500.000000; 
float ref_voltage = 5.0000000;
int adc_value = 0;
int charge = 5;
int present_state = 0;
int pev_state = 0;
int charge1 = 0;
unsigned long startMillis3;
unsigned long currentMillis3;


//Current sensor variables
unsigned int x=0;
float AcsValue=0.0,Samples=0.0,AvgAcs=0.0,AcsValueF=0.0;

unsigned long startMillis;  //some global variables available anywhere in the program
unsigned long currentMillis;
const int period = 1000 ;  //the value is a number of milliseconds
const byte ledPin = 13;    //using the built in LED
float current = 0.50000000;
unsigned long c_current = 0.500000;
float totalCoulumbs = 0.0000000;
float soc = 0.0;
float ocv = 0;
float mocv = 0;

float open_circuit_voltage(){
  lcd.init();
  digitalWrite(2, HIGH);
  delay(2000);
  adc_value = analogRead(ANALOG_IN_PIN);
  adc_voltage  = (adc_value * ref_voltage) / 1024.0;
  in_voltage = adc_voltage / (R2/(R1+R2));
  mocv = in_voltage;
  Serial.print("voltage :");
  Serial.print(in_voltage);
  if (mocv > 3.71 && mocv < 3.9){
    soc = 100;
  }
  else if((mocv <= 3.700) && (mocv >= 3.650)){
    soc = 90.0;
  }
  else if((mocv <= 3.649) && (mocv >= 3.600)){
    soc = 90.0;
  }
  else if((mocv <= 3.590) && (mocv >= 3.570)){
    soc = 85.0;
  }
  else if((mocv <= 3.569) && (mocv >= 3.530)){
    soc = 80;
  }
  else if((mocv <= 3.529) && (mocv >= 3.500)){
    soc = 70;
  }
  else if((mocv <= 3.549) && (mocv >= 3.470)){
    soc = 60;
  }
  else if((mocv <= 3.469) && (mocv >= 3.450)){
    soc = 50;
  }
  else if((mocv <= 3.449) && (mocv >= 3.420)){
    soc = 40;
  }
  else if((mocv <= 3.419) && (mocv >= 3.400)){
    soc = 30;
  }
  else if((mocv <= 3.399) && (mocv >= 3.380)){
    soc = 20;
  }
  else if((mocv <= 3.379) && (mocv >= 3.300)){
    soc = 15;
  }
  else if((mocv <= 3.290) && (mocv >= 3.200)){
    soc = 10;
  }
  else if((mocv <= 3.190) && (mocv >= 3.100)){
    soc = 5;
  }
  else if((mocv <= 3.090) && (mocv >= 3.00)){
    soc = 2;
  }
  else{
    soc = 0.001;
    Serial.print("failed to get SOC");
  }
  lcd.backlight();
  lcd.setCursor(6,0);
  lcd.print("STARTING");
  lcd.setCursor(3,1);
  lcd.print("VOC MEASUREMENT");
  lcd.setCursor(0,2);
  lcd.print("OPEN CIRCUIT VOLTAGE");
  lcd.setCursor(4,3);
  lcd.print("VOC :");
  lcd.setCursor(10,3);
  lcd.print(mocv);
//  lcd.setCursor(3,11);
//  lcd.print("SOC :");
//  lcd.setCursor(4,16);
//  lcd.print(soc);
  return in_voltage,ocv;
}


void setup() {
  espSerial.begin(115200);
  pinMode(buzzer, OUTPUT);
  dht.begin();
  Serial.begin(115200);
  pinMode(2, OUTPUT);
  pinMode(charge, OUTPUT);
  digitalWrite(2, HIGH);
  open_circuit_voltage();
  delay(2000);
  digitalWrite(2, LOW);  //start Serial in case we need to print debugging info
  pinMode(ledPin, OUTPUT);
  startMillis = millis();  //initial start time
  lcd.clear();

}

void loop() {
    //get the current "time" (actually the number of milliseconds since the program started)
  currentMillis = millis();
  float temperature = dht.readTemperature();
  adc_value = analogRead(ANALOG_IN_PIN);
  adc_voltage  = (adc_value * ref_voltage) / 1024.0;
  in_voltage = adc_voltage / (R2/(R1+R2));
  ocv = in_voltage;
  delay(20);
  String svoc = String(ocv,2);
  String ssoc = String(soc,2);
  String stemp = String(temperature,2);
  Serial.println("temperature");
  Serial.println("Temp :"+String(temperature));
  espSerial.println("Temp :"+String(temperature));
  if (currentMillis - startMillis >= period && soc != 0.001 && temperature < 40 && soc > 40 && ocv > 0.1 && ocv <3.9 && charge1 == 0)  //test whether the period has elapsed
    {
      // voltage sensor measurement.
      present_state = 0;
      if (present_state != pev_state){
        lcd.clear();
        delay(20);
      }
      digitalWrite(buzzer, LOW);
      lcd.setCursor(0,2);
      adc_value = analogRead(ANALOG_IN_PIN);
      adc_voltage  = (adc_value * ref_voltage) / 1024.0;
      in_voltage = adc_voltage / (R2/(R1+R2)) ;
      c_current = in_voltage*10; 
      lcd.print("CURRENT(mA) :");
      lcd.setCursor(13,2);
      lcd.print(in_voltage*100);
      lcd.setCursor(0,3);
      lcd.print("SOC :");
      soc = soc -((totalCoulumbs/3600.0)/3);
      lcd.setCursor(5,3);
      lcd.print(soc);
      lcd.setCursor(11,3);
      lcd.print("VOC:");
      lcd.setCursor(16,3);
      lcd.print(ocv);
      totalCoulumbs = totalCoulumbs + (c_current);
      lcd.setCursor(0,0);
      lcd.print("COLUMBS:");
      lcd.setCursor(8,0);
      lcd.print(totalCoulumbs,0);
      lcd.setCursor(14,0);
      lcd.print(temperature,1);
      lcd.setCursor(19,0);
      lcd.print("C");
      lcd.setCursor(0,1);
      lcd.print("CONSUMED Ah :");
      lcd.setCursor(14,1);
      lcd.print(totalCoulumbs/3600.0);
      //Serial.print("Total Ah = ");
      //Serial.println(totalCoulumbs/3600.0);
      digitalWrite(ledPin, !digitalRead(ledPin));  //if so, change the state of the LED.  Uses a neat trick to change the state
      startMillis = currentMillis;  //IMPORTANT to save the start time of the current LED state.
      pev_state = 0;
    }
    else if (soc == 0.001 && temperature <40) {
      present_state = 1;
      if (present_state != pev_state){
        lcd.clear();
        delay(20);
      }
      lcd.setCursor(3,0);
      lcd.print("KINDLY CONNECT "); 
      lcd.setCursor(4,1);
      lcd.print("BATTERY AND");
      lcd.setCursor(3,2);
      lcd.print("RESTART SYSTEM"); 
      adc_value = analogRead(ANALOG_IN_PIN);
      adc_voltage  = (adc_value * ref_voltage) / 1024.0;
      in_voltage = adc_voltage / (R2/(R1+R2));
      mocv = in_voltage;
      lcd.setCursor(3,3);
      lcd.print("VOC :");
      lcd.setCursor(10,3);
      lcd.print(mocv);
      if (mocv < 3.9 && mocv > 3){
        setup();
      }
      pev_state = 1;
    }
    else if(temperature > 40){
      present_state = 2;
      if (present_state != pev_state){
        lcd.clear();
        delay(20);
      }
      digitalWrite(buzzer, HIGH);
      lcd.setCursor(0,0);
      lcd.print("BATTERY TEMPERATURE");
      lcd.setCursor(0,1);
      lcd.print("INCREASE : ");
      lcd.setCursor(13,1);
      lcd.print(temperature);
      lcd.setCursor(0,2);
      lcd.print("RECOMMENDED BATTERY");
      lcd.setCursor(4,3);
      lcd.print(" CHANGE ");
      pev_state = 2;
   }
   else if(soc < 40){
      charge = 1;
      present_state = 3;
      if (present_state != pev_state){
        lcd.clear();
        delay(20);
      }
      lcd.setCursor(0,0);
      lcd.print("CHARGING BATTERY");
      digitalWrite(charge,HIGH);
      startMillis3 = millis();
      if (currentMillis3 - startMillis3 >= period){
          totalCoulumbs = totalCoulumbs + (10);
          soc = soc+((totalCoulumbs/3600.0)/3);
          float temperature = dht.readTemperature();
          lcd.setCursor(0,1);
          lcd.print("SOC :");
          lcd.setCursor(6,1);
          lcd.print(soc);
          lcd.setCursor(0,2);
          lcd.print("TEMPERATURE :");
          lcd.setCursor(15,2);
          lcd.print(temperature);
          startMillis3 = currentMillis3;
          if(soc > 90){
            setup();
            charge1 = 0;
            digitalWrite(charge,LOW);
          }
        }
      pev_state = 3;
   }
}
