#include <SFML/Graphics.hpp>

using namespace std;
void Loadcontent() {}

void Update() {}

void Render() {
  sf::RenderWindow window(sf::VideoMode(1024, 768), "Main Window");
  sf::CircleShape shape(100.f);
  shape.setFillColor(sf::Color::Green);
  while (window.isOpen()) {
    sf::Event event;
    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed)
        window.close();
    }

    window.clear();
    window.draw(shape);
    window.display();
  }
}

int main() {

  Loadcontent();
  Update();
  Render();

  return 0;
}