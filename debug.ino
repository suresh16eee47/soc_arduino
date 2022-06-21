
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

// voltage sensor input "ANALOG_IN_PIN" A1
// current sensor input "currentPin" A0
// buzzer "buzzer" 4
// (on/off relay, VOC)"relay1" 2
// (charging relay) "relay2" 5
// temperature sensor "DHTPIN" 3

//Current sensor variables
const int currentPin = A0;
int sensitivity = 100;
int adcValue= 0;
int offsetVoltage = 2500;
double adcVoltage = 0;
double currentValue = 0;
String charge_state = "";

// voltage sensor variables
#define ANALOG_IN_PIN A1
#define relay1 2
#define relay2 5
float adc_voltage = 0.0000000;
float in_voltage = 0.0000000;
float R1 = 30000.0000000;
float R2 = 7500.000000; 
float ref_voltage = 5.0000000;
int adc_value = 0;
int present_state = 0;
int pev_state = 0;
int charge1 = 0;
unsigned long start_charge;
unsigned long current_charge;


//Current sensor variables
unsigned int x=0;
float AcsValue=0.0,Samples=0.0,AvgAcs=0.0,AcsValueF=0.0;

unsigned long charge_current_millis;
unsigned long charge_start_millis; 
unsigned long discharge_start_millis;  //some global variables available anywhere in the program
unsigned long discharge_current_millis;
const int period = 1000 ;  //the value is a number of milliseconds
const byte ledPin = 13;    //using the built in LED
float current = 0.50000000;
unsigned long c_current = 0.500000;
float totalCoulumbs = 0.0000000;
float soc = 0.0;
float ocv = 0;
float mocv=0;



int measure_voltage(){
  adc_value = analogRead(ANALOG_IN_PIN);
  adc_voltage  = (adc_value * ref_voltage) / 1024.0;
  in_voltage = adc_voltage / (R2/(R1+R2));
}

int measure_Current(){
  adcValue = analogRead(currentPin);
  adcVoltage = adcValue*5/1023.0;
  currentValue = (adcVoltage-2.5)/0.100;
}


float open_circuit_voltage(){
  lcd.init();
  digitalWrite(relay1, HIGH);
  digitalWrite(relay2, LOW);
  measure_voltage();
  delay(2000);
  mocv = in_voltage;
  Serial.println("open circuit voltage :");
  Serial.println(mocv);
  if (mocv > 3.71 && mocv < 4.2){
    if (mocv > 3.72){
      mocv = 3.71;
    }
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
  else if((mocv <= 2.29) && (mocv >= 1)){
    soc = 0;
  }
  else{
    soc = 0.0001;
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
  if (soc>=40){
    charge_state = "start_discharge";
  }
  else if(soc<40){
    charge_state = "start_charge";
  }
}


void setup() {
  espSerial.begin(115200);
  pinMode(buzzer, OUTPUT);
  dht.begin();
  Serial.begin(115200);
  pinMode(relay1, OUTPUT);
  pinMode(relay2, OUTPUT);
  digitalWrite(relay1, HIGH);
  digitalWrite(relay2, LOW);
  open_circuit_voltage();
  delay(2000);
  digitalWrite(relay1, LOW);  //start Serial in case we need to print debugging info
  pinMode(ledPin, OUTPUT);
  lcd.clear();

}

void loop() {
    //get the current "time" (actually the number of milliseconds since the program started)
  if(charge_state == "start_discharge"){
    discharge_current_millis = millis();
    charge_state = "discharging";
  }
  else if(charge_state == "start_dcharge"){
    charge_current_millis = millis();
    charge_state = "charging";
  }
  float temperature = dht.readTemperature();
  delay(20);
  if ( soc != 0.001 && temperature < 40 && charge_state == "discharging")  //test whether the period has elapsed
    {
      digitalWrite(relay1,LOW);
      digitalWrite(relay2,LOW);
      measure_Current();
      if(currentValue < 0){
        Serial.println("Current sensor swapped");
        lcd.setCursor(0,0);
        lcd.print("current sensor swap");
        delay(5000);
      }
      espSerial.println("suresh");
      Serial.println("Measured current : ");
      Serial.print(currentValue);
      if(currentValue < 0.1){
        currentValue = 0;
      }
      present_state = 0;
      if (present_state != pev_state){
        lcd.clear();
        delay(20);
      }
      digitalWrite(buzzer, LOW);
      lcd.setCursor(0,2);
      lcd.print("CURRENT(A) :");
      lcd.setCursor(13,2);
      lcd.print(currentValue,2);
      totalCoulumbs = totalCoulumbs +(currentValue);
      soc = soc -((totalCoulumbs/3600.0)/3);
      lcd.setCursor(11,3);
      lcd.print("DISCHARGE");
      lcd.setCursor(0,0);
      lcd.print("COLUMBS:");
      lcd.setCursor(8,0);
      lcd.print(totalCoulumbs,0);
      lcd.setCursor(0,3);
      lcd.print("SOC :");      
      lcd.setCursor(5,3);
      lcd.print(soc,1);
      lcd.setCursor(14,0);
      lcd.print(temperature,1);
      lcd.setCursor(19,0);
      lcd.print("C");
      lcd.setCursor(0,1);
      lcd.print("CONSUMED Ah :");
      lcd.setCursor(14,1);
      lcd.print(totalCoulumbs/3600.0);
      pev_state = 0;
      if (soc <= 40){
        charge_current_millis = millis();
        charge_start_millis = 0;
        charge_state = "charging";
      }
      discharge_current_millis = discharge_start_millis;
      delay(500);
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
      digitalWrite(relay1,HIGH);
      digitalWrite(relay2,HIGH);
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
   else if(charge_state == "charging" && temperature < 40){
      present_state = 3;
      charge_state = "charging";
      if (present_state != pev_state){
        lcd.clear();
        delay(20);
      }
      lcd.setCursor(0,0);
      lcd.print("CHARGING BATTERY");
      digitalWrite(relay2,HIGH);
      if (charge_state == "charging"){
          measure_Current();
          if(currentValue > 0){
            Serial.println("Current sensor swapped");
            lcd.setCursor(0,0);
            lcd.print("current sensor swap");
            delay(5000);
          }
          if (currentValue < 0){
            if(-currentValue < 0.199){
              currentValue = 0;
            }
            totalCoulumbs = totalCoulumbs -(-currentValue);
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
            discharge_start_millis = discharge_current_millis;
            if(soc > 90){
              charge_state = "discharging";
              totalCoulumbs = 0;
              digitalWrite(relay2,LOW);
              charge_current_millis = millis();
              charge_start_millis = 0;
            }
          }  
        }
      pev_state = 3;
      delay(500);
   }
}
