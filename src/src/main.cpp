#include <SFML/Graphics.hpp>
#include <SFML/System/Vector2.hpp>
#include <iostream>
#include <chrono>
#include <thread>
using namespace std;
using namespace sf;
using namespace chrono;

#define MAX_ENEMIES 255
#define GAME_RESOULUTION 1024,640
const string filepath = "..\\res/";
static const string textureNames[]{"img/spaceship1.png",
                                   "img/spaceship2.png",
                                   "img/spaceship3.png",
                                   "img/Spaceship-Drakir1.png",
                                   "img/Spaceship-Drakir2.png",
                                   "img/Spaceship-Drakir3.png",
                                   "img/Spaceship-Drakir4.png",
                                   "img/Spaceship-Drakir5.png",
                                   "img/Spaceship-Drakir6.png",
                                   "img/Spaceship-Drakir7.png",
                                   "img/bullet.png"};

Texture *textures[11];
Font *gameFont;
Sprite *playerSprite;
Sprite *bulletsprite;
Text *scoreText;
static Sprite *enemies[MAX_ENEMIES];

static unsigned int currentEnemies = 0;
float playerMoveSpeed = 600.0f;
unsigned int score = 0;
float runTime = 0; // time in seconds that the game has been running
high_resolution_clock::time_point previous;

static Vector2f GetNewEnemyPos() { return Vector2f(rand() % 1024, -128.0f); }

void Loadcontent() {
  gameFont = new Font();
  gameFont->loadFromFile(filepath + "/fonts/AmericanCaptain.ttf");
  scoreText = new Text();
  scoreText->setFont(*gameFont);
  scoreText->setPosition(900, 0);
  scoreText->setCharacterSize(24);
  scoreText->setColor(Color::Red);

  for (size_t i = 0; i < 11; i++) {
    textures[i] = new Texture();
    if (!textures[i]->loadFromFile(filepath + textureNames[i])) {
      throw invalid_argument("Loading error!");
    }
  }
  playerSprite = new Sprite();
  playerSprite->setTexture(*textures[0]);
  playerSprite->setPosition(512, 256);
  bulletsprite = new Sprite();
  bulletsprite->setTexture(*textures[10]);
  bulletsprite->setPosition(0, -128.0f);
  for (auto &e : enemies) {
    e = new Sprite();
    e->setTexture(*textures[(rand() % 7) + 3]);
    e->setPosition(GetNewEnemyPos());
  }
}

void Unload() {
  for (auto &t : textures) {
    delete t;
    t = nullptr;
  }
  for (auto &e : enemies) {
    delete e;
    e = nullptr;
  }
  delete gameFont;
  gameFont = nullptr;
  delete scoreText;
  scoreText = nullptr;
  delete bulletsprite;
  bulletsprite = nullptr;
  delete playerSprite;
  playerSprite = nullptr;
}

void Normalize(Vector2f &v) {
  float length = sqrt(v.x * v.x + v.y * v.y);
  if (length == 0.0f) {
    return;
  }
  v.x /= length;
  v.y /= length;
}

void FireBullet() {
  if (bulletsprite->getPosition().y <= -128.0f) {
    bulletsprite->setPosition(playerSprite->getPosition().x + 30,
                              playerSprite->getPosition().y - 1);
  }
}
void ResetGame() {
  score = 0;
  runTime = 0;
  currentEnemies = 0;
  playerSprite->setPosition(512, 256);
  for (auto e : enemies) {
    e->setPosition(GetNewEnemyPos());
  }
}

