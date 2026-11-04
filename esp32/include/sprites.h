#pragma once
#include "types.h"

const int SPRITE_W = 48;
const int SPRITE_H = 48;

// Возвращает картинку кадра или nullptr, если для анимации ещё нет рисунков.
const uint8_t* getSpriteFrame(Animation animation, int frame);