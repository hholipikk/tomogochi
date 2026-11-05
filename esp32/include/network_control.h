#pragma once

#include "types.h"


void networkInit();

void networkControl(DayData &dayData, WeatherData &weather);

NetState networkGetState();