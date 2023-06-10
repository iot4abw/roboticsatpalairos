//================================================================================
// Γυμνάσιο Παλαίρου με Λυκειακές Τάξεις
// Ν. Αιτωλοακαρνανίας
// Σχολικό Έτος 2022 - 2023
// Ομάδα Ρομποτικής R@P - Robotics at Palairos
//================================================================================

//================================================================================
// LIBRARIES
//================================================================================

//Libraries
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <OneWire.h>
#include <DallasTemperature.h>

// Define to which pin of the Arduino the 1-Wire bus is connected:
#define ONE_WIRE_BUS 2
// Create a new instance of the oneWire class to communicate with any OneWire device:
OneWire oneWire(ONE_WIRE_BUS);
// Pass the oneWire reference to DallasTemperature library:
DallasTemperature sensors(&oneWire);

//Init libraries objects
LiquidCrystal_I2C lcd(0x27, 16, 2);


//================================================================================
// VARIABLES
//================================================================================

//Variables for Ultrasonic
int echoPin = 12; // Echo Pin of Ultrasonic Sensor
int trigPin = 13; // Trigger Pin of Ultrasonic Sensor
int maximumRange = 38; // the height of overflow valve
int minimumRange = 1;
int overflowRange = 3;
int fullRange = 20;
long duration, distance;

//Variables for Moisture Sensor
int mois;
int mois_per_cent;

//Variables for Moisture Sensor
int solenoidPin = 4; //This is the output pin on the Arduino we are using

//================================================================================
//SETUP
//================================================================================
void setup () {
  //scan address for lcd monitor
  scanaddress();

  //ultrasonic sensor
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  // solenoid valve:
  pinMode(solenoidPin, OUTPUT);
 
  //lcd initialization
  lcd.begin(16,2);   // Init the LCD for 16 chars 2 lines
  lcd.backlight();   // Turn on the backligt (try lcd.noBacklight() to turn it off)
  lcd.setCursor(0,0); //πρώτη στήλη, πρώτη γραμμή - ξεκινάει από το 0
  lcd.print("The R@P team!");  // ένα μήνυμα
  lcd.setCursor(0,1); //πρώτη στήλη, δεύτερη γραμμή
  lcd.print("Year: 2022-2023"); // ένα μήνυμα
  delay(1000);
  lcd.clear();  
  
}

//================================================================================
//LOOP 
//================================================================================
void loop () {
  
  //code for temperature
  gettemp();
  
  //code for tank level
  getDistance();

  lcd.clear();

  //code for moisture and solenoid valve - smart watering
  getMoist();

 }



//================================================================================
//Read and pring Temperature every 6 seconds from sensor
//================================================================================
void gettemp() {
  // Send the command for all devices on the bus to perform a temperature conversion:
  sensors.requestTemperatures();
  // Fetch the temperature in degrees Celsius for device index:
  float temp = sensors.getTempCByIndex(0); // the index 0 refers to the first device
  lcd.setCursor(0,0);
  lcd.print("Temp:");
  lcd.print(temp);
  lcd.print((char)223);
  lcd.print("C");
  delay(2500);
}

//================================================================================
//Read distance every 6 seconds from the sensor
//================================================================================
void getDistance(){
  
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = (duration / 59); // a deviation should be calculated right here
  lcdPrintDistance();  
}

//================================================================================
//print Distance
//================================================================================
void lcdPrintDistance(){
   lcd.setCursor(0,1);
   if (distance >= maximumRange || distance <= minimumRange){
     lcd.print("Out of range!");
   }
   else   if ( distance <= overflowRange){
      lcd.print("Tank is full!");
      }
    else {
      lcd.print("Tank Water: "); 
      lcd.print(distance/fullRange*100);
      lcd.print("%");
      
      }
   delay(2500);   
}

//================================================================================
//Read moisture every 6 seconds from the sensor
//================================================================================
void getMoist(){
  
  //the sensor will give the percentage value of the moisture
  //The sensor gives value from 0 to 1023 and microcontroller will map the value to the percentage from 0 to 100.
  mois = analogRead(A0);
  mois_per_cent = map(mois, 0, 1023, 100, 0);
  lcdPrintMoist(); 

}  
      
//================================================================================
//print Distance
//================================================================================
void lcdPrintMoist(){      
  
  lcd.setCursor(0,0);
  lcd.print("Humidity: ");
  lcd.print(mois_per_cent);
  lcd.print("%");
  lcd.setCursor(0,1);
   
  if(mois_per_cent <= 0) {
      
      lcd.print("Sensor out of Ground");
      delay(50);
      }
       else if(mois_per_cent <= 55) { 
      
          lcd.print("Dry Ground");
          solenoid_valve_on;
          lcd.print("/ON");
          delay(50);
          lcd.clear();
          solenoid_valve_off;
          lcd.print("Valve --OFF--");
        }
       else if (mois_per_cent <= 90) {
          lcd.clear();
          lcd.print("Humid Ground"); 
          solenoid_valve_off;
          lcd.print("/OFF");
          delay(50);

        }
  else {
      lcd.clear();
      lcd.print("FLOOD!!!");
      solenoid_valve_off;
      lcd.print("/OFF");
      delay(50);
  }
     delay(1000);

}

//================================================================================
//solenoid valve
//================================================================================
void solenoid_valve_on(){      
  
  digitalWrite(solenoidPin, HIGH); //Switch Solenoid ON
  delay(1000); //Wait 1 Second
  digitalWrite(solenoidPin, LOW); //Switch Solenoid OFF
  delay(1000); //Wait 1 Second

}  

void solenoid_valve_off(){
  digitalWrite(solenoidPin, LOW); //Switch Solenoid OFF
  delay(1000); //Wait 1 Second
}

//================================================================================
//Setup screen
//================================================================================

void lcdSetup(){
    lcd.setCursor(0,1);

}

//================================================================================
//scan address
//================================================================================

void scanaddress(){
  Serial.begin (9600);
  Serial.println ("I2C scanner. Scanning ...");
  byte count = 0;
  
  Wire.begin();
  for (byte i = 8; i < 120; i++)
  {
    Wire.beginTransmission (i);
    if (Wire.endTransmission () == 0)
      {
      Serial.print ("Found address: ");
      Serial.print (i, DEC);
      Serial.print (" (0x");
      Serial.print (i, HEX);
      Serial.println (")");
      count++;
      delay (1);  // maybe unneeded?
      } // end of good response
  } // end of for loop
  Serial.println ("Done.");
  Serial.print ("Found ");
  Serial.print (count, DEC);
  Serial.println (" device(s).");
}
