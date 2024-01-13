#pragma once

#include <QtWidgets/QMainWindow>
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLTexture>
#include <QKeyEvent>
#include "ui_game.h"
#include "settings.h"
#include "enemy.h"

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
    void paintGL() override;
    void keyPressEvent(QKeyEvent* event) override;
    void keyReleaseEvent(QKeyEvent* event) override;
    void updateGame();
    bool checkCollision(const Bullet& bullet, const EnemySpaceship& enemy);

private:
    QOpenGLTexture* spaceshipTexture = nullptr;
    QOpenGLTexture* backgroundTexture = nullptr;
    QOpenGLTexture* bulletTexture = nullptr;
    QOpenGLTexture* enemyTexture = nullptr;
    float cameraX, cameraY; // Camera position
    GameSettings::Direction spaceshipDirection;
    float backgroundScrollSpeed = 0.0f;
    float backgroundMomentumX = 0.0f;
    float backgroundMomentumY = 0.0f;
    float spaceshipX = 0.0f, spaceshipY = 0.0f;
    float backgroundX = 0.0f, backgroundY = 0.0f;
    float spaceshipAspectRatio = 0.0f;
    float enemyAspectRatio = 0.0f;

    std::vector<Bullet> bullets;
    EnemyManager enemyManager;
    bool scroll = false;
    float moveSpeedX, moveSpeedY = 0.0f;

    void drawBackground();
    void drawEnemies();
    void drawPlayerSpaceship();
    void drawBullets();

    // TODO FIX:
    int BACKGROUND_TEXTURE_WIDTH;
    int BACKGROUND_TEXTURE_HEIGHT;
};