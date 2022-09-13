

#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <ESP8266WiFi.h>
#include "ESPAsyncWebServer.h"

// Replace with your network credentials
const char* ssid = "SSID";
const char* password = "PASSWORD";
const char* http_username = "SELECTUSERHTTP";
const char* http_password = "SELECTPASSHTTP";

// assign the ESP8266 pins to arduino pins
#define D1 5
#define D2 4
#define D3 0
#define D4 2
#define D0 16
#define D6 12
#define BUTTON D6

// assign the SPI bus to pins
#define BME_SCK D1
#define BME_MISO D0
#define BME_MOSI D2
#define BME1_CS D3
#define BME2_CS D4

#include <Adafruit_GFX.h>      // Adafruit core graphics library
#include <Adafruit_ST7789.h>   // Adafruit hardware-specific library for ST7789
#include <Adafruit_BME280.h>   // Adafruit BME280 sensor library
#define SEALEVELPRESSURE_HPA (1013.25)

// ST7789 TFT module connections
#define TFT_DC 9// TFT DC  pin is connected to NodeMCU pin D3 (GPIO9)
#define TFT_RST 10// TFT RST pin is connected to NodeMCU pin D4 (GPIO10)
#define TFT_CS D8// TFT CS  pin is connected to NodeMCU pin D8 (GPIO15)

Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);
#define SCR_WD 240
#define SCR_HT 240
#define  BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF
Adafruit_BME280 bme1(BME1_CS, BME_MOSI, BME_MISO, BME_SCK); // software SPI
Adafruit_BME280 bme2(BME2_CS, BME_MOSI, BME_MISO, BME_SCK); // software SPI
//Adafruit_BME280 bme(BME_CS); // hardware SPI
//Adafruit_BME280 bme(BME_CS, BME_MOSI, BME_MISO, BME_SCK); // software SPI

// use only ENABLE_RRE_16B = 1 to save 4KB of flash
#include "RREFont.h"
#include "rre_term_10x16.h"
RREFont font;
// needed for RREFont library initialization, define your fillRect
void customRectRRE(int x, int y, int w, int h, int c) { return tft.fillRect(x, y, w, h, c); }  

//PropFont font;
// needed for PropFont library initialization, define your drawPixel and fillRect
//void customPixel(int x, int y, int c) { tft.drawPixel(x, y, c); }
//void customRect(int x, int y, int w, int h, int c) { tft.fillRect(x, y, w, h, c); } 

// -----------------
#include "DigiFont.h"
// needed for DigiFont library initialization, define your customLineH, customLineV and customRect
void customLineH(int x0,int x1, int y, int c) { tft.drawFastHLine(x0,y,x1-x0+1,c); }
void customLineV(int x, int y0,int y1, int c) { tft.drawFastVLine(x,y0,y1-y0+1,c); } 
void customRect(int x, int y,int w,int h, int c) { tft.fillRect(x,y,w,h,c); } 
DigiFont digi(customLineH,customLineV,customRect);
// --------------------------------------------------------------------------
int stateOld = HIGH;
long btDebounce    = 30;
long btDoubleClick = 600;
long btLongClick   = 700;
long btLongerClick = 2000;
long btTime = 0;
long btTime2 = 0;
int clickCnt = 1;

// 0=idle, 1,2,3=click, -1,-2=longclick

int checkButton()
{
  int state = digitalRead(BUTTON);
  if( state == LOW && stateOld == HIGH ) { btTime = millis(); stateOld = state; return 0; } // button just pressed
  if( state == HIGH && stateOld == LOW ) { // button just released
    stateOld = state;
    if( millis()-btTime >= btDebounce && millis()-btTime < btLongClick ) { 
      if( millis()-btTime2<btDoubleClick ) clickCnt++; else clickCnt=1;
      btTime2 = millis();
      return clickCnt; 
    } 
  }
  if( state == LOW && millis()-btTime >= btLongerClick ) { stateOld = state; return -2; }
  if( state == LOW && millis()-btTime >= btLongClick ) { stateOld = state; return -1; }
  return 0;
}
//-----------------------------------------------------------------------------
#define RGBIto565(r,g,b,i) ((((((r)*(i))/255) & 0xF8) << 8) | ((((g)*(i)/255) & 0xFC) << 3) | ((((b)*(i)/255) & 0xFC) >> 3)) 

