#include "network/connection.hpp"

namespace Ennovia
{

void Connection::async_read(ErrorHandler errorHandler, Handler handler, boost::shared_ptr<Connection> conn)
{

    boost::asio::async_read(socket_, boost::asio::buffer(inbound_data_),
                            boost::bind(&Connection::handle_read,
                                        this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred,
                                        boost::make_tuple(errorHandler,handler),conn));
}

void Connection::handle_read(const boost::system::error_code& e, size_t bytes_transferred,
                                    boost::tuple<ErrorHandler,Handler> handler, boost::shared_ptr<Connection> conn)

{
    if (e)
    {
        boost::get<0>(handler)(e);
    }
    else
    {
        std::string archive_data(&inbound_data_[0], inbound_data_.size());
        Json::Value value;
        if(reader.parse(archive_data,value)) {
            boost::get<1>(handler)(value);
        }
    }
}

void Connection::write_(Json::Value val)
{
    outbound_data_ = writer.write(val);
    boost::asio::async_write(socket_, boost::asio::buffer(outbound_data_,outbound_data_.size()),no_handler());
}

}
