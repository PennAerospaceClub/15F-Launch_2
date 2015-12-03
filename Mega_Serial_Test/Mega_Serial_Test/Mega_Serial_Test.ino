int overall = 1; 
void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  Serial1.begin(9600);
}

void loop() { // run over and over

  
//  delay(1000);
  while(Serial1.available() > 0) {
    Serial.print(Serial1.readString());
    Serial1.print("2");
  }
}
