#pragma once

#include <QtWidgets/QMainWindow>
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLTexture>
#include <QKeyEvent>
#include "ui_game.h"
#include "settings.h"
#include "enemy.h"
#include "explosion.h"

struct Bullet {
    float x, y;
    float speed;
};

class game : public QMainWindow
{
    Q_OBJECT

public:
    game(QWidget *parent = nullptr);
    ~game();

private:
    Ui::gameClass ui;
};

class GameWidget : public QOpenGLWidget, protected QOpenGLFunctions {
   
    Q_OBJECT

public:
    GameWidget(QWidget* parent = nullptr);
    ~GameWidget();

protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void drawLives();
    void drawScore();
    void paintGL() override;
    void keyPressEvent(QKeyEvent* event) override;
    void keyReleaseEvent(QKeyEvent* event) override;
    void updateGame();
    bool checkCollision(const Bullet& bullet, const EnemySpaceship& enemy);
    void handleSpaceshipBoundary();

private:
    std::unique_ptr<QOpenGLTexture> playerLifeTexture = nullptr;
    std::unique_ptr<QOpenGLTexture> spaceshipTexture = nullptr;
    std::unique_ptr<QOpenGLTexture> backgroundTexture = nullptr;
    std::unique_ptr<QOpenGLTexture> bulletTexture = nullptr;
    std::unique_ptr<QOpenGLTexture> enemyTexture = nullptr;
    float cameraX, cameraY; // Camera position
    GameSettings::Direction spaceshipDirection;
    float backgroundScrollSpeed = 0.0f;
    float backgroundMomentumX = 0.0f;
    float backgroundMomentumY = 0.0f;
    float spaceshipX = 0.0f, spaceshipY = 0.0f;
    float backgroundX = 0.0f, backgroundY = 0.0f;
    float spaceshipAspectRatio = 0.0f;
    float enemyAspectRatio = 0.0f;
    int playerLives;

    std::vector<Explosion> activeExplosions;
    std::vector<Bullet> bullets;
    EnemyManager enemyManager;
    bool scroll = false;
    float moveSpeedX, moveSpeedY = 0.0f;
    int score = 0;

    void drawBackground();
    void drawEnemies();
    void drawPlayerSpaceship();
    void drawBullets();

    int backgroundWidth;
    int backgroundHeight;
};