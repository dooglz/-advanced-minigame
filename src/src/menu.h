#pragma once
#include "config.h"
#include <SFML/Graphics.hpp>
#include <vector>

using namespace std;
using namespace sf;

typedef void (*callback)(void);

struct MenuItem {
  bool selectable;
  bool isSelected;
  MenuItem(string name, Font &gameFont);
  Text text;
  virtual void Update(Vector2f position);
  virtual void Render();
  virtual void Activate(){};
};

struct MenuButton : public MenuItem {
  MenuButton(string name, Font &gameFont, callback func);
  callback cb;
  void Update(Vector2f position);
  void Render();
  RectangleShape rectangle;
  void Activate();
};

struct Menu {
  ~Menu();
  vector<MenuItem *> items;
  void Update();
  void Render();
  void MenuHadleEvent(Event &e);
  int selected;
};
