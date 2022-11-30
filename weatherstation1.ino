#include "SPIFFS.h"
#include <Arduino.h>
#include <ezButton.h>
#define LEDNIGHT             15
#define LEDDAY             LED_BUILTIN
#include <ESP32AnalogRead.h>
ESP32AnalogRead adc;
#include <TimeLib.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <Servo.h>
Servo myServo1;  // create servo object to control a servo
Servo myServo2;  // create servo object to control a servo
#include <ESPmDNS.h>
#include <Arduino_JSON.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#define SEALEVELPRESSURE_HPA (1013.25)
float temperature;
float pressure;
float press;
float altitude;
float humidity;
float dewpoint;
float dewpoint_out;
String SLIDERVALUE1;
String SLIDERVALUE2;
String lastDewpoint;
String lastDewpoint_out;
String visibility;
Adafruit_BME280 bme;
// Replace with your network credentials
const char* id = "ADDSSID";
const char* password = "ADDPASSWORD";
const char* http_username = "ADDHTTPUSERNAME";
const char* http_password = "ADDHTTPPASSWORD";
const char* ntpServer = "pool.ntp.org";
unsigned long epochTime; 
String openWeatherMapApiKey = "ADDOPENWEATHERKES";
String temperature_unit = "metric";
String city = "ADDCITY";
String countryCode = "ADDCOUNTRY";
String endpoint;
String Description;
String Speed;
String Deg;
String lastDescription;
String lastTemperature;
String lastTemp;
String lastFahrenheit;
String lastHumidity;
String lastHumi;
String lastpressure;
String lastPressure;
String lastpress;
String lastAltitude;
String lastVisibility;
String lastSunrisehour;
String lastSunriseminute;
String lastSunrisesecond;
String lastSunsethour;
String lastBezero;
String lastSunsetminute;
String lastSunsetsecond;
String lastSun_rise_Second;
String lastSun_rise_Minute;
String lastSun_rise_Hour;
String lastSun_set_Second;
String lastSun_set_Minute;
String lastSun_set_Hour;
String lastTemp_feels_like;
String lastTemp_max;
String lastTemp_min;
String lastAdc;
String lastinputMessage1;
String lastinputMessage2;
String Icon_;
String City_name;
String lastCity_name;
String weather_icon;
String Wind_Speed;
String lastWind_Speed;
String Wind_Direction;
String windDir;
String Clouds;
String Cloudspercnt;
String lastCloudspercnt;
String Wind_Dir;
String lastWind_Dir;
String lastWind_Direction;
String lastAddress;
String lastfrom;
String lastSunrise;
String lastSunset;
String jsonBuffer;
String lastLat;
String lastpercntRain;
String lastLon;
String lastClouds;
String lastRain;
int OutHumidity;
int OutPressure;
String Visibility;
float OutTemperature;
int Sun_rise;
int Sun_set;
float windDir_01;
 //char* sector [] = {"N", "NE", "E", "SE", "S", "SW", "W", "NW", "N"}; //OLD  wind sector labels version
char* sector [] = {"N", "NNE", "NE", "ENE", "E", "ESE", "SE", "SSE", "S", "SSW", "SW", "WSW", "W", "WNW", "NW", "NNW", "N"};
 int h;
 String from;

float Clouds_01;
char* sector1 [] = {"Clear sky","Few clouds", "Scattered clouds", "Broken clouds", "Overcast clouds"};
 int j;
 String Cloudsfrom;
 String lastCloudsfrom;
 float Wind_Miles_01;
 char* sector2 [] = {"Calm","Light Air","Light Breeze","Gentle Breeze","Moderate Breeze","Fresh Breeze","Strong Breeze","Near Gale","Gale","Strong Gale","Whole Gale","Storm Force","Hurricane Force"};
 int i;
 String lastWind_Miles;
 String Wind_Miles_Names;
 String lastWind_Miles_Names;
unsigned long last_time = 5000;
unsigned long timer_delay = 5000;
double dewPointFast(double celsius, double humidity)
 {
   double a = 17.62;
   double b = 243.12;
   double temp = (a * celsius) / (b + celsius) + log(humidity * 0.01);
   double Td = (b * temp) / (a - temp);
   return Td;
 }
// Create AsyncWebServer object on port 80
String WiFiSignal() {
  float Signal = WiFi.RSSI();
  Signal = 90 / 40.0 * Signal + 212.5; // From Signal = 100% @ -50dBm and Signal = 10% @ -90dBm and y = mx + c
  if (Signal > 100) Signal = 100;
  return " " + String(Signal, 0) + "";
}
float Signal;
String lastWiFiSignal;
unsigned long getTime() {
  time_t now;
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    //Serial.println("Failed to obtain time");
    return(0);
  }
  time(&now);
  return now;
  delay(5000);
}

static const int PIN_SERVO1 = 2;
static const int PIN_SERVO2 = 5;
String sliderValue1 = "0";
String sliderValue2 = "0";
// setting PWM properties

const char* PARAM_INPUT1 = "value";
const char* PARAM_INPUT2 = "value";

// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0; 
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 250;

AsyncWebServer server(80);
// Create an Event Source on /events
AsyncEventSource events("/events");
// Json Variable to Hold Sensor Readings
JSONVar readings;
JSONVar owmreadings;
JSONVar owmreadings1;
// Timer variables
unsigned long lastTime = 0;
unsigned long lastTimeTemperature = 0;
unsigned long lastTimeAcc = 0;
unsigned long lastTimeAcc1 = 0;
unsigned long SensorDelay = 5000;
unsigned long OWMapDelay = 10000;
unsigned long OWMapDelay1 = 250;
double   speed;
double   deg;

float in_temperature, in_pressure, in_altitude, in_humidity;
float out_temperature, out_humidity, out_pressure; 
int in_temperatureD , out_temperatureD;
sensors_event_t a, g, temp;
// Initialize Sensor
void initSensor() {
  Serial.println(F("BME280 Sensor event test"));
  unsigned status;
  status = bme.begin(0x76);
  if (!status) {
    Serial.println(F("Could not find a valid BME280 sensor, check wiring or "
                     "try a different addre!"));
    while (1) delay(5000);
  }
}
void initSPIFFS() {
  if (!SPIFFS.begin()) {
    Serial.println("An error has occurred while mounting SPIFFS");
  }
  Serial.println("SPIFFS mounted succefully");
}
// Initialize WiFi
void initWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(id, password);
  Serial.println("");
  Serial.print("Connecting to WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(5000);
  }
  if (!MDNS.begin("esp32")) {
    Serial.println("Error starting mDNS");
    return;
  }
  MDNS.addService("http", "tcp", 80);
  Serial.println("");
  Serial.println(WiFi.localIP());
  Serial.print("Local Addre");
  Serial.println("esp32.local");
}
String httpGETRequest(const char* serverName) {
  WiFiClient client;
  HTTPClient http;
  // Your Domain name with URL path or IP addre with path
  http.begin(client, serverName);
  // Send HTTP POST request
  int httpResponseCode = http.GET();
  String payload = "{}";
  if (httpResponseCode > 0) {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    payload = http.getString();
  }
  else {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }
  // Free resources
  http.end();
  return payload;
}

