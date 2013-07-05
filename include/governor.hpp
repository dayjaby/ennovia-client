#ifndef ENNOVIA_GOVERNOR_HPP
#define ENNOVIA_GOVERNOR_HPP

#include <fstream>
#include "network/network.hpp"
#include "world/library.hpp"
#include "world/lua/manager.hpp"
#include "world/actions.hpp"

namespace Ennovia {
class Database;
class Governor {
public:
    Governor();
    ~Governor();
    static Governor& get() {
        static Governor instance;
        return instance;
    }

    Database* getDB() { return db; }

    std::ofstream log;
    bool alive;
    void run();
    void tick();

    int getTicks() { return ticks; }

    /** Interface for the world **/
    void updateLocatablePosition(Locatable* locatable);
    void moveLocatable(int id,float x,float y);
    void writeMessage(int id,const std::string& msg);

    /** Interface for the network **/
    void onClientConnected(ServerConnection connection);
    void getTileOptionList(ServerConnection connection,int map,int x,int y);
    void getLocatableOptionList(ServerConnection connection,int id);
    void walkTo(ServerConnection connection,float x,float y);

    void getLivingObjectOptionList(ServerConnection connection,int optionlist,int id);
    void chooseOption(ServerConnection,int optionlist,int id);
    void getLocatablePosition(ServerConnection connection,int id);
    void getLocatableIntroduction(ServerConnection connection,int id);

    void login(ServerConnection connection, const std::string& username, const std::string& password);

    void requestMap(ServerConnection connection,int id);

private:
    Database* db;
    Lua::Manager lua;

    ActionList actionList;

    void sendTileOptionList(ServerConnection connection,reg<Map> map,int x,int y);
    void sendLocatableOptionList(ServerConnection connection,reg<Locatable> locatable);
    void introduceAllLocatables(ServerConnection connection,int id);

    boost::asio::io_service io_service;
    boost::asio::deadline_timer timer;
    int ticks;
    std::map<Player*,ServerConnection> connections;
    std::map<std::string,int> players;
    Server server;

    /** Library **/
    Library library;
};

}

#endif
