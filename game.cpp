#include "game.h"
#include "settings.h"
#include "explosion.h"
#include <QTimer>
#include <QPainter>
#include <QFontDatabase>

game::game(QWidget *parent) : QMainWindow(parent)
{
    auto* gameWidget = new GameWidget(this);
    setCentralWidget(gameWidget);

    // Set the size of the MainWindow
    resize(800, 600);
}

game::~game() {}

GameWidget::GameWidget(QWidget* parent) : QOpenGLWidget(parent) 
{
    setFocusPolicy(Qt::StrongFocus);
    setAttribute(Qt::WA_AcceptTouchEvents);
    setAttribute(Qt::WA_KeyCompression, false);

    spaceshipX = 0.0f; // Initial spaceship position
    spaceshipY = 0.0f;
    backgroundX = 0.0f;
    backgroundY = 0.0f;
    cameraX = 0.0f;
    cameraY = 0.0f;
    spaceshipDirection = GameSettings::Direction::Right;
    spaceshipAspectRatio = 0.0f;
    backgroundScrollSpeed = 0.0f;
    backgroundWidth = 0;
    backgroundHeight = 0;
    moveSpeedX = moveSpeedY = 0.0f;

    // Create a timer for updating the game at approximately 60fps
    QTimer* timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &GameWidget::updateGame);
    timer->start(GameSettings::FRAME_TIME); 
}

GameWidget::~GameWidget() {

    for (auto* texture : Explosion::explosionTextures) {
        delete texture;
    }
    Explosion::explosionTextures.clear();
}

void GameWidget::drawBackground() {
    backgroundTexture->bind();

     // Calculate texture offset for repeating background
    float backgroundOffsetX = cameraX * GameSettings::SCROLL_FACTOR_X;
    float backgroundOffsetY = cameraY * GameSettings::SCROLL_FACTOR_Y;

    // Repeat the texture
    glBegin(GL_QUADS);
        glTexCoord2f(backgroundOffsetX, backgroundOffsetY); glVertex2f(-GameSettings::BACKGROUND_SCALE_X, -GameSettings::BACKGROUND_SCALE_Y);
        glTexCoord2f(backgroundOffsetX + GameSettings::BACKGROUND_SCALE_X, backgroundOffsetY); glVertex2f(GameSettings::BACKGROUND_SCALE_X, -GameSettings::BACKGROUND_SCALE_Y);
        glTexCoord2f(backgroundOffsetX + GameSettings::BACKGROUND_SCALE_X, backgroundOffsetY + GameSettings::BACKGROUND_SCALE_Y); glVertex2f(GameSettings::BACKGROUND_SCALE_X, GameSettings::BACKGROUND_SCALE_Y);
        glTexCoord2f(backgroundOffsetX, backgroundOffsetY + GameSettings::BACKGROUND_SCALE_Y); glVertex2f(-GameSettings::BACKGROUND_SCALE_X, GameSettings::BACKGROUND_SCALE_Y);
    glEnd();

    backgroundTexture->release();
}

void GameWidget::drawEnemies() {
    // Render enemy spaceships
    enemyTexture->bind();
    float halfEnemyshipWidth = GameSettings::ENEMY_SIZE / 2; 
    float halfEnemyshipHeight = halfEnemyshipWidth / enemyAspectRatio;

    for (const auto& enemy : enemyManager.enemySpaceships) {
        glPushMatrix(); // save current matrix
        glTranslatef(enemy.x, enemy.y, 0.0f);
        glBegin(GL_QUADS); 
            glTexCoord2f(0.0f, 0.0f); glVertex2f(-halfEnemyshipWidth, -halfEnemyshipHeight);    // Bottom-left corner
            glTexCoord2f(1.0f, 0.0f); glVertex2f(halfEnemyshipWidth, -halfEnemyshipHeight);     // Bottom-right corner
            glTexCoord2f(1.0f, 1.0f); glVertex2f(halfEnemyshipWidth, halfEnemyshipHeight);      // Top-right corner
            glTexCoord2f(0.0f, 1.0f); glVertex2f(-halfEnemyshipWidth, halfEnemyshipHeight);     // Top-left corner
        glEnd(); 
        glPopMatrix(); // restor previous matrix
    }
    enemyTexture->release();
}

