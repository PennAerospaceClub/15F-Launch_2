//EXPECT TO HAVE SERIAL HANDSHAKE WITH TRACKUINO IMPLEMENTED LATER

//4.5 Sanity
boolean initiallySane()
{
  int sanityState = 0;

  //Debug
//  Serial.print(currAlt);
//  Serial.print(", ");
//  Serial.print(lat);
//  Serial.print(", ");
//  Serial.println(longit);
  
  if(!(currAlt == -1) && !(lat == -1) && !(longit == -1)){
    GPSgivingDataInitially = true;
    sanityState++;
  }
  if(inBdryBox()){
    inBdry = true;
    sanityState += 2;
  }
  if(!isFalling()){
    falling = false;
    sanityState += 4;
  }
  ardSerial.print("4");
  //if(ardSerial.available() > 0){
//  ardSerial.listen();
//  if(ardSerial.available()){
//    if((char)ardSerial.read() == '3'){
//      sanityState += 16;
//      ardSerial.print("4");
//    }
//  }
  
  Serial.println(sanityState);
  sanityLED(sanityState);
  if(sanityState < 7){
    return false;
  }
  else{
    return true;
  }
}

void sanityLED(int sanityState){
  if(sanityState == 31){
    digitalWrite(LED_RED, LOW);
    digitalWrite(LED_GREEN, LOW);
    digitalWrite(LED_YELLOW, LOW);
  }
  else{
    digitalWrite(LED_RED, HIGH);
    digitalWrite(LED_GREEN, HIGH);
    digitalWrite(LED_YELLOW, HIGH);
  }
}
