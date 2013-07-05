#include "network/server.hpp"
#include "network/message.hpp"
#include "world/player.hpp"
#include "governor.hpp"

namespace Ennovia
{

Server::Server(boost::asio::io_service& io_service, unsigned short port): acceptor_(io_service,
            boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port))
{
    // Start an accept operation for a new connection.
    boost::shared_ptr<Connection> new_conn(new Connection(acceptor_.get_io_service()));
    acceptor_.async_accept(new_conn->socket(),
                           boost::bind(&Server::handle_accept, this,
                                       boost::asio::placeholders::error, new_conn));
}

void Server::start_read(boost::shared_ptr<Connection> connection)
{
    connection->async_read(boost::bind(&Server::finished_read, this,
                                       boost::asio::placeholders::error, connection),
                           boost::bind(&Server::interpret, this,_1,_2,connection),connection);
}

void Server::finished_read(const boost::system::error_code& e, boost::shared_ptr<Connection> connection)
{
    if(e)
    {
        std::cerr << e.message() << std::endl;
        if(e == boost::asio::error::connection_aborted) {
            std::cerr << "aborted" << std::endl;
        } else if(e == boost::asio::error::connection_reset) {
            std::cerr << "reset" << std::endl;
        } else {
            start_read(connection);
        }
    } else {
        start_read(connection);
    }
}

void Server::handle_accept(const boost::system::error_code& e, boost::shared_ptr<Connection> connection)
{
    if (!e)
    {
        start_read(connection);
        Governor::get().onClientConnected(connection);
    }

    // Start an accept operation for a new connection.
    boost::shared_ptr<Connection> new_conn(new Connection(acceptor_.get_io_service()));
    acceptor_.async_accept(new_conn->socket(),
                           boost::bind(&Server::handle_accept, this,
                                       boost::asio::placeholders::error, new_conn));
}

void Server::interpret(int msgid, std::istream& is, boost::shared_ptr<Connection> connection)
{
    Governor& governor = Governor::get();
    std::cout << "Received message: " << msgid << " from " << connection->socket().remote_endpoint().address().to_string() << std::endl;

    switch(msgid)
    {
    case MESSAGE:
    {
        TextMessage in, response;
        in.ParseFromIstream(&is);
        std::cout << "[MSG from Client]: " << in.text() << std::endl;
        governor.log << "[MSG from Client]: " << in.text();
    }
    break;
    case GET_TILE_OPTIONLIST:
    {
        GetTileOptionList msg;
        msg.ParseFromIstream(&is);
        governor.getTileOptionList(connection,msg.map(),msg.x(),msg.y());
    }
    break;
    case GET_LOCATABLE_OPTIONLIST:
    {
        GetLocatableOptionList msg;
        msg.ParseFromIstream(&is);
        governor.log << "Get Locatable OptionList " << msg.id() << std::endl;
        governor.getLocatableOptionList(connection,msg.id());
    }
    break;
    case WALK_TO:
    {
        WalkTo msg;
        msg.ParseFromIstream(&is);
        governor.walkTo(connection,msg.x(),msg.y());
    }
    break;
    case CHOOSE_OPTION:
    {
        ChooseOption option;
        option.ParseFromIstream(&is);
        governor.chooseOption(connection,option.optionlist(),option.id());
        governor.log << "Choose Option" << option.optionlist() << "," << option.id();
    }
    break;
    case GET_LOCATABLE_POSITION:
    {
        GetLocatablePosition locatable;
        locatable.ParseFromIstream(&is);
        governor.getLocatablePosition(connection,locatable.id());
        governor.log << "Get Locatable Position " << locatable.id();
    }
    break;
    case GET_LOCATABLE_INTRODUCTION:
    {
        GetLocatableIntroduction locatable;
        locatable.ParseFromIstream(&is);
        governor.getLocatableIntroduction(connection,locatable.id());
        governor.log << "Get Locatable Introduction " << locatable.id();
    }
    break;
    case CREDENTIALS:
    {
        Credentials credentials;
        credentials.ParseFromIstream(&is);
        governor.login(connection,credentials.username(),credentials.password());
        governor.log << "Credentials " << credentials.username() << "," << credentials.password();
    }
    break;
    case SEND_AES_KEY:
    {
        SendAESKey key;
        key.ParseFromIstream(&is);
    }
    break;
    case REQUEST_MAP:
    {
        RequestMapData request;
        request.ParseFromIstream(&is);
        governor.requestMap(connection,request.id());
        governor.log << "Request map " << request.id();
    }
    break;
    default:
        break;
    }
    governor.log << "|" << std::endl;

}
}
