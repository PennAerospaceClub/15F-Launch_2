//EXPECT TO HAVE SERIAL HANDSHAKE WITH TRACKUINO IMPLEMENTED LATER

//4.5 Sanity
boolean initiallySane()
{
  int sanityState = 0;
  if(!(currAlt == -1) || !(lat == -1) || !(longit == -1)){
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
  if(Serial2 && Serial1){
    softwareSerialsInitially = true;
    sanityState += 8;
  }
  if(Serial1.available() > 0){
    Serial.println("here");
    Serial.print(Serial1.readString());
    Serial1.print("4");
    //sanityState += 16;
  }
  Serial.print("Sanity: ");
  Serial.println(sanityState);
  sanityLED(sanityState);
  if(sanityState < 31){
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

//RED LIGHT: GPS NOT UPDATING
//YELLOW LIGHT: THINKS IT'S FALLING
//GREEN LIGHT: THINKS IT'S OUTSIDE BOUNDARY BOX
//void sanityLED()
//{
//     boolean myBdryBool = bdryBool;
//     boolean myFallingBool = fallingBool;
//     boolean myGpsBool = gpsBool;
//      if(!myBdryBool || !myFallingBool || !myGpsBool){
//        //Serial.println("no sane");
//        if(!redLightOn){
//          redLightBlinkStop = millis() + 1000;
//          digitalWrite(LED_RED, HIGH);
//          digitalWrite(LED_GREEN, LOW);
//          redLightOn = true;
//        }
//        else{
//          if(millis() > redLightBlinkStop){
//            redLightBlinkStop = 0;
//            redLightOn = false;
//            digitalWrite(LED_RED, LOW);
//            digitalWrite(LED_GREEN, LOW);
//          }
//        }
//      }  
//      else{
//          if(!greenLightOn){
//          redLightBlinkStop = millis() + 1000;
//          digitalWrite(LED_RED, LOW);
//          digitalWrite(LED_GREEN, HIGH);
//          greenLightOn = true;
//        }
//        else{
//          if(millis() > greenLightBlinkStop){
//            greenLightOn = false;
//          }
//        }
//      }         
//}
