//4.3 Nichrome
void nichromeCheck()
{
  if (!inBdryBox() && !isFalling())
  {
    nichromeCounter++;
      if(nichromeCounter >= 300){
      startNichrome();
      }
  }
  else{
    nichromeCounter = 0;
  }
  updateNichrome();
}


void initNichrome()
{
  pinMode(NICHROME_GATE_PIN, OUTPUT);
  digitalWrite(NICHROME_GATE_PIN, LOW);
  TCCR2B &= B11111001;//increase PWM frequency
}

void updateNichrome()
{
  if (nichromeStarted && !nichromeFinished && nichromeEndTime < millis()) {
    digitalWrite(NICHROME_GATE_PIN, LOW);
    nichromeFinished = true;
  }
}

void startNichrome()
{
  if (!nichromeStarted) {
    nichromeStarted = true;
    digitalWrite(NICHROME_GATE_PIN, HIGH);// 128//This duty cycle is an estimate. You might need to increase it. Test.
    nichromeEndTime = millis() + 5000;//Again, 5000 ms is an estimate... <- CHANGE!!!
  }
}

//4.6 Experiments
void nichromeExperimentCheck()
{
  if (isFalling() && currAlt < 1600)
  {
    initNichromeExperiment();
  }  
  updateNichromeExperiment();
}

void initNichromeExperiment()
{
  pinMode(NICHROME_EXPERIMENT_PIN, OUTPUT);
  digitalWrite(NICHROME_EXPERIMENT_PIN, LOW);
  TCCR2B &= B11111001;//increase PWM frequency
}

void updateNichromeExperiment()
{
  if (nichromeExperimentStarted && !nichromeExperimentFinished && nichromeExperimentEndTime < millis()) {
    digitalWrite(NICHROME_EXPERIMENT_PIN, LOW);
    nichromeExperimentFinished = true;
  }
}

void startNichromeExperiment()
{
  if (!nichromeStarted) {
    nichromeExperimentStarted = true;
    digitalWrite(NICHROME_EXPERIMENT_PIN, HIGH);
    nichromeExperimentEndTime = millis() + 2000;
  }
}
