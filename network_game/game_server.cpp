#include "game_server.h"

int main()
{
    const auto port1 = 3333;
    const auto port2 = 3334;
    const auto size = 30;

    try
    {
        Server game_server(port1, port2, size);

        game_server.run();
    }
    catch (boost::system::system_error& e)
    {
        std::cout << "Error occured! Error code = " << e.code() << ". Message: " << e.what() << std::endl;

        system("pause");

        return e.code().value();
    }
}