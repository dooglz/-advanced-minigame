#include "config.h"
#include "enemy.h"
#include "weapon.h"
#include <SFML/Graphics.hpp>

using namespace chrono;
using namespace sf;
extern Texture *textures[TEX_COUNT];
extern RenderWindow *window;
extern Sprite *playerSprite;
extern vector<EnemyShip *> enemies;
extern unsigned score;

bool Weapon::CanFire() { return CanFire(cooldown_, reloadTime); }

bool Weapon::CanFire(chrono::high_resolution_clock::time_point tp, unsigned int r) {
  auto now = high_resolution_clock::now();
  auto d = (duration_cast<duration<unsigned int, milli>>(now - tp)).count();
  return d >= r;
}

void Weapon::Fire() {
  //
}

void Weapon::AltFire() { Fire(); }

Weapon::Weapon(float d, unsigned int r, float c) {
  cooldown_ = high_resolution_clock::now();
  damage = d;
  reloadTime = r;
  cost = c;
}

weps::Blaster::Blaster() : Weapon::Weapon(10, 800, 0) {
  for (auto &b : bulletsprites) {
    b = new Sprite();
    b->setTexture(*textures[bullet]);
    b->setPosition(0, -128.0f);
    b->setScale(2.0f, 2.0f);
  }
  for (auto &c : cooldowns) {
    c = high_resolution_clock::now();
  }
}
weps::Blaster::~Blaster() {
  for (auto &b : bulletsprites) {
    delete b;
    b = nullptr;
  }
}

void weps::Blaster::Update(float deltaSeconds) {
  for (auto &b : bulletsprites) {
    if (b->getPosition().y > -128.0f) {
      b->setPosition(b->getPosition().x, b->getPosition().y - 1000.0f * deltaSeconds);
      for (auto &e : enemies) {
        if (e->spr->getGlobalBounds().intersects(b->getGlobalBounds())) {
          e->alive = false;
          b->setPosition(0, -128.0f);
          score += 100;
        }
      }
    } else {
      b->setPosition(0, -128.0f);
    }
  }
}

void weps::Blaster::Render() {
  for (auto &b : bulletsprites) {
    window->draw(*b);
  }
}

void weps::Blaster::Fire() {
  for (size_t i = 0; i < 3; i++) {
    if (CanFire(cooldowns[i], reloadTime)) {
      bulletsprites[i]->setPosition(playerSprite->getPosition().x + 25,
                                    playerSprite->getPosition().y);
      cooldowns[i] = high_resolution_clock::now();
    }
  }
}

void weps::Blaster::AltFire() {
  if (!CanFire()) {
    return;
  }
  Weapon::AltFire();
}

weps::GatlingLaser::GatlingLaser() : Weapon::Weapon(10, 0, 0) {}

void weps::GatlingLaser::Update(float deltaSeconds) {}

void weps::GatlingLaser::Render() {}

void weps::GatlingLaser::Fire() { Weapon::Fire(); }

void weps::GatlingLaser::AltFire() { Weapon::Fire(); }

weps::BeamCannon::BeamCannon() : Weapon::Weapon(10, 0, 0) {}

void weps::BeamCannon::Update(float deltaSeconds) {}

void weps::BeamCannon::Render() {}

void weps::BeamCannon::Fire() { Weapon::Fire(); }

void weps::BeamCannon::AltFire() { Weapon::AltFire(); }
