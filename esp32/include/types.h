#pragma once
#include <Arduino.h>

// Анимации
const unsigned long FRAME_INTERVAL = 500; // миллисекунды между кадрами
const int IDLE_ANIMATION_FRAMES = 6;
const int SLEEP_ANIMATION_FRAMES = 4;
const int REACTION_ANIMATION_FRAMES = 6;
const int RISE_ANIMATION_FRAMES = 8;
const int RESTING_ANIMATION_FRAMES = 4;
const int GOING_TO_SLEEP_ANIMATION_FRAMES = 8;

//Для обновления интернет данных
const unsigned long INTERNET_DATA_UPDATE = 10800000; //3 часа

enum class DayPeriod{

  Morning,   // утро
  Afternoon, // день
  Evening,   // вечер
  Night      // ночь
};

enum class Activity{
  None,         // ничего не происходит
  NewMessage,   // новое сообщение
  ButtonPressed // нажата кнопка
};

enum class Behavior{
  // продолжительное время
  Idle,     // бездействует
  Resting,  // сонный
  Sleeping, // спит
  // переходные процессы
  Reacting,     // реагирует
  GoingToSleep, // готовится ко сну
  Rise
};

enum class Animation{
  Idle,         // бездействует
  GoingToSleep, // засыпает
  Sleeping,     // спит
  Rise,
  Reacting, // реагирует
  Resting
};

enum class WeatherType{
  Unknown, Clear, Cloudy, Rain, Snow, Storm
};
// свойства персонажа
struct Tomogochi{
  String name;

  Behavior behavior;
  Activity activity;
  Animation animation;

  int energy;

  bool tired;

  int interactions;

  unsigned long lastInteractionTime;
  unsigned long animationStartTime;

  int animationFrame;
  unsigned long lastFrameUpdate;

  bool animationFinished;           // true, когда текущая анимация доиграла цикл до конца
  unsigned long behaviorStartTime;  // когда начался текущий Behavior
};

struct DayData{

  DayPeriod period;
  int hour;
  int minute;
};


struct WeatherData{
  bool valid;
  int temperature;
  unsigned long lastUpdate;
  WeatherType type;
};