String getOWMReadings() {
  if (WiFi.status() == WL_CONNECTED) {
    endpoint = "http://api.openweathermap.org/data/2.5/weather?q=" + city + "&units=" + temperature_unit  + "&appid=" + openWeatherMapApiKey;
    jsonBuffer = httpGETRequest(endpoint.c_str());
    JSONVar myObject = JSON.parse(jsonBuffer);
    // JSON.typeof(jsonVar) can be used to get the type of the var
    if (JSON.typeof(myObject) == "undefined") {
      Serial.println("Parsing input failed!");
    }
    Icon_ = myObject["weather"][0]["icon"];
    OutTemperature = double(myObject["main"]["temp"]);
    OutHumidity    = int(myObject["main"]["humidity"]);
    OutPressure    = int(myObject["main"]["pressure"]);
    Sun_rise     = int(myObject["sys"]["sunrise"]);
    Sun_set      = int(myObject["sys"]["sunset"]);
    Wind_Speed      = int(myObject["wind"]["speed"]);
    Wind_Direction = int(myObject["wind"]["deg"]);
    Description = myObject["weather"][0]["description"];
  }
  else {
    Serial.println("WiFi Disconnected");
  }
  int sensetemp;
  sensetemp = OutTemperature;
  owmreadings["out_temperature"] = String(sensetemp);
  sensetemp = OutTemperature * 10;
  owmreadings["out_temperatureD"] = String(sensetemp % 10);
  owmreadings["out_humidity"] = String(OutHumidity);
  owmreadings["out_pressure"] = String(OutPressure);
  owmreadings["Description"] = String(lastDescription);
  owmreadings["Wind_Speed"] = String(lastWind_Speed);
  owmreadings["weather_icon"] = String("http://www.ihome.si:80/" + Icon_ + ".png");
  owmreadings["Sunset"] = String(Sun_set);
  owmreadings["Sunrise"] = String(Sun_rise);
  owmreadings["City"] = String(city);
  owmreadings["Key"] = String(openWeatherMapApiKey);
  owmreadings["Wind_Miles_Names"] = String(lastWind_Miles_Names);
  owmreadings["Wind_Direction"] = String(lastWind_Direction);
  String jsonString = JSON.stringify(owmreadings);
  Serial.println(jsonString);
  return jsonString;
}
String getOWMReadings1() {
    jsonBuffer = httpGETRequest(endpoint.c_str());
    JSONVar myObject = JSON.parse(jsonBuffer);
    // JSON.typeof(jsonVar) can be used to get the type of the var
    if (JSON.typeof(myObject) == "undefined") {
      Serial.println("Parsing input failed!");
    }

  owmreadings1["SLIDERVALUE1"] = String(sliderValue1);
  owmreadings1["SLIDERVALUE2"] = String(sliderValue2);
  String jsonString1 = JSON.stringify(owmreadings1);
  Serial.println(jsonString1);
  return jsonString1;
}

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<meta name="theme-color" content="%COLOR%"/>
<meta name="viewport" content="width=device-width, initial-scale=1">
<meta http-equiv='Content-Type' content='text/html; charset=utf-8'/>
<link rel="icon" type="image/png" href="favicon.ico">
<link rel="stylesheet" href="https://cdnjs.cloudflare.com/ajax/libs/font-awesome/4.7.0/css/font-awesome.min.css">
 <link rel="stylesheet" href="https://cdnjs.cloudflare.com/ajax/libs/font-awesome/4.5.0/css/font-awesome.min.css">
<link rel="stylesheet" href="http://www.ihome.si:80/styler.css">
<link rel="stylesheet" href="http://www.ihome.si:80/clicker.css">
<link rel="stylesheet" href="https://fonts.googleapis.com/css?family=Oxanium">    
<script src="https://code.jquery.com/jquery-3.2.1.min.js"></script>
<link rel="stylesheet" href="https://use.fontawesome.com/releases/v5.7.2/css/all.css" integrity="sha384-fnmOCqbTlWIlj8LyTjo7mOUStjsKC4pOpQbqyi7RrhN7udi9RwhKkMHpvLbHG9Sr" crossorigin="anonymous"/>
</head>
<body style="background: inherit;font-family: 'Oxanium', sans-serif;overflow-x:hidden;">
<button title="Logout" style="font-family: 'Oxanium', sans-serif;z-index:59595959;font-size:25px;background:none;border:none;position:fixed;bottom:1px;right:1px;cursor:pointer;color:inherit;" onclick="logoutButton()"><i class="fas fa-sign-out-alt"></i></button>
<script>function toggleCheckbox(element) {
}
function logoutButton() {
  var xhr = new XMLHttpRequest();
  xhr.open("GET", "/logout", true);
  xhr.send();
  setTimeout(function(){ window.open("/logged-out","_self"); }, 100);
}
</script>
<div style="position:fixed;top:1px;right:5px;"><i title="WiFi Signal &percnt;" style="font-weight:900;font-size:20px;" class='fas fa-wifi'></i><span style="font-family: 'Oxanium', sans-serif;font-size:20px;">%WiFiSignal%</span>&percnt;</div>
 <btn1 id='btn1' title='Night Mode' style='display:block;font-size:22px;cursor:pointer;position:fixed;left:1px;top:1px;z-index:5000;'><i class='fas fa-moon'></i></btn1>
<script src='btn1.js'></script>
<btn2 id='btn2' title='Day Mode' style='display:block;font-size:22px;cursor:pointer;position:fixed;left:40px;top:1px;z-index:5000;'><i class='fas fa-sun'></i></btn2>
<script src='btn2.js'></script> 
<script src='fullscreen.js'></script>
<style>
button,
button:active,
button:focus {
  outline: none;background:transparent;border:none;cursor:pointer;
  color:inherit;
}
</style>
<button class='btn btn-info ' id='show_hide_bt' title='FullScreen' onclick='view_fullscreen(this);fullscreen();' style='z-index:59595959;text-decoration:none;color:inherit;border:none;background:none;left:1px;cursor:pointer;position:fixed;bottom:10px;font-size:14px;'>
<i class='fa fa-eye'></i> Show</button> 
<nav class='sidebar-navigation'>
  <ul>
  <li class='active'>
      <button id='Home-open-button-1' class='Home-open-button-1' value='1'><i class='fa fa-home'></i></button>
    </li>
    <br>
    <li>
      <button id='Weather-open-button-1' class='Weather-open-button-1' value='1'><i class='fa fa-cloud'></i></button>
    </li>
    <br>
    <li>
       <button id='Sensor-open-button-1' class='Sensor-open-button-1' value='1'><i class='fa fa-microchip'></i></button>
    </li>
    <br>
    <li>
      <button id='Sun-open-button-1' class='Sun-open-button-1' value='1'><i class='fa fa-sun-o'></i></button>
    </li>
    <br>
    <li>
      <button id='Moon-open-button-1' class='Moon-open-button-1' value='1'><i class='fa fa-moon-o'></i></button>
    </li>
    <br>
    <li>
      <button id='Radio-open-button-1' class='Radio-open-button-1' value='1'><i class='fa fa-music'></i></button>
    </li>
  </ul>
</nav>
<div style='display: show' class='Home-div-1 div-1'>

    <div id="time"><script src='%BACKCOLOR%'></script>
