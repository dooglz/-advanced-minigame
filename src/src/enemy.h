#pragma once
#include "config.h"

#include <SFML/Graphics.hpp>
using namespace sf;

struct FlyBehavior {
  virtual Vector2f move(Vector2f currentPos) = 0;
};

namespace FlyBehaviors {
struct ZigZag : public FlyBehavior {
  bool dir = false;
  Vector2f move(Vector2f currentPos);
};
struct Standard : public FlyBehavior {
  Standard();
  float start;
  Vector2f move(Vector2f currentPos);
};
struct Line : public FlyBehavior {
  Vector2f move(Vector2f currentPos);
};
struct Homing : public FlyBehavior {
  Homing();
  float start;
  Vector2f move(Vector2f currentPos);
};
}

struct EnemyShip {
  bool alive;
  float speed;
  float health;
  float damageOnCollide;
  FlyBehavior *flyB;
  Sprite *spr;
  virtual void Update(float deltaSeconds);
  EnemyShip(float h, float d, float s, FlyBehavior *f, Texture *t);
  ~EnemyShip();
};

namespace Ships {
struct Frigate : public EnemyShip {
  Frigate();
};
struct Destroyer : public EnemyShip {
  Destroyer();
};
struct Cruiser : public EnemyShip {
  Cruiser();
};
struct BattleCruiser : public EnemyShip {
  BattleCruiser();
};
struct BattleShip : public EnemyShip {
  BattleShip();
};
}
