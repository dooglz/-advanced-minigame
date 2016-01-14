#include "menu.h"

Text *titleText;
Text *playText;
Text *controlText;
Text *exitText;

void LoadMenu(Font &gameFont) {
  titleText = new Text();
  titleText->setFont(gameFont);
  titleText->setCharacterSize(24);
  titleText->setColor(Color::White);
  playText = new Text();
  playText->setFont(gameFont);
  playText->setCharacterSize(24);
  playText->setColor(Color::White);
  controlText = new Text();
  controlText->setFont(gameFont);
  controlText->setCharacterSize(24);
  controlText->setColor(Color::White);
  exitText = new Text();
  exitText->setFont(gameFont);
  exitText->setCharacterSize(24);
  exitText->setColor(Color::White);
}

void RenderMainMenu(RenderWindow &window) {
  window.clear(sf::Color::Black);
  titleText->setString("Insert game name here");
  playText->setString("Play");
  controlText->setString("controls");
  exitText->setString("exit");

  window.draw(*titleText);
  window.draw(*playText);
  window.draw(*controlText);
  window.draw(*exitText);
  window.display();
}

void UpdateMainMenu(RenderWindow &window) {}

void Menu::Update(RenderWindow &window) {
  for (size_t i = 0; i < items.size(); i++) {
    items[i]->Update(Vector2f(GAME_WORLD_X / 3, (i + 1) * (GAME_WORLD_Y / (items.size() + 2))),
                     window);
  }
}

void Menu::Render(RenderWindow &window) {
  for (auto &i : items) {
    i->Render(window);
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

void MenuItem::Update(Vector2f position, RenderWindow &window) {
  const auto tw = text.getLocalBounds().width;
  text.setPosition(position + Vector2f(0.5 * ((GAME_WORLD_X / 3) - tw), 0));
}

MenuButton::MenuButton(string name, Font &gameFont, callback func) : MenuItem(name, gameFont) {
  cb = func;
}

void MenuButton::Update(Vector2f position, RenderWindow &window) {
  MenuItem::Update(position, window);
  rectangle.setPosition(position);
  rectangle.setSize(Vector2f(GAME_WORLD_X / 3, 100.0f));
  Vector2i mp = window.mapCoordsToPixel((Vector2f)Mouse::getPosition(window));
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

void MenuItem::Render(RenderWindow &window) { window.draw(text); }

void MenuButton::Render(RenderWindow &window) {
  window.draw(rectangle);
  window.draw(text);
}