<p><span style="text-transform: uppercase;" class="font_fam" id="Description">%DESCRIPTION%</span><br>%Cloudsfrom%</p>
                    <img class="img" src="%ADDRESS%"/>
                    <p><span style="font-size:52px;" id="TEMP">%TEMP%</span>&deg;C</p>
                    <p>Feels like <span class="font_fam" id="Temp_feels_like">%Temp_feels_like%</span>&deg;C</p>
                    <p>Wind <span class="font_fam" id="Wind_Miles_Names">%Wind_Miles_Names%</span></p>
                    <p><img title='Wind Speed %Wind_Speed% m/s %Wind_Direction%&deg; from %from%' style='transform:rotate(%Wind_Direction%deg);cursor:pointer;position:relative;' class='image' src='http://www.ihome.si:80/arrow.png'/> %from%</p>                    
                    <p><span style="text-transform: uppercase;" class="font_fam" id="Wind_Speed">%Wind_Speed%</span> m/s %Wind_Miles% mph<br>%Wind_Direction%&deg;</p>
                    </div><script src='refresher.js'></script>
    <p><center><iframe style='font-family: 'Oxanium', sans-serif;' class='wind2' src='http://www.ihome.si:80/wind2.html' scrolling='no'></iframe></center></p>
</div>
<div style='display: none' class='Weather-div-1 div-1'>
    <p><center><iframe style='font-family: 'Oxanium', sans-serif;' class='wind' src='http://www.ihome.si:80/wind.html' scrolling='no'></iframe></center></p>
    <div id="time1">
                    <center><img style='position:absolute;top:50px;left:125px;' class="img" src="%ADDRESS%"/></center>
                    <p><span style="text-transform: uppercase;" class="font_fam" id="Description">%DESCRIPTION%</span><br>%Cloudsfrom% %Cloudspercnt%&percnt;</p>
                    <p>Visibility <span class="font_fam" id="Visibility">%VISIBILITY%</span> m</p>
                    <p>Wind <span class="font_fam" id="Wind_Miles_Names">%Wind_Miles_Names%</span></p>
                    <p>Precipitation 0 - <span class="font_fam" id="RAIN">%RAIN%</span> mm</p>
                    <p><span style="text-transform: uppercase;" class="font_fam" id="Wind_Speed">%Wind_Speed%</span> m/s %Wind_Miles% mph<br>%Wind_Direction%&deg; %from%</p><img title='Wind Speed %Wind_Speed% m/s %Wind_Direction%&deg; from %from%' style='transform:rotate(%Wind_Direction%deg);cursor:pointer;position:relative;' class='image' src='http://www.ihome.si:80/arrow.png'/>
                    <p><span class="font_fam" id="STATUS">%STATUS%</span> - LIGHT %STATUS1%</p>
                    <p><span class="font_fam">Sunrise </span>%Sun_rise_Hour%:%Sun_rise_Minute%:%Sun_rise_Second%</p>
                    <p><span class="font_fam">Sunset </span>%Sun_set_Hour%:%Sun_set_Minute%:%Sun_set_Second%</p>
                     <p>SERVO A&deg;<span class="textsliderValue1" id="textsliderValue1">%SLIDERVALUE1%</span></p>
                     <p>SERVO B&deg;<span class="textsliderValue2" id="textsliderValue2">%SLIDERVALUE2%</span></p>
                    </div><script src='refresher1.js'></script></div>
<div style='display: none' class='Sensor-div-1 div-1'>
   <div id="sec1"><div id="time2">
   <center>
<p><span class="font_fam" id="City_name">%City_name%</span></p>
<p><span class="font_fam" id="Lat">%Lat%</span> -  %Lon%</p>
    <article class="sec2">
                <div class="fancy">
                    <p class="subtitle">
                        <span title='Temperature Outside %TEMP% &deg;C'>&#127777;</span>
                    </p>
                    <div class="temp_num temp_num3">
                        <div>
                              <a style="font-family: 'Oxanium', sans-serif;font-size:28px;cursor:none;">%TEMP%</a><span class="celsius">&deg;C</span>
                        </div>
                    </div>
                </div>
              <div class="fancy">
                    <p class="subtitle">
                        <span title='Humidity Outside %HUMI% &percnt;'>&#128167;</span>
                    </p>
                    <div class="temp_num temp_num3">
                        <div>
                              <a style="font-family: 'Oxanium', sans-serif;font-size:28px;cursor:none;">%HUMI%</a><span class="celsius">&percnt;</span>
                        </div>
                    </div>
                </div>
                <div class="clr"></div>
            </article>
            <article class="sec3">
                <div class="fancy">
                    <p class="subtitle">
                        <span title='Temperature Inside %TEMPERATURE% &deg;C'>&#127777;</span>
                    </p>
                    <div class="temp_num temp_num3">
                        <div>
                              <a style="font-family: 'Oxanium', sans-serif;font-size:28px;cursor:none;">%TEMPERATURE%</a><span class="celsius">&deg;C</span>
                        </div>
                    </div>
                </div>
               <div class="fancy">
                    <p class="subtitle">
                        <span title='Humidity Inside %HUMIDITY% &deg;C'>&#128167;</span>
                    </p>
                    <div class="temp_num temp_num3">
                        <div>
                              <a style="font-family: 'Oxanium', sans-serif;font-size:28px;cursor:none;">%HUMIDITY%</a><span class="celsius">&percnt;</span>
                        </div>
                    </div>
                </div>
                <div class="clr"></div>
            </article>
            <article class="sec4">
                <div class="fancy">
                    <p class="subtitle">
                        <span title='Pressure Outside %PRESS% hPa'>&#127811;</span>
                    </p>
                    <div class="temp_num temp_num3">
                        <div>
                              <a style="font-family: 'Oxanium', sans-serif;font-size:28px;cursor:none;">%PRESS%</a><span class="celsius">hPa</span>
                        </div>
                    </div>
                </div>
               <div class="fancy">
                    <p class="subtitle">
                        <span title='Pressure Inside %PRESSURE% hPa'>&#127811;</span>
                    </p>
                    <div class="temp_num temp_num3">
                        <div>
                              <a style="font-family: 'Oxanium', sans-serif;font-size:28px;cursor:none;">%PRESSURE%</a><span class="celsius">hPa</span>
                        </div>
                    </div>
                </div>
                <div class="clr"></div>
            </article>
            <article class="sec5">
                <div class="fancy">
                    <p class="subtitle">
                        <span title='Dewpoint Outside %DEWPOINT_OUT% &deg;C'>&#128166;</span>
                    </p>
                    <div class="temp_num temp_num3">
                        <div>
                              <a style="font-family: 'Oxanium', sans-serif;font-size:28px;cursor:none;">%DEWPOINT_OUT%</a><span class="celsius">&deg;C</span>
                        </div>
                    </div>
                </div>
                <div class="fancy">
                    <p class="subtitle">
                        <span title='Dewpoint Inside %DEWPOINT% &deg;C'>&#128166;</span>
                    </p>
                    <div class="temp_num temp_num3">
                        <div>
                              <a style="font-family: 'Oxanium', sans-serif;font-size:28px;cursor:none;">%DEWPOINT%</a><span class="celsius">&deg;C</span>
                        </div>
                    </div>
                </div>
                <div class="clr"></div>
            </article>
            <article class="sec6">
                <div class="fancy">
                    <p class="subtitle">
                        <span title='Altitude Home %ALTITUDE% m'>&#127968;</span>
                    </p>
                    <div class="temp_num temp_num3">
                        <div>
                              <a style="font-family: 'Oxanium', sans-serif;font-size:28px;cursor:none;">%ALTITUDE%</a><span class="celsius">m</span>
                        </div>
                    </div>
                </div>
                <div class="fancy">
                    <p class="subtitle">
                        <span title='Fahrenheit Inside %FAHRENHEIT% &deg;C'>&#127968;</span>
                    </p>
                    <div class="temp_num temp_num3">
                        <div>
                              <a style="font-family: 'Oxanium', sans-serif;font-size:28px;">%FAHRENHEIT%</a><span class="celsius">&deg;F</span>
                        </div>
                    </div>
                </div>
                <div class="clr"></div>
            </article>
            <article class="sec7">
                <div class="fancy">
                    <p class="subtitle">
                        <span title='Temperature Outside MIN %Temp_min% &deg;C'>&#127780;</span>
                    </p>
                    <div class="temp_num temp_num3">
                        <div>
                              <a style="font-family: 'Oxanium', sans-serif;font-size:28px;cursor:none;">%Temp_min%</a><span class="celsius">&deg;C min</span>
                        </div>
                    </div>
                </div>
                <div class="fancy">
                    <p class="subtitle">
                        <span title='Temperature Outside MAX %Temp_max% &deg;C'>&#127780;</span>
                    </p>
                    <div class="temp_num temp_num3">
                        <div>
                              <a style="font-family: 'Oxanium', sans-serif;font-size:28px;">%Temp_max%</a><span class="celsius">&deg;C max</span>
                        </div>
                    </div>
                </div>
                <div class="clr"></div>
            </article>
            <article class="sec8">
                <div class="fancy">
                    <p class="subtitle1">
                        <span title='Measuring PIN33'>&#128267;</span>
                    </p>
                    <div class="temp_num temp_num3">
                        <div>
                              <a style="font-family: 'Oxanium', sans-serif;font-size:28px;cursor:none;">33</a>
                        </div>
                    </div>
                </div>
                <div class="fancy">
                    <p class="subtitle">
                        <span title='Voltage = %Adc% V'>&#9889;</span>
                    </p>
                    <div class="temp_num temp_num3">
                        <div>
                              <a style="font-family: 'Oxanium', sans-serif;font-size:28px;">%Adc%</a><span class="celsium">V</span>
                        </div>
                    </div>
                </div>
                <div class="clr"></div>
            </article>

   </center>
