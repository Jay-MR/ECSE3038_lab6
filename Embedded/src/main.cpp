#include <Arduino.h>
#include <Wifi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "env.h" 

#define endpoint "rg-lab6-api.onrender.com"

#define fPin 22
#define lPin 23



float getTemp(){

  return random(21.1,33.1);
}

void setup() {

  Serial.begin(9600);

	pinMode(fPin,OUTPUT);
  pinMode(lPin,OUTPUT);

	// WiFi_SSID and WIFI_PASS should be stored in the env.h
  WiFi.begin(WIFI_USER, WIFI_PASS);
  Serial.println("");
	// Connect to wifi
  Serial.println("Connecting");
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  //Check WiFi connection status
  if(WiFi.status()== WL_CONNECTED){
    Serial.println("");
    Serial.println("");
    HTTPClient http;
  
    // Establish a connection to the server
    String url = "https://" + String(endpoint) + "/api/state";
    http.begin(url);
    http.addHeader("Content-type", "application/json");
    http.addHeader("Content-length", "23");



    // Specify content-type header
    //http.addHeader("Content-Type", "application/json");

    StaticJsonDocument<1024> docput;
    String httpRequestData;

    // Serialise JSON object into a string to be sent to the API
  

    docput["temperature"] = getTemp();


    // convert JSON document, doc, to string and copies it into httpRequestData
    serializeJson(docput, httpRequestData);

    // Send HTTP PUT request
    int httpResponseCode = http.PUT(httpRequestData);
    String http_response;

    // check reuslt of PUT request. negative response code means server wasn't reached
    if (httpResponseCode>0) {
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);

      Serial.print("HTTP Response from server: ");
      http_response = http.getString();
      Serial.println(http_response);
    }
    else {
      Serial.print("Error code: ");
      Serial.println(httpResponseCode);
    }

    http.end();    
    http.begin(url);
    httpResponseCode = http.GET();

    Serial.println("");
    Serial.println("");

    if (httpResponseCode>0) {
        Serial.print("HTTP Response code: ");
        Serial.println(httpResponseCode);

        Serial.print("Response from server: ");
        http_response = http.getString();
        Serial.println(http_response);
      }
      else {
        Serial.print("Error code: ");
        Serial.println(httpResponseCode);
    }
 
    StaticJsonDocument<1024> docget;

    DeserializationError error = deserializeJson(docget, http_response);

    if (error) {
      Serial.print("deserializeJson() failed: ");
      Serial.println(error.c_str());
      return;
    }
    
    bool temp = docget["fan"]; 
    bool light= docget["light"]; 

    digitalWrite(fPin,temp);
    digitalWrite(lPin,temp);
    
    // Free resources
    http.end();
  }
  else {
    Serial.println("WiFi Disconnected");
  }
}
