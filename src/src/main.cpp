#include "config.h"
#include "game.h"
#include "menu.h"
#include <SFML/Graphics.hpp>
#include <SFML/System/Vector2.hpp>
#include <chrono>
#include <iostream>
#include <thread>
#include <fstream>

using namespace chrono;
string filepath;
Vector2u scaledGameResolution;
Vector2f scaledGameResolutionNormal;
Vector2u scaledGameOffset;
Vector2f scaledGameOffsetNormal;
RenderWindow *window;
View gameView;
Gamestates state = Gamestates::Start;

Texture *textures[TEX_COUNT];
Font *gameFont;
Text pausedText;
Menu mainMenu;
Menu creditsMenu;
Menu controlsMenu;
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
  pausedText.setFont(*gameFont);
  pausedText.setPosition(GAME_WORLD_X / 2, GAME_WORLD_Y / 2);
  pausedText.setCharacterSize(64);
  pausedText.setColor(Color::White);
  pausedText.setString("PAUSED");
  for (size_t i = 0; i < 16; i++) {
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

    switch (state) {
    case Game:
      GameHandleEvent(e);
      break;
    case Start:
      // MenuHadleEvent(e);
      break;
    case Controls:
      break;
    case Credits:
      break;
    case Pause:
      if (e.type == Event::KeyPressed && e.key.code == Keyboard::P) {
        state = Game;
        return;
      }
      break;
    default:
      break;
    }
  }

  switch (state) {
  case Game:
    GameUpdate(deltaSeconds);
    break;
  case Start:
    mainMenu.Update();
    break;
  case Controls:
    controlsMenu.Update();
    break;
  case Credits:
    creditsMenu.Update();
    break;
  default:
    break;
  }
}

void Render() {
  window->clear(sf::Color::Black);

  switch (state) {
  case Game:
    GameRender();
    break;
  case Start:
    mainMenu.Render();
    break;
  case Controls:
    controlsMenu.Render();
    break;
  case Credits:
    creditsMenu.Render();
    break;
  case Pause:
    GameRender();
    window->draw(pausedText);
    break;

  default:
    break;
  }

  window->display();
}

int main() {
  // fid res dir
  static const std::string filedirs[] = {"", "../", "res/", "../res/"};
  for (const auto s : filedirs) {
    ifstream inFile((s + textureNames[0]).c_str(), ifstream::in);
    if (inFile.good()) {
      filepath = s;
      inFile.close();
      break;
    }
  }
  Loadcontent();
  window = new RenderWindow(VideoMode(DEFAULT_WINDOW_RESOULUTION), "Advanced Minigame");
  window->setVerticalSyncEnabled(VSYNC);
  Resize();

  gameView = View(FloatRect(0, 0, GAME_RESOULUTION));
  gameView.setViewport(FloatRect(scaledGameOffsetNormal.x, scaledGameOffsetNormal.y,
                                 scaledGameResolutionNormal.x, scaledGameResolutionNormal.y));

  mainMenu = Menu();
  mainMenu.items.push_back(new MenuItem(GAME_NAME, *gameFont));
  mainMenu.items.push_back(new MenuButton("Start Game", *gameFont, []() {
    ResetGame();
    state = Game;
  }));
  mainMenu.items.push_back(new MenuButton("Controls", *gameFont, []() { state = Controls; }));
  mainMenu.items.push_back(new MenuButton("Credits", *gameFont, []() { state = Credits; }));
  mainMenu.items.push_back(new MenuButton("Exit", *gameFont, []() { window->close(); }));
  controlsMenu = Menu();
  controlsMenu.items.push_back(new MenuItem("Controls", *gameFont));
  controlsMenu.items.push_back(
      new MenuItem("WASD / arrow keys / Left Thumbstick\t\tMove ship", *gameFont));
  controlsMenu.items.push_back(new MenuItem("Space Bar/ A Button\t\tFire Missile", *gameFont));
  controlsMenu.items.push_back(new MenuItem(" P/ X button\t\tPause Game", *gameFont));
  controlsMenu.items.push_back(new MenuItem(" Escape/ B button\t\tExit Game", *gameFont));
  controlsMenu.items.push_back(new MenuItem("", *gameFont));
  controlsMenu.items.push_back(new MenuItem("", *gameFont));
  controlsMenu.items.push_back(new MenuItem("", *gameFont));
  controlsMenu.items.push_back(new MenuButton("Back", *gameFont, []() { state = Start; }));
  creditsMenu = Menu();
  creditsMenu.items.push_back(new MenuItem("Credits", *gameFont));
  creditsMenu.items.push_back(new MenuItem("Neil Notman", *gameFont));
  creditsMenu.items.push_back(new MenuItem("Sam Serrels", *gameFont));
  creditsMenu.items.push_back(new MenuItem("", *gameFont));
  creditsMenu.items.push_back(new MenuItem("", *gameFont));
  creditsMenu.items.push_back(new MenuItem("", *gameFont));
  creditsMenu.items.push_back(new MenuItem("", *gameFont));
  creditsMenu.items.push_back(new MenuItem("", *gameFont));
  creditsMenu.items.push_back(new MenuItem("", *gameFont));
  creditsMenu.items.push_back(new MenuButton("Back", *gameFont, []() { state = Start; }));

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