const int dw1=23,dh1=23*2,dw2=56,dh2=56*2-10,sp=3,th1=7,th2=15;
const int yBig = dh1+13;
const int ySmall = 0;
const int yGraph = SCR_HT-65;

const uint16_t v1Col1 = RGBIto565(100,250,100,255);
const uint16_t v1Col2 = RGBIto565(100,250,100,200);
const uint16_t v1Col0 = RGBIto565(100,250,100,50);
const uint16_t v2Col1 = RGBIto565(255,250,100,255);
const uint16_t v2Col2 = RGBIto565(255,250,100,200);
const uint16_t v2Col0 = RGBIto565(255,250,100,50);
const uint16_t v3Col1 = RGBIto565(120,250,250,255);
const uint16_t v3Col2 = RGBIto565(120,250,250,200);
const uint16_t v3Col0 = RGBIto565(120,255,250,50);
const uint16_t v4Col1 = RGBIto565(255,120,120,255);
const uint16_t v4Col2 = RGBIto565(255,120,120,200);
const uint16_t v4Col0 = RGBIto565(255,120,120,50);

char txt[20];
int numVal,numAvg;
#define NUM_VAL 48

class value {
public:
  value() { for(int i=0;i<NUM_VAL;i++) tab[i]=0; };
  //~value() {};
  void init(float v, uint16_t _c0, uint16_t _c1, uint16_t _c2) { cur=avg=min=max=v; c0=_c0; c1=_c1; c2=_c2; }
  float cur;
  float avg;  // average value from last 3600 measurements (60 minutes)
  float min,max; // min/max from last 3600 values (60 minutes)
  uint16_t c0,c1,c2;
  float tab[NUM_VAL]={0};

  void store(float v, bool store)
  {
    cur = v;
    if(cur < min) min=cur;
    if(cur > max) max=cur;
    avg = 0.99*avg + 0.01*cur;
    if(store) {
      for(int i=NUM_VAL-1;i--;) tab[i+1]=tab[i];
      tab[0] = avg;
      avg = min = max = cur;
    }
  }

  void debug(int x, int y, int h, int d=4)
  {
    dtostrf(avg,d,1,txt); digi.printNumber7(txt,x,y);
    dtostrf(min,d,1,txt); digi.printNumber7(txt,x,y+h*1);
    dtostrf(max,d,1,txt); digi.printNumber7(txt,x,y+h*2);
  }

  void graph()
  {
    int i, x=1, y=yGraph;
    int wd=4*48, ht=64;
    float min,max;
    min=max=numVal>0 ? tab[0] : avg;
    for(i=1;i<NUM_VAL;i++) {
      if(tab[i]<=0) continue;
      if(tab[i]<min) min=tab[i];
      if(tab[i]>max) max=tab[i];
    }
    //Serial.println(min); Serial.println(max);
    min = floor(min);
    max = ceil(max);
    //Serial.println(min); Serial.println(max);
    int yv,dy=max-min;
    tft.drawRect(x-1,y-1,wd+2,ht+2,RGBIto565(150,150,150,150));
    tft.drawLine(x+wd/2,y,x+wd/2,y+ht,RGBIto565(100,100,100,100));
    digi.setColors(c1,c0);
    digi.setSize7(9,18,3,0);
    digi.setSpacing(2);
    int w = digi.numberWidth("8888");
    dtostrf(max,4,0,txt); digi.printNumber7(txt,SCR_WD-w,y-1);
    dtostrf(min,4,0,txt); digi.printNumber7(txt,SCR_WD-w,y+ht-18+1);
    for(i=0;i<NUM_VAL;i++) {
      if(tab[i]<=0) continue;
      yv=(tab[i]-min)*ht/dy;
      //Serial.print(i);  Serial.print("  ");  Serial.println(yv);
      if(ht-yv>0) tft.fillRect(x+(NUM_VAL-1-i)*4,y,4,ht-yv,BLACK);
      if(yv>0)    tft.fillRect(x+(NUM_VAL-1-i)*4,y+ht-yv,4,yv,(i&1)?c1:c2);
    }
  }
};

