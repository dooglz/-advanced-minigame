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

void Menu::MenuHadleEvent(Event &e) {
  int dir = 0;
  if (e.type == Event::KeyPressed && e.key.code != Keyboard::Space ||
      e.type == Event::JoystickMoved && e.joystickMove.axis == 7) {
    if (e.key.code == Keyboard::W || e.key.code == Keyboard::Up ||
        e.joystickMove.position > 50.0f) {
      dir = -1;
    } else if (e.key.code == Keyboard::S || e.key.code == Keyboard::Down ||
               e.joystickMove.position < -50.0f) {
      dir = 1;
    }

    if (dir != 0) {
      vector<MenuItem *> si;
      for (auto &i : items) {
        if (i->selectable) {
          si.push_back(i);
          i->isSelected = false;
        }
      }
      if (si.size() == 0) {
        selected = 0;
      } else {
        selected += dir;
        if (selected < 0) {
          selected = si.size() - 1;
        } else if (selected > si.size() - 1) {
          selected = 0;
        }
        si[selected]->isSelected = true;
      }
    }
  } else if (e.type == Event::MouseMoved) {
    // is it over a button?
    Vector2i mp = window->mapCoordsToPixel((Vector2f)Mouse::getPosition(*window));
    for (size_t i = 0; i < items.size(); i++) {
      items[i]->isSelected = false;
      Vector2f position = Vector2f(GAME_WORLD_X / 3, (i + 1) * (GAME_WORLD_Y / (items.size() + 2)));
      if (mp.x > position.x && mp.y > position.y && mp.x < position.x + GAME_WORLD_X / 3 &&
          mp.y < position.y + 100.0f) {
        // yes
        if (items[i]->selectable) {
          items[i]->isSelected = true;
        }
        break;
      }
    }
  } else if ((e.type == Event::MouseButtonPressed && e.mouseButton.button == Mouse::Left) ||
             (e.type == Event::KeyPressed && e.key.code == Keyboard::Space) ||
             (e.type == Event::JoystickButtonPressed && e.joystickButton.button == 0)) {
    for (auto &i : items) {
      if (i->isSelected) {
        i->Activate();
      }
    }
  }
}

MenuItem::MenuItem(string name, Font &gameFont) {
  text = Text();
  selectable = false;
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
  selectable = true;
  isSelected = false;
}

void MenuButton::Update(Vector2f position) {
  MenuItem::Update(position);
  rectangle.setPosition(position);
  rectangle.setSize(Vector2f(GAME_WORLD_X / 3, 100.0f));

  if (isSelected) {
    rectangle.setFillColor(Color::Green);
  } else {
    rectangle.setFillColor(Color::Magenta);
  }
}

void MenuItem::Render() { window->draw(text); }

void MenuButton::Render() {

  window->draw(rectangle);
  window->draw(text);
}

void MenuButton::Activate() { cb(); }
