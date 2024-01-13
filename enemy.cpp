#include "enemy.h"
#include "settings.h"
#include <qmath.h>
#include <QDebug>

void EnemyManager::createRandomEnemySpaceship() {
    float randomX, randomY;
    generateRandomCoordinates(randomX, randomY);

    float randomVelocity = generateRandomVelocity(0.001f, 0.005f);
    EnemySpaceship enemy(randomX, randomY, randomVelocity);
    qDebug() << "Enemy spaceship created at position: (" << enemy.x << "," << enemy.y << ") with size: " << GameSettings::SPACESHIP_SIZE;
    enemySpaceships.push_back(enemy);
}

float EnemyManager::getRandomFloat(float min, float max) {
    return min + static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * (max - min);
}

void EnemyManager::generateRandomCoordinates(float& x, float& y) {
    x = getRandomFloat(-GameSettings::WORLD_WIDTH / 2, GameSettings::WORLD_WIDTH / 2);
    y = getRandomFloat(-GameSettings::WORLD_HEIGHT / 2, GameSettings::WORLD_HEIGHT / 2);
}

float EnemyManager::generateRandomVelocity(float minVelocity, float maxVelocity) {
    return getRandomFloat(minVelocity, maxVelocity);
}

void EnemyManager::update() {
    for (auto& enemy : enemySpaceships) {
        if (rand() % 100 < 5) {
            float angle = static_cast<float>(rand()) / RAND_MAX * 2 * M_PI;
            enemy.velocityX = cos(angle) * enemy.speed;
            enemy.velocityY = sin(angle) * enemy.speed;
        }

        enemy.x += enemy.velocityX;
        enemy.y += enemy.velocityY;

        handleBoundary(enemy);
    }
}

void EnemyManager::handleBoundary(EnemySpaceship& enemy) {
    if (enemy.x < -GameSettings::WORLD_WIDTH / 2) {
        enemy.x = -GameSettings::WORLD_WIDTH / 2;
        enemy.velocityX = -enemy.velocityX;
    }
    else if (enemy.x > GameSettings::WORLD_WIDTH / 2) {
        enemy.x = GameSettings::WORLD_WIDTH / 2;
        enemy.velocityX = -enemy.velocityX;
    }

    if (enemy.y < -GameSettings::WORLD_HEIGHT / 2) {
        enemy.y = -GameSettings::WORLD_HEIGHT / 2;
        enemy.velocityY = -enemy.velocityY;
    }
    else if (enemy.y > GameSettings::WORLD_HEIGHT / 2) {
        enemy.y = GameSettings::WORLD_HEIGHT / 2;
        enemy.velocityY = -enemy.velocityY;
    }
}