</div>                    
</div>
</div><script src='refresher2.js'></script>
<div style='display: none' class='Sun-div-1 div-1'>
   <div id="time3"><center><iframe class='myFrame' src='http://www.ihome.si:1653/busola' frameborder='0' scrolling='no'></iframe></center>
   </div>
</div>
<div style='display: none' class='Moon-div-1 div-1'>
<div id="time4">
<script src='moon.js'></script>
<center><iframe class='myFrame' src='https://moon.shawenyao.com/' frameborder='0' scrolling='no'></iframe></center>
</div>
</div>
<div style='display: none' class='Radio-div-1 div-1'>
   <div><iframe style='text-align:center;' src='http://www.ihome.si:888/radio_files/rad.html' width='350px' height='600px'></iframe></div>
</div>
  <script type='text/javascript' src='toggler.js'></script>
  <script  src='scripter.js'></script>
<center><div><iframe class='iframe1' scrolling='no' src='http://www.ihome.si:80/time.html'></iframe></div></center>
<script src="script.js"></script>
<link rel='stylesheet' href='http://www.ihome.si:80/radio.css'/> 
<div id="time5"><div id="tv"><iframe class='iframe' scrolling='no' name='Right' src='http://www.ihome.si:888/dark/speed.html'></iframe></div><script src='refresher5.js'></script>
<script src="gauge.min.js"></script>
<script src="wind2.js"></script>
</body>
</html>
)rawliteral";
const char servo[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
<meta name="theme-color" content="%COLOR%"/>
<meta name="viewport" content="width=device-width, initial-scale=1">
<meta http-equiv='Content-Type' content='text/html; charset=utf-8'/>
<link rel="icon" type="image/png" href="favicon.ico">
<link rel="stylesheet" href="https://cdnjs.cloudflare.com/ajax/libs/font-awesome/4.7.0/css/font-awesome.min.css">
<link rel="stylesheet" href="https://use.fontawesome.com/releases/v5.7.2/css/all.css" integrity="sha384-fnmOCqbTlWIlj8LyTjo7mOUStjsKC4pOpQbqyi7RrhN7udi9RwhKkMHpvLbHG9Sr" crossorigin="anonymous"/>
<link rel="stylesheet" href="https://fonts.googleapis.com/css?family=Oxanium">    
</head>
<body style="background: inherit;color: inherit;font-family: 'Oxanium', sans-serif;overflow-x:hidden;">
  <meta name="viewport" content="width=device-width, initial-scale=1">
<link rel="stylesheet" href="https://fonts.googleapis.com/css?family=Oxanium"> 
 <link rel="stylesheet" href="https://cdnjs.cloudflare.com/ajax/libs/font-awesome/4.5.0/css/font-awesome.min.css"/>
  <title>ESP32 Web Server</title>
<link rel="stylesheet" href="http://www.ihome.si:80/sliders.css"> 
    <link rel="stylesheet" href="https://use.fontawesome.com/releases/v5.7.2/css/all.css" integrity="sha384-fnmOCqbTlWIlj8LyTjo7mOUStjsKC4pOpQbqyi7RrhN7udi9RwhKkMHpvLbHG9Sr" crossorigin="anonymous"/>
<script src="https://code.jquery.com/jquery-3.2.1.min.js"></script>
</head>
<body style="font-family: 'Oxanium', sans-serif;">
<button title="Logout" style="font-family: 'Oxanium', sans-serif;z-index:59595959;font-size:25px;background:none;border:none;position:fixed;bottom:1px;right:1px;cursor:pointer;color:inherit;" onclick="logoutButton()"><i class="fas fa-sign-out-alt"></i></button>
  %BUTTONPLACEHOLDER%
<script src="logout.js"></script>
<div id="time" style='position:fixed;top:1px;right:5px;'><i title='WiFi Signal &percnt;' style='font-weight:900;font-size:20px;' class='fas fa-wifi'></i><span style='font-family:'Oxanium';font-size:20px;'>%WiFiSignal%</span><a class='spanner'></a></div><script src="time.js"></script>
 <btn1 id='btn1' title='Night Mode' style='display:block;font-size:22px;cursor:pointer;position:fixed;left:1px;top:1px;z-index:5000;'><i class='fas fa-moon'></i></btn1>
<script src='btn1.js'></script>
<btn2 id='btn2' title='Day Mode' style='display:block;font-size:22px;cursor:pointer;position:fixed;left:40px;top:1px;z-index:5000;'><i class='fas fa-sun'></i></btn2>
<script src='btn2.js'></script> 
<div><div style="width:300px;position:absolute;left:20px;top:10px;" title="SERVO A"><canvas class="canvas" style="font-family: 'Oxanium';width:300px;height:300px;border-radius:5px;opacity:0.85;color:inherit;" id="gauge-servoA"></canvas></div>
<script src="gauge.min.js"></script>
<script src="servoA.js"></script>
<div style="position:absolute;left:1px;top:300px;"><div id="newdata"><p><span class="textsliderValue1" id="textsliderValue1">%SLIDERVALUE1%</span></p><script src=%BACKCOLOR%></script></div><script src='newdata.js'></script>
<p><input title="SERVO A" type="range" onchange="updateSliderPWM(this)" id="pwmSlider1" min="0" max="180" value="%SLIDERVALUE1%" step="1" class="slider"></p></div>
<div style="width:300px;position:absolute;right:30px;top:10px;"><canvas class="canvas" style="color:inherit;font-family: 'Oxanium';width:300px;height:300px;border-radius:5px;opacity:0.85;" id="gauge-servoB"></canvas></div>
<div style="position:absolute;right:1px;top:300px;"><p><span class="textsliderValue2" id="textsliderValue2">%SLIDERVALUE2%</span></p>
<p><input  title="SERVO B" type="range" onchange="updateSliderPWM(this)" id="pwmSlider2" min="0" max="180" value="%SLIDERVALUE2%" step="1" class="slider1"></p></div>
</div>
<script src="gauge.min.js"></script>
<script src="servoB.js"></script>
<script>
function updateSliderPWM(element) {
  var sliderValue1 = document.getElementById("pwmSlider1").value;
  document.getElementById("textsliderValue1").innerHTML = sliderValue1;
  console.log(sliderValue1);
  var xhr = new XMLHttpRequest();
  xhr.open("GET", "/slider1?value="+sliderValue1, true);
  xhr.send();
  var sliderValue2 = document.getElementById("pwmSlider2").value;
  document.getElementById("textsliderValue2").innerHTML = sliderValue2;
  console.log(sliderValue2);
  var xhr = new XMLHttpRequest();
  xhr.open("GET", "/slider2?value="+sliderValue2, true);
  xhr.send();
}
</script>
<script src='http://www.ihome.si:80/fullscreen.js'></script>

<button class='btn btn-info ' id='show_hide_bt' title='FullScreen' onclick='view_fullscreen(this);fullscreen();' style='z-index:59595959;text-decoration:none;color:inherit;border:none;background:none;left:1px;cursor:pointer;position:fixed;bottom:10px;font-size:14px;'>
<i class='fa fa-eye'></i> Show</button> 
</body>
</html>
)rawliteral";