//-----------------------------------------------------------------------------

value temp, hum, pres, temp1, hum1, pres1;
unsigned long storeTime;
unsigned long ms = 0;
int mode=0;
int lastMode=-1;
int lastDebugMode=0;
int debugMode=0;
int wd,x,y;
float temperature;
float humidity;
float pressure;
float dewpoint;
float fahrenheit;
float altitude;
float temperature1;
float humidity1;
float pressure1;
float dewpoint1;
float fahrenheit1;
float altitude1;
String lastWiFiSignal;
AsyncWebServer server(80);
AsyncEventSource events("/events");

unsigned long lastTime = 0;  
unsigned long timerDelay = 5000;  // send readings timer
double dewPointFast(double celsius, double humidity)
 {
   double a = 17.62;
   double b = 243.12;
   double temp = (a * celsius) / (b + celsius) + log(humidity * 0.01);
   double Td = (b * temp) / (a - temp);
   return Td;
 }
 String WiFiSignal() {
  float Signal = WiFi.RSSI();
  Signal = 90 / 40.0 * Signal + 212.5; // From Signal = 100% @ -50dBm and Signal = 10% @ -90dBm and y = mx + c
  if (Signal > 100) Signal = 100;
  return " " + String(Signal, 0) + "%";
}
void getBME280Readings(){
 
  temperature = bme1.readTemperature();
  pressure = bme1.readPressure() / 100.0F;
  humidity = bme1.readHumidity();
  dewpoint = dewPointFast(bme1.readTemperature(), bme1.readHumidity());
  fahrenheit = 1.8 * bme1.readTemperature() + 32;
  altitude = bme1.readAltitude(SEALEVELPRESSURE_HPA);
}
void getBME280Readings1(){
 
  temperature1 = bme2.readTemperature();
  pressure1 = bme2.readPressure() / 100.0F;
  humidity1 = bme2.readHumidity();
  dewpoint1 = dewPointFast(bme2.readTemperature(), bme2.readHumidity());
  fahrenheit1 = 1.8 * bme2.readTemperature() + 32;
  altitude1 = bme2.readAltitude(SEALEVELPRESSURE_HPA);
}


