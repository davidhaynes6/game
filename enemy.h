#pragma once

#include <vector>
#include <cstdlib>

class EnemySpaceship {
public:
    float x;
    float y;
    float velocityX, velocityY; // Velocity components
    float speed; // Speed of spaceship
    float moveX; // X-direction movement
    float moveY; // Y-direction movement

    EnemySpaceship(float posX, float posY, float vel) : x(posX), y(posY), speed(vel), velocityX(0), velocityY(0) {}
};


class EnemyManager {
public:
    std::vector<EnemySpaceship> enemySpaceships;
    void createRandomEnemySpaceship();
    float getRandomFloat(float min, float max);
    void generateRandomCoordinates(float& x, float& y);
    float generateRandomVelocity(float minVelocity, float maxVelocity);
    void update();
    void handleBoundary(EnemySpaceship& enemy);
};
