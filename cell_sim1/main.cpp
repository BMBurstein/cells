#include <SFML/Graphics.hpp>

#include <unordered_map>
#include <functional>
#include <string>
#include <fstream>

struct PointHash {
  std::size_t operator()(sf::Vector2i const& v) const {
    return std::hash<long long>{}(((long long)v.x << 32) ^ v.y);
  }
};

class CellWorld : public sf::Drawable, public sf::Transformable
{
private:

  struct Cell {
    bool alive = false;
    operator sf::Color() { return alive ? sf::Color::Black : sf::Color::White; }
  };

public:
  CellWorld() : vertices(sf::Quads) {
    //addCell(0, 0);
    //addCell(4, 0);
    //addCell(5, 0);
    //addCell(6, 0);
    //addCell(0, 1);
    //addCell(1, 1);
    //addCell(2, 1);
    //addCell(5, 1);
    //addCell(1, 2);
    //finalize();
  }

  void update() {
    newCells.clear();
    for (auto& c : cells) {
      auto x = c.first.x;
      auto y = c.first.y;
      auto adjacent = cell(x - 1, y - 1).alive + cell(x, y - 1).alive + cell(x + 1, y - 1).alive
                    + cell(x - 1, y    ).alive +           0          + cell(x + 1, y    ).alive
                    + cell(x - 1, y + 1).alive + cell(x, y + 1).alive + cell(x + 1, y + 1).alive;
      if (c.second.alive) {
        if (adjacent == 3 || adjacent == 2) {
          newCells[{x, y}].alive = true;
        }
        else {
          touchNeighbors(x, y);
        }
      }
      else {
        if (adjacent == 3) {
          newCells[{x, y}].alive = true;
          touchNeighbors(x, y);
        }
      }
    }
    finalize();
  }

  void finalize() {
    cells.swap(newCells);

    vertices.clear();
    for (auto& c : cells) {
      if (c.second.alive) {
        vertices.append({ { float(c.first.x    ), float(c.first.y    ) }, c.second });
        vertices.append({ { float(c.first.x + 1), float(c.first.y    ) }, c.second });
        vertices.append({ { float(c.first.x + 1), float(c.first.y + 1) }, c.second });
        vertices.append({ { float(c.first.x    ), float(c.first.y + 1) }, c.second });
      }
    }
  }

  Cell cell(int x, int y) const {
    auto it = cells.find({ x, y });
    if (it == cells.end()) {
      return Cell();
    }
    return it->second;
  }

  void addCell(int x, int y) {
    newCells[{x, y}].alive = true;
    touchNeighbors(x, y);
  }

  void touchNeighbors(int x, int y) {
    newCells[{x - 1, y - 1}];
    newCells[{x - 1, y    }];
    newCells[{x - 1, y + 1}];
    newCells[{x    , y - 1}];
    newCells[{x    , y + 1}];
    newCells[{x + 1, y - 1}];
    newCells[{x + 1, y    }];
    newCells[{x + 1, y + 1}];
  }

  bool load(std::string path) {
    std::ifstream file(path);
    std::string line;

    std::getline(file, line);
    if (line == "#Life 1.06") {
      while (file) {
        int x, y;
        file >> x >> y;
        addCell(x, y);
      }
    }
    else {
      return false;
    }
    finalize();
    return  true;
  }

protected:

  virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const {
    // apply the transform
    states.transform *= getTransform();
    
    states.texture = NULL;

    // draw the vertex array
    target.clear(sf::Color::White);
    target.draw(vertices, states);
  }

private:
  typedef std::unordered_map<sf::Vector2i, Cell, PointHash> CellsMap;

  sf::VertexArray vertices;
  CellsMap cells;
  CellsMap newCells;
};

static const unsigned int WIDTH = 800;
static const unsigned int HEIGHT = 600;

int main() {
  sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "Game of Life v0.3");

  CellWorld cells;
  cells.setPosition(WIDTH / 2, HEIGHT / 2);
  cells.load("max_106.lif");

  sf::View view;
  bool dragging = false;
  sf::Vector2i dragPos;

  // run the main loop
  while (window.isOpen()) {
    // handle events
    sf::Event event;
    while (window.pollEvent(event)) {
      switch (event.type) {
      case sf::Event::Closed:
        window.close();
        break;
      case sf::Event::MouseWheelScrolled:
        if (!dragging) {
          sf::Vector2i pixel(event.mouseWheelScroll.x, event.mouseWheelScroll.y);
          auto coords = window.mapPixelToCoords(pixel);
          view = window.getView();
          view.zoom(event.mouseWheelScroll.delta*-0.1f + 1);
          window.setView(view);
          auto newPixel = window.mapCoordsToPixel(coords);
          newPixel -= pixel;
          view.move(sf::Vector2f(newPixel));
          window.setView(view);
        }
        break;
      case sf::Event::Resized:
        view = window.getView();
        view.setSize(float(event.size.width), float(event.size.height));
        window.setView(view);
        break;
      case sf::Event::MouseButtonPressed:
        if (event.mouseButton.button == sf::Mouse::Left) {
          dragging = true;
          dragPos = { event.mouseButton.x, event.mouseButton.y };
        }
        break;
      case sf::Event::MouseButtonReleased:
        if (event.mouseButton.button == sf::Mouse::Left) {
          dragging = false;
        }
        break;
      case sf::Event::MouseMoved:
        if (dragging) {
          sf::Vector2i newPos = { event.mouseMove.x, event.mouseMove.y };
          dragPos -= newPos;

          view = window.getView();
          float ratio = view.getSize().y / window.getSize().y;
          view.move(sf::Vector2f(dragPos) * ratio);
          window.setView(view);
          
          dragPos = newPos;
        }
      }
    }
    
    // update it
    cells.update();

    // draw it
    window.clear();
    window.draw(cells);
    window.display();
  }

  return 0;
}