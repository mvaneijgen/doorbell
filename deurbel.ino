/*
  Rui Santos
  Complete project details at
  https://RandomNerdTutorials.com/esp32-pushover-notifications-arduino/

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*/
#include "config.h"
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <NTPClient.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <WiFiUdp.h>

const int doorbell = 34; // current sensor connected to pin 34 and ground
int senseDoorbell = 0;   // variable to hold doorbell sensor reading
unsigned long previousMillis =
    0;                       // Store the last time the notification was sent
const long interval = 10000; // Interval between notifications (in milliseconds)
unsigned long prevRing = 0;  // current sensor connected to pin 34 and ground
int LED_BUILTIN = 2;

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 0, 60000); // NTP client setup
//--------------------------------//
// Pushover Setup
//--------------------------------//
const char *pushoverApiEndpoint = "https://api.pushover.net/1/messages.json";

// Pushover root certificate (valid from 11/10/2006 to 15/01/2038)
const char *PUSHOVER_ROOT_CA =
    "-----BEGIN CERTIFICATE-----\n"
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
// END Pushover Setup --------------//

//--------------------------------//
// Send push notification function
//--------------------------------//
void sendPushoverNotification(const char *title, const char *message) {
  // Create a JSON object with notification details
  StaticJsonDocument<512> notification;
  notification["token"] = apiToken;  // required
  notification["user"] = userToken;  // required
  notification["message"] = message; // required
  notification["title"] = title;     // optional
  notification["sound"] = "cosmic";  // optional
  notification["url"] = "";          // optional
  notification["url_title"] = "";    // optional
  notification["html"] = "";         // optional
  notification["priority"] = "";     // optional
  notification["timestamp"] = "";    // optional

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
// END Send push notification function --------------//

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

  // Disable blue led
  digitalWrite(LED_BUILTIN, LOW);

  // Make HTTPS POST request to send notification
  if (WiFi.status() == WL_CONNECTED) {
    sendPushoverNotification("Doorbell setup", "Connected to Wi-Fi");
  }
  timeClient.begin();
}

void loop() {
  timeClient.update(); // Update the NTP client to get the latest time
  unsigned long currentMillis = millis(); // Get the current time

  senseDoorbell = analogRead(doorbell); // Read the doorbell sensor

  if (senseDoorbell > 50 && currentMillis - previousMillis >= interval) {
    String currentTime =
        timeClient
            .getFormattedTime(); // Get the current time as a formatted string

    sendPushoverNotification("Doorbell rang", currentTime.c_str());
    previousMillis =
        currentMillis; // Update the last time the notification was sent
  }
}