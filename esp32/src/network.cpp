#include "network_control.h"
#include "secrets.h"
#include <WiFi.h>

enum class NetState { Idle, Connecting, Connected };
static NetState state = NetState::Idle;
static unsigned long stateStart = 0;
unsigned long DATAUPDT = 0;
static bool firstTry = true;
bool timeSynhc = false;
static bool ntpStarted = false;

const unsigned long CONNECT_TIMEOUT = 15000;   // сколько ждать подключения
const unsigned long RETRY_DELAY = 60000;       // пауза между попытками

void networkInit(){
  WiFi.mode(WIFI_STA);
}

void networkControl(DayData &dayData, WeatherData &weather){

  unsigned long now = millis();

  switch (state){
    case NetState::Idle:
      if (firstTry || now - stateStart >= RETRY_DELAY){
        firstTry = false;
        WiFi.begin(WIFI_SSID, WIFI_PASSWORD);   // возвращается сразу
        stateStart = now;
        state = NetState::Connecting;
      }
      break;

    case NetState::Connecting:
      if (WiFi.status() == WL_CONNECTED){
        state = NetState::Connected;
        // здесь запускаем синхронизацию времени (10.3)
      } else if (now - stateStart >= CONNECT_TIMEOUT){
        WiFi.disconnect();
        stateStart = now;
        state = NetState::Idle;                 // подождём RETRY_DELAY и повторим
      }
      break;

    case NetState::Connected:
      if (WiFi.status() != WL_CONNECTED){
        stateStart = now;
        state = NetState::Idle;
        ntpStarted = false;
        timeSynhc = false;
        break;
      }

if (!ntpStarted) {
  configTzTime(TIME_ZONE, "time.google.com", "time.cloudflare.com");
  ntpStarted = true;
  Serial.println("NTP started");
}

if(!timeSynhc || now - DATAUPDT >= INTERNET_DATA_UPDATE){
    struct tm t;
    if(getLocalTime(&t, 0)){
        dayData.hour = t.tm_hour;
        dayData.minute = t.tm_min;
        if(!timeSynhc){
            timeSynhc = true;
        }

        DATAUPDT = now;
    }
}
        break;
  }
}