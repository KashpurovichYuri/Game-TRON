#pragma once
#include <SFML/Graphics.hpp>

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

    Player(sf::Color c, int width, int height):
        m_color(c), width(width), height(height)
    {
        std::random_device seed;
        std::mt19937 engine(seed());
        std::uniform_int_distribution < int > distribution_x(0, width);
        std::uniform_int_distribution < int > distribution_y(0, height);
        std::uniform_int_distribution < int > distribution_direction(0, 3);

        m_x = distribution_x(engine);
        m_y = distribution_y(engine);
        m_dir = static_cast < Direction > (distribution_direction(engine));
        defineDirection();
    }

    //Player(const Player&) = delete;

    ~Player() = default;

private:

    void defineDirection() noexcept
    {
        switch (m_dir)
        {
        case Direction::up:
            m_dir_str = "up";
            break;
        case Direction::left:
            m_dir_str = "left";
            break;
        case Direction::right:
            m_dir_str = "right";
            break;
        case Direction::down:
            m_dir_str = "down";
            break;
        }
    }

public:

    auto tick() noexcept
    {
        switch (m_dir)
        {
        case Direction::up:
            m_dir_str = "up";
            --m_y;
            break;
        case Direction::left:
            m_dir_str = "left";
            --m_x;
            break;
        case Direction::right:
            m_dir_str = "right";
            ++m_x;
            break;
        case Direction::down:
            m_dir_str = "down";
            ++m_y;
            break;
        }

        if (m_x >= width)
            m_x = 0;
        else if (m_x < 0)
            m_x = width - 1;

        if (m_y >= height)
            m_y = 0;
        else if (m_y < 0)
            m_y = height - 1;
    }

    auto getDirection() const noexcept
    {
        return m_dir;
    }

    auto setDirection(const Direction& direction) noexcept
    {
        m_dir = direction;
    }

    auto getColor() const noexcept
    {
        return m_color;
    }

    auto getPosition() const noexcept
    {
        return sf::Vector2i(m_x, m_y);
    }

    auto getPosition_x() const noexcept
    {
        return m_x;
    }

    auto getPosition_y() const noexcept
    {
        return m_y;
    }

    auto setPosition_x(int x) noexcept
    {
        m_x = x;
    }

    auto setPosition_y(int y) noexcept
    {
        m_y = y;
    }

    auto getWidth() const noexcept
    {
        return width;
    }

    auto getHeight() const noexcept
    {
        return height;
    }

    auto getDirectionStr() const noexcept
    {
        return m_dir_str;
    }

    auto setDirection_by_arrow(std::string& direction)
    {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) && (m_dir != Direction::right))
        {
            m_dir = Direction::left;
            direction = "left";
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right) && (m_dir != Direction::left))
        {
            m_dir = Direction::right;
            direction = "right";
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up) && (m_dir != Direction::down))
        {
            m_dir = Direction::up;
            direction = "up";
        };
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down) && (m_dir != Direction::up))
        {
            m_dir = Direction::down;
            direction = "down";
        };
    }

private:

    const int width;
    const int height;

private:

    int m_x;
    int m_y;
    Direction m_dir;
    std::string m_dir_str;
    sf::Color m_color;
};