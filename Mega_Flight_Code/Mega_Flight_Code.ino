//Maximum Runtime ~50 days due to millis() resetting

//IMU
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_LSM303_U.h>
#include <Adafruit_BMP085_U.h>
#include <Adafruit_L3GD20_U.h>
#include <Adafruit_10DOF.h>
#include <Time.h>

//SD
#include <SD.h>
#include <SPI.h>

//Serial
#include <SoftwareSerial.h>

//IMU
String imuDataString = "";
Adafruit_LSM303_Accel_Unified accel = Adafruit_LSM303_Accel_Unified(30301);
Adafruit_LSM303_Mag_Unified   mag   = Adafruit_LSM303_Mag_Unified(30302);
Adafruit_BMP085_Unified       bmp   = Adafruit_BMP085_Unified(18001);
Adafruit_L3GD20_Unified       gyro  = Adafruit_L3GD20_Unified(20);

//Position
//Boundary Box UPDATE DAY OF LAUNCH WITH MOST RECENT SIMULATION
long int minLat = 40010950;//xx°xx.xxxx'
long int maxLat = 40337066;
long int minLong = 75462700;
long int maxLong = 77044366;
long int maxWantedAlt = 31000; //Maximum wanted altitude (BDRY)

//Initialize Location Data
long int lat = -1; 
long int longit = -1;
long int alt = -1; //altitude in meters
long int maxAlt = 0; //measures in meters

//GPS Utility
String field = "";
String slat = "";
String slongit = "";
String salt = "";
boolean gooddata = false;

//SD
int CSPIN = 53;

//Sanity
boolean sane = false;
boolean inBdry = false;
boolean falling = true;
boolean initSD = false;

//LED
const int LED_GREEN = 23;
const int LED_YELLOW = 25;
const int LED_RED = 27;
 
boolean onLED = false;

//Timing
unsigned long int nextWrite1 = 0;
unsigned long int nextWrite5 = 0;

//Nichrome
const int NICHROME_PIN = 31;
boolean nichromeStarted = false;
boolean nichromeFinished = false;
unsigned long int nichromeEndTime = 0;
int nichromeCounter = 0;

//Nichrome Experiment
const int NICHROME_EX_PIN = 29;
boolean nichromeEXStarted = false;
boolean nichromeEXFinished = false;
unsigned long int nichromeEXEndTime = 0;

//Serial
SoftwareSerial ardSerial(10,11); //rx tx
void setup() {
  Serial.begin(9600);
  Serial1.begin(9600); //GPS
  ardSerial.begin(9600); //Arduino
  
  //LED
  pinMode(LED_GREEN, OUTPUT); digitalWrite(LED_GREEN, HIGH);
  pinMode(LED_YELLOW, OUTPUT); digitalWrite(LED_YELLOW, HIGH);
  pinMode(LED_RED, OUTPUT); digitalWrite(LED_RED, HIGH);

  //IMU
  initIMU();
  delay(1000);

  //Nichrome
  pinMode(NICHROME_PIN, OUTPUT);
  digitalWrite(NICHROME_PIN, LOW);
  pinMode(NICHROME_EX_PIN, OUTPUT);
  digitalWrite(NICHROME_EX_PIN, LOW);
  //TCCR2B &= B11111001;//increase PWM frequency

  //End Setup
  digitalWrite(LED_GREEN, LOW);
  digitalWrite(LED_YELLOW, LOW);

  nextWrite5 = millis() + 5000;
  nextWrite1 = millis() + 1000;
}

  unsigned long int gpsMaxSearchTime = millis() + 5000;

void loop() {
  while(!sane){
    sane = initSane();
      readGPS();
      Serial.print(lat); Serial.print(", "); Serial.print(longit); Serial.print(", "); Serial.println(alt); 

  }
  
  readGPS();
  //Serial.print(lat); Serial.print(", "); Serial.print(longit); Serial.print(", "); Serial.println(alt); 

  runIMU();
  Serial.println(imuDataString);

  nichromeCheck();
  nichromeEXCheck();
  
  //Time Controlled: SD, Serial LED
  if(millis() >= nextWrite5){
    //SD and Arduino
    String logString = (String)longit + "," + (String)lat + "," + (String)alt + ", " + imuDataString;
    Serial.println(logString);
    ardSerial.print(imuDataString);
    sdLog(logString); 
    
    nextWrite5 = millis() + 5000;
  }

  if(millis() >= nextWrite1){
    if(onLED){
      digitalWrite(LED_GREEN, LOW);
      onLED = false;
    }
    else{
      digitalWrite(LED_GREEN, HIGH);
      onLED = true;
    }
    nextWrite1 = millis() + 1000;
  }
}


