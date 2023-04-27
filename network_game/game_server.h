#include <algorithm>
#include <iostream>
#include <string>
#include <thread>

#include <boost/asio.hpp>

class Server
{
private:

    using io_service = boost::asio::io_service;
    using tcp_endpoint = boost::asio::ip::tcp::endpoint;
    using tcp_acceptor = boost::asio::ip::tcp::acceptor;
    using tcp_socket = boost::asio::ip::tcp::socket;

public:

    explicit Server(unsigned int port1, unsigned int port2, int size) :
        m_port1(port1),
        m_port2(port2),
        m_size(size),
        m_exit_flag(false),
        m_endpoint1(boost::asio::ip::address_v4::any(), port1),
        m_endpoint2(boost::asio::ip::address_v4::any(), port2),
        m_acceptor1(m_service, m_endpoint1.protocol()),
        m_acceptor2(m_service, m_endpoint2.protocol()),
        m_socket1(m_service),
        m_socket2(m_service)
    {
        auto connector1 = std::thread(
            &Server::connect, this, std::ref(m_acceptor1), std::ref(m_endpoint1), std::ref(m_socket1));

        auto connector2 = std::thread(
            &Server::connect, this, std::ref(m_acceptor2), std::ref(m_endpoint2), std::ref(m_socket2));

        connector1.join();
        connector2.join();
    }

    ~Server() noexcept
    {
        std::cout << "Game server is destroyed\n";
    }

public:

    void connect(tcp_acceptor& acceptor, tcp_endpoint& epoint, tcp_socket& socket)
    {
        acceptor.bind(epoint);

        acceptor.listen(m_size);

        acceptor.accept(socket);
    }

    void run() noexcept
    {
        initial_settings();

        start();

        auto reader = std::thread(&Server::read, this);

        server_write();

        m_exit_flag = true;

        reader.join();
    }

private:

    void initial_settings() noexcept
    {
        std::string message1;
        std::string message2;

        preparing_for_reading(message1, message2);

        m_client1_name = message1.substr(0, message1.find(":"));
        m_client2_name = message2.substr(0, message2.find(":"));

        boost::asio::write(m_socket1, boost::asio::buffer(message2 + "#"));
        boost::asio::write(m_socket2, boost::asio::buffer(message1 + "#"));
    }

    void preparing_for_reading(std::string& message1, std::string& message2) noexcept
    {
        boost::asio::streambuf buffer1;
        boost::asio::streambuf buffer2;

        boost::asio::read_until(m_socket1, buffer1, '#');
        boost::asio::read_until(m_socket2, buffer2, '#');

        std::istream input_stream1(&buffer1);
        std::istream input_stream2(&buffer2);

        std::getline(input_stream1, message1, '#');
        std::getline(input_stream2, message2, '#');
    }

    void read()
    {
        send_message(m_client1_name + " and " + m_client2_name + " joined the server and started the game!");

        while (!m_exit_flag)
        {
            read_data_until();
        }
    }

    void read_data_until()
    {
        std::string message1;
        std::string message2;

        preparing_for_reading(message1, message2);

        if (message1 == last || message2 == last)
            m_exit_flag = true;

        send_message(m_client1_name + ": " + message1);
        boost::asio::write(m_socket2, boost::asio::buffer(m_client1_name + " " + message1 + "#"));

        send_message(m_client2_name + ": " + message2);
        boost::asio::write(m_socket1, boost::asio::buffer(m_client2_name + " " + message2 + "#"));
    }

    void send_message(const std::string& message)
    {
        std::cout << message << "\n";
    }

    void start() noexcept
    {
        std::string message1;
        std::string message2;

        preparing_for_reading(message1, message2);

        write("the game is starting!#");
    }

    void write(const std::string& message)
    {
        boost::asio::write(m_socket1, boost::asio::buffer(message));
        boost::asio::write(m_socket2, boost::asio::buffer(message));
    }

    void server_write()
    {
        for (
            std::string message;
            std::getline(std::cin, message) && message != last && !m_exit_flag;
            write(message)
            );

        send_message(last);

        write(last + "#");
    }

private:

    inline static const std::string last = "the game is over";

private:

    std::string   m_client1_name;
    std::string   m_client2_name;

    io_service    m_service;
    tcp_endpoint  m_endpoint1;
    tcp_endpoint  m_endpoint2;
    tcp_acceptor  m_acceptor1;
    tcp_acceptor  m_acceptor2;
    tcp_socket    m_socket1;
    tcp_socket    m_socket2;
    unsigned int  m_port1;
    unsigned int  m_port2;
    int           m_size;

    bool          m_exit_flag;

};