#pragma once
#include "config.h"

#include <SFML/Graphics.hpp>
using namespace sf;


typedef Vector2f (*FlyBehavior)(Vector2f currentPos);
namespace FlyBehaviors {
static Vector2f ZigZag(Vector2f currentPos);
static Vector2f Standard(Vector2f currentPos);
static Vector2f Line(Vector2f currentPos);
static Vector2f Homing(Vector2f currentPos);
}

struct EnemyShip {
  bool alive;
  float health;
  float damageOnCollide;
  FlyBehavior flyB;
  Sprite* spr;
  virtual void Update(float deltaSeconds);
  EnemyShip(float h, float d, FlyBehavior f, Texture* t);
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
