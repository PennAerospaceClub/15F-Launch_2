

void initIMU(){
  if(!accel.begin())
  {
    /* There was a problem detecting the ADXL345 ... check your connections */
    Serial.println(F("Ooops, no LSM303 detected ... Check your wiring!"));
    while(1);
  }
  if(!mag.begin())
  {
    /* There was a problem detecting the LSM303 ... check your connections */
    Serial.println("Ooops, no LSM303 detected ... Check your wiring!");
    while(1);
  }
  if(!bmp.begin())
  {
    /* There was a problem detecting the BMP085 ... check your connections */
    Serial.print("Ooops, no BMP085 detected ... Check your wiring or I2C ADDR!");
    while(1);
    
  }
  if(!gyro.begin())
  {
    /* There was a problem detecting the L3GD20 ... check your connections */
    Serial.print("Ooops, no L3GD20 detected ... Check your wiring or I2C ADDR!");
    while(1);
  }
  
  /* Display some basic information on this sensor */
 // displaySensorDetails(); Debug
}

/* Get a new sensor event */
String runIMU()
{
    sensors_event_t event;
    String imuDataString = ""; 
  
         /* Display time */
     
    imuDataString.concat(hour()); imuDataString.concat(":"); imuDataString.concat(minute()); imuDataString.concat(":"); imuDataString.concat(second()); imuDataString.concat(", ");
      
    /* Display the results (acceleration is measured in m/s^2) */
    accel.getEvent(&event);
    imuDataString.concat(event.acceleration.x); imuDataString.concat(", ");
    imuDataString.concat(event.acceleration.y); imuDataString.concat(", ");
    imuDataString.concat(event.acceleration.z); imuDataString.concat(", ");
    
    
    /* Display the results (magnetic vector values are in micro-Tesla (uT)) */
    mag.getEvent(&event);
    imuDataString.concat(event.magnetic.x); imuDataString.concat(", ");
    imuDataString.concat(event.magnetic.y); imuDataString.concat(", ");
    imuDataString.concat(event.magnetic.z); imuDataString.concat(", ");
  
    /* Display the results (gyrocope values in rad/s) */
    gyro.getEvent(&event);
    imuDataString.concat(event.gyro.x); imuDataString.concat(", ");
    imuDataString.concat(event.gyro.y); imuDataString.concat(", ");
    imuDataString.concat(event.gyro.z); imuDataString.concat(", ");
  
    /* Display the pressure sensor results (barometric pressure is measure in hPa) */
    bmp.getEvent(&event);
    if (event.pressure)
    {
      /* Display atmospheric pressure in hPa */
      imuDataString.concat(event.pressure);
      imuDataString.concat(", ");
      /* Display ambient temperature in C */
      float temperature;
      bmp.getTemperature(&temperature);
      imuDataString.concat(temperature);
      imuDataString.concat(", ");
      /* Then convert the atmospheric pressure, SLP and temp to altitude    */
      /* Update this next line with the current SLP for better results      */
      float seaLevelPressure = SENSORS_PRESSURE_SEALEVELHPA;
      imuDataString.concat(bmp.pressureToAltitude(seaLevelPressure,
                                          event.pressure,
                                          temperature)); 
    }
  
    return imuDataString; 
 }  //end of runIMU

 
void displaySensorDetails(void)
{
  sensor_t sensor;
  
  accel.getSensor(&sensor);
  //Serial.println(F("----------- ACCELEROMETER ----------"));
  //Serial.print  (F("Sensor:       ")); //Serial.println(sensor.name);
  //Serial.print  (F("Driver Ver:   ")); //Serial.println(sensor.version);
  //Serial.print  (F("Unique ID:    ")); //Serial.println(sensor.sensor_id);
  //Serial.print  (F("Max Value:    ")); //Serial.print(sensor.max_value); //Serial.println(F(" m/s^2"));
  //Serial.print  (F("Min Value:    ")); //Serial.print(sensor.min_value); //Serial.println(F(" m/s^2"));
  //Serial.print  (F("Resolution:   ")); //Serial.print(sensor.resolution); //Serial.println(F(" m/s^2"));
  //Serial.println(F("------------------------------------"));
  //Serial.println(F(""));

  gyro.getSensor(&sensor);
  //Serial.println(F("------------- GYROSCOPE -----------"));
  //Serial.print  (F("Sensor:       ")); //Serial.println(sensor.name);
  //Serial.print  (F("Driver Ver:   ")); //Serial.println(sensor.version);
  //Serial.print  (F("Unique ID:    ")); //Serial.println(sensor.sensor_id);
  //Serial.print  (F("Max Value:    ")); //Serial.print(sensor.max_value); //Serial.println(F(" rad/s"));
  //Serial.print  (F("Min Value:    ")); //Serial.print(sensor.min_value); //Serial.println(F(" rad/s"));
  //Serial.print  (F("Resolution:   ")); //Serial.print(sensor.resolution); //Serial.println(F(" rad/s"));
  //Serial.println(F("------------------------------------"));
  //Serial.println(F(""));

  mag.getSensor(&sensor);
  //Serial.println(F("----------- MAGNETOMETER -----------"));
  //Serial.print  (F("Sensor:       ")); //Serial.println(sensor.name);
  //Serial.print  (F("Driver Ver:   ")); //Serial.println(sensor.version);
  //Serial.print  (F("Unique ID:    ")); //Serial.println(sensor.sensor_id);
  //Serial.print  (F("Max Value:    ")); //Serial.print(sensor.max_value); //Serial.println(F(" uT"));
  //Serial.print  (F("Min Value:    ")); //Serial.print(sensor.min_value); //Serial.println(F(" uT"));
  //Serial.print  (F("Resolution:   ")); //Serial.print(sensor.resolution); //Serial.println(F(" uT"));  
  //Serial.println(F("------------------------------------"));
  //Serial.println(F(""));

  bmp.getSensor(&sensor);
  //Serial.println(F("-------- PRESSURE/ALTITUDE ---------"));
  //Serial.print  (F("Sensor:       ")); //Serial.println(sensor.name);
  //Serial.print  (F("Driver Ver:   ")); //Serial.println(sensor.version);
  //Serial.print  (F("Unique ID:    ")); //Serial.println(sensor.sensor_id);
  //Serial.print  (F("Max Value:    ")); //Serial.print(sensor.max_value); //Serial.println(F(" hPa"));
  //Serial.print  (F("Min Value:    ")); //Serial.print(sensor.min_value); //Serial.println(F(" hPa"));
  //Serial.print  (F("Resolution:   ")); //Serial.print(sensor.resolution); //Serial.println(F(" hPa"));  
  //Serial.println(F("------------------------------------"));
  //Serial.println(F(""));
  
  //delay(500);
}
