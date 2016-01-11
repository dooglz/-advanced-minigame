#include <SFML/Graphics.hpp>

using namespace std;

static const string textureNames[]{
    "spaceship1.png",        "spaceship2.png",        "spaceship3.png",
    "Spaceship-Drakir1.png", "Spaceship-Drakir2.png", "Spaceship-Drakir3.png",
    "Spaceship-Drakir4.png", "Spaceship-Drakir5.png", "Spaceship-Drakir6.png",
    "Spaceship-Drakir7.png",
};

static sf::Texture *textures[10];

static const string filepath = "..\\res/img/";

static sf::Sprite enemies[16];

sf::Sprite playerSprite;
sf::Texture *playerTexture;

void Loadcontent() {

  for (size_t i = 0; i < 10; i++) {
    textures[i] = new sf::Texture();
    if (!textures[i]->loadFromFile(filepath + textureNames[i])) {
      throw invalid_argument("Ship not loaded!");
    }
  }
  playerSprite.setTexture(*textures[0]);
  playerSprite.setPosition(512, 256);

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
  }
  for (size_t i = 0; i < 16; i++) {
    enemies[i].setPosition(enemies[i].getPosition() +
                           sf::Vector2f(sinf(tick + i), 1));
  }
}

void Render(sf::RenderWindow &window) {
  window.clear();

  window.draw(playerSprite);
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