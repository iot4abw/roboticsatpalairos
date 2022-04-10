//================================================================================
// Γυμνάσιο Παλαίρου με Λυκειακές Τάξεις
// Ν. Αιτωλοακαρνανίας
// Σχολικό Έτος 2021 - 2022
// Ομάδα Ρομποτικής R@P - Robotics at Palairos
//================================================================================

//================================================================================
// LIBRARIES
//================================================================================

//Libraries
#include <Wire.h>
#include <RTClib.h>
#include <LiquidCrystal_I2C.h>
#include <dht.h>


//Init libraries objects
RTC_DS1307 rtc;
LiquidCrystal_I2C lcd(0x27, 16, 2);
dht DHT;

//================================================================================
// CONSTANTS
//================================================================================

//Constants for temperature and humidity
#define DHT21_PIN 7

//Constants for date and time
const long interval = 6000;  // Read data from DHT every 6 sec
const int btSet = 8;
const int btUp = 9;
const int btDown = 10;

//================================================================================
// VARIABLES
//================================================================================

//Variables for temperature and humidity
int chk, temp, hum;

//Variables for date and time
int DD,MM,YY,H,M,S, set_state, up_state, down_state;
int btnCount = 0;
unsigned long previousMillis = 0;
unsigned long currentMillis; 
String sDD;
String sMM;
String sYY;
String sH;
String sM;
String sS;
boolean backlightON = true;
boolean setupScreen = false;

//================================================================================
//SETUP
//================================================================================

void setup () {
  //scan address for lcd monitor
  scanaddress();
  
  pinMode(btSet, INPUT_PULLUP);
  pinMode(btUp, INPUT_PULLUP);
  pinMode(btDown, INPUT_PULLUP);

  //Αρχικοποίηση οθόνης lcd
  lcd.begin(16,2);   // Init the LCD for 16 chars 2 lines
  lcd.backlight();   // Turn on the backligt (try lcd.noBacklight() to turn it off)
  lcd.setCursor(0,0); //πρώτη στήλη, πρώτη γραμμή - ξεκινάει από το 0
  lcd.print("The R@P team!");  // ένα μήνυμα
  lcd.setCursor(0,1); //πρώτη στήλη, δεύτερη γραμμή
  lcd.print("Year: 2021-2022"); // ένα μήνυμα
  delay(1000);
  lcd.clear();  
  
  if (! rtc.begin()) {
    lcd.print("Couldn't find RTC");
    lcd.clear();
    while (1);
  }
  if (! rtc.isrunning()) {
     lcd.print("RTC is NOT running!");
     // Ρυθμίζει την ώρα/ ημερομηνία σύμφωνα με την ώρα/ ημερομηνία που έγινε η μεταγλώττιση του κώδικα
     rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
  // Ρυθμίζει την ώρα/ ημερομηνία σύμφωνα με την ώρα/ ημερομηνία που έγινε η μεταγλώττιση του κώδικα
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
}

//================================================================================
//LOOP 
//================================================================================

void loop () {
  currentMillis = millis();
  readBtns();

  //code for date and time
  getTimeDate();
    
  //code for temperature and humidity
  getTempHum();
  
  
  if (!setupScreen){
    lcdPrintDateTime();
    lcdPrintTempHum();
  }
  else{
    lcdSetup();
  }
}

//================================================================================
//Read buttons
//================================================================================

void readBtns(){
  set_state = digitalRead(btSet);
  up_state = digitalRead(btUp);
  down_state = digitalRead(btDown);
  
  //Turn backlight on/off by pressing the down button
  if (down_state==LOW && btnCount==0){
    
    if (backlightON){
      lcd.noBacklight();
      backlightON = false;
    }
    else{
      lcd.backlight();
      backlightON = true;
    }
    delay(500);
  }
  if (set_state==LOW){
    if(btnCount<5){
      btnCount++;
      
      setupScreen = true;
        if(btnCount==1){
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("------SET------");
          lcd.setCursor(0,1);
          lcd.print("-TIME and DATE-");
          delay(2000);
          lcd.clear();
          Serial.println('case 2.1');
        }
    }
    else{
      lcd.clear();
      rtc.adjust(DateTime(YY, MM, DD, H, M, 0));
      lcd.print("Saving....");
      delay(2000);
      lcd.clear();
      setupScreen = false;
      btnCount=0;
    }
    delay(500);
  }
}

//================================================================================
//Read time and date from rtc ic
//================================================================================

void getTimeDate(){
  if (!setupScreen){
  DateTime now = rtc.now();
  DD = now.day();
  MM = now.month();
  YY = now.year();
  H = now.hour();
  M = now.minute();
  }
  //Make some fixes...
  if (DD<10){ sDD = '0' + String(DD); } else { sDD = DD; }
  if (MM<10){ sMM = '0' + String(MM); } else { sMM = MM; }
  sYY=YY;
  if (H<10){ sH = '0' + String(H); } else { sH = H; }
  if (M<10){ sM = '0' + String(M); } else { sM = M; } 
}

//print Date and Time
void lcdPrintDateTime(){
  lcd.setCursor(0,0); //First row
  lcd.print(sH);
  lcd.print(":");
  lcd.print(sM);
  lcd.print(" ");
  lcd.print(sDD);
  lcd.print("/");
  lcd.print(sMM);
  lcd.print("/");
  lcd.print(sYY);
  delay(1000);
}

//================================================================================
//Read temperature and humidity every 6 seconds from DHT sensor
//================================================================================

void getTempHum(){
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;    
    chk = DHT.read21(DHT21_PIN);
    temp = DHT.temperature;
    hum = DHT.humidity;
  }
}

