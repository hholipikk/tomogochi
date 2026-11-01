#include <Arduino.h>

// настройки времени
const unsigned long ENERGY_TICK = 1000;               // интервал обновления состояния питомца (в миллисекундах)
const unsigned long MinutesInMillis = 60000;          // количество миллисекунд в одной минуте
const unsigned long HoursInMillis = 3600000;          // количество миллисекунд в одном часе
const unsigned long DISPLAY_UPDATE_INTERVAL = 1000;  // интервал обновления дисплея (в миллисекундах)
const unsigned long INTERACTION_RESTING_TIME = 10000; // сколько Tommy остаётся в Resting после последнего взаимодействия или входа в состояние

// пороги энергии
const int ENERGY_LOW = 30;          // энергия <= 30: Tommy хочет отдохнуть; чтобы разбудить уставшего, нужно > 30
const int ENERGY_EXHAUSTED = 10;    // энергия <= 10: Tommy валится с ног (tired = true) и засыпает
const int RESTING_ENERGY_TICKS = 5; // в Resting энергия падает на 1 раз в 5 тиков (медленнее, чем в Idle)

unsigned long LastEnergyUpdate = 0;  // время последнего обновления состояния питомца
unsigned long LastMinuteUpdate = 0;  // время последнего обновления минут
unsigned long LastDisplayUpdate = 0; // время последнего обновления дисплея

// Анимации
const unsigned long FRAME_INTERVAL = 150; // миллисекунды между кадрами
const int IDLE_ANIMATION_FRAMES = 6;
const int SLEEP_ANIMATION_FRAMES = 6;
const int REACTION_ANIMATION_FRAMES = 4;
const int RISE_ANIMATION_FRAMES = 6;
const int RESTING_ANIMATION_FRAMES = 4;
const int GOING_TO_SLEEP_ANIMATION_FRAMES = 6;

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

DayData dayData = {DayPeriod::Morning, 6, 0}; // инициализация структуры DayData с начальными значениями

Tomogochi pet{
    "Tommy",         // имя
    Behavior::Idle,  // поведение
    Activity::None,  // активность
    Animation::Idle, // анимация
    100,             // энергия
    false,
    0, // количество взаимодействий
    0, // время последнего взаимодействия
    0, // время начала анимации
    0,
    0,
    false, // анимация доиграла
    0};    // время начала поведения

void energyControl(Tomogochi &pet){
  if (pet.energy > 100){
    pet.energy = 100;
  }

  if (pet.energy < 0){
    pet.energy = 0;
  }
}

void dayPeriodControl(DayData &dayData)
{
  unsigned long currentMillis = millis();

  if (currentMillis - LastMinuteUpdate >= MinutesInMillis){
    LastMinuteUpdate = currentMillis;
    dayData.minute++;
    if (dayData.minute >= 60){
      dayData.minute = 0;
      dayData.hour++;
      if (dayData.hour >= 24){
        dayData.hour = 0;
      }
    }
  }

  if (dayData.hour >= 6 && dayData.hour < 7){
    dayData.period = DayPeriod::Morning;
  }else if (dayData.hour >= 7 && dayData.hour < 21){
    dayData.period = DayPeriod::Afternoon;
  }else if (dayData.hour >= 21 && dayData.hour < 22){
    dayData.period = DayPeriod::Evening;
  }else dayData.period = DayPeriod::Night;
}

void energyUpdate(Tomogochi &pet)
{
  static int restingTicks = 0; // считает тики в Resting; static: значение живёт между вызовами функции

  unsigned long currentMillis = millis();
  if (currentMillis - LastEnergyUpdate >= ENERGY_TICK){
    LastEnergyUpdate = currentMillis;

    switch (pet.behavior){
    case Behavior::Idle:
      pet.energy -= 1;
      break;

    case Behavior::Resting:
      restingTicks++;
      if (restingTicks >= RESTING_ENERGY_TICKS){
        restingTicks = 0;
        pet.energy -= 1;
      }
      break;

    case Behavior::Sleeping:
      pet.energy += 1;
      break;

    case Behavior::Reacting:
      pet.energy -= 2;
      break;

    default:
      break;
    }
    energyControl(pet);
  }
}

