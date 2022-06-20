

#include <ESP8266WiFi.h>
#include <WiFiManager.h>         // https://github.com/tzapu/WiFiManager
//json library
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>        // JSON decoding library
#include <WiFiClient.h>

//setting location and api keys

String Lat = "18.1420729";
String Lon = "79.5628848";//lattitude and longitude

String API_Key = "d8c370bd23950aab5c3e63bced8e98da";//Your API key
//get location data
//https://api.openweathermap.org/geo/1.0/direct?q=AMBALA,IN&limit=10&appid=d8c370bd23950aab5c3e63bced8e98da
//air Pollution
//http://api.openweathermap.org/data/2.5/air_pollution?lat={lat}&lon={lon}&appid={API key}
//"http://api.openweathermap.org/data/2.5/air_pollution?lat=" + Lat + "&lon=" + Lon + "&appid=" + API_Key
//weather api
//"https://api.openweathermap.org/data/3.0/onecall?lat=" + Lat + "&lon=" + Lon + "&exclude=daily"+ "&appid=" + API_Key
//"https://api.openweathermap.org/data/2.5/onecall?lat="+ Lat + "&lon=" + Lon + "&exclude=minutely,hourly,daily&lang=en&units=metric&"+ "&appid=" + API_Key


String Air_ApiUrl = "http://api.openweathermap.org/data/2.5/air_pollution?lat=" + Lat + "&lon=" + Lon + "&appid=" + API_Key;
String Weather_url = "https://api.openweathermap.org/data/2.5/onecall?lat="+ Lat + "&lon=" + Lon + "&exclude=minutely,hourly,daily&lang=en&units=metric&"+ "&appid=" + API_Key;
void setup() {
  Serial.begin(115200);



  // WiFiManager
  // Local intialization. Once its business is done, there is no need to keep it around
  WiFiManager wifiManager;

  // Uncomment and run it once, if you want to erase all the stored information
  //wifiManager.resetSettings();

  // set custom ip for portal
  //wifiManager.setAPConfig(IPAddress(10,0,1,1), IPAddress(10,0,1,1), IPAddress(255,255,255,0));

  // fetches ssid and pass from eeprom and tries to connect
  // if it does not connect it starts an access point with the specified name
  // here  "AutoConnectAP"
  // and goes into a blocking loop awaiting configuration
  wifiManager.autoConnect("AutoConnectAP");
  // or use this for auto generated name ESP + ChipID
  //wifiManager.autoConnect();

  // if you get here you have connected to the WiFi
  Serial.println("Connected.");

}



void loop() {



  WiFiClient client;

  HTTPClient http; //Declare an object of class HTTPClient
  //http.begin(WifiClient, Air_ApiUrl);
  // specify request destination
  http.begin(client, Air_ApiUrl); // !!

  int httpCode = http.GET(); // send the request

  if (httpCode > 0) // check the returning code
  {
    String payload = http.getString(); //Get the request response payload

    //DynamicJsonBuffer jsonBuffer(512);//json v5
    DynamicJsonDocument jsonBuffer(512);//json v6

    // Parse JSON object
    /*JsonObject& root = jsonBuffer.parseObject(payload);//json v5
      if (!root.success()) {
      Serial.println(F("Parsing failed!"));
      return;
      }*/
    auto error = deserializeJson(jsonBuffer, payload);
    if (error) {
      Serial.print(F("deserializeJson() failed with code "));
      Serial.println(error.c_str());
      return;
    }
    serializeJson(jsonBuffer, Serial);
    Serial.println();

    delay(1000);
  }
    //weather
    http.begin(client, Weather_url); // !!

    httpCode = http.GET(); // send the request

    if (httpCode > 0) // check the returning code
    {
      String payload = http.getString(); //Get the request response payload

      //DynamicJsonBuffer jsonBuffer(512);//json v5
      DynamicJsonDocument jsonBuffer(800);//json v6

      // Parse JSON object
      /*JsonObject& root = jsonBuffer.parseObject(payload);//json v5
        if (!root.success()) {
        Serial.println(F("Parsing failed!"));
        return;
        }*/
      auto error = deserializeJson(jsonBuffer, payload);
      if (error) {
        Serial.print(F("deserializeJson() failed with code "));
        Serial.println(error.c_str());
        return;
      }

      //float temp = (float)(root["main"]["temp"])
      float temp = jsonBuffer["current"]["temp"];
      //serializeJson(jsonBuffer, Serial);
      Serial.println(temp);

      delay(1000);

    }






  }
