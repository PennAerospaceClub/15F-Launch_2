/*
 PAC Flight Code Demo Day
 @ version: 11/21/15
*/

//Note: Update max and min geo boundaries and launch location day of launch in 1.6

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_LSM303_U.h>
#include <Adafruit_BMP085_U.h>
#include <Adafruit_L3GD20_U.h>
#include <Adafruit_10DOF.h>
#include <Time.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <SD.h>
#include <SPI.h>
#include <Servo.h> 
#include <SoftwareSerial.h>

//1.0 Software Serial Declaration

//SoftwareSerial gpsSerial(12,13); //rx,tx 
SoftwareSerial ardSerial(10,11); //rx tx

//1.1 Sanity Check --Inits should be opposite of what we want at beginning
boolean initSane = false;
boolean inBdry = false;
boolean falling = true;
boolean GPSgivingDataInitially = false;
boolean sd_connected = false;

//1.2 LED Declarations
const int LED_GREEN = 23;
const int LED_YELLOW = 25;
const int LED_RED = 27;

unsigned int redLightBlinkStop;
boolean redLightOn = false;
unsigned int greenLightBlinkStop;
boolean greenLightOn = false;

//1.3 Temperature Analog-in Declarations
//const int TEMP1_PIN = A0;
//const int TEMP2_PIN = A1;

//1.4 SD Declarations
int cs_pin = 53;
int nextWrite = millis();

//1.5 IMU Declarations
Adafruit_LSM303_Accel_Unified accel = Adafruit_LSM303_Accel_Unified(30301);
Adafruit_LSM303_Mag_Unified   mag   = Adafruit_LSM303_Mag_Unified(30302);
Adafruit_BMP085_Unified       bmp   = Adafruit_BMP085_Unified(18001);
Adafruit_L3GD20_Unified       gyro  = Adafruit_L3GD20_Unified(20);

//1.6 GPS Declarations
//Boundary Box UPDATE DAY OF LAUNCH WITH MOST RECENT SIMULATION
unsigned long minLat = 39500000;
unsigned long maxLat = 41000000;
unsigned long minLong = 7499000;
unsigned long maxLong = 7760000;

//Holder for previous data point
unsigned long prevLat = -1; 
unsigned long prevLong = -1;
unsigned long prevAlt = -1; //altitude in meters

//Maximum Deviation for each variable in between readings (NEEDS TESTS, used last years deviations)
unsigned long devLat = 3000; 
unsigned long devLong = 3000;
unsigned long devAlt = 1200;

//Initialize Location Data
unsigned long lat = -1; 
unsigned long longit = -1;
unsigned long currAlt = -1; //altitude in meters
unsigned long maxAlt = 0; //measures in meters

//First Reference Data Points for Smoothing UPDATE DAY OF LAUNCH WITH LAUNCH LOCATION (updated to previous good data point)
unsigned long smLat = -1; 
unsigned long smLong = -1;
unsigned long smAlt = 0; //altitude in meters

//1.7 inDryBox Utility
#define SENTENCE_SIZE 75
char sentence[SENTENCE_SIZE];

//1.8 Nichrome Declarations
const int NICHROME_GATE_PIN = 29;
boolean nichromeStarted = false;
unsigned long nichromeEndTime = 0xFFFFFFFFL;
boolean nichromeFinished = false;

const int NICHROME_EXPERIMENT_PIN = 31;
boolean nichromeExperimentStarted = false;
unsigned long nichromeExperimentEndTime = 0xFFFFFFFFL;
boolean nichromeExperimentFinished = false;

int nichromeCounter = 0;

//1.9 Timing Declarations
unsigned int startTime;
unsigned int sanityCheckTime = 0;
boolean initDone;
unsigned int calibrateTime = 10000; //milliseconds until performs sanityCheck
#define nextWritePeriod 5000 //The period between SD and Trackuino writes; 5 seconds

//1.10 Smoothing Variables
unsigned long currGlobal;
unsigned long prevGlobal;

//===========================================

//Section 2: Setup
void setup() {
//Initializations
  
  Serial.begin(9600); //Serial
  Serial1.begin(9600);
  //needs to be in this order
  //gpsSerial.begin(9600); //GPS
  //ardSerial.begin(9600); 
  //SPI.begin(); //SD Card Debug Antonio
  
  //LED Setup
//  pinMode(LED_GREEN, OUTPUT);
//  digitalWrite(LED_GREEN, HIGH);
//  pinMode(LED_YELLOW, OUTPUT);
//  digitalWrite(LED_YELLOW, HIGH);
//  pinMode(LED_RED, OUTPUT);
//  digitalWrite(LED_RED, HIGH);
  
  //SD stuff
  //pinMode(cs_pin, OUTPUT); Debug Antonio
  
  //GPS Setup
//  for(int i = 0; i < 10; i++){
//  updateGPS();
//  Serial.print(currAlt);
//  Serial.print(", ");
//  Serial.print(lat);
//  Serial.print(", ");
//  Serial.println(longit);
//  delay(500); //debug
//  }

  //Nichrome Setup
  //initNichrome(); Debug Antonio
  //initNichromeExperiment(); Debug Antonio

  //IMU Setup
  //initIMU();
  //delay(1000);  //debug

  //Turn off Lights
//    digitalWrite(LED_GREEN, LOW);
//    digitalWrite(LED_YELLOW, LOW);
//    digitalWrite(LED_RED, LOW);
}

//===========================================

boolean gotData = false;
String imuDataString = "";

void loop() {
Serial.println("Loop");
  
    while(!initSane){
      updateGPS();
      initSane = initiallySane();
    }
    while(!gotData){
      updateGPS();  
    }    
    if(gotData){
    Serial.print(currAlt); Serial.print(", "); Serial.print(", "); Serial.println(longit);
    //smooth(); 
    //updateMaxAlt(); //requires smoothing
    //Serial.print("prev: "); Serial.print(prevAlt); Serial.print(", "); Serial.print(prevLat); Serial.print(", "); Serial.println(prevLong);
    //Serial.print("sm: "); Serial.print(smAlt);Serial.print(", "); Serial.print(smLat); Serial.print(", "); Serial.println(smLong);
    
    gotData = false;
    } 
    
    //delay(10);
    //Serial.print("A");
    //runIMU();
    //Serial.print("B");

  //Smoothing
  //smooth(); 
  //updateMaxAlt(); //requires smoothing

  //Serial.print(currAlt); Serial.print(", "); Serial.print(lat); Serial.print(", "); Serial.println(longit);
  //Serial.print("prev: "); Serial.print(prevAlt); Serial.print(", "); Serial.print(prevLat); Serial.print(", "); Serial.println(prevLong);
  //Serial.print("sm: "); Serial.print(smAlt);Serial.print(", "); Serial.print(smLat); Serial.print(", "); Serial.println(smLong);

  //delay(100);
  
//
//  //Needs to be after smoothing -- AND TRACKUINO STUFF
//  String stringForSD = imuData + "," + GPStoString(); // the final string that we print to SD
//  nichromeCheck();
//  updateNichrome();
//  nichromeExperimentCheck();
//  updateNichromeExperiment();
//  if(millis() - nextWrite >= nextWritePeriod){
//    Serial1.write(imuData.c_str());
//    SDLog(stringForSD); 
//    nextWrite += nextWritePeriod; 
//    }

}
