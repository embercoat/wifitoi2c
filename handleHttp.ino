/** Handle root or redirect to captive portal */
void handleRoot() {
  if (captivePortal()) { // If caprive portal redirect instead of displaying the page.
    return;
  }
  server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
  server.sendHeader("Pragma", "no-cache");
  server.sendHeader("Expires", "-1");
  server.setContentLength(CONTENT_LENGTH_UNKNOWN);
  server.send(200, "text/html", ""); // Empty content inhibits Content-length header so we have to close the socket ourselves.
  server.sendContent(
    "<html><head></head><body>"
    "<h1>HELLO WORLD!!</h1>"
  );
  if (server.client().localIP() == apIP) {
    server.sendContent(String("<p>You are connected through the soft AP: ") + softAP_ssid + "</p>");
  } else {
    server.sendContent(String("<p>You are connected through the wifi network: ") + ssid + "</p>");
  }
  server.sendContent(
    "<p>You may want to <a href='/wifi'>config the wifi connection</a>.</p>"
    "</body></html>"
  );
  server.client().stop(); // Stop is needed because we sent no content length
}

/** Redirect to captive portal if we got a request for another domain. Return true in that case so the page handler do not try to handle the request again. */
boolean captivePortal() {
  if (!isIp(server.hostHeader()) && server.hostHeader() != (String(myHostname)+".local")) {
    Serial.print("Request redirected to captive portal");
    server.sendHeader("Location", String("http://") + toStringIp(server.client().localIP()), true);
    server.send ( 302, "text/plain", ""); // Empty content inhibits Content-length header so we have to close the socket ourselves.
    server.client().stop(); // Stop is needed because we sent no content length
    return true;
  }
  return false;
}

/** Wifi config page handler */
void handleWifi() {
  server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
  server.sendHeader("Pragma", "no-cache");
  server.sendHeader("Expires", "-1");
  server.setContentLength(CONTENT_LENGTH_UNKNOWN);
  server.send(200, "text/html", ""); // Empty content inhibits Content-length header so we have to close the socket ourselves.
  server.sendContent(
    "<html><head></head><body>"
    "<h1>Wifi config</h1>"
  );
  if (server.client().localIP() == apIP) {
    server.sendContent(String("<p>You are connected through the soft AP: ") + softAP_ssid + "</p>");
  } else {
    server.sendContent(String("<p>You are connected through the wifi network: ") + ssid + "</p>");
  }
  server.sendContent(
    "\r\n<br />"
    "<table><tr><th align='left'>SoftAP config</th></tr>"
  );
  server.sendContent(String() + "<tr><td>SSID " + String(softAP_ssid) + "</td></tr>");
  server.sendContent(String() + "<tr><td>IP " + toStringIp(WiFi.softAPIP()) + "</td></tr>");
  server.sendContent(
    "</table>"
    "\r\n<br />"
    "<table><tr><th align='left'>WLAN config</th></tr>"
  );
  server.sendContent(String() + "<tr><td>SSID " + String(ssid) + "</td></tr>");
  server.sendContent(String() + "<tr><td>IP " + toStringIp(WiFi.localIP()) + "</td></tr>");
  server.sendContent(
    "</table>"
    "\r\n<br />"
    "<table><tr><th align='left'>WLAN list (refresh if any missing)</th></tr>"
  );
  Serial.println("scan start");
  int n = WiFi.scanNetworks();
  Serial.println("scan done");
  if (n > 0) {
    for (int i = 0; i < n; i++) {
      server.sendContent(String() + "\r\n<tr><td>SSID " + WiFi.SSID(i) + String((WiFi.encryptionType(i) == ENC_TYPE_NONE)?" ":" *") + " (" + WiFi.RSSI(i) + ")</td></tr>");
    }
  } else {
    server.sendContent(String() + "<tr><td>No WLAN found</td></tr>");
  }
  server.sendContent(
    "</table>"
    "\r\n<br /><form method='POST' action='wifisave'><h4>Connect to network:</h4>"
    "<input type='text' placeholder='network' name='n'/>"
    "<br /><input type='password' placeholder='password' name='p'/>"
    "<br /><input type='submit' value='Connect/Disconnect'/></form>"
    "<p>You may want to <a href='/'>return to the home page</a>.</p>"
    "</body></html>"
  );
  server.client().stop(); // Stop is needed because we sent no content length
}