void displayControl(Tomogochi &pet, DayData &dayData)
{
  unsigned long currentMillis = millis();
  if (currentMillis - LastDisplayUpdate >= DISPLAY_UPDATE_INTERVAL)
  {
    LastDisplayUpdate = currentMillis;
    Serial.print("Name: ");
    Serial.println(pet.name);
    Serial.print("Behavior: ");
    Serial.println(static_cast<int>(pet.behavior));
    Serial.print("Activity: ");
    Serial.println(static_cast<int>(pet.activity));
    Serial.print("Animation: ");
    Serial.println(static_cast<int>(pet.animation));
    Serial.print("Energy: ");
    Serial.println(pet.energy);
    Serial.print("Tired: ");
    Serial.println(pet.tired);
    Serial.print("Interactions: ");
    Serial.println(pet.interactions);
    Serial.print("Last Interaction Time: ");
    Serial.println(pet.lastInteractionTime);
    Serial.print("Day Period: ");
    Serial.println(static_cast<int>(dayData.period));
    Serial.print("Hour: ");
    Serial.println(dayData.hour);
    Serial.print("Minute: ");
    Serial.println(dayData.minute);
  }
}

// Временный ввод для тестов через Serial Monitor: 'b' = кнопка, 'm' = сообщение.
// Потом заменится на настоящую кнопку.
void registerActivity(Tomogochi &pet, Activity newActivity){
  pet.activity = newActivity;
  pet.interactions++;
  pet.lastInteractionTime = millis();
}

void inputControl(Tomogochi &pet){
  while (Serial.available() > 0){
    char command = Serial.read();
    if (command == 'b'){
      registerActivity(pet, Activity::ButtonPressed);
    }else if (command == 'm'){
      registerActivity(pet, Activity::NewMessage);
    }
  }
}

void animationControl(Tomogochi &pet, DayData &dayData){
  unsigned long currentMillis = millis();

  Animation requiredAnimation = pet.animation; // безопасное значение по умолчанию: ничего не меняется

  switch (pet.behavior){
  case Behavior::Idle:
    requiredAnimation = Animation::Idle;
    break;

  case Behavior::GoingToSleep:
    requiredAnimation = Animation::GoingToSleep;
    break;

  case Behavior::Resting:
    requiredAnimation = Animation::Resting;
    break;

  case Behavior::Reacting:
    requiredAnimation = Animation::Reacting;
    break;

  case Behavior::Rise:
    requiredAnimation = Animation::Rise;
    break;

  case Behavior::Sleeping:
    requiredAnimation = Animation::Sleeping;
    break;


  default:
    break;
  }

  // Если анимация изменилась, начинаем её сначала.
  if (pet.animation != requiredAnimation)
  {
    pet.animation = requiredAnimation;
    pet.animationFrame = 0;
    pet.animationStartTime = currentMillis;
    pet.lastFrameUpdate = currentMillis;
    pet.animationFinished = false; // новая анимация ещё не доиграла
  }

  // Определяем количество кадров текущей анимации.
  int totalFrames = 1;

  switch (pet.animation)
  {
  case Animation::Idle:
    totalFrames = IDLE_ANIMATION_FRAMES;
    break;

  case Animation::Sleeping:
    totalFrames = SLEEP_ANIMATION_FRAMES;
    break;

  case Animation::Reacting:
    totalFrames = REACTION_ANIMATION_FRAMES;
    break;

  case Animation::GoingToSleep:
    totalFrames = GOING_TO_SLEEP_ANIMATION_FRAMES;
    break;

  case Animation::Rise:
    totalFrames = RISE_ANIMATION_FRAMES;
    break;

  case Animation::Resting:
    totalFrames = RESTING_ANIMATION_FRAMES;
    break;
  }

  // Переключаем кадр, когда истёк интервал.
  if (currentMillis - pet.lastFrameUpdate >= FRAME_INTERVAL)
  {
    pet.lastFrameUpdate = currentMillis;

    pet.animationFrame++;

    // Зацикливаем последовательность кадров.
    if (pet.animationFrame >= totalFrames)
    {
      pet.animationFrame = 0;
      pet.animationFinished = true; // цикл доигран до конца
    }
  }
}

