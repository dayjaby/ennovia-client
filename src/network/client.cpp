#include "network/client.hpp"
#include "network/message.hpp"
#include "world/optionlist.hpp"
#include "mayor.hpp"

namespace Ennovia
{

Client::Client(boost::asio::io_service& io_service_,
               const std::string& host_, const std::string& service_)
    : io_service(io_service_), host(host_), service(service_), connection(new Connection(io_service))
{
    connect();
}

void Client::connect() {
    // Resolve the host name into an IP address.
    boost::asio::ip::tcp::resolver resolver(io_service);
    boost::asio::ip::tcp::resolver::query query(host, service);
    boost::asio::ip::tcp::resolver::iterator end;
    boost::asio::ip::tcp::resolver::iterator endpoint_iterator =
        resolver.resolve(query);
    std::cout << "Establish Connection..." << std::endl;
    // Start an asynchronous connect operation.
    boost::asio::async_connect(connection->socket(), endpoint_iterator,
                               boost::bind(&Client::handle_connect, this,
                                           boost::asio::placeholders::error));

}


/// Handle completion of a connect operation.
void Client::handle_connect(const boost::system::error_code& e)
{
    if (!e)
    {
        std::cout << "Connected. Start reading" << std::endl;
        start_read();
    }
    else
    {
        std::cerr << e.message() << std::endl;
    }
}

void Client::start_read()
{
    connection->async_read(boost::bind(&Client::finished_read, this,
                                 boost::asio::placeholders::error),
                     boost::bind(&Client::interpret, this,_1),
                     connection);
}

void Client::finished_read(const boost::system::error_code& e)
{
    std::cout << "Read next message" << std::endl;
    if(e)
    {
        std::cerr << e.message() << std::endl;
    }
 //   start_read();
}

void Client::interpret(const Json::Value& in)
{
    Mayor& mayor = Mayor::get();
    mayor.log << "Read: " << in.asString() << std::endl;
    switch(in["msg"].asInt())
    {
    case SEND_MESSAGE:
    {
        std::cout << "[MSG from Server]: " << in["text"].asString() << std::endl;
        mayor.log << "[MSG from Server]: " << in["text"].asString() << std::endl;
    }
    break;
    case SEND_LOCATABLE_OPTIONLIST:
    {

        OptionList* optionList = new OptionList();
        for(Json::ValueIterator i = in["options"].begin(); i!=in["options"].end();++i) {
            const Json::Value& option = *i;
            optionList->add(new OptionProxy(option["description"].asString(),option["optionlist"].asInt(),option["index"].asInt()));
        }
        mayor.receiveLocatableOptionList(in["id"].asInt(),optionList);
    }
    break;
    case MOVE_TO:
    {
        mayor.moveTo(in["id"].asInt(),in["x"].asFloat(),in["y"].asFloat());
    }
    break;
    case REQUEST_CREDENTIALS:
    {
        mayor.onRequestCredentials();
    }
    break;
    case LOGIN_USERNAME_INVALID:
    {
        mayor.onLoginUsernameInvalid();
    }
    break;
    case LOGIN_PASSWORD_INVALID:
    {
        mayor.onLoginPasswordInvalid();
    }
    break;
    case LOGIN_VALID:
    {
        mayor.onLoginValid();
    }
    break;
    case SEND_AES_KEY:
    {

    }
    break;
    case YOU_ARE:
    {
        mayor.youAre(in["id"].asInt());
        mayor.log << "You are " << in["id"].asInt();
    }
    break;
    case LOCATABLE_POSITION:
    {
        mayor.setLocatablePosition(in["id"].asInt(),in["map"].asInt(),in["x"].asFloat(),in["y"].asFloat());
    }
    break;
    case INTRODUCE_LOCATABLE:
    {
        mayor.introduceLocatable(in["id"].asInt(),in["name"].asString());
        if(in.isMember("model")) {
            mayor.setLocatableModel(in["id"].asInt(),in["model"].asString(),in.isMember("texture") ? in["texture"].asString() : std::string(""));
        }
    }
    break;
    case SEND_MAP:
    {
        mayor.sendMapData(in["id"].asInt(),in["path"].asString(),in["heightmap"].asString(),in["width"].asInt(),in["height"].asInt());
    }
    break;
    default:
        break;
    }

    mayor.log << "|" << std::endl;


}
}