String processor(const String& var){
  getBME280Readings();
  getBME280Readings1();
  //Serial.println(var);
  if(var == "TEMPERATURE"){
    return String(temperature);
  }
  else if(var == "HUMIDITY"){
    return String(humidity);
  }
  else if(var == "PRESSURE"){
    return String(pressure);
  }
  else if(var == "DEWPOINT"){
    return String(dewpoint);
  }
  else if(var == "FAHRENHEIT"){
    return String(fahrenheit);
  }
  else if(var == "ALTITUDE"){
    return String(altitude);
  }
  else if(var == "TEMPERATURE1"){
    return String(temperature1);
  }
  else if(var == "HUMIDITY1"){
    return String(humidity1);
  }
  else if(var == "PRESSURE1"){
    return String(pressure1);
  }
  else if(var == "DEWPOINT1"){
    return String(dewpoint1);
  }
  else if(var == "FAHRENHEIT1"){
    return String(fahrenheit1);
  }
  else if(var == "ALTITUDE1"){
    return String(altitude1);
  }
  else if(var == "WiFiSignal"){
    return lastWiFiSignal;
  }
  else if(var == "BUTTONPLACEHOLDER"){
     return String();
  }
}

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <title>BME280 Web Server</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <script src="https://code.jquery.com/jquery-3.2.1.min.js"></script>
  <link rel="stylesheet" href="https://use.fontawesome.com/releases/v5.7.2/css/all.css" integrity="sha384-fnmOCqbTlWIlj8LyTjo7mOUStjsKC4pOpQbqyi7RrhN7udi9RwhKkMHpvLbHG9Sr" crossorigin="anonymous">
  <link rel="icon" href="data:,">
  <style>
    html {font-family: 'Calibri', sans-serif; display: inline-block; text-align: center; }
    p {  font-size: 1.2rem;}
    body {  font-family: 'Calibri', sans-serif;margin: 0;}
    .content { padding:20px;}
    .topnav { overflow: hidden; background-color: #4B1D3F; color: white; font-size: 1.2rem; }
    .card { background-color: white; box-shadow: 2px 2px 12px 1px rgba(140,140,140,.5); }
    .cards { max-width: 1920px; margin: 10 auto; display: grid; grid-gap: 1rem; grid-template-columns: repeat(auto-fit, minmax(150px, 1fr)); }
    .reading { font-size: 1.8rem; }
    .card.temperature { color: #ff0000; }
    .card.humidity { color: #17bebb; }
    .card.pressure { color: #3fca6b; }
    .card.dewpoint { color: blue; }
    .card.fahrenheit { color: yellow; }
    .card.altitude { color: grey; }
    .card.gas { color: #d62246; }
  </style>
</head>
<body>
 <button title="Logout" style="z-index:98989898;text-shadow:1px 1px #fff;font-size:20px;background:none;border:none;position:fixed;bottom:1px;left:1px;cursor:pointer;" onclick="logoutButton()"><i class="fas fa-sign-out-alt"></i></button>
  %BUTTONPLACEHOLDER%
<script>function toggleCheckbox(element) {
  
}
function logoutButton() {
  var xhr = new XMLHttpRequest();
  xhr.open("GET", "/logout", true);
  xhr.send();
  setTimeout(function(){ window.open("/logged-out","_self"); }, 2000);
}
</script>
  <center><div id="screen" class="content">
   <i style="z-index:98989898;text-shadow:1px 1px #fff;font-size:20px;background:none;border:none;position:fixed;bottom:1px;right:1px;cursor:pointer;" class="fas fa-wifi">%WiFiSignal%</i> 
    <div class="cards">
      <div class="card temperature">
        <h4><i class="fas fa-thermometer-half"></i> TEMPERATURE</h4><p><span class="reading"><span id="temp">%TEMPERATURE%</span> &deg;C</span></p>
      </div>
      <div class="card humidity">
        <h4><i class="fas fa-tint"></i> HUMIDITY</h4><p><span class="reading"><span id="hum">%HUMIDITY%</span> &percnt;</span></p>
      </div>
      <div class="card pressure">
        <h4><i class="fas fa-wind"></i> PRESSURE</h4><p><span class="reading"><span id="pres">%PRESSURE%</span> hPa</span></p>
      </div>
      <div class="card dewpoint">
        <h4 style="color:blue;"><i class="fas fa-water"></i> DEWPOINT</h4><p><span class="reading"><span style="color:blue;" id="dew">%DEWPOINT%</span> &deg;C</span></p>
      </div>
      <div class="card fahrenheit">
        <h4><i class="fas fa-thermometer-full"></i> FAHRENHEIT</h4><p><span class="reading"><span id="far">%FAHRENHEIT%</span> &deg;F</span></p>
      </div>
      <div class="card altitude">
        <h4><i class="fas fa-mountain"></i> ALTITUDE</h4><p><span class="reading"><span id="alt">%ALTITUDE%</span> m</span></p>
      </div>
    </div>
    <hr />
    <div class="cards">
      <div class="card temperature">
        <h4><i class="fas fa-thermometer-half"></i> TEMPERATURE1</h4><p><span class="reading"><span id="temp1">%TEMPERATURE1%</span> &deg;C</span></p>
      </div>
      <div class="card humidity">
        <h4><i class="fas fa-tint"></i> HUMIDITY1</h4><p><span class="reading"><span id="hum1">%HUMIDITY1%</span> &percnt;</span></p>
      </div>
      <div class="card pressure">
        <h4><i class="fas fa-wind"></i> PRESSURE1</h4><p><span class="reading"><span id="pres1">%PRESSURE1%</span> hPa</span></p>
      </div>
      <div class="card dewpoint">
        <h4 style="color:blue;"><i class="fas fa-water"></i> DEWPOINT1</h4><p><span class="reading"><span style="color:blue;" id="dew1">%DEWPOINT1%</span> &deg;C</span></p>
      </div>
      <div class="card fahrenheit">
        <h4><i class="fas fa-thermometer-full"></i> FAHRENHEIT1</h4><p><span class="reading"><span id="far1">%FAHRENHEIT1%</span> &deg;F</span></p>
      </div>
      <div class="card altitude">
        <h4><i class="fas fa-mountain"></i> ALTITUDE1</h4><p><span class="reading"><span id="alt1">%ALTITUDE%</span> m</span></p>
      </div>
    </div>
 </div>
 </center>
<script type="text/javascript">
var auto_refresh = setInterval(
function () {
$("#screen").load(" #screen > *");
}, 2000);
auto_refresh = null;
</script>
<script>
if (!!window.EventSource) {
 var source = new EventSource('/events');
 
 source.addEventListener('open', function(e) {
  console.log("Events Connected");
 }, false);
 source.addEventListener('error', function(e) {
  if (e.target.readyState != EventSource.OPEN) {
    console.log("Events Disconnected");
  }
 }, false);
 
 source.addEventListener('message', function(e) {
  console.log("message", e.data);
 }, false);
 
 source.addEventListener('temperature', function(e) {
  console.log("temperature", e.data);
  document.getElementById("temp").innerHTML = e.data;
 }, false);
 
 source.addEventListener('humidity', function(e) {
  console.log("humidity", e.data);
  document.getElementById("hum").innerHTML = e.data;
 }, false);
 
 source.addEventListener('pressure', function(e) {
  console.log("pressure", e.data);
  document.getElementById("pres").innerHTML = e.data;
 }, false);
 source.addEventListener('dewpoint', function(e) {
  console.log("dewpoint", e.data);
  document.getElementById("dew").innerHTML = e.data;
 }, false);
 source.addEventListener('fahrenheit', function(e) {
  console.log("fahrenheit", e.data);
  document.getElementById("far").innerHTML = e.data;
 }, false);
 source.addEventListener('altitude', function(e) {
  console.log("altitude", e.data);
  document.getElementById("alt").innerHTML = e.data;
 }, false);
  source.addEventListener('temperature1', function(e) {
  console.log("temperature1", e.data);
  document.getElementById("temp1").innerHTML = e.data;
 }, false);
 
 source.addEventListener('humidity1', function(e) {
  console.log("humidity1", e.data);
  document.getElementById("hum1").innerHTML = e.data;
 }, false);
 
 source.addEventListener('pressure1', function(e) {
  console.log("pressure1", e.data);
  document.getElementById("pres1").innerHTML = e.data;
 }, false);
 source.addEventListener('dewpoint1', function(e) {
  console.log("dewpoint1", e.data);
  document.getElementById("dew1").innerHTML = e.data;
 }, false);
 source.addEventListener('fahrenheit1', function(e) {
  console.log("fahrenheit1", e.data);
  document.getElementById("far1").innerHTML = e.data;
 }, false);
 source.addEventListener('altitude1', function(e) {
  console.log("altitude1", e.data);
  document.getElementById("alt1").innerHTML = e.data;
 }, false);
source.addEventListener('lastWiFiSignal', function(e) {
  console.log("lastWiFiSignal", e.data);
  document.getElementById("wifi").innerHTML = e.data;
 }, false);
 
}
</script>
</body>
</html>)rawliteral";
void notFound(AsyncWebServerRequest *request) {
  request->send(404, "text/plain", "Not found");
}
const char logout_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
</head>
<body>
  <p>Logged out - <a href="/">return to Temperature webserver</a>.</p>
</body>
</html>
)rawliteral";
void showVal(float v, int x, int y, int w, uint16_t col1, uint16_t col2, uint16_t col0)
{
  dtostrf(v,w,1,txt);
  digi.setColors(col1,col2,col0);
  digi.printNumber7(txt,x,y);
}

void readBME280()
{
  tft.setTextSize(1);
  tft.setTextColor(ST77XX_WHITE, ST77XX_BLACK);  // set text color to yellow and black background
  tft.setCursor(220, 120);
  tft.printf("IN ");
  float t = bme1.readTemperature();
  float h = bme1.readHumidity();
  float p = bme1.readPressure()/100.0;
   // bool store = ( millis()-storeTime>60L*60L*1000L );  // store value every 60 minutes
   bool store = ( millis()-storeTime>20*1000L ); // store value every 120 seconds
  if(t>-20 && t<100) temp.store(t,store);
  if(h>0 && h<100) hum.store(h,store);
  if(p>800 && p<1300) pres.store(p,store);
  numAvg++;
  if(store) {
    storeTime = millis();
    if(++numVal>=NUM_VAL) numVal=NUM_VAL;
    numAvg = 0;
  }
}
void readBME2801()
{
  tft.setTextSize(1);
  tft.setTextColor(ST77XX_WHITE, ST77XX_BLACK);  // set text color to yellow and black background
  tft.setCursor(220, 120);
  tft.printf("OUT");
  float t = bme2.readTemperature();
  float h = bme2.readHumidity();
  float p = bme2.readPressure()/100.0;
   // bool store = ( millis()-storeTime>60L*60L*1000L );  // store value every 60 minutes
   bool store = ( millis()-storeTime>20*1000L ); // store value every 120 seconds
  if(t>-20 && t<100) temp1.store(t,store);
  if(h>0 && h<100) hum1.store(h,store);
  if(p>800 && p<1300) pres1.store(p,store);
  numAvg++;
  if(store) {
    storeTime = millis();
    if(++numVal>=NUM_VAL) numVal=NUM_VAL;
    numAvg = 0;
  }
}
void bigT()
{
  digi.setSize7(dw2,dh2,th2,th2/2);
  digi.setSpacing(4);
  wd = digi.numberWidth("88.8");
  x = (SCR_WD-wd-24)/2;
  if(temp.cur<0 || temp.cur>99) return;
  showVal(temp.cur, x,yBig, 4,v1Col1,v1Col2,v1Col0);
  tft.fillCircle(x+wd+4+12,yBig+12,12,v1Col1);
  tft.fillCircle(x+wd+4+12,yBig+12,6,v1Col0);
}


void bigH()
{
  digi.setSize7(dw2,dh2,th2,th2/2);
  digi.setSpacing(4);
  wd = digi.numberWidth("88.8");
  x = (SCR_WD-wd-8*3-3)/2;
  if(hum.cur<0 || hum.cur>99) return;
  showVal(hum.cur, x,yBig, 4,v2Col1,v2Col2,v2Col0);
  font.setScale(3,3);
  font.setColor(v2Col0); font.printStr(x+wd+4+3,yBig+3,"%");
  font.setColor(v2Col1); font.printStr(x+wd+4,yBig,"%");
}

void bigP()
{
  digi.setSize7(dw2*7/10,dh2,13,6);
  digi.setSpacing(4);
  wd = digi.numberWidth("8888.8");
  x = (SCR_WD-wd)/2;
  showVal(pres.cur, x,yBig, 6,v3Col1,v3Col2,v3Col0);
}

void smallT()
{
  showVal(temp.cur, 0,y, 4,v1Col1,v1Col1,v1Col0);
  font.setScale(2);
  font.setColor(v1Col0); font.printStr(wd+3+3,y+3,"'");
  font.setColor(v1Col1); font.printStr(wd+3,y,"'");
}

void smallH(int x)
{
  if(x) {
    wd = digi.numberWidth("88.8");
    x = SCR_WD-wd-8*2-3;
  }
  showVal(hum.cur, x,y, 4,v2Col1,v2Col1,v2Col0);
  font.setScale(2);
  font.setColor(v2Col0); font.printStr(x+wd+3+3,y+3,"%");
  font.setColor(v2Col1); font.printStr(x+wd+3,y,"%");
}

void smallP()
{
  wd = digi.numberWidth("8888.8");
  showVal(pres.cur, SCR_WD-wd,y, 6,v3Col1,v3Col1,v3Col0);
}

void smallCommon()
{
  //digi.setSize7(dw1,dw1*2,th1,th1/2);
  digi.setSize7(dw1,dw1*2,7,2);
  digi.setSpacing(3);
  wd = digi.numberWidth("88.8");
  y = ySmall;
}

void debug()
{
digi.setColors(RED,RGBIto565(100,0,0,0));
digi.setSize7(11,20,3,1);
  digi.setSpacing(2);
  y = yGraph;
  temp.debug(0,y,23,4);
  hum.debug(60-4,y,23,4);
  pres.debug(120-8,y,23,6);
  dtostrf(numAvg,4,0,txt); digi.printNumber7(txt,190,y);
  dtostrf(numVal,4,0,txt); digi.printNumber7(txt,190,y+23);
}

void mode0()
{
  bigT();
  smallCommon();
  smallH(0);
  smallP();
  debugMode ? debug() : temp.graph();
}

void mode1()
{
  bigH();
  smallCommon();
  smallT();
  smallP();
  debugMode ? debug() : hum.graph();
}

void mode2()
{
  bigP();
  smallCommon();
  smallT();
  smallH(1);
  debugMode ? debug() : pres.graph();
}

void drawScreen()
{
  if(mode==0) mode0(); else
  if(mode==1) mode1(); else
  if(mode==2) mode2();
}


void setup() {
 pinMode(BUTTON, INPUT_PULLUP);
   tft.init(240, 240, SPI_MODE2);

  // if the screen is flipped, remove this command
  tft.setRotation(2);
  font.init(customRectRRE, SCR_WD, SCR_HT); // custom fillRect function and screen width and height values 
  font.setFont(&rre_term_10x16);
  font.setScale(2);
  // fill the screen with black color
  temp.init(bme1.readTemperature(),v1Col0,v1Col1,v1Col2);
  hum.init(bme1.readHumidity(),v2Col0,v2Col1,v2Col2);
  pres.init(bme1.readPressure()/100.0,v3Col0,v3Col1,v3Col2);
  temp1.init(bme2.readTemperature(),v1Col0,v1Col1,v1Col2);
  hum1.init(bme2.readHumidity(),v2Col0,v2Col1,v2Col2);
  pres1.init(bme2.readPressure()/100.0,v3Col0,v3Col1,v3Col2);
  numVal = numAvg = 0;
  storeTime = millis();
  
  tft.setTextWrap(false);         // turn off text wrap option
  tft.setTextColor(ST77XX_WHITE, ST77XX_BLACK);  // set text color to green and black background
  //tft.print("TEMPERATURE:");
  //tft.print("HUMIDITY:");
  //tft.print("PRESSURE:");
  tft.setTextSize(4);        // text size = 4
 
  Serial.begin(115200);

  // Set the device as a Station and Soft Access Point simultaneously
  //WiFi.mode(WIFI_AP_STA);
  
  // Set device as a Wi-Fi Station
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(2000);
    Serial.println("Setting as a Wi-Fi Station..");
  }
  Serial.print("Station IP Address: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  bool status;

  // default settings
  // (you can also pass in a Wire library object like &Wire2)
   status = bme1.begin();
    if (!status) {
        Serial.println("Could not find a valid BME280 sensor 1 , check wiring!");
    }

    // default settings
    status = bme2.begin();
    if (!status) {
        Serial.println("Could not find a valid BME280 sensor 2 , check wiring!");
    }    
  
  // Handle Web Server
 server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    if(!request->authenticate(http_username, http_password))
      return request->requestAuthentication();
    request->send_P(200, "text/html", index_html, processor);
  });
    
  server.on("/logout", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(401);
  });

  server.on("/logged-out", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", logout_html, processor);
  });

  // Handle Web Server Events
  events.onConnect([](AsyncEventSourceClient *client){
    if(client->lastId()){
      Serial.printf("Client reconnected! Last message ID that it got is: %u\n", client->lastId());
    }
    // send event with message "hello!", id current millis
    // and set reconnect delay to 1 second
    client->send("hello!", NULL, millis(), 10000);
  });
  server.addHandler(&events);
  server.onNotFound(notFound);
  server.begin();
}

