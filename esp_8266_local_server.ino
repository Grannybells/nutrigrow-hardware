#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

const char* ssid = "HUAWEI-2.4G-Paulskie";
const char* password = "!J11223344";

long randNumber;

String sensorValue1;
String sensorValue2;
String sensorValue3;
String sensorValue4;
String sensorValue5;
String sensorValue6;
String sensorValue7;
String sensorValue8;
String sensorValue9;
String sensorValue10;
String sensorValue11;
String sensorValue12;

void setup() {
  Serial.begin(9600);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
}

void putData(const char* url, const char* payload) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    WiFiClient client;

    http.begin(client, url);
    http.addHeader("Content-Type", "application/json");

    int httpResponseCode = http.PUT(payload);

    if (httpResponseCode > 0) {
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
      String response = http.getString();
      Serial.println(response);
    } else {
      Serial.print("Error code: ");
      Serial.println(httpResponseCode);
    }
    http.end();
  }
}

void loop() {

  if (Serial.available() > 0) {
    String dataString = Serial.readStringUntil('\n');
    int firstCommaIndex = dataString.indexOf(',');
    int secondCommaIndex = dataString.indexOf(',', firstCommaIndex + 1);
    int thirdCommaIndex = dataString.indexOf(',', secondCommaIndex + 1);
    int fourthCommaIndex = dataString.indexOf(',', thirdCommaIndex + 1);
    int fifthCommaIndex = dataString.indexOf(',', fourthCommaIndex + 1);
    int sixthCommaIndex = dataString.indexOf(',', fifthCommaIndex + 1);
    int seventhCommaIndex = dataString.indexOf(',', sixthCommaIndex + 1);
    int eighthCommaIndex = dataString.indexOf(',', seventhCommaIndex + 1);
    int ninthCommaIndex = dataString.indexOf(',', eighthCommaIndex + 1);
    int tenthCommaIndex = dataString.indexOf(',', ninthCommaIndex + 1);
    int eleventhCommaIndex = dataString.indexOf(',', tenthCommaIndex + 1);

    if (firstCommaIndex != -1 && secondCommaIndex != -1 && thirdCommaIndex != -1 && fourthCommaIndex != -1 && fifthCommaIndex != -1 && sixthCommaIndex != -1 && seventhCommaIndex != -1 && eighthCommaIndex != -1 && ninthCommaIndex != -1 && tenthCommaIndex != -1 && eleventhCommaIndex != -1) {
      String sensorValue1String = dataString.substring(0, firstCommaIndex);
      String sensorValue2String = dataString.substring(firstCommaIndex + 1, secondCommaIndex);
      String sensorValue3String = dataString.substring(secondCommaIndex + 1, thirdCommaIndex);
      String sensorValue4String = dataString.substring(thirdCommaIndex + 1, fourthCommaIndex);
      String sensorValue5String = dataString.substring(fourthCommaIndex + 1, fifthCommaIndex);
      String sensorValue6String = dataString.substring(fifthCommaIndex + 1, sixthCommaIndex);
      String sensorValue7String = dataString.substring(sixthCommaIndex + 1, seventhCommaIndex);
      String sensorValue8String = dataString.substring(seventhCommaIndex + 1, eighthCommaIndex);
      String sensorValue9String = dataString.substring(eighthCommaIndex + 1, ninthCommaIndex);
      String sensorValue10String = dataString.substring(ninthCommaIndex + 1, tenthCommaIndex);
      String sensorValue11String = dataString.substring(tenthCommaIndex + 1, eleventhCommaIndex);
      String sensorValue12String = dataString.substring(eleventhCommaIndex + 1, dataString.length() - 1);

      sensorValue1 = sensorValue1String;
      sensorValue2 = sensorValue2String;
      sensorValue3 = sensorValue3String;
      sensorValue4 = sensorValue4String;
      sensorValue5 = sensorValue5String;
      sensorValue6 = sensorValue6String;
      sensorValue7 = sensorValue7String;
      sensorValue8 = sensorValue8String;
      sensorValue9 = sensorValue9String;
      sensorValue10 = sensorValue10String;
      sensorValue11 = sensorValue11String;
      sensorValue12 = sensorValue12String;
    }
  }
  int randNumber = random(300);  // Generate a random number

  // Example POST request to recommendation table
  const char* url1 = "http://192.168.100.10:9000/recommendation.json?ns=fir-nutrigrow-default-rtdb";
  String payload1 = "{\"new_nitrogen\": \"" + sensorValue7 + "\", \"new_phosphorus\": \"" + sensorValue8 + "\", \"new_potassium\": \"" + sensorValue9 + "\", \"plant\": \"" + sensorValue6 + "\"}";
  putData(url1, payload1.c_str());
  delay(2000);  // Delay between requests

  // Example POST request to sensor table
  const char* url2 = "http://192.168.100.10:9000/sensor.json?ns=fir-nutrigrow-default-rtdb";
  String payload2 = "{\"moisture\": \"" + sensorValue1 + "\", \"pH\": \"" + sensorValue2 + "\", \"nitrogen\": \"" + sensorValue3 + "\", \"phosphorus\": \"" + sensorValue4 + "\", \"potassium\": \"" + sensorValue5 + "\"}";
  putData(url2, payload2.c_str());
  delay(2000);  // Delay between requests

  // Example POST request to status table
  const char* url3 = "http://192.168.100.10:9000/status.json?ns=fir-nutrigrow-default-rtdb";
  String payload3 = "{\"ph_level\": \"" + String(randNumber) + "\", \"n_level\": \"" + sensorValue10 + "\", \"p_level\": \"" + sensorValue11 + "\", \"k_level\": \"" + sensorValue12 + "\"}";
  putData(url3, payload3.c_str());
  delay(2000);  // Delay between requests
}
