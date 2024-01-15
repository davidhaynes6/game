#include "game.h"
#include "settings.h"
#include "explosion.h"
#include <QTimer>
#include <QPainter>

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
    spaceshipDirection = GameSettings::Direction::Right;
    spaceshipAspectRatio = 0.0f;
    backgroundScrollSpeed = 0.0f;
    moveSpeedX = moveSpeedY = 0.0f;

    // Create a timer for updating the game at approximately 60fps
    QTimer* timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &GameWidget::updateGame);
    timer->start(16); // Approximately 60fps
}

GameWidget::~GameWidget() {
    delete spaceshipTexture;
    delete backgroundTexture;
    delete bulletTexture;
    delete enemyTexture;
    
    for (auto* texture : Explosion::explosionTextures) {
        delete texture;
    }
    Explosion::explosionTextures.clear();
}

void GameWidget::drawBackground() {
    backgroundTexture->bind();

    // Adjust these factors to control the speed of scrolling and the scale of the background
    const float SCROLL_FACTOR_X = 0.005f;
    const float SCROLL_FACTOR_Y = 0.005f;
    const float BACKGROUND_SCALE_X = 2.0f; // Adjust as needed for your texture
    const float BACKGROUND_SCALE_Y = 2.0f; // Adjust as needed for your texture

    // Calculate texture offset for repeating background
    float backgroundOffsetX = cameraX * SCROLL_FACTOR_X;
    float backgroundOffsetY = cameraY * SCROLL_FACTOR_Y;

    // Repeat the texture
    glBegin(GL_QUADS);
    glTexCoord2f(backgroundOffsetX, backgroundOffsetY); glVertex2f(-BACKGROUND_SCALE_X, -BACKGROUND_SCALE_Y);
    glTexCoord2f(backgroundOffsetX + BACKGROUND_SCALE_X, backgroundOffsetY); glVertex2f(BACKGROUND_SCALE_X, -BACKGROUND_SCALE_Y);
    glTexCoord2f(backgroundOffsetX + BACKGROUND_SCALE_X, backgroundOffsetY + BACKGROUND_SCALE_Y); glVertex2f(BACKGROUND_SCALE_X, BACKGROUND_SCALE_Y);
    glTexCoord2f(backgroundOffsetX, backgroundOffsetY + BACKGROUND_SCALE_Y); glVertex2f(-BACKGROUND_SCALE_X, BACKGROUND_SCALE_Y);
    glEnd();

    backgroundTexture->release();
}


void GameWidget::drawEnemies() {
    // Render enemy spaceships
    enemyTexture->bind();
    float enemyshipWidth = GameSettings::SPACESHIP_SIZE;
    float enemyshipHeight = enemyshipWidth / enemyAspectRatio;
    for (const auto& enemy : enemyManager.enemySpaceships) {
        glPushMatrix();
        glTranslatef(enemy.x, enemy.y, 0.0f);
        glBegin(GL_QUADS); // Begin defining a quadrilateral
        glTexCoord2f(0.0f, 0.0f); glVertex2f(-enemyshipWidth / 2, -enemyshipHeight / 2);    // Bottom-left corner
        glTexCoord2f(1.0f, 0.0f); glVertex2f(enemyshipWidth / 2, -enemyshipHeight / 2);     // Bottom-right corner
        glTexCoord2f(1.0f, 1.0f); glVertex2f(enemyshipWidth / 2, enemyshipHeight / 2);      // Top-right corner
        glTexCoord2f(0.0f, 1.0f); glVertex2f(-enemyshipWidth / 2, enemyshipHeight / 2);     // Top-left corner
        glEnd(); // End the definition and render the textured quadrilateral
        glPopMatrix();
    }
    enemyTexture->release();

    // Draw bounding box for visual debugging
    glColor3f(0.0f, 1.0f, 0.0f); // Green color for the bounding box
    for (const auto& enemy : enemyManager.enemySpaceships) {
        float enemyshipWidth = GameSettings::SPACESHIP_SIZE;
        float enemyshipHeight = enemyshipWidth / enemyAspectRatio;

        glBegin(GL_LINE_LOOP);
        glVertex2f(enemy.x - enemyshipWidth / 2, enemy.y - enemyshipHeight / 2);
        glVertex2f(enemy.x + enemyshipWidth / 2, enemy.y - enemyshipHeight / 2);
        glVertex2f(enemy.x + enemyshipWidth / 2, enemy.y + enemyshipHeight / 2);
        glVertex2f(enemy.x - enemyshipWidth / 2, enemy.y + enemyshipHeight / 2);
        glEnd();
    }
    glColor3f(1.0f, 1.0f, 1.0f); // Reset color
}