void loop() {
  if ((millis() - lastTime) > timerDelay) {
    getBME280Readings();    
    if(mode!=lastMode || debugMode!=lastDebugMode) {
    lastMode = mode;
    lastDebugMode = debugMode;
    tft.fillScreen(BLACK);
    drawScreen();
  }

  if(millis()-ms>1000) {
    ms = millis();
    readBME280();
    drawScreen();
  }

  int st = checkButton();
  int sensorVal = digitalRead(BUTTON);
  if(st>0) { if(++mode>2) mode=0; }
  if(st<0) { debugMode=!debugMode; if(sensorVal==LOW); }   
    
    Serial.printf("Temperature = %.2f ºC \n", temperature);
    Serial.printf("Humidity = %.2f % \n", humidity);
    Serial.printf("Pressure = %.2f hPa \n", pressure);
    Serial.printf("Dewpoint = %.2f ºC \n", dewpoint);
    Serial.printf("Altitude = %.2f ºC \n", altitude);
    Serial.printf("Fahrenheit = %.2f ºC \n", fahrenheit);
  //print out the value of the pushbutton
  Serial.println(st);    

    // Send Events to the Web Server with the Sensor Readings
    events.send("ping",NULL,millis());
    events.send(String(temperature).c_str(),"temperature",millis());
    events.send(String(humidity).c_str(),"humidity",millis());
    events.send(String(pressure).c_str(),"pressure",millis());
    events.send(String(dewpoint).c_str(),"dewpoint",millis());
    events.send(String(altitude).c_str(),"altitude",millis());
    events.send(String(fahrenheit).c_str(),"fahrenheit",millis());
    lastWiFiSignal = String (WiFiSignal());
    lastTime = millis();
  }
  else if ((millis() - lastTime) > timerDelay) {
    getBME280Readings1();    
    if(mode!=lastMode || debugMode!=lastDebugMode) {
    lastMode = mode;
    lastDebugMode = debugMode;
    tft.fillScreen(BLACK);
    drawScreen();
  }

  if(millis()-ms>1000) {
    ms = millis();
    readBME2801();
    drawScreen();
  }

  int st1 = checkButton();
  int sensorVal = digitalRead(BUTTON);
  if(st1>0) { if(++mode>2) mode=0; }
  if(st1<0) { debugMode=!debugMode; if(sensorVal==LOW); }

    Serial.printf("Temperature = %.2f ºC \n", temperature1);
    Serial.printf("Humidity = %.2f % \n", humidity1);
    Serial.printf("Pressure = %.2f hPa \n", pressure1);
    Serial.printf("Dewpoint = %.2f ºC \n", dewpoint1);
    Serial.printf("Altitude = %.2f ºC \n", altitude1);
    Serial.printf("Fahrenheit = %.2f ºC \n", fahrenheit1);
  //print out the value of the pushbutton
  Serial.println(st1);
    // Send Events to the Web Server with the Sensor Readings
    events.send("ping",NULL,millis());
    events.send(String(temperature1).c_str(),"temperature",millis());
    events.send(String(humidity1).c_str(),"humidity",millis());
    events.send(String(pressure1).c_str(),"pressure",millis());
    events.send(String(dewpoint1).c_str(),"dewpoint",millis());
    events.send(String(altitude1).c_str(),"altitude",millis());
    events.send(String(fahrenheit1).c_str(),"fahrenheit",millis());
    lastWiFiSignal = String (WiFiSignal());
    lastTime = millis();
  }
}