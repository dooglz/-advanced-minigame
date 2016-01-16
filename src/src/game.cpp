#include "config.h"
#include "enemy.h"
#include "enemy.h"
#include "game.h"
#include "particles.h"
#include "weapon.h"
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <iostream>
#include <stack>

Sprite *playerSprite;
Sprite *backgroundSprite;

Sprite *powersprite;
Sprite *ExplosionSprite;

Sound *fire;
Sound *explosion;
Sound *laser;
SoundBuffer *fireBuffer;
SoundBuffer *explosionBuffer;
SoundBuffer *laserBuffer;

Text *scoreText;
Text *pausedText;
ParticleSystem *ps;
bool shielded;
sf::CircleShape *shield;
int powertime = 1000;

extern RenderWindow *window;
extern Gamestates state;
extern Font *gameFont;
extern Texture *textures[TEX_COUNT];

Weapon *playerWeapon;

int playerlives = 3;
stack<Sprite *> unusedSprites;
vector<EnemyShip *> enemies;
int PowerChance = 0;
static unsigned int currentEnemies = 0;
float playerMoveSpeed = 600.0f;
unsigned score = 0;
float runTime = 0; // time in seconds that the game has been running

void LoadGameContent() {
  scoreText = new Text();
  scoreText->setFont(*gameFont);
  scoreText->setPosition(0, 0);
  scoreText->setCharacterSize(24);
  scoreText->setColor(Color::Red);

  fire = new Sound();
  explosion = new Sound();
  laser = new Sound();
  shield = new CircleShape(20, 120);

  fireBuffer = new SoundBuffer();
  explosionBuffer = new SoundBuffer();
  laserBuffer = new SoundBuffer();

  fireBuffer->loadFromFile(filepath + soundNames[MISSILE]);
  explosionBuffer->loadFromFile(filepath + soundNames[EXPLOSION]);
  laserBuffer->loadFromFile(filepath + soundNames[LASER]);

  fire->setBuffer(*fireBuffer);
  explosion->setBuffer(*explosionBuffer);
  laser->setBuffer(*laserBuffer);

  fire->setMinDistance(0.3f);
  playerSprite = new Sprite();
  playerSprite->setTexture(*textures[0]);
  playerSprite->setPosition(512, 256);

  ExplosionSprite = new Sprite();
  ExplosionSprite->setTexture(*textures[15]);

  powersprite = new Sprite();
  powersprite->setScale(2.0f, 2.0f);

  backgroundSprite = new Sprite();
  backgroundSprite->setTexture(*textures[11]);
  backgroundSprite->setPosition(GAME_WORLD_X, GAME_WORLD_Y);
  auto c = backgroundSprite->getColor();
  c.a = 100;
  backgroundSprite->setColor(c);

  for (size_t i = 0; i < MAX_ENEMIES; i++) {
    Sprite *s = new Sprite();
    s->setPosition(0, -200.0f);
    unusedSprites.push(s);
  }

  playerWeapon = new weps::Blaster();

  ps = new ParticleSystem(Vector2u(GAME_RESOULUTION));
  ps->setDissolutionRate(1);
  // prefuel
  ps->fuel(250, Vector2f(0, GAME_WORLD_Y), Vector2f(0, GAME_WORLD_X));
}
void UnLoadGameContent() {
  scoreText = new Text();
  scoreText->setFont(*gameFont);
  scoreText->setPosition(0, 0);
  scoreText->setCharacterSize(24);
  scoreText->setColor(Color::Red);
  for (auto &e : enemies) {
    delete e;
    e = nullptr;
  }
}

void GameHandleEvent(Event &e) {
  if (e.type == Event::KeyPressed) {
    if (e.key.code == Keyboard::P) {
      state = Pause;
    } else if (e.key.code == Keyboard::Escape) {
      state = Start;
    }
  }
  if (e.type == Event::JoystickButtonPressed) {
    if (e.joystickButton.button == 7) {
      state = Start;
    }
    if (e.joystickButton.button == 2 && state == Game) {
      state = Pause;
    }
  }
}

void Normalize(Vector2f &v) {
  float length = sqrt(v.x * v.x + v.y * v.y);
  if (length == 0.0f) {
    return;
  }
  v.x /= length;
  v.y /= length;
}