// Какое состояние подходит Tommy прямо сейчас. Условия проверяются сверху вниз, побеждает первое.
Behavior chooseBehavior(const Tomogochi &pet, const DayData &dayData){
  if (dayData.period == DayPeriod::Night){
    return Behavior::GoingToSleep;
  }
  if (pet.energy <= ENERGY_EXHAUSTED){
    return Behavior::GoingToSleep;
  }
  if (dayData.period == DayPeriod::Evening){
    return Behavior::Resting;
  }
  if (pet.energy <= ENERGY_LOW){
    return Behavior::Resting;
  }
  return Behavior::Idle;
}

// Единственное место, где меняется Behavior: запоминает время входа и ставит tired.
void setBehavior(Tomogochi &pet, Behavior next){
  pet.behavior = next;
  pet.behaviorStartTime = millis();

  if (next == Behavior::GoingToSleep && pet.energy <= ENERGY_EXHAUSTED){
    pet.tired = true;
  }
}

void behaviorControl(Tomogochi &pet, DayData &dayData){
  unsigned long currentMillis = millis();

  // tired сбрасывается, когда энергия полностью восстановилась.
  if (pet.tired && pet.energy >= 100){
    pet.tired = false;
  }

  switch (pet.behavior){
  case Behavior::Idle:
    if (pet.activity != Activity::None){
      setBehavior(pet, Behavior::Reacting);
    }else{
      Behavior next = chooseBehavior(pet, dayData);
      if (next != Behavior::Idle){
        setBehavior(pet, next);
      }
    }
    break;

  case Behavior::Resting:
    if (pet.activity != Activity::None){
      setBehavior(pet, Behavior::Reacting);
    }else if (currentMillis - pet.lastInteractionTime >= INTERACTION_RESTING_TIME &&
              currentMillis - pet.behaviorStartTime >= INTERACTION_RESTING_TIME){
      Behavior next = chooseBehavior(pet, dayData);
      if (next != Behavior::Resting){
        setBehavior(pet, next);
      }
    }
    break;

  case Behavior::GoingToSleep:
    if (pet.animationFinished){
      setBehavior(pet, Behavior::Sleeping);
    }
    break;

  case Behavior::Sleeping:
    if (pet.activity != Activity::None){
      if (!pet.tired){
        setBehavior(pet, Behavior::Rise);
      }else if (pet.energy > ENERGY_LOW){
        pet.tired = false;
        setBehavior(pet, Behavior::Rise);
      }else{
        pet.activity = Activity::None; // слишком устал: взаимодействие игнорируется
      }
    }else if (!pet.tired && dayData.period != DayPeriod::Night){
      setBehavior(pet, Behavior::Rise);
    }
    break;

  case Behavior::Reacting:
    if (pet.animationFinished){
      pet.activity = Activity::None;
      setBehavior(pet, chooseBehavior(pet, dayData));
    }
    break;

  case Behavior::Rise:
    if (pet.animationFinished){
      pet.activity = Activity::None;
      setBehavior(pet, Behavior::Resting);
    }
    break;

  default:
    break;
  }
}

void setup(){
  LastEnergyUpdate = millis();        // инициализация времени последнего обновления состояния питомца
  pet.lastInteractionTime = millis(); // инициализация времени последнего взаимодействия с питомцем
  pet.behaviorStartTime = millis();   // инициализация времени начала текущего поведения

  Serial.begin(115200); // инициализация последовательного порта для отладки
}

void loop(){
  inputControl(pet);
  energyUpdate(pet);
  dayPeriodControl(dayData);
  behaviorControl(pet, dayData);
  animationControl(pet, dayData);
  displayControl(pet, dayData);
}