//4.3 Nichrome
void nichromeCheck(){

  //check that we are outside the boundary and not falling thirty consecutive times
  if (!inBdryBox() && !isFalling() && !nichromeStarted && !nichromeFinished){
  //if(millis() > 30000 && !nichromeStarted && !nichromeFinished){
    nichromeCounter++;
    //if we have met this condition thirty consecutive times, start the nichrome
    if(nichromeCounter >= 30){
      Serial.println("Nichrome Started");
      digitalWrite(LED_YELLOW, HIGH);
      nichromeStarted = true;
      digitalWrite(NICHROME_PIN, HIGH);// 128//This duty cycle is an estimate. You might need to increase it. Test.
      nichromeEndTime = millis() + 20000;
    }
  }
  //if we started the nichrome, check if it has finished
  else if(nichromeStarted && !nichromeFinished){
    if(millis() > nichromeEndTime){
      Serial.println("Nichrome Ended");
      digitalWrite(LED_YELLOW, LOW);
      digitalWrite(NICHROME_PIN, LOW);
      nichromeFinished = true;
    }
  }
  //if we stopped meeting the condition before thirty, reset the counter
  else{
    nichromeCounter = 0;
  }
}

void nichromeEXCheck(){
  //check that we are outside the boundary and not falling thirty consecutive times
  if (millis() > 4500000 && !nichromeEXStarted && !nichromeEXFinished){
      Serial.println("Nichrome Ex Started");
      digitalWrite(LED_YELLOW, HIGH);
      nichromeEXStarted = true;
      digitalWrite(NICHROME_EX_PIN, HIGH);
      nichromeEXEndTime = millis() + 4000;
  }
  //if we started the nichrome, check if it has finished
  else if(nichromeEXStarted && !nichromeEXFinished){
    if(millis() > nichromeEXEndTime){
      Serial.println("Nichrome Ex Ended");
      digitalWrite(LED_YELLOW, LOW);
      digitalWrite(NICHROME_EX_PIN, LOW);
      nichromeEXFinished = true;
    }
  }
  //if we stopped meeting the condition before thirty, reset the counter
  else{
    nichromeCounter = 0;
  }
}
