#pragma once
#include "player.h"
#include "client-gamer.h"

int main()
{
    const auto width = 600;
    const auto height = 480;

    Player p1(sf::Color::Red, width, height);
    Player p2(sf::Color::Green, width, height);

    const auto port = 3333;
    const auto speed = 3;
    const std::string nickname = "Yuri";
    const std::string raw_ip_address = "127.0.0.1";

    try
    {
        ClientGamer game(p1, p2, speed, nickname, raw_ip_address, port);

        game.run();
    }
    catch (boost::system::system_error& e)
    {
        std::cout << "Error occured! Error code = " << e.code() << ". Message: " << e.what() << std::endl;

        system("pause");

        return e.code().value();
    }

    return EXIT_SUCCESS;

}