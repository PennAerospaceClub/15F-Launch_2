boolean initSane(){
  readGPS();
  //Serial.print(lat); Serial.print(", "); Serial.print(longit); Serial.print(", "); Serial.println(alt); 

  
  if(inBdryBox()){
    //Serial.println("bdry");
    inBdry = true;
  }
  if(!isFalling()){
    falling = false;
    //Serial.println("fall");
  }
  if(!initSD){
    if(SD.begin(cspin)){
      //Serial.println("sd");
      initSD = true;
    }
  }
  
  if(inBdry && !falling && initSD){
    digitalWrite(LED_GREEN, HIGH);
    digitalWrite(LED_RED, LOW);
    digitalWrite(LED_YELLOW, LOW);
    onLED = true;
    
    return true;
  }
  else{
    digitalWrite(LED_GREEN, LOW);
    digitalWrite(LED_RED, LOW);
    digitalWrite(LED_YELLOW, HIGH);
    return false;
  }
}

boolean inBdryBox(){
  if ((lat < maxLat) && (lat > minLat) && (longit < maxLong) && (longit > minLong) && (alt < maxWantedAlt)){
    return true;
  }
  else{
    return false;
  }
}

boolean isFalling(){
  if (alt + 500 < maxAlt){
    return true;
  }
  else{
    return false;
  }
}


