/*
 PAC Flight Code for Trackuino
 @version: 12/3/15
 @author Omkar Savant, based on source code from community

 //Changes to architecture: 
   As of 12.3.15 Version v2 has working GPS and sensor debugging. This v3 version is the next step which begins to test the
   AFSK and APRS as well as reliable serial coms with the Mega. 
   
   This is the only code that goes on the Duemilanove. An input serial from the MEGA gives us data 
   for temperature, imu, and voltage which is sent as a string via the aprs.send() function. 

 NOTE: the new clock for the Mega should be: 

  if((millis() - nextWrite) >= 0){
    arduinoSerial.write(imuData.c_str());
    SDLog(stringForSD); 
    nextWrite += nextWritePeriod; 
    }
  
*/

#if (ARDUINO + 0) == 0
#error "Oops! We need the real Arduino IDE (version 22 or 23) for Arduino builds."
#error "See trackuino.pde for details on this"

// Refuse to compile on arduino version 21 or lower. 22 includes an 
// optimization of the USART code that is critical for real-time operation
// of the AVR code.
#elif (ARDUINO + 0) < 22
#error "Oops! We need Arduino 22 or 23"
#error "See trackuino.pde for details on this"

#endif

// Serial library
#include <SoftwareSerial.h>

// Trackuino custom libs
#include "config.h"
#include "afsk_avr.h"
#include "afsk_pic32.h"
#include "aprs.h"
#include "buzzer.h"
#include "gps.h"
#include "pin.h"
#include "power.h"
#include "sensors_avr.h"
#include "sensors_pic32.h"

// Arduino/AVR libs
#if (ARDUINO + 1) >= 100
#  include <Arduino.h>
#else
#  include <WProgram.h>
#endif

//Section 1: Declarations

//0.0 Serial

String megaInput = "no data yet"; //This is the input from the Mega

SoftwareSerial arduinoSerial(10, 11); //rx,tx
//1.0 Trackuino

boolean globalSanity = false; 

// Module constants
static const uint32_t VALID_POS_TIMEOUT = 2000;  // ms

// Module variables
static int32_t next_aprs = 0;

//Section 2: Setup
void setup() {

  //Mega Serial Comms
  
  arduinoSerial.begin(9600); 
  arduinoSerial.println("Hello, Mega world?");

  //2.0 Trackuino Setup
  pinMode(LED_PIN, OUTPUT);
  pin_write(LED_PIN, LOW);

  Serial.begin(GPS_BAUDRATE);
    #ifdef DEBUG_RESET
      Serial.println("RESET");
    #endif

  //buzzer_setup();
  afsk_setup();
  gps_setup();
  sensors_setup();
  
  #ifdef DEBUG_SENS
  Serial.print("Ti=");
  Serial.print(sensors_int_lm60());
  Serial.print(", Te=");
  Serial.print(sensors_ext_lm60());
  Serial.print(", Vin=");
  Serial.println(sensors_vin());
  #endif


  // Do not start until we get a valid time reference
  // for slotted transmissions.
 if (APRS_SLOT >= 0) {
    do {
      while (! Serial.available())
        power_save();
    } while (! gps_decode(Serial.read()));
    
    next_aprs = millis() + 1000 *
      (APRS_PERIOD - (gps_seconds + APRS_PERIOD - APRS_SLOT) % APRS_PERIOD);
  }
  else {
    next_aprs = millis();
  }  
}

void get_pos()
  {
    // Get a valid position from the GPS
    int valid_pos = 0;
    uint32_t timeout = millis();
    do {
      if (Serial.available())
        valid_pos = gps_decode(Serial.read());
    } while ( (millis() - timeout < VALID_POS_TIMEOUT) && ! valid_pos);
  
    /*if (valid_pos) {
      if (gps_altitude > BUZZER_ALTITUDE) {
        buzzer_off();   // In space, no one can hear you buzz
       } else {
       buzzer_on();
      }
    }*/
  }
  
//Section 3: Loop
void loop() {

while(!globalSanity){
  serialSanity(); // once the first contact with the mega is established, this is never entered again
}
//Trackuino Section -- sends the next APRS frame
  //String gpsData = createGPSString(); //create a string to send to the Mega for SD writing
  
  if (arduinoSerial.available()){
    Serial.println("we're in here"); 
    //Serial.println(arduinoSerial.readString()); 
    megaInput = "PAC: "; 
    megaInput.concat(arduinoSerial.read());
    Serial.println(megaInput); 
  }
  
  if ((int32_t) (millis() - next_aprs) >= 0) {
    get_pos();
    aprs_send(megaInput); //Sending our IMU data
    next_aprs += APRS_PERIOD * 1000L; //iterates by 30 seconds
    
    //arduinoSerial.write(gpsData.c_str()); //send the data to the Mega to save to SD - not necessary anymore
    
    while (afsk_flush()) {
      power_save();
    }
  }
  
  if ((int32_t) (millis() - next_aprs) >= 0) {
    get_pos();
    aprs_send(megaInput); //Sending our IMU data
    next_aprs += APRS_PERIOD * 1000L;
    while (afsk_flush()) {
      power_save();
    }

    #ifdef DEBUG_MODEM
    // Show modem ISR stats from the previous transmission
    afsk_debug();
    #endif
  }
  power_save(); // Incoming GPS data or interrupts will wake us up
  
}

/*String createGPSString(){

  return String(gps_time) + "," + String(gps_aprs_lat) + "," + String(gps_aprs_lon) + "," + String(int(gps_course + 0.5)) + "," + String(int(gps_speed + 0.5)) + "," + String(long((gps_altitude)/0.3048) + 0.5); 

}*/

void serialSanity(){

  if (arduinoSerial){
      Serial.println("in here");
      arduinoSerial.print("3"); 
  }
  
  if (arduinoSerial.available()>0){
   if((char)arduinoSerial.read() == '4'){
      
      Serial.println("got it" + arduinoSerial.read());
      globalSanity = true; 
    }
  }
  else globalSanity = false;
}




