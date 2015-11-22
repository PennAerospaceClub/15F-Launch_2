
#include <SoftwareSerial.h>

SoftwareSerial mySerial(11, 10); // RX, TX
int overall = 0; 
void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }


  Serial.println("Goodnight moon!");
  Serial.println("WE DID IT MEGA!"); 

  // set the data rate for the SoftwareSerial port
  mySerial.begin(4800);
  mySerial.println("Hello, Mega world?");
}
 

void loop() { // run over and over

  mySerial.write("0"); 
   if (mySerial.available()) {
    Serial.write(mySerial.read());
  }
   if (Serial.available()) {
    mySerial.write(Serial.read());
    
  }
}
