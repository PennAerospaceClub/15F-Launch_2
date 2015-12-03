
#include <SoftwareSerial.h>

SoftwareSerial arduinoSerial(12, 13); // RX, TX
int overall = 0; 
void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }


//  Serial.println("Goodnight moon!");
//  Serial.println("WE DID IT MEGA!"); 

  // set the data rate for the SoftwareSerial port
  arduinoSerial.begin(9600);
  arduinoSerial.println("Hello, Mega world?");
}
 

void loop() { // run over and over

 delay(1000);
 arduinoSerial.print("3");
 while (arduinoSerial.available()>0) {
  Serial.print(arduinoSerial.readString());
 }

//   if (Serial.available()) {
//    arduinoSerial.write(Serial.read());
//      }
}
