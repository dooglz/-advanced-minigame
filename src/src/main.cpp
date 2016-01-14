#include "particles.h"
#include <SFML/Graphics.hpp>
#include <SFML/System/Vector2.hpp>
#include <chrono>
#include <iostream>
#include <thread>
using namespace std;
using namespace sf;
using namespace chrono;

#define MAX_ENEMIES 255
#define DEFAULT_WINDOW_RESOULUTION_X 600  // 461
#define DEFAULT_WINDOW_RESOULUTION_Y 1000 // 768
#define DEFAULT_WINDOW_RESOULUTION DEFAULT_WINDOW_RESOULUTION_X, DEFAULT_WINDOW_RESOULUTION_Y
#define GAME_WORLD_X 600
#define GAME_WORLD_Y 1000
#define GAME_RESOULUTION GAME_WORLD_X, GAME_WORLD_Y

Vector2u scaledGameResolution;
Vector2f scaledGameResolutionNormal;
Vector2u scaledGameOffset;
Vector2f scaledGameOffsetNormal;

ParticleSystem *ps;

View gameView;
View menuView;
View controlView;
enum Gamestates { Start, Game, Pause, Dead, controls };
Gamestates state = Gamestates::Start;

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
                                   "img/bullet.png",
                                   "img/Background.png","img/Explosion.png","img/Life.png","img/Shield.png","img/Missile.png"};

Texture *textures[16];
Font *gameFont;
Sprite *playerSprite;
Sprite *backgroundSprite;
Sprite *bulletsprite;
Text *scoreText;
Text *titleText;
Text *playText;
Text *controlText;
Text *pausedText;
Text *exitText;
int playerlives = 3;
static Sprite *enemies[MAX_ENEMIES];

static unsigned int currentEnemies = 0;
float playerMoveSpeed = 600.0f;
unsigned int score = 0;
float runTime = 0; // time in seconds that the game has been running
high_resolution_clock::time_point previous;

static Vector2f GetNewEnemyPos() { return Vector2f((float)(rand() % GAME_WORLD_Y), -128.0f); }

void Resize(RenderWindow &window) {
  const Vector2f win = (Vector2f)window.getSize();
  float scale = 1.0f;
  float scale2 = 1.0f;
  const float GWX = (float)GAME_WORLD_X;
  const float GWY = (float)GAME_WORLD_Y;
  if (win.y > win.x) {
    // wide
    scale = win.x / GWX;
    if (win.y < (scale * GWY)) {
      scale2 = win.y / GWY;
    }
  } else {
    // tall (or square)
    scale = win.y / GWY;
    if (win.x < (scale * GWX)) {
      scale2 = win.x / GWX;
    }
  }
  scale = min(scale, scale2);
  scaledGameResolution =
      Vector2u((unsigned int)floor(scale * GWX), (unsigned int)floor(scale * GWY));
  scaledGameOffset = Vector2u((unsigned int)max(0.0f, (win.x - scaledGameResolution.x) * 0.5f),
                              (unsigned int)max(0.0f, (win.y - scaledGameResolution.y) * 0.5f));
  scaledGameResolutionNormal =
      Vector2f((float)scaledGameResolution.x / win.x, (float)scaledGameResolution.y / win.y);
  scaledGameOffsetNormal =
      Vector2f((float)scaledGameOffset.x / win.x, (float)scaledGameOffset.y / win.y);
}

