#pragma once

#include <SFML/Graphics.hpp>
using namespace sf;

enum Gamestates { Start, Game, Pause, Dead, controls };

void GameUpdate(float delta);
void GameHandleEvent(Event &e);
void GameRender(RenderWindow &window);

void ResetGame();

void LoadGameContent();
void UnLoadGameContent();