#pragma once
#include "constants.h"

class Bullet {
public:
    float x, y; // Position of the bullet
    float speed;
    bool remove = false;

    // Method to check if the bullet is out of the game scene
    bool isOutOfScene() const {
        return x < GAME_LEFT_BOUNDARY || x > GAME_RIGHT_BOUNDARY ||
            y < GAME_BOTTOM_BOUNDARY || y > GAME_TOP_BOUNDARY;
    }
};
