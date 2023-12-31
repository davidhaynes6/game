#pragma once

#include <vector>
#include <cstdlib>

class EnemySpaceship {
public:
    float x;
    float y;
    float velocity;
    float moveX; // X-direction movement
    float moveY; // Y-direction movement

    EnemySpaceship(float startX, float startY, float startVelocity)
        : x(startX), y(startY), velocity(startVelocity) {
    }
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

private:
    const float GAME_LEFT_BOUNDARY = -1.0f;  // Left boundary
    const float GAME_RIGHT_BOUNDARY = 1.0f;  // Right boundary
    const float GAME_TOP_BOUNDARY = 1.0f;    // Top boundary
    const float GAME_BOTTOM_BOUNDARY = -1.0f; // Bottom boundary
    const float screenBoundary = 1.0f; // Boundary for wrapping or removing enemy spaceships

};
