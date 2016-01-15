#pragma once

#include <SFML/Graphics.hpp>
using namespace sf;

enum Gamestates { Start, Game, Pause, Credits, Controls };

void GameUpdate(float delta);
void GameHandleEvent(Event &e);
void GameRender();

void ResetGame();

void LoadGameContent();
void UnLoadGameContent();


struct FlyBehavior {
	Vector2f move(Vector2f& currentPos);
};

struct EnemyShip {
	float health;
	float damageOnCollide;
	FlyBehavior* flyB;
};