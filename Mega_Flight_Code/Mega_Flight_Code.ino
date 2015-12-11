#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_LSM303_U.h>
#include <Adafruit_BMP085_U.h>
#include <Adafruit_L3GD20_U.h>
#include <Adafruit_10DOF.h>
#include <Time.h>
#include <SD.h>
#include <SPI.h>


//IMU
String imuDataString = "";
Adafruit_LSM303_Accel_Unified accel = Adafruit_LSM303_Accel_Unified(30301);
Adafruit_LSM303_Mag_Unified   mag   = Adafruit_LSM303_Mag_Unified(30302);
Adafruit_BMP085_Unified       bmp   = Adafruit_BMP085_Unified(18001);
Adafruit_L3GD20_Unified       gyro  = Adafruit_L3GD20_Unified(20);

//Position
//Boundary Box UPDATE DAY OF LAUNCH WITH MOST RECENT SIMULATION
long int minLat = 39500000; //xx°xx'xx.xx"
long int maxLat = 41000000; //xx°xx'xx.xx"
long int minLong = 74990000; //xx°xx'xx.xx"
long int maxLong = 77600000; //xx°xx'xx.xx"
long int maxWantedAlt = 29000; //Maximum wanted altitude (BDRY)

//Holder for previous data point
long int prevLat = -1; 
long int prevLong = -1;
long int prevAlt = -1;

//Maximum Deviation for each variable in between readings (NEEDS TESTS, used last years deviations)
long int devLat = 3000; 
long int devLong = 3000;
long int devAlt = 1200;

//Initialize Location Data
long int lat = -1; 
long int longit = -1;
long int alt = -1; //altitude in meters
long int maxAlt = 0; //measures in meters

//First Reference Data Points for Smoothing UPDATE DAY OF LAUNCH WITH LAUNCH LOCATION (updated to previous good data point)
long int smLat = -1; 
long int smLong = -1;
long int smAlt = 0;

//GPS Utility
String field = "";
String slat = "";
String slongit = "";
String salt = "";
boolean gooddata = false;

//SD
int cspin = 53;
long int nextWrite = 0;

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
long int nextLED = 0;

void setup() {
  Serial.begin(9600);
  Serial1.begin(9600); //GPS
  
  //LED
  pinMode(LED_GREEN, OUTPUT); digitalWrite(LED_GREEN, HIGH);
  pinMode(LED_YELLOW, OUTPUT); digitalWrite(LED_YELLOW, HIGH);
  pinMode(LED_RED, OUTPUT); digitalWrite(LED_RED, HIGH);

  //IMU
  initIMU();
  delay(1000);
  nextWrite = second() + 5;

  //End Setup
  digitalWrite(LED_GREEN, LOW);
  digitalWrite(LED_YELLOW, LOW);
}

void loop() {
  while(!sane){
    sane = initSane();
    Serial.print(".");
  }
  
  readGPS();
  Serial.print(lat); Serial.print(", "); Serial.print(longit); Serial.print(", "); Serial.println(alt); 

  runIMU();
  Serial.println(imuDataString);

  //Time Controlled: SD, Serial LED
  if(second() >= nextWrite){
    //SD and Arduino
    String logstring = (String)longit + "," + (String)lat + "," + (String)alt + ", " + imuDataString;
    //arduino serial here
    sdLog(logstring); 

    if(onLED){
      digitalWrite(LED_GREEN, LOW);
      onLED = false;
    }
    else{
      digitalWrite(LED_GREEN, HIGH);
      onLED = true;
    }
    
    nextWrite = second() + 5;
  }
  
}


