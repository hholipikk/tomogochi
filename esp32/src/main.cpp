#include <Arduino.h>


//настройки времени
const unsigned long ENERGY_TICK = 1000; // интервал обновления состояния питомца (в миллисекундах)
const unsigned long MinutesInMillis = 60000; // количество миллисекунд в одной минуте
const unsigned long HoursInMillis = 3600000; // количество миллисекунд в одном часе
const unsigned long DISPLAY_UPDATE_INTERVAL = 10000; // интервал обновления дисплея (в миллисекундах)

unsigned long lastEnergyUpdate = 0; // время последнего обновления состояния питомца
unsigned long LastMinuteUpdate = 0; // время последнего обновления минут
unsigned long LastDisplayUpdate = 0; // время последнего обновления часов


//Анимации
const unsigned long FRAME_INTERVAL = 150; // миллисекунды между кадрами
const int IDLE_ANIMATION_FRAMES = 6;
const int SLEEP_ANIMATION_FRAMES = 6;
const int REACTION_ANIMATION_FRAMES = 4;
const int WAKEUP_ANIMATION_FRAMES = 5;
const int RISE_ANIMATION_FRAMES = 6;
const int RESTING_ANIMATION_FRAMES = 4;
const int GOING_TO_SLEEP_ANIMATION_FRAMES = 6;




enum class DayPeriod{
  
  Morning,      // утро
  Afternoon,    // день
  Evening,      // вечер
  Night         // ночь
};

enum class Activity {           
  None,               // ничего не происходит
  NewMessage,         // новое сообщение
  ButtonPressed       // нажата кнопка
};



enum class Behavior {
  //продолжительное время
  Idle,     // бездействует
  Resting,  // сонный
  Sleeping, // спит
  WakingUp, // просыпается на какоето-время
  //переходные процессы
  Reacting,  // реагирует
  GoingToSleep, //готовится ко сну
  Rise
};

enum class Animation {
  Idle,         // бездействует      
  GoingToSleep, // засыпает
  Sleeping,     // спит
  Rise,
  WakeUp,       // просыпается
  Reacting,     // реагирует
  Resting

};

//свойства персонажа
struct Tomogochi {
  String name;


  Behavior behavior;
  Activity activity;
  Animation animation;

  int energy;

  bool tired;
  bool canSleep;
  bool canRest;

  int interactions;

  unsigned long lastInteractionTime;
  unsigned long animationStartTime;

  int animationFrame;
  unsigned long lastFrameUpdate;

};

struct DayData {

  DayPeriod period;

  int hour;
  int minute;

};

DayData dayData = {DayPeriod::Morning, 6, 0}; // инициализация структуры DayData с начальными значениями

Tomogochi pet{
  "Tommy",        // имя     
  Behavior::Idle, // поведение
  Activity::None, // активность
  Animation::Idle, // анимация
  100,            // энергия
  false,
  false,
  false,
  0,              // количество взаимодействий
  0,              // время последнего взаимодействия
  0,               // время начала анимации
  0,
  0
};


void energyControl(Tomogochi& pet){
  if(pet.energy > 100){
    pet.energy = 100;
  }

  if(pet.energy < 0){
    pet.energy = 0;
  }
}

void dayperiodControl(DayData& dayData){
  unsigned long currentMillis = millis();

  if (currentMillis - LastMinuteUpdate >= MinutesInMillis) {
    LastMinuteUpdate = currentMillis;
    dayData.minute++;
    if (dayData.minute >= 60) {
      dayData.minute = 0;
      dayData.hour++;
      if (dayData.hour >= 24) {
        dayData.hour = 0;
      }
    }
  }

  if( dayData.hour >= 6 && dayData.hour < 7){
    dayData.period = DayPeriod::Morning;
  } else if(dayData.hour >= 7 && dayData.hour < 21){
    dayData.period = DayPeriod::Afternoon;
  } else if(dayData.hour >= 21 && dayData.hour < 22){
    dayData.period = DayPeriod::Evening;
  } else {
    dayData.period = DayPeriod::Night;
  }

}

void energyUpdate(Tomogochi& pet){
  unsigned long currentMillis = millis();
  if(currentMillis - lastEnergyUpdate >= ENERGY_TICK){
    lastEnergyUpdate = currentMillis;
    if(pet.behavior == Behavior::Idle){
      pet.energy -= 1;
    } else if(pet.behavior == Behavior::Sleeping){
      pet.energy += 1;
    }
    else if(pet.behavior == Behavior::Reacting){
      pet.energy -= 2;
    }
  energyControl(pet);
  }
}