void GameWidget::drawPlayerSpaceship() {
    // Draw spaceship
    spaceshipTexture->bind();
    float spaceshipWidth = GameSettings::SPACESHIP_SIZE;
    float spaceshipHeight = spaceshipWidth / spaceshipAspectRatio;


    glPushMatrix();
    glTranslatef(spaceshipX, spaceshipY, 0.0f);
    if (spaceshipDirection == GameSettings::Direction::Right) {
        glRotatef(180.0f, 0.0f, 1.0f, 0.0f); // Rotate 180 degrees to face right
    }

    glBegin(GL_QUADS); // Begin defining a quadrilateral
    glTexCoord2f(0.0f, 0.0f); glVertex2f(-spaceshipWidth / 2, -spaceshipHeight / 2);    // Bottom-left corner
    glTexCoord2f(1.0f, 0.0f); glVertex2f(spaceshipWidth / 2, -spaceshipHeight / 2);     // Bottom-right corner
    glTexCoord2f(1.0f, 1.0f); glVertex2f(spaceshipWidth / 2, spaceshipHeight / 2);      // Top-right corner
    glTexCoord2f(0.0f, 1.0f); glVertex2f(-spaceshipWidth / 2, spaceshipHeight / 2);     // Top-left corner
    glEnd(); // End the definition and render the textured quadrilateral

    glPopMatrix();

    spaceshipTexture->release();


    // Draw bounding box for debugging
    glColor3f(1.0f, 0.0f, 0.0f); // Set color to red for the bounding box
    glPushMatrix();
    glTranslatef(spaceshipX, spaceshipY, 0.0f);
    glBegin(GL_LINE_LOOP); // Begin drawing lines
    glVertex2f(-spaceshipWidth / 2, -spaceshipHeight / 2); // Bottom-left corner
    glVertex2f(spaceshipWidth / 2, -spaceshipHeight / 2);  // Bottom-right corner
    glVertex2f(spaceshipWidth / 2, spaceshipHeight / 2);   // Top-right corner
    glVertex2f(-spaceshipWidth / 2, spaceshipHeight / 2);  // Top-left corner
    glEnd(); // End drawing lines
    glPopMatrix();

    glColor3f(1.0f, 1.0f, 1.0f); // Reset color to white
}