const char logout_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML>
<html>
<head>
<meta name="theme-color" content="%COLOR%"/>
<meta name="viewport" content="width=device-width, initial-scale=1">
<meta http-equiv='Content-Type' content='text/html; charset=utf-8'/>
<link rel="icon" type="image/png" href="favicon.ico">
<link rel="stylesheet" href="https://cdnjs.cloudflare.com/ajax/libs/font-awesome/4.7.0/css/font-awesome.min.css">
<link rel="stylesheet" href="https://fonts.googleapis.com/css?family=Oxanium">    
<link rel="stylesheet" href="https://use.fontawesome.com/releases/v5.7.2/css/all.css" integrity="sha384-fnmOCqbTlWIlj8LyTjo7mOUStjsKC4pOpQbqyi7RrhN7udi9RwhKkMHpvLbHG9Sr" crossorigin="anonymous"/>
</head>
<body style="background: inherit;font-family: 'Oxanium', sans-serif;overflow-x:hidden;">
<script src=%BACKCOLOR%></script>
<div style="posotion:fixed;left:50%;transform:translateX(-50%);"><a style="position:inherit;top:50%;transform:translateY(-50%);font-size: 20px;color:#000000;text-shadow:1px 1px #ffffff;" href="/">return to homepage</a></div>
 
