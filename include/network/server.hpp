#ifndef ENNOVIA_SERVER_HPP
#define ENNOVIA_SERVER_HPP


#include <boost/tuple/tuple.hpp>

#include "network/connection.hpp"

namespace Ennovia
{
class Player;
class Server
{
public:

    Server(boost::asio::io_service& io_service, unsigned short port);

    template <int msgid, typename Type>
    void write(boost::shared_ptr<Connection> connection, Type& t)
    {
        std::cout << "Write message " << msgid << std::endl;
        connection->write<msgid>(t);
    }


private:

    void start_read(boost::shared_ptr<Connection> connection);
    void finished_read(const boost::system::error_code& e, boost::shared_ptr<Connection> connection);
    void handle_accept(const boost::system::error_code& e, boost::shared_ptr<Connection> connection);

    void interpret(int msgid, std::istream& is, boost::shared_ptr<Connection> connection);

    boost::asio::ip::tcp::acceptor acceptor_;
};
}
#endif
