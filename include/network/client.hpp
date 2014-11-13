#ifndef ENNOVIA_NETWORK_CLIENT_HPP
#define ENNOVIA_NETWORK_CLIENT_HPP


#include "network/connection.hpp"

namespace Ennovia
{

class Client
{
public:

    /// Constructor starts the asynchronous connect operation.
    Client(boost::asio::io_service& io_service,
           const std::string& host, const std::string& service);

    void connect();

    template <typename Type>
    void write(Type t)
    {
        connection->write(t);
    }

    boost::asio::io_service& io_service;
    const std::string& host;
    const std::string& service;
    boost::shared_ptr<Connection> connection;
private:

    /// Handle completion of a connect operation.
    void handle_connect(const boost::system::error_code& e);

    /// Start to read a message
    void start_read();

    /// Handle a finished read operation
    void finished_read(const boost::system::error_code& e);

    /// Interpret a read message
    void interpret(const Json::Value& is);


};
}

#endif
