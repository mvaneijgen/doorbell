/*
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/esp32-pushover-notifications-arduino/
  
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.
  
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*/
#include "config.h"
#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>

const int doorbell = 34;            //current sensor connected to pin 34 and ground
int senseDoorbell = 0;              //variable to hold doorbell sensor reading
int debounce = 1000;                //only allow one DingDong per second
unsigned long currentMillis = 0;    //how many milliseconds since the Arduino booted
unsigned long prevRing = 0;            //current sensor connected to pin 34 and ground

int LED_BUILTIN = 2;

//Pushover API endpoint
const char* pushoverApiEndpoint = "https://api.pushover.net/1/messages.json";

//Pushover root certificate (valid from 11/10/2006 to 15/01/2038)
const char *PUSHOVER_ROOT_CA = "-----BEGIN CERTIFICATE-----\n"
                  "MIIDjjCCAnagAwIBAgIQAzrx5qcRqaC7KGSxHQn65TANBgkqhkiG9w0BAQsFADBh\n"
                  "MQswCQYDVQQGEwJVUzEVMBMGA1UEChMMRGlnaUNlcnQgSW5jMRkwFwYDVQQLExB3\n"
                  "d3cuZGlnaWNlcnQuY29tMSAwHgYDVQQDExdEaWdpQ2VydCBHbG9iYWwgUm9vdCBH\n"
                  "MjAeFw0xMzA4MDExMjAwMDBaFw0zODAxMTUxMjAwMDBaMGExCzAJBgNVBAYTAlVT\n"
                  "MRUwEwYDVQQKEwxEaWdpQ2VydCBJbmMxGTAXBgNVBAsTEHd3dy5kaWdpY2VydC5j\n"
                  "b20xIDAeBgNVBAMTF0RpZ2lDZXJ0IEdsb2JhbCBSb290IEcyMIIBIjANBgkqhkiG\n"
                  "9w0BAQEFAAOCAQ8AMIIBCgKCAQEAuzfNNNx7a8myaJCtSnX/RrohCgiN9RlUyfuI\n"
                  "2/Ou8jqJkTx65qsGGmvPrC3oXgkkRLpimn7Wo6h+4FR1IAWsULecYxpsMNzaHxmx\n"
                  "1x7e/dfgy5SDN67sH0NO3Xss0r0upS/kqbitOtSZpLYl6ZtrAGCSYP9PIUkY92eQ\n"
                  "q2EGnI/yuum06ZIya7XzV+hdG82MHauVBJVJ8zUtluNJbd134/tJS7SsVQepj5Wz\n"
                  "tCO7TG1F8PapspUwtP1MVYwnSlcUfIKdzXOS0xZKBgyMUNGPHgm+F6HmIcr9g+UQ\n"
                  "vIOlCsRnKPZzFBQ9RnbDhxSJITRNrw9FDKZJobq7nMWxM4MphQIDAQABo0IwQDAP\n"
                  "BgNVHRMBAf8EBTADAQH/MA4GA1UdDwEB/wQEAwIBhjAdBgNVHQ4EFgQUTiJUIBiV\n"
                  "5uNu5g/6+rkS7QYXjzkwDQYJKoZIhvcNAQELBQADggEBAGBnKJRvDkhj6zHd6mcY\n"
                  "1Yl9PMWLSn/pvtsrF9+wX3N3KjITOYFnQoQj8kVnNeyIv/iPsGEMNKSuIEyExtv4\n"
                  "NeF22d+mQrvHRAiGfzZ0JFrabA0UWTW98kndth/Jsw1HKj2ZL7tcu7XUIOGZX1NG\n"
                  "Fdtom/DzMNU+MeKNhJ7jitralj41E6Vf8PlwUHBHQRFXGU7Aj64GxJUTFy8bJZ91\n"
                  "8rGOmaFvE7FBcf6IKshPECBV1/MUReXgRPTqh5Uykw7+U0b6LJ3/iyK5S9kJRaTe\n"
                  "pLiaWN0bfVKfjllDiIGknibVb63dDcY3fe0Dkhvld1927jyNxF1WW6LZZm6zNTfl\n"
                  "MrY=\n"
                  "-----END CERTIFICATE-----\n";

void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  // Set blue led if wifi is active
  digitalWrite(LED_BUILTIN, LOW);

  //Make HTTPS POST request to send notification
  if (WiFi.status() == WL_CONNECTED) {
    
  }
}

void loop() {
  currentMillis = millis();
 
  //only check the doorbell if it hasn't been hit in the last second
  // if(currentMillis - prevRing >= debounce){
    senseDoorbell = analogRead(doorbell);  //read the doorbell sensor
    if(senseDoorbell > 50){     
            
          //mine read between 0 and 7 with no current and 200 with it.  50 seemed to be safe.
          Serial.println("DingDong");

          // // Function that gets current epoch time
          // unsigned long getTime() {
          //   time_t now;
          //   struct tm timeinfo;
          //   if (!getLocalTime(&timeinfo)) {
          //     //Serial.println("Failed to obtain time");
          //     return(0);
          //   }
          //   time(&now);
          //   return now;
          // }
          // Create a JSON object with notification details
          // Check the API parameters: https://pushover.net/api
          StaticJsonDocument<512> notification; 
          notification["token"] = apiToken; //required
          notification["user"] = userToken; //required
          notification["message"] = "get current time"; //required
          notification["title"] = "Doorbell rang"; //optional
          notification["url"] = ""; //optional
          notification["url_title"] = ""; //optional
          notification["html"] = ""; //optional
          notification["priority"] = ""; //optional
          notification["sound"] = "cosmic"; //optional
          notification["timestamp"] = ""; //optional

          // Serialize the JSON object to a string
          String jsonStringNotification;
          serializeJson(notification, jsonStringNotification);

          // Create a WiFiClientSecure object
          WiFiClientSecure client;
          // Set the certificate
          client.setCACert(PUSHOVER_ROOT_CA);

          // Create an HTTPClient object
          HTTPClient https;

          // Specify the target URL
          https.begin(client, pushoverApiEndpoint);

          // Add headers
          https.addHeader("Content-Type", "application/json");

          // Send the POST request with the JSON data
          int httpResponseCode = https.POST(jsonStringNotification);

          // Check the response
          if (httpResponseCode > 0) {
            Serial.printf("HTTP response code: %d\n", httpResponseCode);
            String response = https.getString();
            Serial.println("Response:");
            Serial.println(response);
          } else {
            Serial.printf("HTTP response code: %d\n", httpResponseCode);
          }

          // Close the connection
          https.end();
        }
      prevRing = currentMillis;            //engage debounce mode!
  // }
}