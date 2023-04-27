#pragma once

#include <algorithm>
#include <iostream>
#include <string>
#include <chrono>

#include "player.h"


class Game
{
public:

	explicit Game(const Player& m_player1, int speed):
        m_window(sf::VideoMode(m_player1.getWidth(), m_player1.getHeight()), "The Tron Game!"),
        m_speed(speed),
        m_field(boost::extents[m_player1.getWidth()][m_player1.getHeight()])
	{
        m_window.setFramerateLimit(60);

        const auto m_width = m_window.getSize().x;
        const auto m_height = m_window.getSize().y;

        m_texture.loadFromFile("background.jpg");
        sf::Sprite sBackground(m_texture);

        m_t.create(m_width, m_height);
        m_t.setSmooth(true);
        m_sprite.setTexture(m_t.getTexture());
        m_t.clear();
        m_t.draw(sBackground);
	}

	~Game() noexcept = default;

public:

    const auto& getWindow() const noexcept
    {
        return m_window;
    }

    auto& setWindow() noexcept
    {
        return m_window;
    }

    auto getSpeed() const noexcept
    {
        return m_speed;
    }

    auto& setField() noexcept
    {
        return m_field;
    }

    const auto& getSprite() const noexcept
    {
        return m_sprite;
    }

    auto& getT() noexcept
    {
        return m_t;
    }

private:

    sf::RenderWindow m_window;
    int m_width;
    int m_height;
    int m_speed;
    boost::multi_array < bool, 2 > m_field;
    sf::Texture m_texture;
    sf::Sprite m_sprite;
    sf::RenderTexture m_t;
};