void GameWidget::drawPlayerSpaceship() {
    // Draw spaceship
    spaceshipTexture->bind();
    float halfSpaceshipWidth = GameSettings::SPACESHIP_SIZE / 2;
    float halfSpaceshipHeight = halfSpaceshipWidth / spaceshipAspectRatio;

    glPushMatrix();
    glTranslatef(spaceshipX, spaceshipY, 0.0f);
    if (spaceshipDirection == GameSettings::Direction::Right) {
        glRotatef(180.0f, 0.0f, 1.0f, 0.0f); // Rotate 180 degrees to face right
    }

    glBegin(GL_QUADS); // Begin defining a quadrilateral
        glTexCoord2f(0.0f, 0.0f); glVertex2f(-halfSpaceshipWidth, -halfSpaceshipHeight);    // Bottom-left corner
        glTexCoord2f(1.0f, 0.0f); glVertex2f(halfSpaceshipWidth, -halfSpaceshipHeight);     // Bottom-right corner
        glTexCoord2f(1.0f, 1.0f); glVertex2f(halfSpaceshipWidth, halfSpaceshipHeight);      // Top-right corner
        glTexCoord2f(0.0f, 1.0f); glVertex2f(-halfSpaceshipWidth, halfSpaceshipHeight);     // Top-left corner
    glEnd(); // End the definition and render the textured quadrilateral

    glPopMatrix();

    spaceshipTexture->release();
}

void GameWidget::drawBullets() {
    // Render bullets
    bulletTexture->bind();
    for (const auto& bullet : bullets) {
        // Save current transformation state
        glPushMatrix();

        glTranslatef(bullet.x, bullet.y, 0.0f);
        float halfBulletWidth = GameSettings::BULLET_SIZE / 2;
        float halfBulletHeight = halfBulletWidth; // Adjust based on the texture aspect ratio

        glBegin(GL_QUADS); // Begin defining a quadrilateral
            glTexCoord2f(0.0f, 0.0f); glVertex2f(-halfBulletWidth, -halfBulletHeight);
            glTexCoord2f(1.0f, 0.0f); glVertex2f(halfBulletWidth, -halfBulletHeight);
            glTexCoord2f(1.0f, 1.0f); glVertex2f(halfBulletWidth, halfBulletHeight);
            glTexCoord2f(0.0f, 1.0f); glVertex2f(-halfBulletWidth, halfBulletHeight);
        glEnd(); // End the definition and render the textured quadrilateral

        // Remove the top matrix from the current matrix stack
        glPopMatrix();
    }
    bulletTexture->release();
}

// Initializes OpenGL settings.
// Enables 2D texturing.
// Sets the clear color(background color of the window).
// Loads and configures textures for the game objects
void GameWidget::initializeGL() {
    initializeOpenGLFunctions();
    glEnable(GL_TEXTURE_2D);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // Load Textures
    // Enemy
    QImage enemyImage(":/game/enemy.png");
    if (enemyImage.isNull()) {
        qDebug() << "Failed to load enemy image";
    }
    enemyAspectRatio = static_cast<float>(enemyImage.width()) / static_cast<float>(enemyImage.height());
    enemyTexture = std::make_unique<QOpenGLTexture>(enemyImage.mirrored());
    enemyTexture->setMinificationFilter(QOpenGLTexture::Nearest);
    enemyTexture->setMagnificationFilter(QOpenGLTexture::Linear);
    enemyTexture->setWrapMode(QOpenGLTexture::Repeat);

    // Load the Background Texture
    QImage backgroundImage(":/game/background.png");
    if (backgroundImage.isNull()) {
        qDebug() << "Failed to load background image";
    }
    // set width and height
    backgroundWidth = backgroundImage.width();
    backgroundHeight = backgroundImage.height();

    backgroundTexture = std::make_unique<QOpenGLTexture>(backgroundImage.mirrored());
    backgroundTexture->setWrapMode(QOpenGLTexture::Repeat);
    backgroundTexture->setMinificationFilter(QOpenGLTexture::Nearest);
    backgroundTexture->setMagnificationFilter(QOpenGLTexture::Linear);
    backgroundTexture->setWrapMode(QOpenGLTexture::Repeat);

    // Spacecraft
    QImage spaceshipImage(":/game/spaceship.png");
    if (spaceshipImage.isNull()) {
        qDebug() << "Failed to load spacecraft image";
    }
    spaceshipAspectRatio = static_cast<float>(spaceshipImage.width()) / static_cast<float>(spaceshipImage.height());
    spaceshipTexture = std::make_unique<QOpenGLTexture>(spaceshipImage.mirrored(false, true));
    spaceshipTexture->setMinificationFilter(QOpenGLTexture::Nearest);
    spaceshipTexture->setMagnificationFilter(QOpenGLTexture::Linear);
    spaceshipTexture->setWrapMode(QOpenGLTexture::Repeat);

    // Bullet 
    QImage bulletImage(":/game/bullet.png");
    if (bulletImage.isNull()) {
        qDebug() << "Failed to load bullet image";
    }
    bulletTexture = std::make_unique<QOpenGLTexture>(bulletImage.mirrored());
    bulletTexture->setMinificationFilter(QOpenGLTexture::Nearest);
    bulletTexture->setMagnificationFilter(QOpenGLTexture::Linear);
    bulletTexture->setWrapMode(QOpenGLTexture::Repeat);
 
    // Explosion
    Explosion::loadTextures(this); // Load explosion textures
}

