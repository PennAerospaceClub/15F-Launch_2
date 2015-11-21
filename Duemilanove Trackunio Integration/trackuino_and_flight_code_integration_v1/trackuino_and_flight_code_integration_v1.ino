/*
 PAC Flight Code for Trackuino
 @version: 11/18/15
 @author Omkar Savant

 //Changes to architecture: 
   This is the only code that goes on the Duemilanove. An input serial from the MEGA gives us data 
   for temperature, imu, and voltage which is sent as a string via the aprs.send() function. 

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

#define rxPin 3
#define txPin 2

SoftwareSerial mySerial = SoftwareSerial(rxPin, txPin);
//1.0 Trackuino

// Module constants
static const uint32_t VALID_POS_TIMEOUT = 2000;  // ms

// Module variables
static int32_t next_aprs = 0;

//Section 2: Setup
void setup() {

  //Mega Serial Comms
    pinMode(rxPin, INPUT);
    pinMode(txPin, OUTPUT);

    mySerial.begin(9600);

  //2.0 Trackuino Setup
  pinMode(LED_PIN, OUTPUT);
  pin_write(LED_PIN, LOW);

  Serial.begin(GPS_BAUDRATE);
    #ifdef DEBUG_RESET
      Serial.println("RESET");
    #endif

  buzzer_setup();
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
    } while ( (millis() - timeout < VALID_POS_TIMEOUT) && ! valid_pos) ;
  
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

 //Serial Comms with Mega

 if ( mySerial.available() )
    {
         while ( mySerial.available() )
             Serial.write(mySerial.read());

         Serial.println();
    }

    if ( Serial.available() )
    {
        delay(5);

        while ( Serial.available() )
            mySerial.write( Serial.read() );
    }
    
//Trackuino Section -- sends the next APRS frame

  String serialInput = "!!!NO DATA!!!"; //This is the input from the Mega
  
  if ((int32_t) (millis() - next_aprs) >= 0) {
    get_pos();
    aprs_send(serialInput); //Sending our IMU data
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