void GameWidget::drawBullets() {
    // Render bullets
    bulletTexture->bind();
    for (const auto& bullet : bullets) {
        // Save current transformation state
        glPushMatrix();

        glTranslatef(bullet.x, bullet.y, 0.0f);
        float bulletWidth = GameSettings::BULLET_SIZE;
        float bulletHeight = bulletWidth; // Adjust based on the texture aspect ratio

        glBegin(GL_QUADS); // Begin defining a quadrilateral
        glTexCoord2f(0.0f, 0.0f); glVertex2f(-bulletWidth / 2, -bulletHeight / 2);
        glTexCoord2f(1.0f, 0.0f); glVertex2f(bulletWidth / 2, -bulletHeight / 2);
        glTexCoord2f(1.0f, 1.0f); glVertex2f(bulletWidth / 2, bulletHeight / 2);
        glTexCoord2f(0.0f, 1.0f); glVertex2f(-bulletWidth / 2, bulletHeight / 2);
        glEnd(); // End the definition and render the textured quadrilateral

        // Remove the top matrix from the current matrix stack
        glPopMatrix();
    }
    bulletTexture->release();


    // Draw bounding box for visual debugging
    glColor3f(1.0f, 0.0f, 0.0f); // Red color for the bounding box
    for (const auto& bullet : bullets) {
        float bulletWidth = GameSettings::BULLET_SIZE;
        float bulletHeight = bulletWidth; // Assuming square bullets

        glBegin(GL_LINE_LOOP);
        glVertex2f(bullet.x - bulletWidth / 2, bullet.y - bulletHeight / 2);
        glVertex2f(bullet.x + bulletWidth / 2, bullet.y - bulletHeight / 2);
        glVertex2f(bullet.x + bulletWidth / 2, bullet.y + bulletHeight / 2);
        glVertex2f(bullet.x - bulletWidth / 2, bullet.y + bulletHeight / 2);
        glEnd();
    }
    glColor3f(1.0f, 1.0f, 1.0f); // Reset color
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
    enemyTexture = new QOpenGLTexture(enemyImage.mirrored());
    enemyTexture->setMinificationFilter(QOpenGLTexture::Nearest);
    enemyTexture->setMagnificationFilter(QOpenGLTexture::Linear);
    enemyTexture->setWrapMode(QOpenGLTexture::Repeat);

    // Load the Background Texture
    QImage backgroundImage(":/game/background.png");
    if (backgroundImage.isNull()) {
        qDebug() << "Failed to load background image";
    }
    // set width and height
    BACKGROUND_TEXTURE_WIDTH = backgroundImage.width();
    BACKGROUND_TEXTURE_HEIGHT = backgroundImage.height();

    backgroundTexture = new QOpenGLTexture(backgroundImage.mirrored());
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
    spaceshipTexture = new QOpenGLTexture(spaceshipImage.mirrored(false, true));
    spaceshipTexture->setMinificationFilter(QOpenGLTexture::Nearest);
    spaceshipTexture->setMagnificationFilter(QOpenGLTexture::Linear);
    spaceshipTexture->setWrapMode(QOpenGLTexture::Repeat);

    // Bullet 
    QImage bulletImage(":/game/bullet.png");
    if (bulletImage.isNull()) {
        qDebug() << "Failed to load bullet image";
    }
    bulletTexture = new QOpenGLTexture(bulletImage.mirrored());
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

// Handles the rendering of each frame.
// Clears the screen.
// Renders a scrolling background.
// Renders the spaceship with a fixed aspect ratio and position.
void GameWidget::paintGL() {
    QPainter painter(this);

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

    // Draw the player's score
    painter.setPen(Qt::white); // Set the color for the text
    painter.setFont(QFont("Arial", 12)); // Set the font for the text
    painter.drawText(10, 30, QString("Score: %1").arg(score));

    glPopMatrix();


}

void GameWidget::keyPressEvent(QKeyEvent* event) {
    float backgroundScrollSpeed = 0.01f; 
    //qDebug() << "Key pressed: " << event->key();

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
        
        qDebug() << "Bullet created at position: (" << newBullet.x << "," << newBullet.y << ") with size: " << GameSettings::BULLET_SIZE;

        // Set bullet speed based on spaceship direction
        if (spaceshipDirection == GameSettings::Direction::Left) {
            newBullet.speed = -0.01f; // Negative speed for leftward movement
        }
        else { // spaceshipDirection == Right
            newBullet.speed = 0.01f; // Positive speed for rightward movement
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
    float screenBoundary = 1.0f; // Boundary for wrapping
    
    // Is player currently moving??
    if (scroll) {
        spaceshipX += moveSpeedX;
        spaceshipY += moveSpeedY;
    }
    else { // No? Slowly come to a stop
        moveSpeedX *= GameSettings::FRICTION;
        moveSpeedY *= GameSettings::FRICTION;
        spaceshipX += moveSpeedX;
        spaceshipY += moveSpeedY;
    }

    cameraX = -spaceshipX;
    cameraY = -spaceshipY;
    
    // Limit player movement within world boundaries
    spaceshipX = qBound(-GameSettings::WORLD_WIDTH / 2, spaceshipX, GameSettings::WORLD_WIDTH / 2);
    spaceshipY = qBound(-GameSettings::WORLD_HEIGHT / 2, spaceshipY, GameSettings::WORLD_HEIGHT / 2);

    qDebug() << "Player position: " << spaceshipX << ", " << spaceshipY;
    qDebug() << "Camera position: " << cameraX << ", " << cameraY;

    // Constrain the spaceship within world boundaries
    handleSpaceshipBoundary();

    // Update enemy spaceships by passing player's spaceship position
    enemyManager.update();

    // Update bullets and check boundaries
    for (size_t i = 0; i < bullets.size();) {
        bullets[i].x += bullets[i].speed;
        if (bullets[i].x > screenBoundary || bullets[i].x < -screenBoundary) {
            bullets.erase(bullets.begin() + i);
        }
        else {
            ++i;
        }
    }

    // Update bullets and check for collisions
    for (size_t i = 0; i < bullets.size();) {
        bool bulletRemoved = false;
        bullets[i].x += bullets[i].speed;
        qDebug() << "Bullet position: " << bullets[i].x << ", " << bullets[i].y;
    
        // Check for collision with enemy spaceships
        for (size_t j = 0; j < enemyManager.enemySpaceships.size() && !bulletRemoved;) {

            // Log positions right before checking for collision
            qDebug() << "Checking collision between bullet at index " << i << " and enemy spaceship at index " << j;
            qDebug() << "Bullet position: " << bullets[i].x << ", " << bullets[i].y;
            qDebug() << "Enemy position: " << enemyManager.enemySpaceships[j].x << ", " << enemyManager.enemySpaceships[j].y;

            if (checkCollision(bullets[i], enemyManager.enemySpaceships[j])) {
                qDebug() << "Collision detected. Removing bullet and enemy spaceship.";

                Explosion explosion(bullets[i].x, bullets[i].y);
                activeExplosions.push_back(explosion);

                // Remove bullet and enemy spaceship on collision
                bullets.erase(bullets.begin() + i);
                enemyManager.enemySpaceships.erase(enemyManager.enemySpaceships.begin() + j);
                bulletRemoved = true;

                // Update score
                score += 10;
            }
            else {
                qDebug() << "No collision detected between bullet at index" << i << "and enemy spaceship at index" << j;
                ++j;
            }
        }

        if (!bulletRemoved && (bullets[i].x > screenBoundary || bullets[i].x < -screenBoundary)) {
            bullets.erase(bullets.begin() + i);
        }
        else if (!bulletRemoved) {
            ++i;
        }
    }

    // Update active explosions
    for (auto& explosion : activeExplosions) {
        explosion.render();
    }

    // Remove finished explosions
    activeExplosions.erase(
        std::remove_if(activeExplosions.begin(), activeExplosions.end(),
            [](const Explosion& e) { return e.isFinished(); }),
        activeExplosions.end());

    update(); // Schedule a repaint
}

bool GameWidget::checkCollision(const Bullet& bullet, const EnemySpaceship& enemy) {
    float bulletWidth = GameSettings::BULLET_SIZE;
    float bulletHeight = bulletWidth; // Assuming square bullets

    float enemyshipWidth = GameSettings::SPACESHIP_SIZE;
    float enemyshipHeight = enemyshipWidth / enemyAspectRatio;

    QRectF bulletRect(bullet.x - bulletWidth / 2, bullet.y - bulletHeight / 2, bulletWidth, bulletHeight);
    QRectF enemyRect(enemy.x - enemyshipWidth / 2, enemy.y - enemyshipHeight / 2, enemyshipWidth, enemyshipHeight);

    bool collision = bulletRect.intersects(enemyRect);

    if (collision) {
        qDebug() << "Collision detected between bullet and enemy spaceship.";
    }
    else {
        qDebug() << "No collision. Bullet Rect:" << bulletRect << "Enemy Rect:" << enemyRect;
    }

    return collision;
}

void GameWidget::handleSpaceshipBoundary() {
    if (spaceshipX < -GameSettings::WORLD_WIDTH / 2) {
        spaceshipX = -GameSettings::WORLD_WIDTH / 2;
    }
    else if (spaceshipX > GameSettings::WORLD_WIDTH / 2) {
        spaceshipX = GameSettings::WORLD_WIDTH / 2;
    }

    if (spaceshipY < -GameSettings::WORLD_HEIGHT / 2) {
        spaceshipY = -GameSettings::WORLD_HEIGHT / 2;
    }
    else if (spaceshipY > GameSettings::WORLD_HEIGHT / 2) {
        spaceshipY = GameSettings::WORLD_HEIGHT / 2;
    }
}