</body>
</html>
)rawliteral";
String processor(const String& var){
  //Serial.println(var);
   
   if(var == "STATUS"){
    bool stateLedPin = digitalRead(LEDNIGHT);
  String strLedPin = stateLedPin ? "DAY" : "NIGHT"; 
    return strLedPin;
  }
 
  else if (var == "SLIDERVALUE1"){
    return sliderValue1;
  }
  else if (var == "SLIDERVALUE2"){
    return sliderValue2;
  }
  else if(var == "BUTTONPLACEHOLDER"){
   
  }
  else if(var == "STATUS1"){
    bool stateLedPin = digitalRead(LEDNIGHT);
  String strLedPin = stateLedPin ? "<a title='Light OFF' style='font-size:28px;cursor:none;color:red;'>OFF</a>" : "<a title='Light ON' style='font-size:28px;color:green;cursor:none;'>ON</a>";
    return strLedPin;
  }
  else if(var == "BACKCOLOR"){
    bool stateLedPin = digitalRead(LEDNIGHT);
  String strLedPin = stateLedPin ? "day.js" : "night.js";
    return strLedPin;
  }
  else if(var == "COLOR"){
    bool stateLedPin = digitalRead(LEDNIGHT);
  String strLedPin = stateLedPin ? "white" : "black";
    return strLedPin;
  }
  else if(var == "TEMPERATURE"){
    return lastTemperature;
  }
  else if(var == "TEMP"){
    return lastTemp;
  }
  else if(var == "Adc"){
    return lastAdc;
  }
  else if(var == "FAHRENHEIT"){
    return lastFahrenheit;
  }
 else if(var == "HUMIDITY"){
    return lastHumidity;
  }
 else if(var == "HUMI"){
    return lastHumi;
  }
 else if(var == "PRESSURE"){
    return lastPressure;
  }
  else if(var == "PRESS"){
    return lastpress;
  }
 else if(var == "SUNRISE"){
    return lastSunrise;
  }
  else if(var == "Sunsethour"){
    return lastSunsethour;
  }
  else if(var == "Sunsetminute"){
    return lastSunsetminute;
  }
  else if(var == "Sunsetsecond"){
    return lastSunsetsecond;
  }
  else if(var == "Sunrisehour"){
    return lastSunrisehour;
  }
  else if(var == "Sunriseminute"){
    return lastSunriseminute;
  }
  else if(var == "Sunrisesecond"){
    return lastSunrisesecond;
  }
else if(var == "Temp_feels_like"){
    return lastTemp_feels_like;
  }
  else if(var == "Temp_max"){
    return lastTemp_max;
  }
  else if(var == "Temp_min"){
    return lastTemp_min;
  }
 else if(var == "ALTITUDE"){
    return lastAltitude;
  }
  else if(var == "VISIBILITY"){
    return lastVisibility;
  }
  else if(var == "DEWPOINT"){
    return lastDewpoint;
  }
   else if(var == "ADDRESS"){
    return lastAddress;
  }
  else if(var == "Lat"){
    return lastLat;
  }
  else if(var == "Lon"){
    return lastLon;
  }
  else if(var == "DEWPOINT_OUT"){
    return lastDewpoint_out;
  }
  else if(var == "DESCRIPTION"){
    return lastDescription;
  }
else if(var == "Sun_rise_Second"){
    return lastSun_rise_Second;
  }
else if(var == "Sun_rise_Minute"){
    return lastSun_rise_Minute;
  }
else if(var == "Sun_rise_Hour"){
    return lastSun_rise_Hour;
  }
else if(var == "Sun_set_Second"){
    return lastSun_set_Second;
  }
else if(var == "Sun_set_Minute"){
    return lastSun_set_Minute;
  }
else if(var == "Sun_set_Hour"){
    return lastSun_set_Hour;
  }
  else if(var == "City_name"){
    return lastCity_name;
  }
 else if(var == "Wind_Speed"){
    return lastWind_Speed;
  } 
  else if(var == "Wind_Direction"){
    return lastWind_Direction;
  }
  else if(var == "Wind_Dir"){
    return lastWind_Dir;
  }
 else if(var == "from"){
    return lastfrom;
  }
else if(var == "Cloudsfrom"){
    return lastCloudsfrom;
  }
else if(var == "RAIN"){
    return lastpercntRain;
  }  
  else if(var == "Cloudspercnt"){
    return lastCloudspercnt;
  }
else if(var == "Wind_Miles_Names"){
    return lastWind_Miles_Names;
  }
  else if(var == "Wind_Miles"){
    return lastWind_Miles;
  }
  else if(var == "WiFiSignal"){
    return lastWiFiSignal;
  }
}
char* addZeroToFront(unsigned long input, byte maxLength) 
{
  static char addZeroBuffer[11];
  ltoa(input, addZeroBuffer, 10);
  byte inputLength = strlen(addZeroBuffer);

  if( maxLength > 10 )
  {
    maxLength = 10;
  }

  if ( maxLength > inputLength )
  {
    addZeroBuffer[maxLength] = '\0';
    byte difference = maxLength - inputLength;

    for (int i = maxLength - 1; 0 <= i; i--)
    {
      if ( i >= difference )
      {
        addZeroBuffer[i] = addZeroBuffer[i - difference];
      }
      else
      {
        addZeroBuffer[i] = '0';
      }
    }
  }

  return addZeroBuffer;
}

  
void setup() {
Serial.begin(115200);
 pinMode(LEDNIGHT, OUTPUT);
digitalWrite(LEDNIGHT, HIGH);
pinMode(LEDDAY, OUTPUT);
digitalWrite(LEDDAY, HIGH);
adc.attach(33);initWiFi();
configTime(0, 0, ntpServer);
initSPIFFS();
myServo1.attach(PIN_SERVO1);  // attaches the servo on the PIN_SERVO1 to the servo object
    myServo2.attach(PIN_SERVO2);  // attaches the servo on the PIN_SERVO1 to the servo object
  // configure LED PWM functionalitites
  
  // attach the channel to the GPIO to be controlled
  
  myServo1.write(sliderValue1.toInt());
  myServo2.write(sliderValue2.toInt());

initSensor();
  // Handle Web Server
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    if(!request->authenticate(http_username, http_password))
      return request->requestAuthentication();
    request->send_P(200, "text/html", index_html, processor);
  });
  server.on("/servo", HTTP_GET, [](AsyncWebServerRequest *request){
    if(!request->authenticate(http_username, http_password))
      return request->requestAuthentication();
    request->send_P(200, "text/html", servo, processor);
  });
  
  // Send a GET request to <ESP_IP>/slider?value=<inputMessage>
  server.on("/slider1", HTTP_GET, [] (AsyncWebServerRequest *request) {
    String inputMessage1;
    // GET input1 value on <ESP_IP>/slider?value=<inputMessage1>
    if (request->hasParam(PARAM_INPUT1)) {
      if(!request->authenticate(http_username, http_password))
      return request->requestAuthentication();
      inputMessage1 = request->getParam(PARAM_INPUT1)->value();
      sliderValue1 = inputMessage1;
      myServo1.write(sliderValue1.toInt());
    }
    else {
      inputMessage1 = "No message sent";
    }
    Serial.println(inputMessage1);
    request->send(200, "text/plain", "OK");
  });
  server.on("/slider2", HTTP_GET, [] (AsyncWebServerRequest *request) {
    String inputMessage2;
    // GET input1 value on <ESP_IP>/slider?value=<inputMessage2>
    if (request->hasParam(PARAM_INPUT2)) {
      if(!request->authenticate(http_username, http_password))
      return request->requestAuthentication();
      inputMessage2 = request->getParam(PARAM_INPUT2)->value();
      sliderValue2 = inputMessage2;
      myServo2.write(sliderValue2.toInt());
    }
    else {
      inputMessage2 = "No message sent";
    }
    Serial.println(inputMessage2);
    request->send(200, "text/plain", "OK");
  });
  server.on("/sliders.css", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/sliders.css", "text/css");
  });
  server.on("/day.js", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/day.js", "text/javascript");
  });
  server.on("/night.js", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/night.js", "text/javascript");
  });
  server.on("/btn1.js", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/btn1.js", "text/javascript");
  });
  server.on("/btn2.js", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/btn2.js", "text/javascript");
  });
  server.on("/gauge.min.js", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/gauge.min.js", "text/javascript");
  });
  server.on("/logout.js", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/logout.js", "text/javascript");
  });
   server.on("/time.html", HTTP_GET, [](AsyncWebServerRequest * request) {
    if(!request->authenticate(http_username, http_password))
      return request->requestAuthentication();
    request->send(SPIFFS, "/time.html", "text/html");
  });
   server.on("/wind.html", HTTP_GET, [](AsyncWebServerRequest * request) {
    if(!request->authenticate(http_username, http_password))
      return request->requestAuthentication();
    request->send(SPIFFS, "/wind.html", "text/html");
  });
  server.on("/servoA.html", HTTP_GET, [](AsyncWebServerRequest * request) {
    if(!request->authenticate(http_username, http_password))
      return request->requestAuthentication();
    request->send(SPIFFS, "/servoA.html", "text/html");
  });
  server.on("/servoA.js", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/servoA.js", "text/javascript");
  });
  server.on("/newdata.js", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/newdata.js", "text/javascript");
  });
  server.on("/servoB.html", HTTP_GET, [](AsyncWebServerRequest * request) {
    if(!request->authenticate(http_username, http_password))
      return request->requestAuthentication();
    request->send(SPIFFS, "/servoB.html", "text/html");
  });
  server.on("/servoB.js", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/servoB.js", "text/javascript");
  });
  server.on("/wind2.html", HTTP_GET, [](AsyncWebServerRequest * request) {
    if(!request->authenticate(http_username, http_password))
      return request->requestAuthentication();
    request->send(SPIFFS, "/wind2.html", "text/html");
  });
  server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/style.css", "text/css");
  });
  server.on("/styler.css", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/styler.css", "text/css");
  });
  server.on("/logout", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(401);
  });
  server.on("/logged-out", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", logout_html, processor);
  });
  server.serveStatic("/", SPIFFS, "/");
  server.on("/reset", HTTP_GET, [](AsyncWebServerRequest * request) {
    out_temperature = 0;
    out_temperatureD = 0;
    out_humidity = 0;
    out_pressure = 0;
    lastWind_Miles_Names = String(Wind_Miles_Names);
    lastWind_Speed = String(Wind_Speed);
    lastWind_Direction = String(Wind_Direction);
    sliderValue1 = String(SLIDERVALUE1);
    sliderValue2 = String(SLIDERVALUE2);
    request->send(200, "text/plain", "OK");
  });
  // Handle Web Server Events
  events.onConnect([](AsyncEventSourceClient * client) {
    if (client->lastId()) {
      Serial.printf("Client reconnected! Last meage ID that it got is: %u\n", client->lastId());
    }
   // send event with meage "CircuitDigest", id current millis
    // and set reconnect delay to 1 second
    client->send("iHOME", NULL, millis(), 5000);
  });
  server.addHandler(&events);
  server.begin();
}

