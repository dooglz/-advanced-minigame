#pragma once
#include "config.h"
#include <SFML/Graphics.hpp>
#include <chrono>

using namespace sf;

struct Weapon {
  float damage;
  unsigned int reloadTime;
  float cost;
  chrono::high_resolution_clock::time_point cooldown_;
  bool CanFire();
  bool CanFire(chrono::high_resolution_clock::time_point tp, unsigned int r);
  virtual void Fire() = 0;
  virtual void Update(float deltaSeconds) = 0;
  virtual void Render() = 0;
  virtual void AltFire() = 0;
  Weapon(float d, unsigned int r, float c);
};

namespace weps {

struct Blaster : public Weapon {
  unsigned int bcount = 0;
  Sprite *bulletsprites[3];
  chrono::high_resolution_clock::time_point cooldowns[3];
  Blaster();
  ~Blaster();
  void Update(float deltaSeconds);
  void Render();
  void Fire();
  void AltFire();
};

struct GatlingLaser : public Weapon {
  GatlingLaser();
  void Update(float deltaSeconds);
  void Render();
  void Fire();
  void AltFire();
};

struct BeamCannon : public Weapon {
  BeamCannon();
  void Update(float deltaSeconds);
  void Render();
  void Fire();
  void AltFire();
};
}
