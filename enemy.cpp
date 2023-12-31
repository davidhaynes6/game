#include "enemy.h"
#include <qmath.h>
    
void EnemyManager::createRandomEnemySpaceship() {
    float randomX, randomY;
    generateRandomCoordinates(randomX, randomY);

    float randomVelocity = generateRandomVelocity(0.001f, 0.005f); // Example velocity range

    EnemySpaceship enemy(randomX, randomY, randomVelocity);
    enemySpaceships.push_back(enemy);
}

float EnemyManager::getRandomFloat(float min, float max)
{
    return min + static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * (max - min);
}

void EnemyManager::generateRandomCoordinates(float& x, float& y)
{
    x = getRandomFloat(GAME_LEFT_BOUNDARY, GAME_RIGHT_BOUNDARY);
    y = getRandomFloat(GAME_BOTTOM_BOUNDARY, GAME_TOP_BOUNDARY);
}

// Function to generate a random velocity within a given range
float EnemyManager::generateRandomVelocity(float minVelocity, float maxVelocity) {
    return getRandomFloat(minVelocity, maxVelocity);
}

// Function to update enemy spaceships
void EnemyManager::update() {
    for (auto& enemy : enemySpaceships) {
        // Randomly decide whether to change direction
        if (getRandomFloat(0.0f, 1.0f) < 0.05f) { // 5% chance to change direction each frame
            // Calculate a new random direction vector for movement
            float randomAngle = getRandomFloat(0.0f, 2.0f * M_PI);
            enemy.moveX = std::cos(randomAngle) * enemy.velocity;
            enemy.moveY = std::sin(randomAngle) * enemy.velocity;
        }

        // Update enemy spaceship position based on the direction
        enemy.x += enemy.moveX;
        enemy.y += enemy.moveY;

        // Check and handle if the enemy spaceship is out of bounds
        handleBoundary(enemy);
    }
}

void EnemyManager::handleBoundary(EnemySpaceship& enemy) {
    if (enemy.x < GAME_LEFT_BOUNDARY) {
        enemy.x = GAME_LEFT_BOUNDARY;
        enemy.moveX = -enemy.moveX; // Reverse direction
    }
    else if (enemy.x > GAME_RIGHT_BOUNDARY) {
        enemy.x = GAME_RIGHT_BOUNDARY;
        enemy.moveX = -enemy.moveX; // Reverse direction
    }

    if (enemy.y < GAME_BOTTOM_BOUNDARY) {
        enemy.y = GAME_BOTTOM_BOUNDARY;
        enemy.moveY = -enemy.moveY; // Reverse direction
    }
    else if (enemy.y > GAME_TOP_BOUNDARY) {
        enemy.y = GAME_TOP_BOUNDARY;
        enemy.moveY = -enemy.moveY; // Reverse direction
    }
}





