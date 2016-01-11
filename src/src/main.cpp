#include <SFML/Graphics.hpp>
#include <iostream>
#include <chrono>
#include <thread>
using namespace std;

static const string textureNames[]{"spaceship1.png",
                                   "spaceship2.png",
                                   "spaceship3.png",
                                   "Spaceship-Drakir1.png",
                                   "Spaceship-Drakir2.png",
                                   "Spaceship-Drakir3.png",
                                   "Spaceship-Drakir4.png",
                                   "Spaceship-Drakir5.png",
                                   "Spaceship-Drakir6.png",
                                   "Spaceship-Drakir7.png",
                                   "bullet.png"};

static sf::Texture *textures[11];

static const string filepath = "..\\res/img/";

#define MAX_ENEMIES 255
static unsigned int currentEnemies = 0;
static sf::Sprite enemies[MAX_ENEMIES];

static sf::Vector2f GetNewEnemyPos() {
  return sf::Vector2f(rand() % 1024, -128.0f);
}

sf::Sprite playerSprite;
sf::Texture *playerTexture;
sf::Sprite bulletsprite;
sf::Texture *bulletTexture;
void Loadcontent() {

  for (size_t i = 0; i < 11; i++) {
    textures[i] = new sf::Texture();
    if (!textures[i]->loadFromFile(filepath + textureNames[i])) {
      throw invalid_argument("Loading error!");
    }
  }

  playerSprite.setTexture(*textures[0]);
  playerSprite.setPosition(512, 256);
  bulletsprite.setTexture(*textures[10]);

  for (size_t i = 0; i < MAX_ENEMIES; i++) {
    enemies[i].setTexture(*textures[(i % 7) + 3]);
    enemies[i].setPosition(GetNewEnemyPos());
  }
}

static chrono::high_resolution_clock::time_point previous;
static unsigned int score;

void Update(sf::RenderWindow &window) {

  chrono::high_resolution_clock::time_point now =
      chrono::high_resolution_clock::now();
  const unsigned int delta =
      (std::chrono::duration_cast<std::chrono::duration<int, std::nano>>(
           now - previous))
          .count();

  const double deltaPercent =
      (((double)delta) / 1000000000.0); // delta as a percentage of 1 second
  previous = now;

  static float tick = 0.0f;
  tick += deltaPercent;
  score = (unsigned int)ceil(tick);
  currentEnemies = (unsigned int)ceil(tick * 0.6f) + 1;

  cout << score << " - " << currentEnemies << " - " << delta << " - "
       << deltaPercent << endl;

  sf::Event e;

  while (window.pollEvent(e)) {
    if (e.type == sf::Event::Closed)
      window.close();

    // keyboard event handling
    if (e.type == sf::Event::KeyPressed) {
      if (e.key.code == sf::Keyboard::Escape) {
        window.close();
      }

      if (e.key.code == sf::Keyboard::W) {
        playerSprite.setPosition(playerSprite.getPosition().x,
                                 playerSprite.getPosition().y - tick);
      }

      if (e.key.code == sf::Keyboard::S) {
        playerSprite.setPosition(playerSprite.getPosition().x,
                                 playerSprite.getPosition().y + tick);
      }
      if (e.key.code == sf::Keyboard::A) {
        playerSprite.setPosition(playerSprite.getPosition().x - tick,
                                 playerSprite.getPosition().y);
      }
      if (e.key.code == sf::Keyboard::D) {
        playerSprite.setPosition(playerSprite.getPosition().x + tick,
                                 playerSprite.getPosition().y);
      }
    }
    // joystick input
    if (sf::Joystick::isConnected(0)) {
      float joystickX = sf::Joystick::getAxisPosition(0, sf::Joystick::X);
      float joystickY = sf::Joystick::getAxisPosition(0, sf::Joystick::Y);

      playerSprite.setPosition(playerSprite.getPosition().x + (joystickX * 0.1),
                               playerSprite.getPosition().y +
                                   (joystickY * 0.1));

      // if the B button is pressed fire a bullet
      if (e.JoystickButtonPressed) {
        if (e.joystickButton.button == 0) {
          bulletsprite.setPosition(playerSprite.getPosition().x + 30,
                                   playerSprite.getPosition().y - 1);
        }
      }

      bulletsprite.setPosition(bulletsprite.getPosition().x,
                               bulletsprite.getPosition().y - tick);
    }
  }

  for (size_t i = 0; i < MAX_ENEMIES; i++) {
    if (i < currentEnemies) {
      // if not dead, move
      if (enemies[i].getPosition().y < 700.0) {
        enemies[i].setPosition(
            enemies[i].getPosition() +
            sf::Vector2f(sinf(tick + i) * 100.0 * deltaPercent,
                         100.0 * deltaPercent));
		//collisions
		if (bulletsprite.getGlobalBounds().intersects(enemies[i].getGlobalBounds())){
			enemies[i].setPosition(GetNewEnemyPos());
			score += 100;
		}
      } 
	  else {
        // ofscreen kill
        enemies[i].setPosition(GetNewEnemyPos());
      }
    } else {
      // if alive
      if (enemies[i].getPosition().y != -128.0f) {
        // kill
        enemies[i].setPosition(GetNewEnemyPos());
      }
    }
  }
}

void Render(sf::RenderWindow &window) {
  window.clear();

  window.draw(playerSprite);
  window.draw(bulletsprite);
  for (size_t i = 0; i < MAX_ENEMIES; i++) {
    window.draw(enemies[i]);
  }

  window.display();
}

int main() {
  Loadcontent();
  sf::RenderWindow window(sf::VideoMode(1024, 768), "Main Window");
  window.setVerticalSyncEnabled(true);
  previous = chrono::high_resolution_clock::now();
  while (window.isOpen()) {
    Update(window);
    Render(window);
  }
  return 0;
}