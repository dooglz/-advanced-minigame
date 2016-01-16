#include "menu.h"

extern RenderWindow *window;
extern int playerlives;

void Menu::Update() {
  if (playerlives != 3) {
    playerlives = 3;
  }
  for (size_t i = 0; i < items.size(); i++) {
    items[i]->Update(Vector2f(GAME_WORLD_X / 3, (i + 1) * (GAME_WORLD_Y / (items.size() + 2))));
  }
}

void Menu::Render() {
  for (auto &i : items) {
    i->Render();
  }
}

MenuItem::MenuItem(string name, Font &gameFont) {
  text = Text();
  text.setFont(gameFont);
  text.setCharacterSize(24);
  text.setColor(Color::White);
  text.setString(name);
}

Menu::~Menu() {
  for (auto &i : items) {
    delete i;
    i = nullptr;
  }
  items.clear();
}

void MenuItem::Update(Vector2f position) {
  const auto tw = text.getLocalBounds().width;
  text.setPosition(position + Vector2f(0.5 * ((GAME_WORLD_X / 3) - tw), 0));
}

MenuButton::MenuButton(string name, Font &gameFont, callback func) : MenuItem(name, gameFont) {
  cb = func;
}

void MenuButton::Update(Vector2f position) {
  MenuItem::Update(position);
  rectangle.setPosition(position);
  rectangle.setSize(Vector2f(GAME_WORLD_X / 3, 100.0f));
  Vector2i mp = window->mapCoordsToPixel((Vector2f)Mouse::getPosition(*window));
  if (mp.x > position.x && mp.y > position.y && mp.x < position.x + GAME_WORLD_X / 3 &&
      mp.y < position.y + 100.0f) {
    rectangle.setFillColor(Color::Red);
    if (Mouse::isButtonPressed(Mouse::Left)) {
      cb();
    }
  } else {
    rectangle.setFillColor(Color::Magenta);
  }
}

void MenuItem::Render() { window->draw(text); }

void MenuButton::Render() {
  window->draw(rectangle);
  window->draw(text);
}
