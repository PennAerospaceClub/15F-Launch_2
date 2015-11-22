void smooth(){
  prevLat = smLat;
  smLat = smoothVector(lat, prevLat, devLat);

  prevLong = smLong;
  smLong = smoothVector(longit, prevLong, devLong);

  prevAlt = smAlt;
  smAlt = smoothVector(currAlt, prevAlt, devAlt);
}

long smoothVector(long currEl, long prevEl, long maxDev)
{
  if (abs(currEl - prevEl) < maxDev)
  {
    return currEl;
  }
  else
  {
    return prevEl;
  }
}
