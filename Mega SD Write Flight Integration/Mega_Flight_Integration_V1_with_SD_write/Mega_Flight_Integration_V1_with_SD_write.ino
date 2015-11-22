/*
 PAC Flight Code Demo Day
 @ version: 11/21/15
*/

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
#include <SoftwareSerial.h>
#include <Servo.h> 

//Serial
SoftwareSerial arduinoSerial(12,13);

//1.1 Sanity Check
boolean sane = false;

//1.2 LED Declarations

const int LED_GREEN = 36;
const int LED_YELLOW = 38;
const int LED_RED = 40;

//1.3 Temperature Analog-in Declarations
const int TEMP1_PIN = A0;
const int TEMP2_PIN = A1;

//1.4 SD Declarations
int cs_pin = 53;
boolean sd_connected = false;
int nextWrite = millis();

//1.5 IMU Declarations
/* Assign a unique ID to the sensors */
Adafruit_LSM303_Accel_Unified accel = Adafruit_LSM303_Accel_Unified(30301);
Adafruit_LSM303_Mag_Unified   mag   = Adafruit_LSM303_Mag_Unified(30302);
Adafruit_BMP085_Unified       bmp   = Adafruit_BMP085_Unified(18001);
Adafruit_L3GD20_Unified       gyro  = Adafruit_L3GD20_Unified(20);

//1.6 GPS Declarations
SoftwareSerial GPSSerial(10, 11);
//Boundary Box UPDATE DAY OF LAUNCH WITH MOST RECENT SIMULATION
unsigned long minLat = 3970000;
unsigned long maxLat = 4066000;
unsigned long maxLong = 7760000;
unsigned long minLong = 7499000;

//Initialize Location Data
unsigned long lat = -1; 
unsigned long longit = -1;
unsigned long currAlt = -1; //altitude in meters
unsigned long maxAlt = 0; //measures in meters

//1.7 inDryBox Utility
#define SENTENCE_SIZE 75
char sentence[SENTENCE_SIZE];

//1.8 Nichrome Declarations
const int NICHROME_GATE_PIN = 30;
boolean nichromeStarted = false;
unsigned long nichromeEndTime = 0xFFFFFFFFL;
boolean nichromeFinished = false;

const int NICHROME_EXPERIMENT_PIN = 34;
boolean nichromeExperimentStarted = false;
unsigned long nichromeExperimentEndTime = 0xFFFFFFFFL;
boolean nichromeExperimentFinished = false;

int nichromeCounter = 0;

//1.9 Timing Declarations
unsigned int startTime;
unsigned int sanityCheckTime = 0;
boolean initDone;
unsigned int calibrateTime = 10000; //milliseconds until performs sanityCheck

unsigned int redLightBlinkStop;
boolean redLightOn = false;

unsigned int greenLightBlinkStop;
boolean greenLightOn = false;

#define nextWritePeriod 5000 

//Section 2: Setup
void setup() {

 //2.1 Initializations

  arduinoSerial.begin(9600); 
  
  //GPS Serial serial 
  pinMode(10, INPUT); //rx
  pinMode(11, OUTPUT); //tx
  

  Serial.begin(9600); //115200
  
  //2.5 LED Setup
  pinMode(LED_GREEN, OUTPUT);
  digitalWrite(LED_GREEN, HIGH);
  pinMode(LED_YELLOW, OUTPUT);
  digitalWrite(LED_YELLOW, HIGH);
  pinMode(LED_RED, OUTPUT);
  digitalWrite(LED_RED, HIGH);
  
  //SD stuff
  SPI.begin();
  pinMode(cs_pin, OUTPUT);
  if(!SD.begin(cs_pin)){
    //Serial.println("SD not connected");
  }
  else{
    //Serial.println("SD Connected");
    sd_connected = true;
  }
  
//2.2 GPS Setup
  initGPS();
  updateGPS();
  delay(10000);
  updateGPS();
  delay(1000);
  updateGPS();
  delay(1000);
  updateGPS();

//2.3 Nichrome Setup
  initNichrome();
  initNichromeExperiment();

//2.4 IMU Setup
  initIMU();

  delay(10000);

  digitalWrite(LED_GREEN, LOW);
  digitalWrite(LED_YELLOW, LOW);
  digitalWrite(LED_RED, LOW);
  //Serial.println("SETUP DONE");

  if(!sd_connected){
    digitalWrite(LED_YELLOW, HIGH);
  }

}

//Section 3: Loop
void loop() {
  
//3.1 GPS Section
  updateGPS();
  updateMaxAlt();
  String GPSdataString = ""; 
  if (GPSSerial.available())
  {
     String latstr = String(lat);
     String longitstr = String(longit);
     String currALTstr = String(currAlt);
     GPSdataString = latstr + " " + longitstr + " " + currALTstr;
  }
  
//3.2 IMU Section
   String imuData = runIMU();
  String stringForSD = imuData + " " + GPSdataString; // the final string that we print to SD

//3.4 Sanity and Nichrome

  if(!sane){
    sane = sanityCheck();
  } 
  else{
  //Hey should we burn the nichrome or nah?
  nichromeCheck();
  updateNichrome();

  nichromeExperimentCheck();
  updateNichromeExperiment();
  }

//Clock to write to SD and send a string to the trackuino

  if(millis() - nextWrite >= nextWritePeriod){
      arduinoSerial.write(imuData.c_str());
      SDLog(stringForSD); 
      nextWrite += nextWritePeriod; 
  }

} //end of loop