void GameUpdate(float deltaSeconds) {
  runTime += deltaSeconds;
  currentEnemies = min((int)ceil(runTime * 0.6f) + 1, MAX_ENEMIES);

  if (currentEnemies > enemies.size()) {
    const auto r = rand() % 100;
    if (r < 30) {
      enemies.push_back(new Ships::Frigate());
    } else if (r < 40) {
      enemies.push_back(new Ships::Destroyer());
    } else if (r < 50) {
      enemies.push_back(new Ships::Cruiser());
    } else if (r < 60) {
      enemies.push_back(new Ships::BattleCruiser());
    } else if (r < 70) {
      enemies.push_back(new Ships::BattleShip());
    } else {
      enemies.push_back(new Ships::Frigate());
    }
  }

  if (Keyboard::isKeyPressed(Keyboard::Space)) {

    playerWeapon->Fire();
    fire->play();
  }

  Vector2f moveDirection(0, 0);
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

    if (Joystick::isButtonPressed(0, 0)) {
      playerWeapon->Fire();
      fire->play();
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

  int lifespan = 1000;

  if (playerSprite->getGlobalBounds().intersects(powersprite->getGlobalBounds())) {
    if (powersprite->getTexture() == textures[12]) {
      playerlives += 1;
      powersprite->setPosition(670, -128);
    } else if (powersprite->getTexture() == textures[13]) {
      shielded = true;
      powersprite->setPosition(670, -128);
    } else if (powersprite->getTexture() == textures[14]) {

      while (powertime >= 0) {
        playerWeapon->reloadTime = 0;
        powertime--;
        break;
      }

      powersprite->setPosition(670, -128);
    }
  }

  for (size_t i = 0; i < enemies.size(); i++) {
    auto e = enemies[i];
    e->Update(deltaSeconds);
    if (!e->alive) {
      PowerChance = rand() % 101;
      if (PowerChance >= 30) {
        while (lifespan >= 0) {
          powersprite->setPosition(e->spr->getPosition().x, e->spr->getPosition().y + 1);
          powersprite->setTexture(*textures[13]);
          lifespan--;
          break;
        }
      }
      if (PowerChance >= 60) {
        while (lifespan >= 0) {
          powersprite->setPosition(e->spr->getPosition().x, e->spr->getPosition().y + 1);
          powersprite->setTexture(*textures[14]);
          lifespan--;
          break;
        }
      }
      if (PowerChance >= 90) {
        while (lifespan >= 0) {
          powersprite->setPosition(e->spr->getPosition().x, e->spr->getPosition().y + 1);
          powersprite->setTexture(*textures[12]);
          lifespan--;
          break;
        }
      }

      lifespan = 1000;
      delete e;
      enemies[i] = nullptr;
      enemies.erase(enemies.begin() + i);
      --i;
    }
  }

  if (powertime == 0) {
    powertime = 1000;
  }
  if (playerlives == 0) {
    state = Gamestates::Credits;
  }
  static int pcount = 0;
  ++pcount;
  if (pcount % 5 == 0) {
    ps->fuel(1, Vector2f(0, 0), Vector2f(0, GAME_WORLD_X));
    pcount = 0;
    ps->fuel(1, Vector2f(0, 0), Vector2f(0, GAME_WORLD_X));
  }
  playerWeapon->Update(deltaSeconds);
  ps->update(deltaSeconds);
  scoreText->setString("Score =" + to_string(score + ceil(runTime)) + "\n\n"
                                                                      "lives = " +
                       to_string(playerlives));
}

void GameRender() {
  window->draw(*ps);
  playerWeapon->Render();

  if (shielded == true) {
    shield->setPosition(playerSprite->getPosition().x + 12, playerSprite->getPosition().y + 10);
    shield->setFillColor(Color::Transparent);
    shield->setOutlineThickness(5);
    shield->setOutlineColor(Color::Blue);
  }
  window->draw(*playerSprite);

  for (auto &e : enemies) {
    window->draw(*e->spr);
    if (e->alive == false) {
      ExplosionSprite->setPosition(e->spr->getPosition().x, e->spr->getPosition().y);
      window->draw(*ExplosionSprite);
      explosion->play();
    }
  }

  if (PowerChance >= 30) {
    window->draw(*powersprite);
  }
  window->draw(*powersprite);
  window->draw(*scoreText);
}

void ResetGame() {
  score = 0;
  runTime = 0;
  currentEnemies = 0;
  playerSprite->setPosition(512, 256);
  for (auto e : enemies) {
    delete e;
  }
  enemies.clear();
}