/** Handle the WLAN save form and redirect to WLAN config page again */
void handleWifiSave() {
  Serial.println("wifi save");
  server.arg("n").toCharArray(ssid, sizeof(ssid) - 1);
  server.arg("p").toCharArray(password, sizeof(password) - 1);
  server.sendHeader("Location", "wifi", true);
  server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
  server.sendHeader("Pragma", "no-cache");
  server.sendHeader("Expires", "-1");
  server.send ( 302, "text/plain", "");  // Empty content inhibits Content-length header so we have to close the socket ourselves.
  server.client().stop(); // Stop is needed because we sent no content length
  saveCredentials();
  connect = strlen(ssid) > 0; // Request WLAN connect with new credentials if there is a SSID
}


void handleNotFound() {
  if (captivePortal()) { // If caprive portal redirect instead of displaying the error page.
    return;
  }
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += ( server.method() == HTTP_GET ) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";

  for ( uint8_t i = 0; i < server.args(); i++ ) {
    message += " " + server.argName ( i ) + ": " + server.arg ( i ) + "\n";
  }
  server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
  server.sendHeader("Pragma", "no-cache");
  server.sendHeader("Expires", "-1");
  server.send ( 404, "text/plain", message );
}

void i2c(){
  StaticJsonBuffer<1000> jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(server.arg("plain"));
  Serial.println(server.arg("plain"));

  for(JsonObject::iterator it=root.begin(); it!=root.end(); ++it) 
  {
    // *it contains the key/value pair
    const char* key = it->key;

    // it->value contains the JsonVariant which can be cast as usual
    const char* value = it->value;

    Serial.print("Key/Value: ");
    Serial.print(key);
    Serial.print("/");
    Serial.println(value);     
  }
  
  int addr;
  if(root.containsKey("address")) {
    addr = root["address"].as<int>();
  } else {
    Serial.println("No address found, returning");
    server.send (400, "text/plain", "ADDRESS MISSING");
    return;
  }
  
  JsonArray& sequence = root["sequence"].asArray();

  JsonArray& response = jsonBuffer.createArray();
  
  for(JsonArray::iterator it=sequence.begin(); it!=sequence.end(); ++it) 
  {
    JsonObject& jo = it->as<JsonObject&>();
    String type = jo.get<String>("type");
    Serial.print("Type: ");
    Serial.println(type);

    if(type.equals("byte")){
      Serial.print("Value: ");
      Serial.println(jo.get<String>("value"));
      byte error;
      // Send the bytes on the wire
      Serial.print("Sending bytes on the Wire to address: ");
      Serial.println(addr);
      Wire.beginTransmission(addr);
      
      JsonArray& bytes = jo.get<JsonArray&>("value");
      for (auto by : bytes) {
        int b = by.as<int>();
        Serial.print("Sending byte: ");
        Serial.println(b);
        
        Wire.write(b);
      }
      //const char* value = jo.get<const char*>("value");
      //Wire.write(value);
      error = Wire.endTransmission();                 // stop transmitting 
      Serial.print("Errorcode: ");
      Serial.println(error);
    } 
    if(type.equals("delay")){
      Serial.print("Delay for: ");
      Serial.println(jo.get<int>("value"));
      delay(jo.get<int>("value"));
    }
    if(type.equals("read")){
      Serial.print("Reading length: ");
      int len = jo["length"].as<int>();

      Serial.println(len);
      JsonArray& array = jsonBuffer.createArray();
      Wire.requestFrom(addr, len);
      while(Wire.available()){
        char c = Wire.read();
        array.add((int)c);
        Serial.print("Read: ");
        Serial.println((int)c);
      }
      response.add(array);
    }
  }
  Serial.println("Processing complete");
  Serial.println();
  String json;
  response.prettyPrintTo(json);

  server.send ( 200, "text/plain", json);
  
}

void identify(){
  for(int i=0;i<10;i++){ 
      digitalWrite(IDENTIFIER, HIGH);
      delay(200);
      digitalWrite(IDENTIFIER, LOW);
      delay(200);
  }
}

void handleFactoryReset(){
  Serial.println("Doing factory reset");
  Serial.println("DC wifi");
  WiFi.disconnect();
  delay(2000);
  Serial.println("eraseconfig");
  ESP.eraseConfig();
  delay(2000);
  Serial.println("clear EEPROM");
  clearCredentials();
  delay(2000);
  Serial.println("restart");
  ESP.restart();
}