void Update(RenderWindow &window) {

  high_resolution_clock::time_point now = high_resolution_clock::now();
  // time in nanoseconds since last update
  const auto delta = (duration_cast<duration<unsigned int, nano>>(now - previous)).count();
  const double deltaSeconds = ((double)delta) / 1000000000.0;
  previous = now;

  runTime += deltaSeconds;
  currentEnemies = (unsigned int)ceil(runTime * 0.6f) + 1;

  { // Input
    Event e;
    Vector2f moveDirection(0, 0);

    while (window.pollEvent(e)) {

      if (e.type == Event::Closed)
        window.close();

      // keyboard event handling
      if (e.type == Event::KeyPressed) {
        if (e.key.code == Keyboard::Escape) {
          window.close();
        } else if (e.key.code == Keyboard::Space) {
          FireBullet();
        }
      }
      // if the B button is pressed fire a bullet
      if (e.JoystickButtonPressed) {
        if (Joystick::isButtonPressed(0, 1)) {
          FireBullet();
        }
      }
    }
    if (Keyboard::isKeyPressed(Keyboard::Up) || Keyboard::isKeyPressed(Keyboard::W)) {
      moveDirection += Vector2f(0, -1);
    }
    if (Keyboard::isKeyPressed(Keyboard::Down) || Keyboard::isKeyPressed(Keyboard::S)) {
      moveDirection += Vector2f(0, 1);
    }
    if (Keyboard::isKeyPressed(Keyboard::Left) || Keyboard::isKeyPressed(Keyboard::A)) {
      moveDirection += Vector2f(-1, 0);
    }
    if (Keyboard::isKeyPressed(Keyboard::Right) || Keyboard::isKeyPressed(Keyboard::D)) {
      moveDirection += Vector2f(1, 0);
    }

    // joystick input
    if (Joystick::isConnected(0)) {
      const float joystickX = Joystick::getAxisPosition(0, Joystick::X);
      const float joystickY = Joystick::getAxisPosition(0, Joystick::Y);

      if (abs(joystickX) > 40.0f) {
        moveDirection += Vector2f(((signbit(joystickX)) * -2) + 1, 0);
      }
      if (abs(joystickY) > 40.0f) {
        moveDirection += Vector2f(0, ((signbit(joystickY)) * -2) + 1);
      }
    }

    Normalize(moveDirection);
    moveDirection = (moveDirection * playerMoveSpeed) * (float)deltaSeconds;

    playerSprite->setPosition(playerSprite->getPosition() + moveDirection);
  } // End Input

  if (bulletsprite->getPosition().y > -128.0f) {
    bulletsprite->setPosition(bulletsprite->getPosition().x,
                              bulletsprite->getPosition().y - 1000.0 * deltaSeconds);
  }

  for (size_t i = 0; i < MAX_ENEMIES; i++) {
    if (i < currentEnemies) {
      // if not dead, move
      if (enemies[i]->getPosition().y < 700.0) {
        enemies[i]->setPosition(enemies[i]->getPosition() +
                                Vector2f(sinf(+i) * 100.0 * deltaSeconds, 100.0 * deltaSeconds));
        // collisions
        if (bulletsprite->getGlobalBounds().intersects(enemies[i]->getGlobalBounds())) {
          enemies[i]->setPosition(GetNewEnemyPos());
          score += 100;
        }
      } else {
        // offscreen kill
        enemies[i]->setPosition(GetNewEnemyPos());
      }
    } else {
      // if alive
      if (enemies[i]->getPosition().y != -128.0f) {
        // kill
        enemies[i]->setPosition(GetNewEnemyPos());
      }
    }
  }

  scoreText->setString("Score =" + to_string(score + ceil(runTime)));
}

void Render(RenderWindow &window) {
  window.clear();
  window.draw(*playerSprite);
  window.draw(*bulletsprite);
  for (size_t i = 0; i < currentEnemies; i++) {
    window.draw(*enemies[i]);
  }
  window.draw(*scoreText);
  window.display();
}

int main() {
  Loadcontent();
  RenderWindow *window = new RenderWindow(VideoMode(GAME_RESOULUTION), "Advanced Minigame");
  window->setVerticalSyncEnabled(true);
  previous = high_resolution_clock::now();
  while (window->isOpen()) {
    Update(*window);
    Render(*window);
  }
  Unload();
  delete window;
  window = nullptr;
  return 0;
}