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
#include <SoftwareSerial.h>
#include <Servo.h> 

//Serial
SoftwareSerial arduinoSerial(12,13);
SoftwareSerial GPSSerial(10, 11);

//1.1 Sanity Check --Inits should be opposite of what we want at beginning
boolean initSane = false;
boolean inBdry = false;
boolean falling = true;
boolean GPSgivingDataInitially = false;
boolean softwareSerialsInitially = false;
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
unsigned long minLat = 3970000;
unsigned long maxLat = 4066000;
unsigned long maxLong = 7760000;
unsigned long minLong = 7499000;

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
unsigned long smAlt = -1; //altitude in meters

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
  arduinoSerial.begin(9600); 
  Serial.begin(9600);
  Serial.println(arduinoSerial.isListening()); //Debug
  arduinoSerial.write("Hello Uno"); 
  
  GPSSerial.begin(9600);
  SPI.begin();

//I don't think this is necessary, keeping for now --Antonio
//  //GPS Serial serial 
//  pinMode(10, INPUT); //rx
//  pinMode(11, OUTPUT); //tx
  
  //LED Setup
  pinMode(LED_GREEN, OUTPUT);
  digitalWrite(LED_GREEN, HIGH);
  pinMode(LED_YELLOW, OUTPUT);
  digitalWrite(LED_YELLOW, HIGH);
  pinMode(LED_RED, OUTPUT);
  digitalWrite(LED_RED, HIGH);
  
  //SD stuff
  pinMode(cs_pin, OUTPUT);
  
  //GPS Setup
  for(int i = 0; i < 10; i++){
  updateGPS();
  delay(1000);
  }

  //Nichrome Setup
  initNichrome();
  initNichromeExperiment();

  //IMU Setup
  initIMU();
  delay(10000);

  //Turn off Lights
  digitalWrite(LED_GREEN, LOW);
  digitalWrite(LED_YELLOW, LOW);
  digitalWrite(LED_RED, LOW);
}

//===========================================
void loop() {
  Serial.println("In loop"); //Debug
  while(!initSane){
    initSane = initiallySane();
    //Debug
    Serial.println(arduinoSerial.available());
    arduinoSerial.write("Mega Here"); //Debug
    delay(1000);
    while(arduinoSerial.available() > 0){
      Serial.print((arduinoSerial.read()));
    }
    delay(1000);
  //End Debug
  }
  updateGPS();
  updateMaxAlt();
  
  String imuData = runIMU();

  //Smoothing
  smooth(); 

  //Needs to be after smoothing -- AND TRACKUINO STUFF
  String stringForSD = imuData + "," + GPStoString(); // the final string that we print to SD
  nichromeCheck();
  updateNichrome();
  nichromeExperimentCheck();
  updateNichromeExperiment();
  if(millis() - nextWrite >= nextWritePeriod){
    arduinoSerial.write(imuData.c_str());
    SDLog(stringForSD); 
    nextWrite += nextWritePeriod; 
    }

}