void Loadcontent() {

  gameFont = new Font();
  gameFont->loadFromFile(filepath + "/fonts/AmericanCaptain.ttf");
  scoreText = new Text();
  scoreText->setFont(*gameFont);
  scoreText->setPosition(0, 0);
  scoreText->setCharacterSize(24);
  scoreText->setColor(Color::Red);

  titleText = new Text();
  titleText->setFont(*gameFont);
  titleText->setCharacterSize(24);
  titleText->setColor(Color::White);

  pausedText = new Text();
  pausedText->setFont(*gameFont);
  pausedText->setPosition(GAME_WORLD_X / 2.5, 512);
  pausedText->setString("Game Paused");
  pausedText->setCharacterSize(32);
  pausedText->setColor(Color::Red);
  pausedText->Bold;

  playText = new Text();
  playText->setFont(*gameFont);
  playText->setCharacterSize(24);
  playText->setColor(Color::White);

  controlText = new Text();
  controlText->setFont(*gameFont);
  controlText->setCharacterSize(24);
  controlText->setColor(Color::White);

  controlText = new Text();
  controlText->setFont(*gameFont);
  controlText->setCharacterSize(24);
  controlText->setColor(Color::Blue);

  exitText = new Text();
  exitText->setFont(*gameFont);
  exitText->setCharacterSize(24);
  exitText->setColor(Color::White);

  for (size_t i = 0; i < 16; i++) {
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

  backgroundSprite = new Sprite();
  backgroundSprite->setTexture(*textures[11]);
  backgroundSprite->setPosition(GAME_WORLD_X, GAME_WORLD_Y);
  auto c = backgroundSprite->getColor();
  c.a = 100;
  backgroundSprite->setColor(c);
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

void menuUpdate(RenderWindow &window) {

  Event e;

  while (window.pollEvent(e)) {

    if (e.type == Event::Closed)
      window.close();

    if (e.type == sf::Event::Resized) {
      Resize(window);
      menuView.setViewport(FloatRect(scaledGameOffsetNormal.x, scaledGameOffsetNormal.y,
                                     scaledGameResolutionNormal.x, scaledGameResolutionNormal.y));
      window.setView(menuView);
    }

    sf::Vector2i mousePosition = sf::Mouse::getPosition(window);

    if (exitText->getGlobalBounds().contains((sf::Vector2f)mousePosition)) {
      exitText->setColor(Color::Red);
      if (Mouse::isButtonPressed(Mouse::Left)) {
        window.close();
      }
    } else {
      exitText->setColor(Color::White);
    }

    if (playText->getGlobalBounds().contains((sf::Vector2f)mousePosition)) {
      playText->setColor(Color::Red);

      if (Mouse::isButtonPressed(Mouse::Left)) {
        state = Gamestates::Game;
      }

    } else {
      playText->setColor(Color::White);
    }

    if (controlText->getGlobalBounds().contains((sf::Vector2f)mousePosition)) {
      controlText->setColor(Color::Red);

      if (Mouse::isButtonPressed(Mouse::Left)) {
        state = Gamestates::controls;
        playText->setPosition(0, 48);
        controlText->setPosition(0, 148);
        exitText->setPosition(0, 850);
        controlText->setString("Menu controls"
                               "\n\n"
                               "Use the mouse to select your choice"
                               "\n\n"
                               "controller"
                               "\t\t\t\tA"
                               "\t\t\tPlay Game"
                               "\n"
                               "\t\t\t\t\t\t\t\t\t\t\t\tB"
                               "\t\t\tExit Game"
                               "\n\n"
                               "Game Controls"
                               "\n\n"
                               "Keyboard \t\t\t"
                               "\t\t\t\t\t\t\t Controller"
                               "\n\n"
                               "WASD  or arrow keys \t"
                               "\t\tLeft Thumbstick"
                               " \t\tMove ship"
                               "\n\n"
                               "Space"
                               "\t\t\t\t\t\t\t\t\t\t\t\t\t\tA"
                               "\t\t\t\t\t\t\t\t\t\t\t\t\tFire Bullet"
                               "\n\n"
                               "P"
                               "\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\tX"
                               "\t\t\t\t\t\t\t\t\t\t\t\t\tPause Game"
                               "\n\n"
                               "Escape or exit button"
                               "\t\tB"
                               "\t\t\t\t\t\t\t\t\t\t\t\t\t\tExit Game");
      }

    } else {
      controlText->setColor(Color::White);
    }

    // keyboard event handling
    if (e.type == Event::KeyPressed) {
      if (e.key.code == Keyboard::Escape) {
        window.close();
      } else if (e.key.code == Keyboard::B) {
        Resize(window);
      } else if (e.key.code == Keyboard::S && state == Gamestates::Start ||
                 e.key.code == Keyboard::S && state == Gamestates::Pause) {
        state = Gamestates::Game;
      }
    }

    if (Joystick::isButtonPressed(0, 0)) {
      state = Gamestates::Game;
    }

    if (Joystick::isButtonPressed(0, 1)) {
      window.close();
    }
  }
}

void Update(RenderWindow &window) {

  high_resolution_clock::time_point now = high_resolution_clock::now();
  // time in nanoseconds since last update
  const auto delta = (duration_cast<duration<unsigned int, nano>>(now - previous)).count();
  const float deltaSeconds = ((float)delta) / 1000000000.0f;
  previous = now;

  runTime += deltaSeconds;
  currentEnemies = min((int)ceil(runTime * 0.6f) + 1, MAX_ENEMIES);

  { // Input
    Event e;
    Vector2f moveDirection(0, 0);
    playerSprite->setColor(Color::Red);
    while (window.pollEvent(e)) {

      if (e.type == Event::Closed)
        window.close();

      if (e.type == sf::Event::Resized) {
        Resize(window);
        gameView.setViewport(FloatRect(scaledGameOffsetNormal.x, scaledGameOffsetNormal.y,
                                       scaledGameResolutionNormal.x, scaledGameResolutionNormal.y));
        window.setView(gameView);
      }

      // keyboard event handling
      if (e.type == Event::KeyPressed) {
        if (e.key.code == Keyboard::Escape) {
          window.close();
        } else if (e.key.code == Keyboard::Space) {
          FireBullet();
        } else if (e.key.code == Keyboard::B) {
          Resize(window);
        }
      }
      if (Keyboard::isKeyPressed(Keyboard::P)) {
        state = Gamestates::Pause;
      }
    }

    if (Keyboard::isKeyPressed(Keyboard::Space)) {
      FireBullet();
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

    // Joystick input
    if (Joystick::isConnected(0)) {
      const float joystickX = Joystick::getAxisPosition(0, Joystick::X);
      const float joystickY = Joystick::getAxisPosition(0, Joystick::Y);

      if (abs(joystickX) > 40.0f) {
        moveDirection += Vector2f((float)((signbit(joystickX) * -2) + 1), 0);
      }
      if (abs(joystickY) > 40.0f) {
        moveDirection += Vector2f(0, (float)((signbit(joystickY) * -2) + 1));
      }

      if (Joystick::isButtonPressed(0, 2) && state == Gamestates::Game) {
        state = Gamestates::Pause;
      }
      if (Joystick::isButtonPressed(0, 0)) {
        FireBullet();
      }
    }
    if (playerSprite->getPosition().x < 0) {
      moveDirection.x = max(0.0f, moveDirection.x);
    }
    if (playerSprite->getPosition().y < 0) {
      moveDirection.y = max(0.0f, moveDirection.y);
    }
    if (playerSprite->getPosition().x > (GAME_WORLD_X - playerSprite->getLocalBounds().width)) {
      moveDirection.x = min(0.0f, moveDirection.x);
    }
    if (playerSprite->getPosition().y > (GAME_WORLD_Y - playerSprite->getLocalBounds().height)) {
      moveDirection.y = min(0.0f, moveDirection.y);
    }
    Normalize(moveDirection);
    moveDirection = (moveDirection * playerMoveSpeed) * deltaSeconds;

    playerSprite->setPosition(playerSprite->getPosition() + moveDirection);
  } // End Input

  if (bulletsprite->getPosition().y > -128.0f) {
    bulletsprite->setPosition(bulletsprite->getPosition().x,
                              bulletsprite->getPosition().y - 1000.0f * deltaSeconds);
  }

  for (size_t i = 0; i < MAX_ENEMIES; i++) {
    if (i < currentEnemies) {
      // if not dead, move
      if (enemies[i]->getPosition().y < GAME_WORLD_X) {
        enemies[i]->setPosition(
            enemies[i]->getPosition() +
            Vector2f(sinf(runTime + i) * 100.0f * deltaSeconds, 50.0f * deltaSeconds));
        // collisions
        if (bulletsprite->getGlobalBounds().intersects(enemies[i]->getGlobalBounds())) {
          enemies[i]->setPosition(GetNewEnemyPos());
          score += 100;
        }
        if (playerSprite->getGlobalBounds().intersects(enemies[i]->getGlobalBounds())) {
          enemies[i]->setPosition(GetNewEnemyPos());
          playerlives -= 1;
        }
      } else {
        // enemy is offscreen, kill
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

  if (playerlives == 0) {
    // state = Gamestates::Dead;
  }
  ps->fuel(1, Vector2f(0, 0), Vector2f(0, GAME_WORLD_X));
  ps->update(deltaSeconds);
  scoreText->setString("Score =" + to_string(score + ceil(runTime)) + "\n\n"
                                                                      "lives = " +
                       to_string(playerlives));
}

void menuRender(RenderWindow &window) {
  window.clear(sf::Color::Black);
  titleText->setString("Insert game name here");
  playText->setString("Play");
  controlText->setString("controls");
  exitText->setString("exit");

  RectangleShape rectangle(Vector2f(0, 0));
  rectangle.setSize(Vector2f(GAME_RESOULUTION));
  rectangle.setTexture(textures[11]);
  titleText->setPosition(rectangle.getSize().x / 2.5f, 0);
  playText->setPosition(rectangle.getSize().x / 2.5f, 74);
  controlText->setPosition(rectangle.getSize().x / 2.5f, 148);
  exitText->setPosition(rectangle.getSize().x / 2.5f, 222);
  window.draw(rectangle);
  window.draw(*titleText);
  window.draw(*playText);
  window.draw(*controlText);
  if (state == Gamestates::Pause) {
    window.draw(*pausedText);
  }
  window.draw(*exitText);
  window.display();
}

void controlsRender(RenderWindow &window) {
  window.clear(sf::Color::Black);
  titleText->setString("Insert game name here");
  playText->setString("Play");
  exitText->setString("exit");
  RectangleShape rectangle(Vector2f(0, 0));
  rectangle.setSize(Vector2f(GAME_RESOULUTION));
  rectangle.setTexture(textures[11]);
  titleText->setPosition(rectangle.getSize().x / 2.5f, 0);
  window.draw(rectangle);
  window.draw(*titleText);
  window.draw(*playText);
  window.draw(*controlText);
  window.draw(*exitText);
  window.display();
}

void Render(RenderWindow &window) {
  window.clear(sf::Color::Black);
  RectangleShape rectangle(Vector2f(0, 0));
  rectangle.setSize(Vector2f(GAME_RESOULUTION));
  rectangle.setTexture(textures[11]);
  window.draw(rectangle);
  window.draw(*ps);
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
  RenderWindow *window =
      new RenderWindow(VideoMode(DEFAULT_WINDOW_RESOULUTION), "Advanced Minigame");
  window->setVerticalSyncEnabled(true);
  Resize(*window);

  // let's define  the game view
  gameView = View(FloatRect(0, 0, GAME_RESOULUTION));
  gameView.setViewport(FloatRect(scaledGameOffsetNormal.x, scaledGameOffsetNormal.y,
                                 scaledGameResolutionNormal.x, scaledGameResolutionNormal.y));

  // let's define the menu view
  menuView = View(FloatRect(0, 0, GAME_RESOULUTION));
  menuView.setViewport(FloatRect(scaledGameOffsetNormal.x, scaledGameOffsetNormal.y,
                                 scaledGameResolutionNormal.x, scaledGameResolutionNormal.y));

  // let's define the control layout view
  controlView = View(FloatRect(0, 0, GAME_RESOULUTION));
  controlView.setViewport(FloatRect(scaledGameOffsetNormal.x, scaledGameOffsetNormal.y,
                                    scaledGameResolutionNormal.x, scaledGameResolutionNormal.y));

  ps = new ParticleSystem(window->getSize());
  ps->setDissolutionRate(1);
  ps->setCanvasSize(Vector2u(GAME_RESOULUTION));
  previous = high_resolution_clock::now();

  while (window->isOpen()) {
    if (state == Gamestates::Start) {
      menuUpdate(*window);
      menuRender(*window);

    } else if (state == Gamestates::Game) {
      // Activate gameview
      Update(*window);
      Render(*window);
    } else if (state == Gamestates::Pause) {
      menuUpdate(*window);
      menuRender(*window);
    } else if (state == Gamestates::controls) {
      menuUpdate(*window);
      controlsRender(*window);

    } else if (state == Gamestates::Dead) {
      menuUpdate(*window);
      menuRender(*window);
    }
  }

  Unload();
  delete window;
  window = nullptr;
  return 0;
}