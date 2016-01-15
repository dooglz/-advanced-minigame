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

void SpawnPower();