// Sets the viewport dimensions whenever the widget is resized.
void GameWidget::resizeGL(int w, int h) {
    glViewport(0, 0, w, h);
}

void GameWidget::drawLives()
{
    QPainter painter(this);

    // Draw player life icons above the score
    auto xPosition = 20; // Adjust X position for life icons
    auto yPosition = 10; // Adjust Y position to place icons above the score
    auto iconSpacing = 10; // Adjust the spacing between icons

    // Draw the player life icon
    QPixmap playerLifePixmap(":/game/life.png");
    for (int life = 0; life < GameSettings::PLAYER_LIVES; ++life) {

        auto width = playerLifePixmap.width();
        auto height = playerLifePixmap.height();

        // Calculate the position for each player life icon
        auto iconXPosition = xPosition + life * (width  + iconSpacing); // Adjust X position for each icon

        playerLifePixmap = playerLifePixmap.scaled(width, height, Qt::KeepAspectRatio);
        painter.drawPixmap(iconXPosition, yPosition, playerLifePixmap);
    }

    // Enable 2D texturing again for other OpenGL rendering
    glEnable(GL_TEXTURE_2D);
}

void GameWidget::drawScore()
{
    // Enable 2D texturing
    glEnable(GL_TEXTURE_2D);

    // Set the position for the score text (screen coordinates)
    auto xPosition = 20; // Adjust X position
    auto yPosition = 60; // Adjust Y position

    // Use QPainter to draw the score text on the screen
    QPainter painter(this);
    QFontDatabase::addApplicationFont(":/game/defender.ttf");
    painter.setPen(Qt::white); // Set the color for the text
    painter.setFont(QFont("Defender", 12)); // Set the font for the text
    painter.drawText(xPosition, yPosition, QString("Score: %1").arg(score));

    // Disable 2D texturing
    glDisable(GL_TEXTURE_2D);
}


void GameWidget::paintGL() {

    // Clear the screen to the clear color
    glClear(GL_COLOR_BUFFER_BIT);

    // Apply camera transformation
    glPushMatrix();
    glTranslatef(cameraX, cameraY, 0.0f);

    // Draw the background, enemies, etc., relative to the camera
    drawBackground();

    drawEnemies();

    // Player Spaceship is always at the center
    drawPlayerSpaceship();
    drawBullets();

    // Render active explosions
    for (auto& explosion : activeExplosions) {
        explosion.render();
    }

    // Draw spacecraft lives
    drawLives();

    // Draw the player's score
    drawScore();
 
    glPopMatrix();
}

void GameWidget::keyPressEvent(QKeyEvent* event) {

    switch (event->key()) {
    case Qt::Key_Up:
        moveSpeedY = GameSettings::ACCELERATION;
        scroll = true;
        break;
    case Qt::Key_Down:
        moveSpeedY = -GameSettings::ACCELERATION;
        scroll = true;
        break;
    case Qt::Key_Left:
        spaceshipDirection = GameSettings::Direction::Left;
        moveSpeedX = -GameSettings::ACCELERATION;
        scroll = true;
        break;
    case Qt::Key_Right:
        spaceshipDirection = GameSettings::Direction::Right;
        moveSpeedX = GameSettings::ACCELERATION;
        scroll = true;
        break;
    case Qt::Key_Space:
        Bullet newBullet;
        newBullet.x = spaceshipX; // Initial position at the spaceship
        newBullet.y = spaceshipY;

        // Set bullet speed based on spaceship direction
        if (spaceshipDirection == GameSettings::Direction::Left) {
            newBullet.speed = -GameSettings::BACKGROUND_SCROLL_SPEED; // Negative speed for leftward movement
        }
        else { // spaceshipDirection == Right
            newBullet.speed = GameSettings::BACKGROUND_SCROLL_SPEED; // Positive speed for rightward movement
        }

        bullets.push_back(newBullet);
        break;
    case Qt::Key_E:
        enemyManager.createRandomEnemySpaceship();
        break;
    default:
        break;
    }
    
    updateGame();
}

