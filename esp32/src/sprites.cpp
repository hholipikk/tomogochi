
#include "sprites.h"

// =====================================
// IDLE — 6 кадров, LOOP
// =====================================

const char* const IDLE_FRAMES[] = {
    "/\\_/\\ \n(o.o)  \n >^<   \n/| |\\  \n(_|_|_)",

    "/\\_/\\ \n(o.o)  \n >^<   \n/| |\\  \n(_|_|_)",

    "/\\_/\\ \n(-.-)  \n >^<   \n/| |\\  \n(_|_|_)",

    "/\\_/\\ \n(-.-)  \n >^<   \n/| |\\  \n(_|_|_)",

    "/\\_/\\ \n(o.o)  \n >^<   \n/| |\\  \n(_|_|_)",

    "/\\_/\\ \n(o.o)  \n >^<   \n/| |\\  \n(_|_|_)"
};

// =====================================
// RESTING — 4 кадра, LOOP
// =====================================

const char* const RESTING_FRAMES[] = {
    "/\\_/\\ \n(-.-)  \n >~<   \n/| |\\  \n(_|_|_)",

    "/\\_/\\ \n(-.-)  \n >~<   \n/| |\\  \n(_|_|_)",

    "/\\_/\\ \n(=.=)  \n >~<   \n/| |\\  \n(_|_|_)",

    "/\\_/\\ \n(-.-)  \n >~<   \n/| |\\  \n(_|_|_)"
};

// =====================================
// SLEEPING — 4 кадра, LOOP
// =====================================

const char* const SLEEPING_FRAMES[] = {
    " /\\_/\\ \n (-.-) z\n/|___|\\\n(_____) ",

    " /\\_/\\ \n (-.-)  z\n/|___|\\\n(_____) ",

    " /\\_/\\ \n (-.-)  Z\n/|___|\\\n(_____) ",

    " /\\_/\\ \n (-.-)  z\n/|___|\\\n(_____) "
};

// =====================================
// GOING TO SLEEP — 8 кадров, ONE-SHOT
// =====================================

const char* const GOING_TO_SLEEP_FRAMES[] = {
    "/\\_/\\ \n(-.-)  \n >~<   \n/| |\\  \n(_|_|_)",

    "/\\_/\\ \n(-.-)  \n >~<   \n/| |\\  \n(_|_|_)",

    "/\\_/\\ \n(-.-)  \n/|___|\\\n(_____) ",

    "/\\_/\\ \n(-.-)  \n/|___|\\\n(_____) ",

    "/\\_/\\\n (-.-) \n /|___|\\\n(_____) ",

    "/\\_/\\\n (-.-) \n /|___|\\\n(_____) ",

    "/\\_/\\ \n(-.-) z\n/|___|\\\n(_____) ",

    "/\\_/\\ \n(-.-) z\n/|___|\\\n(_____) "
};

// =====================================
// RISE — 8 кадров, ONE-SHOT
// =====================================

const char* const RISE_FRAMES[] = {
    "/\\_/\\ \n(-.-) z\n/|___|\\\n(_____) ",

    "/\\_/\\ \n(-.-)  \n/|___|\\\n(_____) ",

    "/\\_/\\\n (-.-) \n /|___|\\\n(_____) ",

    "/\\_/\\\n (-.-) \n /|___|\\\n(_____) ",

    "/\\_/\\ \n(-.-)  \n/|___|\\\n(_____) ",

    "/\\_/\\ \n(-.-)  \n >~<   \n/| |\\  \n(_|_|_)",

    "/\\_/\\ \n(o.o)  \n >^<   \n/| |\\  \n(_|_|_)",

    "/\\_/\\ \n(o.o)  \n >^<   \n/| |\\  \n(_|_|_)"
};

// =====================================
// REACTING — 6 кадров, ONE-SHOT
// =====================================

const char* const REACTING_FRAMES[] = {
    "/\\_/\\ \n(o.o)  \n >^<   \n/| |\\  \n(_|_|_)",

    "/\\_/\\ \n(O.O)  \n >o<   \n/| |\\  \n(_|_|_)",

    "/\\_/\\!\n(O.O)  \n >o<   \n/| |\\  \n(_|_|_)",

    "/\\_/\\ \n(O.O)  \n >o<   \n/| |\\  \n(_|_|_)",

    "/\\_/\\ \n(^.^)  \n >w<   \n/| |\\  \n(_|_|_)",

    "/\\_/\\ \n(o.o)  \n >^<   \n/| |\\  \n(_|_|_)"
};

const char* getSpriteFrame(Animation animation, int frame) {
    switch (animation) {
        case Animation::Idle:
            return IDLE_FRAMES[frame % IDLE_ANIMATION_FRAMES];

        case Animation::Resting:
            return RESTING_FRAMES[frame % RESTING_ANIMATION_FRAMES];

        case Animation::Sleeping:
            return SLEEPING_FRAMES[frame % SLEEP_ANIMATION_FRAMES];

        case Animation::GoingToSleep:
            return GOING_TO_SLEEP_FRAMES[
                (frame < GOING_TO_SLEEP_ANIMATION_FRAMES)
                    ? frame
                    : GOING_TO_SLEEP_ANIMATION_FRAMES - 1
            ];

        case Animation::Rise:
            return RISE_FRAMES[
                (frame < RISE_ANIMATION_FRAMES)
                    ? frame
                    : RISE_ANIMATION_FRAMES - 1
            ];

        case Animation::Reacting:
            return REACTING_FRAMES[
                (frame < REACTION_ANIMATION_FRAMES)
                    ? frame
                    : REACTION_ANIMATION_FRAMES - 1
            ];

        default:
            return IDLE_FRAMES[0];
    }
}