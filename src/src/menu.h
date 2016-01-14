#pragma once
#include "game.h"
#include <SFML/Graphics.hpp>
#include <vector>

using namespace std;
using namespace sf;

typedef void(*callback)(void);

void LoadMenu(Font &gameFont);
void RenderMainMenu(RenderWindow &window);
void UpdateMainMenu(RenderWindow &window);

struct MenuItem {
  MenuItem(string name, Font &gameFont);
  Text text;
  virtual void Update(Vector2f position, RenderWindow &window);
  virtual void Render(RenderWindow &window);
};

struct MenuButton : public MenuItem {
  MenuButton(string name, Font &gameFont, callback func);
  callback cb;
  void Update(Vector2f position, RenderWindow &window);
  void Render(RenderWindow &window);
  RectangleShape rectangle;
};

struct Menu {
  vector<MenuItem*> items;
  void Update(RenderWindow &window);
  void Render(RenderWindow &window);
};
