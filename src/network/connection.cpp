#include "network/connection.hpp"

namespace Ennovia
{

void Connection::async_read(ErrorHandler errorHandler, Handler handler, boost::shared_ptr<Connection> conn)
{
    std::cout << "async reaaaad started!" << std::endl;
    inbound_buffer_.resize(128);
    socket_.async_read_some(boost::asio::buffer(inbound_buffer_),
                            boost::bind(&Connection::handle_read,
                                        this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred,
                                        boost::make_tuple(errorHandler,handler),conn));
}

void Connection::handle_read(const boost::system::error_code& e, size_t bytes_transferred,
                                    boost::tuple<ErrorHandler,Handler> handler, boost::shared_ptr<Connection> conn)

{
    try
    {
        if (e)
        {
            boost::get<0>(handler)(e);
        }
        else
        {
            int originalSize = inbound_data_.size();
            inbound_data_.resize(originalSize + bytes_transferred);
            int buffSize = originalSize + bytes_transferred;
            std::cout << originalSize << "+" << bytes_transferred << "=" << buffSize << std::endl;
            std::memcpy(&inbound_data_[originalSize],&inbound_buffer_[0],bytes_transferred);
            while(buffSize>=HEADER_SIZE) { // this means we have at least a header in our buffer
                int sz = inbound_data_[3];
                sz = (sz << 8) | inbound_data_[2];
                sz = (sz << 8) | inbound_data_[1];
                sz = (sz << 8) | inbound_data_[0];
                if(buffSize>=HEADER_SIZE+sz) { // we have a full message to parse!
                    std::string archive_data(&inbound_data_[HEADER_SIZE], sz);
                    Json::Value value;
                    std::cout << archive_data << std::endl;
                    if(reader.parse(archive_data,value))
                    {
                        boost::get<1>(handler)(value);
                    }
                    if(HEADER_SIZE+sz<buffSize) {
                        std::copy(&inbound_data_[HEADER_SIZE+sz],&inbound_data_[buffSize],&inbound_data_[0]);
                        buffSize-=HEADER_SIZE+sz;
                    } else {
                        inbound_data_.clear();
                        buffSize = 0;
                    }
                    inbound_buffer_.resize(4);
                } else {
                    inbound_buffer_.resize(sz);
                }
            }
            std::cout << "async read again: " << inbound_buffer_.size() << std::endl;
            boost::asio::async_read(socket_, boost::asio::buffer(inbound_buffer_),
                            boost::bind(&Connection::handle_read,
                                        this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred,
                                        handler,conn));
        }
    }
    catch(std::exception& e)
    {
        std::cout << "Connection::handle_read_header: " << e.what() << std::endl;
    }
}

void Connection::write_(Json::Value val)
{
    try
    {
        std::string json = writer.write(val);
        outbound_data_.resize(HEADER_SIZE + json.size());
        int sz = json.size();
        outbound_data_[0] = sz & 255;
        sz >>= 8;
        outbound_data_[1] = sz & 255;
        sz >>= 8;
        outbound_data_[2] = sz & 255;
        sz >>= 8;
        outbound_data_[3] = sz & 255;
        std::memcpy(&inbound_data_[HEADER_SIZE],&json[0],json.size());
        std::cout << json.size() << ":" << json << std::endl;
        boost::asio::async_write(socket_, boost::asio::buffer(outbound_data_),no_handler());
    }
    catch(std::exception& e)
    {
        std::cout << "Connection::write: " << e.what() << std::endl;
    }
}

}
