#include "enemy.h"
#include <stack>
extern stack<Sprite *> unusedSprites;
extern Texture *textures[TEX_COUNT];
extern Sprite *playerSprite;
extern Sprite *bulletsprite;
static Vector2f GetNewEnemyPos() { return Vector2f((float)(rand() % GAME_WORLD_Y), -128.0f); }

Vector2f FlyBehaviors::ZigZag(Vector2f currentPos) { return Vector2f(0,1.0f); }
Vector2f FlyBehaviors::Standard(Vector2f currentPos) { return Vector2f(0, 1.0f); }
Vector2f FlyBehaviors::Line(Vector2f currentPos) { return Vector2f(0, 1.0f); }
Vector2f FlyBehaviors::Homing(Vector2f currentPos) { return Vector2f(0, 1.0f); }

void EnemyShip::Update(float deltaSeconds) {
  spr->setPosition(spr->getPosition() + flyB(spr->getPosition()) * deltaSeconds * 50.0f);
  if (spr->getPosition().y > GAME_WORLD_X) {
	  alive = false;
  }
  else if (spr->getGlobalBounds().intersects(playerSprite->getGlobalBounds())) {
	  alive = false;
  }
  else if (spr->getGlobalBounds().intersects(bulletsprite->getGlobalBounds())) {
	  alive = false;
  }
}


EnemyShip::EnemyShip(float h, float d, FlyBehavior f, Texture *t) {
  flyB = f;
  damageOnCollide = d;
  health = h;
  spr = unusedSprites.top();
  unusedSprites.pop();
  spr->setTexture(*t);
  spr->setPosition(GetNewEnemyPos());
 // spr->setColor(Color::Color());
  spr->setRotation(0);
  alive = true;
}

EnemyShip::~EnemyShip()
{
	alive = false;
	unusedSprites.push(spr);
	spr = nullptr;
}

namespace Ships {
Frigate::Frigate() : EnemyShip(1.0f, 10.0f, FlyBehaviors::Standard, textures[enemy1]) {}
Destroyer::Destroyer() : EnemyShip(1.0f, 10.0f, FlyBehaviors::Standard, textures[enemy2]) {}
BattleCruiser::BattleCruiser() : EnemyShip(1.0f, 10.0f, FlyBehaviors::Standard, textures[enemy3]) {}
Cruiser::Cruiser() : EnemyShip(1.0f, 10.0f, FlyBehaviors::Standard, textures[enemy4]) {}
BattleShip::BattleShip() : EnemyShip(1.0f, 10.0f, FlyBehaviors::Standard, textures[enemy5]) {}
}
