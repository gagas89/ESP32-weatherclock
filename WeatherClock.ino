#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <TFT_eSPI.h>   // Graphics and font library
#include "FreeFont.h"  
#include <Time.h>
#include <PNGdec.h>

PNG png; // PNG decoder instance

#define MAX_IMAGE_WIDTH 240 // Adjust for your images
#define ICON_SUNNY 0
#define ICON_PARTLY 1
#define ICON_CLOUDY 2
#define ICON_RAIN 3
#define ICON_THUNDER 4

int16_t xpos = 10;
int16_t ypos = 165;

// === WiFi ===
const char* ssid     = "Kedai Unay";
const char* password = "baksoSAPI";
// === NTP ===
const long  gmtOffset_sec = 7 * 3600;   // GMT+7 (WIB)
const int   daylightOffset_sec = 0;     // no daylight saving in Indonesia
const char* weekDays[7] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
char year[5]; 
char month[3];
char day;
String forecastData[4][5];  

// === Weather API ===
String apiKey = "52113b2cee5bb722c12342dc852aaa3c";
String city   = "Sidoarjo";
String units  = "metric";   // "metric" for °C, "imperial" for °F

// === TFT ===
TFT_eSPI tft = TFT_eSPI();

#include "weatherIcon.h"
// === Setup ===
void setup() {
  Serial.begin(115200);
  tft.init();
  tft.setRotation(1);
  tft.fillScreen(TFT_WHITE);
  tft.setTextColor(TFT_BLACK, TFT_WHITE);
  tft.setTextFont(2);
  /*
  drawSun(40, 40, 15);
  drawCloud(100, 40);
  drawRain(40, 100);
  drawThunder(120, 100);
  
  while(1);
  */
  WiFi.begin(ssid, password);
  tft.println("Connecting WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  tft.println("Connected!");
  //NTP Config
  configTime(gmtOffset_sec, daylightOffset_sec, "216.239.35.0","202.162.32.12" );
  struct tm timeinfo;
  while (!getLocalTime(&timeinfo)) 
  {
    Serial.println("NTP Sync Fail");
    delay(1000);
  }
    Serial.println("NTP Sync Success");
  tft.fillScreen(tft.color565(0x21, 0x76, 0xAE));
  timeNdate();
  fetchWeather();
  Serial.println("fetchweather_done");
  forecast(); 
}
//==== NTP Time & Date Inquiry ====
void timeNdate()
{
  struct tm timeinfo;
    if (!getLocalTime(&timeinfo)) {
    Serial.println("Failed to obtain time");
    return;
    }
    
  char timeStr[6];
  strftime(timeStr, sizeof(timeStr), "%H:%M", &timeinfo);

  // Format date as "dd/mm/yyyy"
  char dateStr[11];
  strftime(dateStr, sizeof(dateStr), "%d/%m/%Y", &timeinfo);
  strftime(year, sizeof(year), "%Y", &timeinfo);   // 4-digit year
  strftime(month, sizeof(month), "%m", &timeinfo); // 2-digit month
  day = timeinfo.tm_mday + 2;
  tft.fillRect(0, 0, 160, 120, tft.color565(0x21, 0x76, 0xAE));
  tft.setTextSize(1);
  tft.setTextDatum(TL_DATUM);
  tft.setFreeFont(FF24);
  tft.setTextColor(tft.color565(0xFB, 0xB1, 0x3C), tft.color565(0x21, 0x76, 0xAE));
  tft.drawString(timeStr, 15, 20, 1);
  tft.setTextColor(TFT_BLACK, tft.color565(0x21, 0x76, 0xAE));
  tft.setFreeFont(FF18);
  tft.drawString(weekDays[timeinfo.tm_wday], 15, 65, 1);
  tft.setFreeFont(FF21);
  tft.drawString(dateStr, 15, 90, 1);

  
}
// === Fetch Weather Function ===
void fetchWeather() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    String url = "http://api.openweathermap.org/data/2.5/weather?q=" 
                 + city + "&appid=" + apiKey + "&units=" + units;
    http.begin(url);
    int httpCode = http.GET();

    if (httpCode == 200) {
      String payload = http.getString();
      Serial.println(payload);

      // Allocate JsonBuffer
      const size_t capacity = 1024;
      DynamicJsonDocument doc(capacity);

      deserializeJson(doc, payload);
      //dtostrf(doc["main"]["temp"], 6, 1, temp);
      float temp = doc["main"]["temp"];
      float hum = doc["main"]["humidity"];
      const char* condition = doc["weather"][0]["main"];

      // Display on TFT
      //tft.fillScreen(TFT_BLACK);
      
      tft.fillRect(160, 0, 160, 120, tft.color565(0x21, 0x76, 0xAE));
      tft.setTextDatum(TR_DATUM);
      tft.setTextColor(TFT_WHITE, tft.color565(0x21, 0x76, 0xAE));
      tft.setTextSize(1);
      tft.setFreeFont(FF22);
      tft.drawString(city, 305, 22, 1);
      tft.setFreeFont(FF17);
      tft.drawString(condition, 305, 48, 1);
      tft.drawString(String(temp,1) + " c", 305, 68, 1);
      tft.drawString(String(hum,0) + "%RH", 305, 88, 1);
      tft.drawCircle(294, 70, 2, TFT_WHITE);
      
    } else {
      tft.println("HTTP Error: " + String(httpCode));
    }
    http.end();
  }
}

