#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "screen.h"
#include "sprites.h"
#include "types.h"

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

void screenControl(const Tomogochi &pet, const DayData &dayData, const WeatherData &weather, NetState networkState) {
    if (!screenReady) return;

    static Animation lastAnimation;
    static int lastFrame = -1;
    static int lastMinute = -1;
    static int lastTemp = -1000;
    static WeatherType lastType = WeatherType::Unknown;
    static bool lastValid = false;
    static NetState lastNetworkState;

    bool changed = pet.animation != lastAnimation ||
                   pet.animationFrame != lastFrame ||
                   dayData.minute != lastMinute ||
                   weather.temperature != lastTemp ||
                   weather.type != lastType ||
                   weather.valid != lastValid
                   || lastNetworkState != networkState;

    if (!changed) return;

    lastAnimation = pet.animation;
    lastFrame = pet.animationFrame;
    lastMinute = dayData.minute;
    lastTemp = weather.temperature;
    lastType = weather.type;
    lastValid = weather.valid;
    lastNetworkState = networkState;

    oled.clearDisplay();

    // Кадр ЮКИ
    oled.setTextSize(1);
    oled.setTextColor(SSD1306_WHITE);
    oled.setCursor(0, 0);

    const char* sprite = getSpriteFrame(pet.animation, pet.animationFrame);
    oled.print(sprite);

    // Энергия
    oled.drawRect(10, 54, 102, 6, SSD1306_WHITE);
    oled.fillRect(11, 55, pet.energy, 4, SSD1306_WHITE);

    // Время
    char timeText[6];
    snprintf(timeText, sizeof(timeText), "%02d:%02d", dayData.hour, dayData.minute);

    oled.setCursor(70, 4);
    oled.print(timeText);

    // Температура
    oled.setCursor(80, 30);

    if (weather.valid) {
        oled.print(weather.temperature);
        oled.print(" C");
    } else {
        oled.print("-- C");
    }

    //WIFI
   if (networkState == NetState::Connected) {
    oled.drawLine(113, 6, 119, 1, SSD1306_WHITE);
    oled.drawLine(119, 1, 125, 6, SSD1306_WHITE);
    oled.drawLine(116, 7, 119, 4, SSD1306_WHITE);
    oled.drawLine(119, 4, 122, 7, SSD1306_WHITE);
    oled.fillCircle(119, 10, 1, SSD1306_WHITE);
    } else {
    oled.drawLine(113, 6, 119, 1, SSD1306_WHITE);
    oled.drawLine(119, 1, 125, 6, SSD1306_WHITE);
    oled.drawLine(116, 7, 119, 4, SSD1306_WHITE);
    oled.drawLine(119, 4, 122, 7, SSD1306_WHITE);
    oled.fillCircle(119, 10, 1, SSD1306_WHITE);
    oled.drawLine(113, 1, 125, 9, SSD1306_WHITE);
    }

    oled.display();
}