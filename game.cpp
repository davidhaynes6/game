#include "game.h"
#include "constants.h"
#include <QTimer>

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
    spaceshipDirection = Direction::Right;
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
    float enemyshipWidth = SPACESHIP_SIZE;
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
}

void GameWidget::drawPlayerSpaceship() {
    // Draw spaceship
    spaceshipTexture->bind();
    float spaceshipWidth = SPACESHIP_SIZE;
    float spaceshipHeight = spaceshipWidth / spaceshipAspectRatio;

    glPushMatrix();
    glTranslatef(spaceshipX, spaceshipY, 0.0f);
    if (spaceshipDirection == Right) {
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
}

void GameWidget::drawBullets() {
    // Render bullets
    bulletTexture->bind();
    for (const auto& bullet : bullets) {
        // Save current transformation state
        glPushMatrix();

        glTranslatef(bullet.x, bullet.y, 0.0f);
        float bulletWidth = BULLET_SIZE;
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
    // Clear the screen to the clear color
    glClear(GL_COLOR_BUFFER_BIT);

    // Apply camera transformation
    glPushMatrix();
    glTranslatef(cameraX, cameraY, 0.0f);

    // Draw the background, enemies, etc., relative to the camera
    drawBackground();
    drawEnemies();

    glPopMatrix();

    // Player Spaceship is always at the center
    drawPlayerSpaceship();
    drawBullets();
}

void GameWidget::keyPressEvent(QKeyEvent* event) {
    float backgroundScrollSpeed = 0.01f; 
    qDebug() << "Key pressed: " << event->key();

    switch (event->key()) {
    case Qt::Key_Up:
        moveSpeedY = ACCELERATION;
        scroll = true;
        break;
    case Qt::Key_Down:
        moveSpeedY = -ACCELERATION;
        scroll = true;
        break;
    case Qt::Key_Left:
        spaceshipDirection = Direction::Left;
        moveSpeedX = -ACCELERATION;
        scroll = true;
        break;
    case Qt::Key_Right:
        spaceshipDirection = Direction::Right;
        moveSpeedX = ACCELERATION;
        scroll = true;
        break;
    case Qt::Key_Space:
        Bullet newBullet;
        newBullet.x = spaceshipX; // Initial position at the spaceship
        newBullet.y = spaceshipY;

        // Set bullet speed based on spaceship direction
        if (spaceshipDirection == Left) {
            newBullet.speed = -0.1f; // Negative speed for leftward movement
        }
        else { // spaceshipDirection == Right
            newBullet.speed = 0.1f; // Positive speed for rightward movement
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

    if (scroll) {
        spaceshipX += moveSpeedX;
        spaceshipY += moveSpeedY;
    }
    else {
        moveSpeedX *= FRICTION;
        moveSpeedY *= FRICTION;
        spaceshipX += moveSpeedX;
        spaceshipY += moveSpeedY;
    }

    cameraX = -spaceshipX;
    cameraY = -spaceshipY;

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

    update(); // Schedule a repaint
}