void forecast() {
  HTTPClient http;
  String url = "http://api.openweathermap.org/data/2.5/forecast?q=" 
                + String(city) + "&units=metric&appid=" + String(apiKey);

  http.begin(url);
  Serial.println("forecast begin");
  int httpCode = http.GET();
  Serial.println("forecast GET"+ String(httpCode));
  if (httpCode == 200) {
    String payload = http.getString();
    Serial.println(payload);
    const size_t capacity = 30*1024; // adjust if needed
    DynamicJsonDocument doc(capacity);
    deserializeJson(doc, payload);

    String today = String(year) + "-" + String(month) + "-" + String(day-2); // from NTP/RTC
    Serial.println(today);
    String tomorrow = String(year) + "-" + String(month) + "-" + String(day-1);
    Serial.println(tomorrow);
    int slot = 0;

    for (JsonObject f : doc["list"].as<JsonArray>()) {
      String time = f["dt_txt"];  // "2025-09-19 06:00:00"
      if ((time.startsWith(today) && (time.endsWith("00:00:00") ||time.endsWith("06:00:00") || time.endsWith("12:00:00") || time.endsWith("18:00:00"))) 
        || (time.startsWith(tomorrow) && (time.endsWith("00:00:00") ||time.endsWith("06:00:00") || time.endsWith("12:00:00") || time.endsWith("18:00:00")))) {
        Serial.println(time);
        forecastData[0][slot] = String((float)f["main"]["temp"],1);
        Serial.println(forecastData[0][slot]);
        forecastData[1][slot] = String((int)f["main"]["humidity"]);
        Serial.println(forecastData[1][slot]);
        forecastData[2][slot] = String((const char*)f["weather"][0]["icon"]);
        Serial.println(forecastData[2][slot]);
        if(time.startsWith(today))
        {
          if(time.endsWith("00:00:00")){
            forecastData[3][slot] = "00:00";
          }
          if(time.endsWith("06:00:00")){
            forecastData[3][slot] = "06:00";
          }
          if(time.endsWith("12:00:00")){
            forecastData[3][slot] = "12:00";
          }
          if(time.endsWith("18:00:00")){
            forecastData[3][slot] = "18:00";
          }

        }
        if(time.startsWith(tomorrow))
        {
          if(time.endsWith("00:00:00")){
            forecastData[3][slot] = "00:00+";
          }
          if(time.endsWith("06:00:00")){
            forecastData[3][slot] = "06:00+";
          }
          if(time.endsWith("12:00:00")){
            forecastData[3][slot] = "12:00+";
          }
          if(time.endsWith("18:00:00")){
            forecastData[3][slot] = "18:00+";
          }

        }
        Serial.println(forecastData[3][slot]);
        slot++;
        if (slot >= 4) break;
      }
    }
  } else {
    Serial.println("Forecast API call failed: " + String(httpCode));
  }
  http.end();
  tft.fillRect(0, 120, 320, 120, TFT_WHITE);
  for(char r=0;r<4;r++){
    xpos = (80*r)+5;
    ypos = 142;
    tft.setTextColor(tft.color565(0x21, 0x76, 0xAE),TFT_WHITE);
    tft.setTextDatum(TC_DATUM);
    tft.setFreeFont(FF21);
    tft.drawString(forecastData[3][r], (80*r)+40, 127, 1);
    tft.setTextColor(TFT_BLACK,TFT_WHITE);
    tft.setFreeFont(FF17);
    tft.setTextDatum(TR_DATUM);
    tft.drawString(forecastData[0][r] + " c", (80*r)+70, 217, 1);
    tft.drawCircle((80*r)+59, 219, 2, TFT_BLACK);
    drawWeatherIcon(mapIcon(forecastData[2][r]));
    tft.fillRect(xpos-1, ypos, 4, 70, TFT_WHITE);
    //drawWeatherIcon(mapIcon(forecastData[2][r]));

  }
}

static unsigned long lastUpdate = 0;
unsigned char itr=0, longItr=0;
void loop() {
  // refresh every 10 minutes
  
  if (millis() - lastUpdate > 60000) {
    timeNdate();
    lastUpdate = millis();
    itr++;
    if(itr>9)
    {
      fetchWeather();
      itr=0;
      longItr++;
      if(longItr==6){
        forecast();
        longItr=0; 
      }
    }
  }
}