//================================================================================
//print Temperature and Humidity
//================================================================================

void lcdPrintTempHum(){
  lcd.setCursor(0,1);
  lcd.print("Tem:");
  lcd.print(temp);
  lcd.print((char)223);
  lcd.print("C");
  lcd.print(" Hum:");
  lcd.print(hum);
  lcd.print("%");
  delay(1000);
}

//================================================================================
//Setup screen
//================================================================================

void lcdSetup(){
  if (btnCount==1){
    lcd.setCursor(0,1);
    lcd.print(">"); 
    if (up_state == LOW){
      if (H<23){
        H++;
      }
      else {
        H=0;
      }
      delay(500);
    }
    if (down_state == LOW){
      if (H>0){
        H--;
      }
      else {
        H=23;
      }
      delay(500);
    }
  }
  else if (btnCount==2){
    lcd.setCursor(0,1);
    lcd.print(" ");
    lcd.setCursor(3,1);
    lcd.print(">");
    if (up_state == LOW){
      if (M<59){
        M++;
      }
      else {
        M=0;
      }
      delay(500);
    }
    if (down_state == LOW){
      if (M>0){
        M--;
      }
      else {
        M=59;
      }
      delay(500);
    }
  }
  else if (btnCount==3){
    lcd.setCursor(3,1);
    lcd.print(" ");
    lcd.setCursor(6,1);
    lcd.print(">");
    if (up_state == LOW){
      if (DD<31){
        DD++;
      }
      else {
        DD=1;
      }
      delay(500);
    }
    if (down_state == LOW){
      if (DD>1){
        DD--;
      }
      else {
        DD=31;
      }
      delay(500);
    }
  }
  else if (btnCount==4){
    lcd.setCursor(6,1);
    lcd.print(" ");
    lcd.setCursor(9,1);
    lcd.print(">");
    if (up_state == LOW){
      if (MM<12){
        MM++;
      }
      else {
        MM=1;
      }
      delay(500);
    }
    if (down_state == LOW){
      if (MM>1){
        MM--;
      }
      else {
        MM=12;
      }
      delay(500);
    }
  }
  else if (btnCount==5){
    lcd.setCursor(9,1);
    lcd.print(" ");
    lcd.setCursor(12,1);
    lcd.print(">");
    if (up_state == LOW){
      if (YY<2999){
        YY++;
      }
      else {
        YY=2000;
      }
      delay(500);
    }
    if (down_state == LOW){
      if (YY>2000){
        YY--;
      }
      else {
        YY=2999;
      }
      delay(500);
    }
  }
  lcdPrintDateTime();
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
