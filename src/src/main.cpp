#include "config.h"
#include "game.h"
#include "menu.h"
#include <SFML/Graphics.hpp>
#include <SFML/System/Vector2.hpp>
#include <chrono>
#include <iostream>
#include <thread>

using namespace chrono;

Vector2u scaledGameResolution;
Vector2f scaledGameResolutionNormal;
Vector2u scaledGameOffset;
Vector2f scaledGameOffsetNormal;
RenderWindow *window;
View gameView;
Gamestates state = Gamestates::Start;

Texture *textures[TEX_COUNT];
Font *gameFont;
Menu mainMenu;

static high_resolution_clock::time_point previous;

void Resize() {
  const Vector2f win = (Vector2f)window->getSize();
  float scale = 1.0f;
  float scale2 = 1.0f;
  const float GWX = (float)GAME_WORLD_X;
  const float GWY = (float)GAME_WORLD_Y;
  if (win.y > win.x) {
    // wide
    scale = win.x / GWX;
    if (win.y < (scale * GWY)) {
      scale2 = win.y / GWY;
    }
  } else {
    // tall (or square)
    scale = win.y / GWY;
    if (win.x < (scale * GWX)) {
      scale2 = win.x / GWX;
    }
  }
  scale = min(scale, scale2);
  scaledGameResolution =
      Vector2u((unsigned int)floor(scale * GWX), (unsigned int)floor(scale * GWY));
  scaledGameOffset = Vector2u((unsigned int)max(0.0f, (win.x - scaledGameResolution.x) * 0.5f),
                              (unsigned int)max(0.0f, (win.y - scaledGameResolution.y) * 0.5f));
  scaledGameResolutionNormal =
      Vector2f((float)scaledGameResolution.x / win.x, (float)scaledGameResolution.y / win.y);
  scaledGameOffsetNormal =
      Vector2f((float)scaledGameOffset.x / win.x, (float)scaledGameOffset.y / win.y);
}

void Loadcontent() {

  gameFont = new Font();
  gameFont->loadFromFile(filepath + "/fonts/AmericanCaptain.ttf");

  for (size_t i = 0; i < 12; i++) {
    textures[i] = new Texture();
    if (!textures[i]->loadFromFile(filepath + textureNames[i])) {
      throw invalid_argument("Loading error!");
    }
  }

  LoadGameContent();
}

void Unload() {
  UnLoadGameContent();
  for (auto &t : textures) {
    delete t;
    t = nullptr;
  }
  delete gameFont;
  gameFont = nullptr;
}

void Update() {

  high_resolution_clock::time_point now = high_resolution_clock::now();
  // time in nanoseconds since last update
  const auto delta = (duration_cast<duration<unsigned int, nano>>(now - previous)).count();
  const float deltaSeconds = ((float)delta) / 1000000000.0f;
  previous = now;

  Event e;
  while (window->pollEvent(e)) {
    switch (e.type) {
    case Event::Closed:
      window->close();
      break;
    case Event::Resized:
      Resize();
      gameView.setViewport(FloatRect(scaledGameOffsetNormal.x, scaledGameOffsetNormal.y,
                                     scaledGameResolutionNormal.x, scaledGameResolutionNormal.y));
      window->setView(gameView);
      break;
    case Event::KeyPressed:
      switch (e.key.code) {
      case Keyboard::Escape:
        window->close();
        break;
      case Keyboard::B:
        BREAKPOINT
        break;
      default:
        break;
      }
    default:
      break;
    }
    if (state == Gamestates::Game) {
      GameHandleEvent(e);
    } else if (state == Gamestates::Start) {
      // MenuHadleEvent(e);
    }
  }

  if (state == Gamestates::Game) {
    GameUpdate(deltaSeconds);
  } else if (state == Gamestates::Start) {
    mainMenu.Update();
  }
}

void Render() {
  window->clear(sf::Color::Black);
  if (state == Gamestates::Game) {
    GameRender();
  } else if (state == Gamestates::Start) {
    mainMenu.Render();
  }
  window->display();
}

int main() {

  Loadcontent();
  window = new RenderWindow(VideoMode(DEFAULT_WINDOW_RESOULUTION), "Advanced Minigame");
  window->setVerticalSyncEnabled(VSYNC);
  Resize();

  gameView = View(FloatRect(0, 0, GAME_RESOULUTION));
  gameView.setViewport(FloatRect(scaledGameOffsetNormal.x, scaledGameOffsetNormal.y,
                                 scaledGameResolutionNormal.x, scaledGameResolutionNormal.y));

  mainMenu = Menu();
  mainMenu.items.push_back(new MenuItem(GAME_NAME, *gameFont));
  mainMenu.items.push_back(
      new MenuButton("Start Game", *gameFont, []() { state = Gamestates::Game; }));
  mainMenu.items.push_back(
      new MenuButton("Controlls", *gameFont, []() { state = Gamestates::Game; }));
  mainMenu.items.push_back(
      new MenuButton("Credits", *gameFont, []() { state = Gamestates::Game; }));
  mainMenu.items.push_back(new MenuButton("Exit", *gameFont, []() { window->close(); }));

  previous = high_resolution_clock::now();

  while (window->isOpen()) {
    Update();
    Render();
  }

  Unload();
  delete window;
  window = nullptr;
  return 0;
}