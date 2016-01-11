#include <SFML/Graphics.hpp>

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

static sf::Sprite enemies[16];

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

  for (size_t i = 0; i < 16; i++) {
    enemies[i].setTexture(*textures[(i % 7) + 3]);
    enemies[i].setPosition(sf::Vector2f((1024 / 16) * i, (i % 4) * 20));
  }
}

void Update(sf::RenderWindow &window) {
  static float tick = 0.0f;
  tick += 0.01f;
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

  for (size_t i = 0; i < 16; i++) {
    enemies[i].setPosition(enemies[i].getPosition() +
                           sf::Vector2f(sinf(tick + i), 1));
  }
}

void Render(sf::RenderWindow &window) {
  window.clear();

  window.draw(playerSprite);

  window.draw(bulletsprite);

  for (size_t i = 0; i < 16; i++) {
    window.draw(enemies[i]);
  }

  window.display();
}

int main() {
  Loadcontent();
  sf::RenderWindow window(sf::VideoMode(1024, 768), "Main Window");
  window.setVerticalSyncEnabled(true);
  while (window.isOpen()) {
    Update(window);
    Render(window);
  }
  return 0;
}