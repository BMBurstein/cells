#include <SFML/Graphics.hpp>

#include <vector>

static const unsigned int WIDTH = 800;
static const unsigned int HEIGHT = 600;

class CellWorld : public sf::Drawable, public sf::Transformable
{
public:
  static const std::size_t NUM_CELLS = HEIGHT * WIDTH;

  CellWorld() :
    m_vertices(sf::Points, NUM_CELLS),
    m_cells(NUM_CELLS),
    m_new_cells(NUM_CELLS)
  {
    for (std::size_t i = 1; i < WIDTH - 1; ++i) {
      for (std::size_t j = 1; j < HEIGHT - 1; ++j) {
        m_vertices[i*HEIGHT + j] = { sf::Vector2f(i, j), sf::Color::White };
        m_cells[i*HEIGHT + j].alive = false;// std::rand() % 2;
      }
    }
    m_cells[400 * HEIGHT + 300].alive = m_cells[399 * HEIGHT + 301].alive = m_cells[400 * HEIGHT + 301].alive = m_cells[400 * HEIGHT + 302].alive = m_cells[401 * HEIGHT + 300].alive = true;
    for (std::size_t i = 1; i < NUM_CELLS - 1; ++i) {
      m_vertices[i].color = m_cells[i];
    }
  }

  void update()
  {
    auto cell = [&](int x, int y) { return m_cells[x * HEIGHT + y].alive ? 1 : 0; };

    for (std::size_t i = 1; i < WIDTH-1; ++i) {
      for (std::size_t j = 1; j < HEIGHT-1; ++j) {
        auto adjacent = cell(i - 1, j - 1) + cell(i, j - 1) + cell(i + 1, j - 1)
                      + cell(i - 1, j    ) +                + cell(i + 1, j    )
                      + cell(i - 1, j + 1) + cell(i, j + 1) + cell(i + 1, j + 1);
        m_new_cells[i*HEIGHT + j].alive = adjacent == 3 || (adjacent == 2 && cell(i,j));
        m_vertices[i*HEIGHT + j].color = m_new_cells[i*HEIGHT + j];
      }
    }
    m_cells.swap(m_new_cells);
  }

protected:

  virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const
  {
    // apply the transform
    states.transform *= getTransform();

    // our particles don't use a texture
    states.texture = NULL;

    // draw the vertex array
    target.draw(m_vertices, states);
  }

private:

  struct Cell
  {
    bool alive;
    operator sf::Color() { return alive ? sf::Color::Black : sf::Color::White; }
  };

  sf::VertexArray m_vertices;
  std::vector<Cell> m_cells;
  std::vector<Cell> m_new_cells;
};

int main()
{
  sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "Cell Sim v0.1", sf::Style::Close);

  // create the particle system
  CellWorld cells;

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