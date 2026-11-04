#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "screen.h"

const int SCREEN_WIDTH = 128;
const int SCREEN_HEIGHT = 64;
const uint8_t OLED_ADDRESS = 0x3C;   // адрес из I2C-сканера

const int SDA_PIN = 9;
const int SCL_PIN = 8;

static Adafruit_SSD1306 oled(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
static bool screenReady = false;

void screenInit(){
    Wire.begin(SDA_PIN, SCL_PIN);
  // Если пины нестандартные: сначала Wire.begin(SDA_PIN, SCL_PIN),
  // а ниже в begin() четвёртым параметром передай false.
  if (!oled.begin(SSD1306_SWITCHCAPVCC, OLED_ADDRESS)){
    Serial.println("OLED not found");
    return;                          // screenReady остаётся false
  }
  screenReady = true;
  oled.clearDisplay();
  oled.display();
}

static const char* animationName(Animation a){
    switch (a)
    {
    case Animation::Idle:         return "Idle";
    case Animation::Resting:      return "Resting";
    case Animation::Sleeping:     return "Sleeping";
    case Animation::GoingToSleep: return "GoingToSleep";
    case Animation::Rise:         return "Rise";
    case Animation::Reacting:     return "Reacting";
    }
    return "?";
}

void screenControl(const Tomogochi &pet, const DayData &dayData, const WeatherData &weather){
  if (!screenReady) return;

  // Перерисовываем только если что-то изменилось.
static Animation lastAnimation;
static int lastFrame = -1;
static int lastMinute = -1;
static int lastTemp = -1000;
static WeatherType lastType = WeatherType::Unknown;
static bool lastValid = false;

bool changed = pet.animation != lastAnimation || pet.animationFrame != lastFrame
            || dayData.minute != lastMinute || weather.temperature != lastTemp
            || weather.type != lastType || weather.valid != lastValid;
if (!changed) return;

lastAnimation = pet.animation;
lastFrame = pet.animationFrame;
lastMinute = dayData.minute;
lastTemp = weather.temperature;
lastType = weather.type;
lastValid = weather.valid;

  oled.clearDisplay();
  oled.setTextSize(1);
  oled.setTextColor(SSD1306_WHITE);
  oled.setCursor(0, 0);
  oled.print(animationName(pet.animation));
  oled.print(" ");
  oled.print(pet.animationFrame);
  oled.setCursor(0, 25);
  oled.print("Energy:  ");
  oled.print(pet.energy);
  char timeText[6];                                   // "06:05" и завершающий ноль
  snprintf(timeText, sizeof(timeText), "%02d:%02d", dayData.hour, dayData.minute);
  oled.setTextSize(1);
  oled.setCursor(80, 8);
  oled.print(timeText);
  oled.setTextSize(1);
  oled.setCursor(80, 40);
  if (weather.valid){
    oled.print(weather.temperature);
    oled.print(" C");
  } else {
    oled.print("-- C");                  // данных ещё нет
  }
  oled.display();                    // отправляет весь буфер на экран
}