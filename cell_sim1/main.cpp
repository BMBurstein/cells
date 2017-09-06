#include <SFML/Graphics.hpp>

#include <unordered_map>
#include <functional>

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
  CellWorld() :
    m_vertices(sf::Points)
  {
    addCell(0, 0);
    addCell(4, 0);
    addCell(5, 0);
    addCell(6, 0);
    addCell(0, 1);
    addCell(1, 1);
    addCell(2, 1);
    addCell(5, 1);
    addCell(1, 2);
    m_cells.swap(m_new_cells);
  }

  void update()
  {
    m_new_cells.clear();
    for (auto& c : m_cells) {
      auto x = c.first.x;
      auto y = c.first.y;
      auto adjacent = cell(x - 1, y - 1).alive + cell(x, y - 1).alive + cell(x + 1, y - 1).alive
                    + cell(x - 1, y    ).alive +           0          + cell(x + 1, y    ).alive
                    + cell(x - 1, y + 1).alive + cell(x, y + 1).alive + cell(x + 1, y + 1).alive;
      if (adjacent == 3 || (adjacent == 2 && c.second.alive)) {
        addCell(x, y);
      }
    }

    m_cells.swap(m_new_cells);

    m_vertices.clear();
    for (auto& c : m_cells) {
      if (c.second.alive) {
        m_vertices.append({ { float(c.first.x),float(c.first.y) }, c.second });
      }
    }
  }

  Cell cell(int x, int y) {
    auto it = m_cells.find({ x, y });
    if (it == m_cells.end()) {
      return Cell();
    }
    return it->second;
  }

  void addCell(int x, int y) {
    m_new_cells[{x, y}].alive = true;
    for (int i = x - 1; i <= x + 1; ++i) {
      for (int j = y - 1; j <= y + 1; ++j) {
        m_new_cells[{i, j}];//.alive &= true;
      }
    }
  }

protected:

  virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const
  {
    // apply the transform
    states.transform *= getTransform();
    
    states.texture = NULL;

    // draw the vertex array
    target.clear(sf::Color::White);
    target.draw(m_vertices, states);
  }

private:
  typedef std::unordered_map<sf::Vector2i, Cell, PointHash> CellsMap;
  sf::VertexArray m_vertices;
  CellsMap m_cells;
  CellsMap m_new_cells;
};

static const unsigned int WIDTH = 800;
static const unsigned int HEIGHT = 600;

int main()
{
  sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "Game of Life v0.2", sf::Style::Close);

  // create the particle system
  CellWorld cells;
  cells.setPosition(WIDTH / 2, HEIGHT / 2);

  // create a clock to track the elapsed time
  sf::Clock clock;

  // run the main loop
  while (window.isOpen())
  {
    // handle events
    sf::Event event;
    while (window.pollEvent(event))
    {
      switch (event.type) {
      case sf::Event::Closed:
        window.close();
      //case sf::Event::MouseButtonPressed:
      //  cells.update();
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