void DisplayControl(Tomogochi& pet, DayData& dayData){
  unsigned long currentMillis = millis();
  if(currentMillis - LastDisplayUpdate >= DISPLAY_UPDATE_INTERVAL){
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

void animationControl(Tomogochi& pet, DayData& dayData){
  unsigned long currentMillis = millis();

  Animation requiredAnimation;

  switch (pet.behavior)
  {
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
  
  case Behavior::WakingUp:
    requiredAnimation = Animation::WakeUp;
    break;
  default:
    break;
  }

  
// Если анимация изменилась, начинаем её сначала.
  if (pet.animation != requiredAnimation) {
    pet.animation = requiredAnimation;
    pet.animationFrame = 0;
    pet.animationStartTime = currentMillis;
    pet.lastFrameUpdate = currentMillis;
  }

  // Определяем количество кадров текущей анимации.
  int totalFrames = 1;

  switch (pet.animation) {
    case Animation::Idle:
      totalFrames = IDLE_ANIMATION_FRAMES;
      break;

    case Animation::Sleeping:
      totalFrames = SLEEP_ANIMATION_FRAMES;
      break;

    case Animation::WakeUp:
      totalFrames = WAKEUP_ANIMATION_FRAMES;
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
  if (currentMillis - pet.lastFrameUpdate >= FRAME_INTERVAL) {
    pet.lastFrameUpdate = currentMillis;

    pet.animationFrame++;

    // Зацикливаем последовательность кадров.
    if (pet.animationFrame >= totalFrames) {
      pet.animationFrame = 0;
    }
  }
}

void behaviorControl(Tomogochi& pet, DayData& dayData) {
  //продолжение анимации сна
  if(pet.canSleep){
    pet.behavior = Behavior::Sleeping;
    pet.canSleep = false;
    return;
  }

  //продолжение анимации подъема
  if(pet.canRest){
      if(dayData.period == DayPeriod::Morning || dayData.period == DayPeriod::Evening || pet.energy <= 30){
        pet.behavior = Behavior::Resting;
      }else {
        pet.behavior = Behavior::Idle;
      }
    pet.canRest = false;
    return;
  }

  //при флаге tried сон до 100% или пробуждение по активности на короткое время
  if(pet.tired && pet.energy < 100){
    if(pet.energy > 10 && pet.activity != Activity::None){
      pet.tired = false;
      pet.behavior = Behavior::WakingUp;
    }
    return;
  }

  //проверка условий при wakingUp
  if(pet.behavior == Behavior::WakingUp){
    //путь в sleeping
    if(dayData.period == DayPeriod::Night){
      pet.behavior = Behavior::GoingToSleep;
                                                                                  //добавить анимацию с переходом флага canSleep
    }else{
      pet.behavior = Behavior::Resting;
    }
    return;          
  }

  //ночное пробуждение
  if (pet.activity != Activity::None && pet.energy > 10 && dayData.period == DayPeriod::Night) {
    pet.behavior = Behavior::WakingUp;
    return;
  }
  
  //состояние ночью


  if(dayData.period == DayPeriod::Night && pet.behavior != Behavior::WakingUp){
    pet.behavior = Behavior::Sleeping;
    return;
  }
  if(dayData.period == DayPeriod::Night && (pet.behavior != Behavior::Sleeping || pet.behavior != Behavior::GoingToSleep)){
      pet.behavior = Behavior::GoingToSleep;
      return;
  }
  //блок с энергией
  if(pet.energy <= 10){
    pet.behavior = Behavior::GoingToSleep;
    pet.tired = true;
    return;
  }

  if(pet.energy <= 30){
    pet.behavior = Behavior::Resting;
    return;
  }

  if(pet.activity != Activity::None){
    pet.behavior = Behavior::Reacting;
    return;
  }

  if(dayData.period == DayPeriod::Morning && pet.behavior == Behavior::Sleeping){
    pet.behavior = Behavior::Rise;
                                                                                //добавить анимацию с переходом флага canresting
    return;
  }
  if(dayData.period == DayPeriod::Evening && pet.behavior != Behavior::Sleeping){
    pet.behavior = Behavior::Resting;
    return;
  }

  if(pet.energy == 100 && pet.behavior == Behavior::Sleeping){
    pet.behavior = Behavior::Rise;
    return;
  }


  pet.behavior = Behavior::Idle;
}

void setup() {
lastEnergyUpdate = millis(); // инициализация времени последнего обновления состояния питомца
pet.lastInteractionTime = millis(); // инициализация времени последнего взаимодействия с питомцем

Serial.begin(115200); // инициализация последовательного порта для отладки

}

void loop() {

  energyUpdate(pet);
  dayperiodControl(dayData);
  behaviorControl(pet, dayData);
  DisplayControl(pet, dayData);

}

