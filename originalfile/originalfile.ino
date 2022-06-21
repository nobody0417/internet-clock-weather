

#include <ESP8266WiFi.h>
#include <WiFiManager.h>         // https://github.com/tzapu/WiFiManager
//json library and http client library
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>        // JSON decoding library
#include <WiFiClient.h>

//software serial


//variables
float cTemp, uvi, pressure, humidity, visibility,clouds, windspeed, rainVol;

String weatherDes, weathermain;
uint8_t aqi;


//setting location and api keys

String Lat = "18.1420729";
String Lon = "79.5628848";//lattitude and longitude

//String API_Key = "d8c370bd23950aab5c3e63bced8e98da";//Your API key

String API_Key = "cabf6282befe192e0fc3ad7b86824612";//2
//String API_Key = "dabf6282befe192e0fc3ad7b86824612";//invalid


//get location data
//https://api.openweathermap.org/geo/1.0/direct?q=AMBALA,IN&limit=10&appid=d8c370bd23950aab5c3e63bced8e98da
//air Pollution
//http://api.openweathermap.org/data/2.5/air_pollution?lat={lat}&lon={lon}&appid={API key}
//"http://api.openweathermap.org/data/2.5/air_pollution?lat=" + Lat + "&lon=" + Lon + "&appid=" + API_Key
//weather api
//"https://api.openweathermap.org/data/3.0/onecall?lat=" + Lat + "&lon=" + Lon + "&exclude=daily"+ "&appid=" + API_Key
//"https://api.openweathermap.org/data/2.5/onecall?lat="+ Lat + "&lon=" + Lon + "&exclude=minutely,hourly,daily&lang=en&units=metric&"+ "&appid=" + API_Key


String Air_ApiUrl = "http://api.openweathermap.org/data/2.5/air_pollution?lat=" + Lat + "&lon=" + Lon + "&appid=" + API_Key;
String Weather_url = "https://api.openweathermap.org/data/2.5/onecall?lat=" + Lat + "&lon=" + Lon + "&exclude=minutely,hourly,daily&lang=en&units=metric&" + "&appid=" + API_Key;
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
    //serializeJson(jsonBuffer, Serial);
    //Serial.println();
    //aqi = String(jsonBuffer["list"][0]["main"]["aqi"]);
     aqi = String(jsonBuffer["list"][0]["main"]["aqi"]).toInt();

    if (jsonBuffer["cod"] == 401) {
      Serial.println("invalid api key");
    }
    else if (jsonBuffer["cod"] == 429) {
      Serial.println("account blocked reach limit exceed");
      return;
    }

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

    cTemp = jsonBuffer["current"]["temp"];
    uvi = jsonBuffer["current"]["uvi"];
    pressure = jsonBuffer["current"]["pressure"];
    humidity = jsonBuffer["current"]["humidity"];
    clouds = jsonBuffer["current"]["clouds"];
    visibility = jsonBuffer["current"]["visibility"];
    windspeed = jsonBuffer["current"]["wind_speed"];


    weathermain = String(jsonBuffer["current"]["weather"][0]["main"]);
    weatherDes = String(jsonBuffer["current"]["weather"][0]["description"]);

    rainVol = jsonBuffer["current"][weatherDes]["1h"];



    delay(10000);

  }

  //float cTemp, uvi, pressure, humidity, clouds, visibility, windspeed;

  //float rainVol, weatherDes, weathermain,aqi;

  Serial.println("Temp\t:\t" + String(cTemp, 2) + "*C\n" + "UVI\t:\t" + String(uvi, 2) + "\nPressure\t:\t" + String(pressure, 2) + "hPa \nHumidity\t:\t" + String(humidity, 2) + " % ");
  Serial.println("Clouds \t:\t" + String(clouds, 2) + "% \nVisibility\t:\t" + String(visibility, 2) + "\nWind Speed\t:\t" + String(windspeed, 2));

  Serial.println("Rain Volume\t:\t" + String(rainVol, 2) + "\nWeather\t:\t" + weathermain + "\nWeather Description :\t" + weatherDes + "\nAQI\t:\t" + aqi);
  

  delay(50000);


}