void GameWidget::keyReleaseEvent(QKeyEvent* event) {
    switch (event->key()) {
    case Qt::Key_Left:
    case Qt::Key_Right:
        moveSpeedX = 0.0f;
        break;
    case Qt::Key_Up:
    case Qt::Key_Down:
        moveSpeedY = 0.0f;
        break;
    }
    scroll = false;
}

void GameWidget::updateGame() {
    
    // Is player currently moving??
    if (scroll) {
        spaceshipX += moveSpeedX;
        spaceshipY += moveSpeedY;
    }
    else { // No? Slowly come to a stop
        moveSpeedX *= GameSettings::MOMENTUM_DECREASE;
        moveSpeedY *= GameSettings::MOMENTUM_DECREASE;
        spaceshipX += moveSpeedX;
        spaceshipY += moveSpeedY;
    }

    cameraX = -spaceshipX;
    cameraY = -spaceshipY;
    
    // Limit player movement within world boundaries
    spaceshipX = qBound(-GameSettings::WORLD_WIDTH / 2, spaceshipX, GameSettings::WORLD_WIDTH / 2);
    spaceshipY = qBound(-GameSettings::WORLD_HEIGHT / 2, spaceshipY, GameSettings::WORLD_HEIGHT / 2);

    // Update enemy spaceships, check collision
    enemyManager.update();

    // Update bullets, check boundaries, and check for collisions
    updateBullets();

    // Update active explosions
    updateExplosions();

    update(); // Schedule a repaint
}

bool GameWidget::checkCollision(const Bullet& bullet, const EnemySpaceship& enemy) {
    float bulletWidth = GameSettings::BULLET_SIZE;
    float bulletHeight = bulletWidth; 

    float enemyshipWidth = GameSettings::SPACESHIP_SIZE;
    float enemyshipHeight = enemyshipWidth / enemyAspectRatio;

    QRectF bulletRect(bullet.x - bulletWidth / 2, bullet.y - bulletHeight / 2, bulletWidth, bulletHeight);
    QRectF enemyRect(enemy.x - enemyshipWidth / 2, enemy.y - enemyshipHeight / 2, enemyshipWidth, enemyshipHeight);

    bool collision = bulletRect.intersects(enemyRect);

    return collision;
}

void GameWidget::updateBullets()
{
    for (auto bullet = bullets.begin(); bullet != bullets.end();) {
        bool bulletRemoved = false;
        bullet->x += bullet->speed;

        // Check for collision with enemy spaceships
        for (auto enemy = enemyManager.enemySpaceships.begin(); enemy != enemyManager.enemySpaceships.end() && !bulletRemoved;) {
            if (checkCollision(*bullet, *enemy)) {
                Explosion explosion(bullet->x, bullet->y);
                activeExplosions.push_back(explosion);

                // Remove bullet and enemy spaceship on collision
                bullet = bullets.erase(bullet);
                enemy = enemyManager.enemySpaceships.erase(enemy);
                bulletRemoved = true;

                // Update score
                score += 10;
            }
            else {
                ++enemy;
            }
        }

        // Check if bullet is out of screen boundaries
        if (!bulletRemoved && (bullet->x > GameSettings::SCREENBOUNDARY || bullet->x < -GameSettings::SCREENBOUNDARY)) {
            bullet = bullets.erase(bullet);
        }
        else if (!bulletRemoved) {
            ++bullet;
        }
    }
}

void GameWidget::updateExplosions()
{
    for (auto& explosion : activeExplosions) {
        explosion.render();
    }

    // Remove finished explosions
    activeExplosions.erase(
        std::remove_if(activeExplosions.begin(), activeExplosions.end(), [](const Explosion& e) { return e.isFinished(); }), activeExplosions.end());
}