void looper1() {
epochTime = getTime();
const unsigned long startTime1  = Sun_rise;
const unsigned long stopTime1 = Sun_set;
  // Send an HTTP GET request
  if ((millis() - last_time) > timer_delay) {
    // Check WiFi connection status
    if (WiFi.status() == WL_CONNECTED) {
      String serverPath = "http://api.openweathermap.org/data/2.5/weather?q=" + city + "," + countryCode + "&APPID=" + openWeatherMapApiKey;
      jsonBuffer = httpGETRequest(serverPath.c_str());
      Serial.println(jsonBuffer);
      JSONVar myObject = JSON.parse(jsonBuffer);
      // JSON.typeof(jsonVar) can be used to get the type of the var
      if (JSON.typeof(myObject) == "undefined") {
        Serial.println("Parsing input failed!");
        return;
      }
      Serial.print("JSON object = ");
      //Serial.println(myObject);
      Serial.print("Sunrise: ");
      //Serial.println(myObject["sys"]["sunrise"]);
      Serial.print("Sunset: ");
      //Serial.println(myObject["sys"]["sunset"]);
      Serial.print("Clouds: ");
      //Serial.println(myObject["clouds"]["all"]);
      int Clouds = myObject ["clouds"]["all"];
      int Cloudspercnt = myObject ["clouds"]["all"];
      lastCloudspercnt = String(Cloudspercnt);
      //Serial.println("Clouds percentage");
      //Serial.println(Cloudspercnt);
      Clouds_01 = Clouds;
      if (Clouds<11) j = 0;
   if (Clouds>11) j = 1;
   if (Clouds>25) j = 2;
   if (Clouds>51) j = 3; 
   if (Clouds>85) j = 4;
   //char* sector1 [] = {"Clear sky","Few clouds","Scattered clouds","Broken clouds","Overcast clouds"};
   if (j==0) Cloudsfrom="Clear sky";
   if (j==1) Cloudsfrom="Few clouds";
   if (j==2) Cloudsfrom="Scattered clouds";
   if (j==3) Cloudsfrom="Broken clouds";
   if (j==4) Cloudsfrom="Overcast clouds";
     lastCloudsfrom = String(Cloudsfrom);
     //Serial.println(Cloudsfrom);
     //Serial.println("Forecast area rainfall");
//Serial.println(myObject ["rain"]["1h"]);
     double percntRain = myObject ["rain"]["1h"];
     if (isnan(percntRain)) {
   double percntRain=0.00;
   double lastpercntRain=0.00;
  } else {
    lastpercntRain = String((String(percntRain).toDouble())*1);
     //Serial.println(lastpercntRain);
  }
     //lastRain = String(Rain).toDouble();
      //Serial.println("City: ");
      //Serial.println(myObject["name"]);
      City_name = myObject["name"];
      lastCity_name = String(City_name);
      Serial.print("Lat: ");
      //Serial.println(myObject["coord"]["lat"]);
      double Lat = myObject["coord"]["lat"];
      lastLat = String(String(Lat).toDouble(),3);
      Serial.print("Lon: ");
      //Serial.println(myObject["coord"]["lon"]);
      double Lon = myObject["coord"]["lon"];
      lastLon = String(String(Lon).toDouble(),3);
      Icon_ = myObject["weather"][0]["icon"];
      String Address = "http://www.ihome.si:80/" + Icon_ + ".png";
      weather_icon = String(Address);
      //Serial.println("Address:");
      //Serial.println(Address);
      //Serial.println("Weather Icon:");
      //Serial.println(myObject["weather"][0]["icon"]);
      //Serial.println("Temperature: ");
      //Serial.println(myObject["main"]["temp"]);// Reading in kelvin
      double Temp = (myObject["main"]["temp"]);// Reading in kelvin
      //Serial.println("Temp: ");
      //Serial.println(Temp);
      double Temp_feels_like = (myObject["main"]["feels_like"]);// Reading in kelvin
      //Serial.println("Temp_feels_like: ");
      //Serial.println(Temp_feels_like);
      lastTemp_feels_like=(String(Temp_feels_like).toInt()-273);
      double Temp_max = (myObject["main"]["temp_max"]);// Reading in kelvin
      //Serial.println("Temp_max: ");
      //Serial.println(Temp_max);
      lastTemp_max=(String(Temp_max).toInt()-273);
      double Temp_min = (myObject["main"]["temp_min"]);// Reading in kelvin
      //Serial.println("Temp_min: ");
      //Serial.println(Temp_min);
      lastTemp_min=(String(Temp_min).toInt()-273);
      //Serial.println("Humidity: ");
      //Serial.println(myObject["main"]["humidity"]);
      double vlaga (myObject["main"]["humidity"]);
      //Serial.println("vlaga: ");
      //Serial.println(vlaga);
      //Serial.println("Pressure Outside: ");
      int pritisak  = (myObject ["main"]["pressure"]);
      //Serial.println("pritisak");
      //Serial.println(pritisak);
      int Visibility = int(myObject["visibility"]);
      //Serial.println("Visibility");
      //Serial.println(Visibility);
      //Serial.println("Relay State: ");
      //Serial.println(digitalRead(LEDNIGHT));
      //Serial.println("UNIX Time: ");
      //Serial.println(epochTime);
      //Serial.println("Sunrise Time: ");
      //Serial.println(startTime1);
      //Serial.println("Sunset Time: ");
      //Serial.println(stopTime1);
      
      if (epochTime>=startTime1 && epochTime<=stopTime1) 
      {digitalWrite(LEDNIGHT,HIGH);digitalWrite(LEDDAY,HIGH);Serial.println("DAY");}
      else 
      {digitalWrite(LEDNIGHT,LOW);digitalWrite(LEDDAY,LOW);Serial.println("NIGHT");}
      
      Serial.print("Wind Speed: ");
      //Serial.println(myObject["wind"]["speed"]);
      Serial.print("Wind Direction: ");
      //Serial.println(myObject["wind"]["deg"]);
//char* sector2 [] = {"Calm","Light Air","Light Breeze","Gentle Breeze","Moderate Breeze","Fresh Breeze","Strong Breeze","Near Gale","Gale","Strong Gale","Whole Gale","Storm Force","Hurricane Force"}

      double Wind_Speed = myObject["wind"]["speed"];
      double Wind_Miles = Wind_Speed*2.237;
      //Serial.println("Wind Miles");
      //Serial.println(Wind_Miles);
      lastWind_Miles= String(Wind_Miles);
      Wind_Miles_01 = Wind_Miles;
      i=0;
      if(Wind_Miles<1) i = 0;
      if(Wind_Miles>1) i = 1;
      if(Wind_Miles>4) i = 2;
      if(Wind_Miles>8) i = 3;
      if(Wind_Miles>13) i = 4;
      if(Wind_Miles>19) i = 5;
      if(Wind_Miles>25) i = 6;
      if(Wind_Miles>32) i = 7;
      if(Wind_Miles>39) i = 8;
      if(Wind_Miles>47) i = 9;
      if(Wind_Miles>55) i = 10;
      if(Wind_Miles>64) i = 11;
      if(Wind_Miles>75) i = 12;
      if (i==0) Wind_Miles_Names="Calm";
      if (i==1) Wind_Miles_Names="Light Air";
      if (i==2) Wind_Miles_Names="Light Breeze";
      if (i==3) Wind_Miles_Names="Gentle Breeze";
      if (i==4) Wind_Miles_Names="Moderate Breeze";
      if (i==5) Wind_Miles_Names="Fresh Breeze";
      if (i==6) Wind_Miles_Names="Strong Breeze";
      if (i==7) Wind_Miles_Names="Near Gale";
      if (i==8) Wind_Miles_Names="Gale";
      if (i==9) Wind_Miles_Names="Strong Gale";
      if (i==10) Wind_Miles_Names="Whole Gale";
      if (i==11) Wind_Miles_Names="Storm Force";
      if (i==12) Wind_Miles_Names="Hurricane Force";
      lastWind_Miles_Names = String(Wind_Miles_Names);
      Serial.print(" i ");
      Serial.print(i);
      Serial.print(" wind name ");
      Serial.print(Wind_Miles_Names);
      double Wind_Direction = myObject["wind"]["deg"];
      Serial.print("Wind From: ");
      double windDir = myObject ["wind"]["deg"];
      windDir_01 = windDir;
      bme.readTemperature();
    float temperature = bme.readTemperature();
    //Serial.print(temperature);
    //Serial.println(" *C");
    lastTemperature = String(temperature);
    float fahrenheit = 1.8 * bme.readTemperature() + 32;
    //Serial.print(fahrenheit);
    //Serial.println(" *F");
    lastFahrenheit = String(fahrenheit);
    bme.readHumidity();
    float humidity = bme.readHumidity();
    //Serial.print(humidity);
    //Serial.println(" %");
    lastHumidity = String(humidity);
    bme.readPressure();
    float Pressure = bme.readPressure() / 100.0F;
    //Serial.print(Pressure);
    //Serial.println(" HPa");
    lastPressure = String(Pressure);
    lastpress = double(pritisak);
    lastTemp = (String(Temp).toInt()-273);
    bme.readAltitude(SEALEVELPRESSURE_HPA);
    float altitude = bme.readAltitude(SEALEVELPRESSURE_HPA);
    //Serial.print(altitude);
    //Serial.println(" m");
    lastAltitude = String(altitude);
    float dewpoint = dewPointFast(bme.readTemperature(), bme.readHumidity());
    //Serial.print(dewpoint);
    //Serial.println(" *C");
    lastDewpoint = String(dewpoint);
    lastWiFiSignal = String(WiFiSignal());
    lastWind_Speed = double(Wind_Speed);
    lastWind_Direction = double(Wind_Direction);
    lastVisibility = String(Visibility);
    lastHumi = String(vlaga);
    lastAddress = String(Address);
    lastAdc = String(adc.readVoltage()+3);
  
  time_t t =  int(Sun_rise); //unix timestamp
  setTime(t);
  //Serial.print("DateSunrise: ");
  int sunrisehour(hour()+1);
  lastSunrisehour = String(sunrisehour);
  //Serial.print(sunrisehour);
  //Serial.print(":");
  int sunriseminute(minute());

  lastSunriseminute = String(sunriseminute);
  //Serial.print(sunriseminute);
  //Serial.print(":");
  int sunrisesecond(second());
  lastSunrisesecond = String(sunrisesecond);
  //Serial.print(sunrisesecond);
    lastSunrise = int(Sun_rise);

  time_t tt =  int(Sun_set); //unix timestamp
  setTime(tt);
  //Serial.print("DateSunset: ");
  int sunsethour(hour()+1);
  lastSunsethour = String(sunsethour);
 // Serial.print(sunsethour);
 // Serial.print(":");
  int sunsetminute(minute());
  lastSunsetminute = String(sunsetminute);
 // Serial.print(sunsetminute);
 // Serial.print(":");
  int sunsetsecond(second());
  lastSunsetsecond = String(sunsetsecond);
//  Serial.print(sunsetsecond);
    lastSunset = String(Sun_set);
    h = 0;
   if (Wind_Direction>0.5) h = 0;
   if (Wind_Direction>22.0) h = 1;
   if (Wind_Direction>44.5) h = 2; 
   if (Wind_Direction>67.5) h = 3;
   if (Wind_Direction>89.5) h = 4;
   if (Wind_Direction>112.5) h = 5;
   if (Wind_Direction>134.5) h = 6;
   if (Wind_Direction>157.5) h = 7;
   if (Wind_Direction>179.5) h = 8;   
   if (Wind_Direction>202.5) h = 9;
   if (Wind_Direction>224.5) h = 10;
   if (Wind_Direction>247.5) h = 11;
   if (Wind_Direction>269.5) h = 12;
   if (Wind_Direction>292.5) h = 13; 
   if (Wind_Direction>314.5) h = 14;
   if (Wind_Direction>337.5) h = 15;
   if (Wind_Direction>359.5) h = 16;
   if (h==0) from="N";
   if (h==1) from="NNE";
   if (h==2) from="NE";
   if (h==3) from="ENE";
   if (h==4) from="E";
   if (h==5) from="ESE";
   if (h==6) from="SE";
   if (h==7) from="SSE";
   if (h==8) from="S";
   if (h==9) from="SSW";
   if (h==10) from="SW";
   if (h==11) from="WSW";
   if (h==12) from="W";
   if (h==13) from="WNW";
   if (h==14) from="NW";
   if (h==15) from="NNW";
   if (h==16) from="N";
      //Serial.print(" h ");
      //Serial.print(h);
      //Serial.print(" from ");
      //Serial.print(from);
      //Serial.println("");
      
      
    }
    else {
      Serial.println("WiFi Disconnected");
    }
    last_time = millis();
  }
} 
void loop() {
  if ((millis() - lastTime) > SensorDelay) {  
    lastWiFiSignal = String (WiFiSignal());
    float dewpoint = dewPointFast(bme.readTemperature(), bme.readHumidity());
    float dewpoint_out = dewPointFast(OutTemperature,OutHumidity);
    lastDewpoint = String(dewpoint);
    lastDewpoint_out = String(dewpoint_out);
    lastDescription = String(Description);
    lastfrom = String(from);
    lastClouds = String(Clouds);
    //Serial.println("Sun_rise-Hour");
    //Serial.println(addZeroToFront(String(lastSunrisehour).toInt() , 2));
    String Sun_rise_Second = (addZeroToFront(String(lastSunrisesecond).toInt() , 2));
    String Sun_rise_Minute = (addZeroToFront(String(lastSunriseminute).toInt() , 2));
    String Sun_rise_Hour = (addZeroToFront(String(lastSunrisehour).toInt() , 2));
    String Sun_set_Second = (addZeroToFront(String(lastSunsetsecond).toInt() , 2));
    String Sun_set_Minute = (addZeroToFront(String(lastSunsetminute).toInt() , 2));
    String Sun_set_Hour = (addZeroToFront(String(lastSunsethour).toInt() , 2));
    lastSun_rise_Second = String(Sun_rise_Second);
    lastSun_rise_Minute = String(Sun_rise_Minute);
    lastSun_rise_Hour = String(Sun_rise_Hour);
    lastSun_set_Second = String(Sun_set_Second);
    lastSun_set_Minute = String(Sun_set_Minute);
    lastSun_set_Hour = String(Sun_set_Hour);
    //Serial.println(Sun_rise_Second);
    //Serial.println(Sun_rise_Minute);
    //Serial.println(Sun_rise_Hour);
    //Serial.println(Sun_set_Second);
    //Serial.println(Sun_set_Minute);
    //Serial.println(Sun_set_Hour);

 
      //Serial.print(" j ");
      //Serial.print(j);
      //Serial.print(" clouds ");
      //Serial.print(Clouds);
      //Serial.println("");
    looper1();
    // Send Events to the Web Server with the Sensor Readings
    lastTime = millis();
  }
  if ((millis() - lastTimeAcc1) > OWMapDelay1) {
    events.send(getOWMReadings1().c_str(), "OWM_readings1", millis());
    lastTimeAcc1 = millis();
  }
  if ((millis() - lastTimeAcc) > OWMapDelay) {
    lastWiFiSignal = String (WiFiSignal());
    looper1();
    // Send Events to the Web Server with the Sensor Readings
    events.send(getOWMReadings().c_str(), "OWM_readings", millis());
    lastTimeAcc = millis();
  }
  
}
