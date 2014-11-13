#ifndef ENNOVIA_CONNECTION_HPP
#define ENNOVIA_CONNECTION_HPP

#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/asio.hpp>
#include <boost/function.hpp>
#include <iomanip>
#include <string>
#include <sstream>
#include <vector>

#include <misc/json/json.h>

namespace Ennovia
{
struct no_handler
{
    void operator()() {}
    template <class T1>
    void operator()(T1 t1) {}
    template <class T1, class T2>
    void operator()(T1 t1, T2 t2) {}
};

class Player;
class Connection
{
public:
    typedef boost::function<void(const boost::system::error_code&)> ErrorHandler;
    typedef boost::function<void(Json::Value&)> Handler;

    Connection(boost::asio::io_service& io_service)
        : socket_(io_service)
    {
    }


    boost::asio::ip::tcp::socket& socket()
    {
        return socket_;
    }

    void disconnect() {
        boost::system::error_code ec;
        socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
    }

    void write(Json::Value val)
    {
        write_(val);
    }


    void async_read(ErrorHandler errorHandler, Handler handler, boost::shared_ptr<Connection> conn);
    void handle_read_header(const boost::system::error_code& e, size_t bytes_transferred,
                            boost::tuple<ErrorHandler,Handler> handler, boost::shared_ptr<Connection> conn);

    void handle_read(const boost::system::error_code& e, size_t bytes_transferred,
                            boost::tuple<ErrorHandler,Handler> handler, boost::shared_ptr<Connection> conn);
private:

    void write_(Json::Value val);

    Json::Reader reader;
    Json::FastWriter writer;

    /// The underlying socket.
    boost::asio::ip::tcp::socket socket_;

    /// Holds the outbound data.
    char outbound_header_[4];
    std::string outbound_data_;

    /// Holds the inbound data.
    char inbound_header_[4];
    std::vector<char> inbound_data_;
};



}
#endif
