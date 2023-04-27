#pragma once
#include <SFML/Graphics.hpp>

#include <algorithm>
#include <iostream>
#include <sstream>
#include <string>
#include <thread>
#include <chrono>
#include <utility>

#include <boost/asio.hpp>

#include "player.h"
#include "game.h"

class ClientGamer
{
private:

    using io_service = boost::asio::io_service;
    using tcp_endpoint = boost::asio::ip::tcp::endpoint;
    using tcp_socket = boost::asio::ip::tcp::socket;

public:

    explicit ClientGamer(
        Player&& player1,
        Player&& player2,
        int speed,
        const std::string& client_name,
        const std::string& raw_ip_address,
        unsigned int port) :
        m_player1(std::move(player1)),
        m_player2(std::move(player2)),
        game(player1, speed),
        m_client_name(client_name),
        m_raw_ip_address(raw_ip_address),
        m_port(port),
        m_exit_flag(false),
        m_endpoint(boost::asio::ip::address::from_string(raw_ip_address), port),
        m_socket(m_service, m_endpoint.protocol())
    {

    }

    ~ClientGamer() noexcept = default;

public:

    auto run()
    {
        connect();

        // share initial parameters of game
        send_p1();
        configure_p2();

        // need to synchronize here before start!
        wait();

        play();

        boost::asio::write(m_socket, boost::asio::buffer(last + "#"));
        read_data_until();
        std::this_thread::sleep_for(std::chrono::seconds(5));
        game.setWindow().close();
    }

private:

    void connect()
    {
        m_socket.connect(m_endpoint);
    }

    void send_p1() noexcept
    {
        auto x = std::to_string(m_player1.getPosition_x());
        auto y = std::to_string(m_player1.getPosition_y());

        std::string direction = m_player1.getDirectionStr();

        boost::asio::write(m_socket, boost::asio::buffer(m_client_name + ": " + x + " " + y + " " + direction + "#"));
    }

    void configure_p2() noexcept
    {
        boost::asio::streambuf buffer;

        boost::asio::read_until(m_socket, buffer, '#');

        std::string message;

        std::istream input_stream(&buffer);
        std::getline(input_stream, message, '#');

        std::array <int, 2> xy{ };
        define_xy(message, xy);
        m_player2.setPosition_x(xy.at(0));
        m_player2.setPosition_y(xy.at(1));
        m_player2.setDirection(define_p2_direction(message));

        send_message("server: " + message);
    }

    void define_xy(const std::string& message, std::array < int, 2 >& array) const noexcept
    {
        std::stringstream ss;
        ss << message;

        std::string temp;
        int found;
        int i = 0;
        while (!ss.eof() && i < 2)
        {
            ss >> temp;

            if (std::stringstream(temp) >> found)
                array[i++] = found;

            temp = "";
        }
    }

    Direction define_p2_direction(const std::string& message) const noexcept
    {
        // may be it is worth to optimize here
        if (message.find(" up") != std::string::npos)
            return Direction::up;
        if (message.find(" left") != std::string::npos)
            return Direction::left;
        if (message.find(" right") != std::string::npos)
            return Direction::right;
        if (message.find(" down") != std::string::npos)
            return Direction::down;

        return m_player2.getDirection();
    }

    void wait() noexcept
    {
        boost::asio::write(m_socket, boost::asio::buffer("is ready to continue#"));
        read_data_until();
        send_message(m_client_name + " is ready to continue");
    }

    void read_data_until() noexcept
    {
        boost::asio::streambuf buffer;

        boost::asio::read_until(m_socket, buffer, '#');

        std::string message;

        std::istream input_stream(&buffer);
        std::getline(input_stream, message, '#');

        if (message == last)
            m_exit_flag = true;

        m_player2.setDirection(define_p2_direction(message));

        send_message("server: " + message);
    }

    void send_message(const std::string& message) const noexcept
    {
        std::cout << message << "\n";
    }

    void play() noexcept
    {
        while (game.getWindow().isOpen())
        {
            sf::Event e;
            while (game.setWindow().pollEvent(e))
            {
                if (e.type == sf::Event::Closed)
                    game.setWindow().close();
            }

            std::string message;
            m_player1.setDirection_by_arrow(message);
            send_message(m_client_name + " " + message);

            boost::asio::write(m_socket, boost::asio::buffer(message + "#"));

            read_data_until();

            wait();

            if (m_exit_flag)
                break;

            for (auto i = 0; i < game.getSpeed(); ++i)
            {
                m_player1.tick();
                m_player2.tick();

                check_if_end(game.setField());

                update_field(game.setField());

                draw_field(game.getT());
                game.getT().display();
            }

            ////// draw  //////
            game.setWindow().clear();
            game.setWindow().draw(game.getSprite());
            game.setWindow().display();
        }
    }

    void check_if_end(boost::multi_array < bool, 2 >& field) noexcept
    {
        if (field[m_player1.getPosition_x()][m_player1.getPosition_y()] == true)
            m_exit_flag = true;
        if (field[m_player2.getPosition_x()][m_player2.getPosition_y()] == true)
            m_exit_flag = true;
    }

    void update_field(boost::multi_array < bool, 2 >& field) noexcept
    {
        field[m_player1.getPosition_x()][m_player1.getPosition_y()] = true;
        field[m_player2.getPosition_x()][m_player2.getPosition_y()] = true;
    }

    void draw_field(sf::RenderTexture& t) noexcept
    {
        sf::CircleShape c(3);
        c.setPosition(m_player1.getPosition_x(), m_player1.getPosition_y()); c.setFillColor(m_player1.getColor());
        t.draw(c);
        c.setPosition(m_player2.getPosition_x(), m_player2.getPosition_y()); c.setFillColor(m_player2.getColor());
        t.draw(c);
    }

private:

    inline static const std::string last = "the game is over";

private:

    io_service    m_service;
    tcp_endpoint  m_endpoint;
    tcp_socket    m_socket;

    std::string   m_client_name;
    std::string   m_raw_ip_address;
    unsigned int  m_port;
    bool          m_exit_flag;

    Player        m_player1;
    Player        m_player2;
    Game          game;
};