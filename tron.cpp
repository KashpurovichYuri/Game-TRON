#include <SFML/Graphics.hpp>

#include <iostream>
#include <random>

#include <boost/multi_array.hpp>

enum class Direction
{
    up,
    left,
    right,
    down,
};

class Player
{ 
public:

    explicit Player(sf::Color c, int width, int height):
        color (c), width (width), height (height)
    {
        std::random_device seed;
        std::mt19937 engine(seed());
        std::uniform_int_distribution < int > distribution_x(0, width);
        std::uniform_int_distribution < int > distribution_y(0, height);
        std::uniform_int_distribution < int > distribution_direction(0, 3);

        x = distribution_x(engine);
        y = distribution_y(engine);
        dir = static_cast < Direction > (distribution_direction(engine));
    }

    Player(const Player &) = delete;

    Player (Player && other):
        m_color(other.m_color),
        width(other.width),
        height(other.height),
        m_x(other.m_x),
        m_y(other.m_y),
        m_dir(other.m_dir),
        m_dir_str(other.m_dir_str)
    {

    }

    ~Player() noexcept = default;

public:

    auto tick() noexcept
    {   
        switch (dir)
        {
        case Direction::up:
            --y;
            break;
        case Direction::left:
            --x;
            break;
        case Direction::right:
            ++x;
            break;
        case Direction::down:
            ++y;
            break;
        }

        if (x >= width)
            x = 0;
        else if (x < 0)
            x = width - 1;

        if (y >= height) 
            y = 0;
        else if (y < 0)
            y = height - 1;
    }

    auto getDirection() const noexcept
    {
        return dir;
    }

    auto setDirection(const Direction & direction) noexcept
    {
        dir = direction;
    }

    auto getColor() const noexcept
    {
        return color;
    }

    auto getPosition() const noexcept
    {
        return sf::Vector2i(x, y);
    }

    auto getPosition_x() const noexcept
    {
        return x;
    }

    auto getPosition_y() const noexcept
    {
        return y;
    }

private:

    const int width;
    const int height;

private:

    int x;
    int y;
    Direction dir;
    sf::Color color;
};

auto setDirection_by_arrow(Player & p)
{
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) && (p.getDirection() != Direction::right))
        p.setDirection(Direction::left);
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right) && (p.getDirection() != Direction::left))
        p.setDirection(Direction::right);
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up) && (p.getDirection() != Direction::down))
        p.setDirection(Direction::up);
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down) && (p.getDirection() != Direction::up))
        p.setDirection(Direction::down);
}

auto setDirection_by_letter(Player& p)
{
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) && (p.getDirection() != Direction::right))
        p.setDirection(Direction::left);
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D) && (p.getDirection() != Direction::left))
        p.setDirection(Direction::right);
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W) && (p.getDirection() != Direction::down))
        p.setDirection(Direction::up);
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S) && (p.getDirection() != Direction::up))
        p.setDirection(Direction::down);
}

auto check_if_end(const boost::multi_array < bool, 2 > & field, const Player& p1, const Player& p2, bool& Game)
{
    if (field[p1.getPosition_x()][p1.getPosition_y()] == true)
        Game = false;
    if (field[p2.getPosition_x()][p2.getPosition_y()] == true)
        Game = false;
}

auto update_field(boost::multi_array < bool, 2 >& field, const Player& p1, const Player& p2)
{
    field[p1.getPosition_x()][p1.getPosition_y()] = true;
    field[p2.getPosition_x()][p2.getPosition_y()] = true;
}

auto draw_field(const Player& p1, const Player& p2, sf::RenderTexture& t)
{
    sf::CircleShape c(3);
    c.setPosition(p1.getPosition_x(), p1.getPosition_y()); c.setFillColor(p1.getColor());
    t.draw(c);
    c.setPosition(p2.getPosition_x(), p2.getPosition_y()); c.setFillColor(p2.getColor());
    t.draw(c);;
}


int main()
{
    const auto W = 1080;
    const auto H = 720;

    sf::RenderWindow window(sf::VideoMode(W, H), "The Tron Game!");
    window.setFramerateLimit(60);

    const auto width = window.getSize().x;
    const auto height = window.getSize().y;
    const auto speed = 3;
    boost::multi_array < bool, 2 > field(boost::extents[width][height]);

    sf::Texture texture;
    texture.loadFromFile("background.jpg");
    sf::Sprite sBackground(texture);

    Player p1(sf::Color::Red, width, height);
    Player p2(sf::Color::Green, width, height);

    sf::Sprite sprite;
    sf::RenderTexture t;
    t.create(width, height);
    t.setSmooth(true);
    sprite.setTexture(t.getTexture());
    t.clear();
    t.draw(sBackground);

    bool Game = true;

    while (window.isOpen())
    {
        sf::Event e;
        while (window.pollEvent(e))
        {
            if (e.type == sf::Event::Closed)
                window.close();
        }

        setDirection_by_arrow(p1);
        setDirection_by_letter(p2);

        if (!Game)
            continue;

        for(auto i = 0; i < speed; ++i)
        {
            p1.tick();
            p2.tick();

            check_if_end(field, p1, p2, Game);

            update_field(field, p1, p2);

            draw_field(p1, p2, t);
            t.display();
        }

        ////// draw  //////
        window.clear();
        window.draw(sprite);
        window.display();
    }

    return EXIT_SUCCESS;
}
