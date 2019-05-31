/** Load WLAN credentials from EEPROM */

void loadCredentials() {
  EEPROM.begin(EEPROMSize);
  EEPROM.get(0, ssid);
  EEPROM.get(0+sizeof(ssid), password);
  char ok[2+1];
  EEPROM.get(0+sizeof(ssid)+sizeof(password), ok);
  EEPROM.end();
  if (String(ok) != String("OK")) {
    ssid[0] = 0;
    password[0] = 0;
  }
  Serial.println("Recovered credentials:");
  Serial.println(ssid);
  Serial.println(strlen(password)>0?"********":"<no password>");
}

/** Store WLAN credentials to EEPROM */
void saveCredentials() {
  EEPROM.begin(EEPROMSize);
  EEPROM.put(0, ssid);
  EEPROM.put(0+sizeof(ssid), password);
  char ok[2+1] = "OK";
  EEPROM.put(0+sizeof(ssid)+sizeof(password), ok);
  EEPROM.commit();
}
void clearCredentials(){
  EEPROM.begin(EEPROMSize);
  for (int i = 0 ; i < EEPROMSize ; i++) {
    EEPROM.write(i, 0);
  }
  EEPROM.end();
}

