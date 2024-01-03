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

// Initializes OpenGL settings.
// Enables 2D texturing.
// Sets the clear color(background color of the window).
// Loads and configures textures for the spaceship and background.
void GameWidget::initializeGL() {
    initializeOpenGLFunctions();
    glEnable(GL_TEXTURE_2D);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    // Load the Spacecraft Texture
    QImage spaceshipImage(":/game/spaceship.png");
    if (spaceshipImage.isNull()) {
        qDebug() << "Failed to load spacecraft image";
    }

    spaceshipAspectRatio = static_cast<float>(spaceshipImage.width()) / static_cast<float>(spaceshipImage.height());
    spaceshipTexture = new QOpenGLTexture(spaceshipImage.mirrored(false, true));
    spaceshipTexture->setMinificationFilter(QOpenGLTexture::Nearest);
    spaceshipTexture->setMagnificationFilter(QOpenGLTexture::Linear);
    spaceshipTexture->setWrapMode(QOpenGLTexture::Repeat);

    // Load the Background Texture
    QImage backgroundImage(":/game/background.png");
    if (backgroundImage.isNull()) {
        qDebug() << "Failed to load background image";
    }

    backgroundTexture = new QOpenGLTexture(backgroundImage.mirrored());
    backgroundTexture->setWrapMode(QOpenGLTexture::Repeat);
    backgroundTexture->setMinificationFilter(QOpenGLTexture::Nearest);
    backgroundTexture->setMagnificationFilter(QOpenGLTexture::Linear);
    backgroundTexture->setWrapMode(QOpenGLTexture::Repeat);

    // Load the Bullet Texture
    QImage bulletImage(":/game/bullet.png");
    if (bulletImage.isNull()) {
        qDebug() << "Failed to load bullet image";
    }

    bulletTexture = new QOpenGLTexture(bulletImage.mirrored());
    bulletTexture->setMinificationFilter(QOpenGLTexture::Nearest);
    bulletTexture->setMagnificationFilter(QOpenGLTexture::Linear);
    bulletTexture->setWrapMode(QOpenGLTexture::Repeat);

    // Load the Enemy Texture
    QImage enemyImage(":/game/enemy.png");
    if (enemyImage.isNull()) {
        qDebug() << "Failed to load enemy image";
    }
    enemyAspectRatio = static_cast<float>(enemyImage.width()) / static_cast<float>(enemyImage.height());
    enemyTexture = new QOpenGLTexture(enemyImage.mirrored());
    enemyTexture->setMinificationFilter(QOpenGLTexture::Nearest);
    enemyTexture->setMagnificationFilter(QOpenGLTexture::Linear);
    enemyTexture->setWrapMode(QOpenGLTexture::Repeat);
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

    qDebug() << "In paintGL - backgroundX:" << backgroundX << "backgroundY:" << backgroundY;

    // Draw Background
    backgroundTexture->bind();

    glBegin(GL_QUADS); // Begin defining a quadrilateral
    glTexCoord2f(backgroundX, backgroundY); glVertex2f(-1.0f, -1.0f);
    glTexCoord2f(backgroundX + 1.0f, backgroundY); glVertex2f(1.0f, -1.0f);
    glTexCoord2f(backgroundX + 1.0f, backgroundY + 1.0f); glVertex2f(1.0f, 1.0f);
    glTexCoord2f(backgroundX, backgroundY + 1.0f); glVertex2f(-1.0f, 1.0f);
    glEnd();// End the definition and render the textured quadrilateral
   
    backgroundTexture->release();

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

void GameWidget::keyPressEvent(QKeyEvent* event) {
    float backgroundScrollSpeed = 0.01f; 
    qDebug() << "Key pressed: " << event->key();

    switch (event->key()) {
    case Qt::Key_Up:
        backgroundMomentumY = backgroundScrollSpeed;
        scroll = true;
        break;
    case Qt::Key_Down:
        backgroundMomentumY = -backgroundScrollSpeed;
        scroll = true;
        break;
    case Qt::Key_Left:
        spaceshipDirection = Direction::Left;
        backgroundMomentumX = -backgroundScrollSpeed;
        scroll = true;
        break;
    case Qt::Key_Right:
        spaceshipDirection = Direction::Right;
        backgroundMomentumX = backgroundScrollSpeed;
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
    case Qt::Key_Up:
    case Qt::Key_Down:
        scroll = false;
        break;
    default:
        break;
    }
}

void GameWidget::updateGame() {
    float screenBoundary = 1.0f; // Boundary for wrapping

    if (scroll) {
        // Update the background's position based on the current direction
        backgroundX += backgroundMomentumX;
        backgroundY += backgroundMomentumY;
    }
    else {
        // Apply momentum effect for gradual slowdown
        backgroundMomentumX *= MOMENTUM_DECREASE_FACTOR;
        backgroundMomentumY *= MOMENTUM_DECREASE_FACTOR;

        backgroundX += backgroundMomentumX;
        backgroundY += backgroundMomentumY;
    }

    // Reset momentum when it's very low to stop the background
    if (fabs(backgroundMomentumX) < 0.001f) backgroundMomentumX = 0.0f;
    if (fabs(backgroundMomentumY) < 0.001f) backgroundMomentumY = 0.0f;

    // Boundary checks for the spaceship
    if (spaceshipX > WORLD_WIDTH / 2) spaceshipX = -WORLD_WIDTH / 2;
    if (spaceshipX < -WORLD_WIDTH / 2) spaceshipX = WORLD_WIDTH / 2;

    // Boundary checks for Y coordinates
    if (spaceshipY > WORLD_HEIGHT / 2) spaceshipY = -WORLD_HEIGHT / 2;
    if (spaceshipY < -WORLD_HEIGHT / 2) spaceshipY = WORLD_HEIGHT / 2;

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



