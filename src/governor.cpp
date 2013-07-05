#include "governor.hpp"
#include "core.hpp"
#include "world/map.hpp"
#include "world/tile.hpp"
#include "world/optionlist.hpp"
#include "world/player.hpp"
#include "world/database.hpp"

namespace Ennovia {

Governor::Governor() :
    db(new Database("tcp://127.0.0.1:3306","root","")), log("server_log.txt"),
    timer(io_service, boost::posix_time::milliseconds(TICK)),
    server(io_service,PORT)
{
}

Governor::~Governor() {
    delete db;
}

void Governor::run() {
    io_service.run();
    alive = true;
    tick();
}

void Governor::tick()
{
    if(alive) {
        ticks++;
        timer.expires_at(timer.expires_at() + boost::posix_time::milliseconds(TICK));
        timer.async_wait(boost::bind(&Governor::tick, this));

        actionList.tick();

      //  server.write<Client::SEND_ENTITIES>(player,...);
    }
}

void Governor::updateLocatablePosition(Locatable* locatable_) {
    if(locatable_) {
        log << "UPDATE LOCATABLE THAT EXISTS" << std::endl;
        reg<Locatable> locatable(locatable_);
        reg<Map> map(locatable->getPosition().map);
        LocatablePosition position;
        position.set_id(locatable.id());
        position.set_map(map.id());
        position.set_x(locatable->getPosition().x);
        position.set_y(locatable->getPosition().y);

        typedef std::set<Locatable*>::iterator Iterator;
        for(Iterator i = map->locatables.begin(); i != map->locatables.end(); ++i) {
            if((*i)->getPosition().map == map) {
                reg<Player> player = library.getPlayer(locatable.id());
                if(player.exists()) {

                    connections[player.get()]->write<LOCATABLE_POSITION>(position);
                    log << "sent message to " << locatable.id() << std::endl;
                }
            }
        }

    }
}


void Governor::moveLocatable(int id,float x,float y) {
    reg<Locatable> locatable(id);
    if(locatable.exists()) {
        log << "MOVE LOCATABLE THAT EXISTS" << std::endl;
        reg<Map> map(locatable->getPosition().map);
        actionList.add(new Move(locatable.get(),locatable->getPosition(),Position(x,y,map.get())));
        log << "movement added to list" << std::endl;
        MoveTo msg;
        msg.set_id(id);
        msg.set_x(x);
        msg.set_y(y);
        if(library.isPlayer(id)) {
            connections[library.getPlayer(id).get()]->write<MOVE_TO>(msg);
        }
        typedef std::set<Locatable*>::iterator Iterator;
        for(Iterator i = map->locatables.begin(); i != map->locatables.end(); ++i) {
            Locatable* loc = *i;
            if(loc->getPosition().map == map.get()) {
                reg<Locatable> loc_(loc);
                reg<Player> player = library.getPlayer(loc_.id());
                if(player.exists()) {
                    connections[player.get()]->write<MOVE_TO>(msg);
                    log << "sent message to " << loc_.id() << std::endl;
                }
            }
        }
    }
}

void Governor::writeMessage(int id,const std::string& msg) {
    reg<Player> player(id);
    if(player.exists()) {
        ServerMessage response;
        response.set_text(msg);
        connections[player.get()]->write<SEND_MESSAGE>(response);
    }
}

void Governor::onClientConnected(ServerConnection connection) {
    ServerMessage response;
    response.set_text("Welcome to our server!");
    connection->write<SEND_MESSAGE>(response);
    connection->write<REQUEST_CREDENTIALS>();
}

void Governor::getTileOptionList(ServerConnection connection,int mapid,int x,int y) {
    reg<Map> map(mapid);
    if(map.exists()) {
        sendTileOptionList(connection,map,x,y);
    }
}

void Governor::getLocatableOptionList(ServerConnection connection,int id) {
    reg<Locatable> locatable = library.getLocatable(id);
    if(locatable.exists())
        sendLocatableOptionList(connection,locatable);
}

void Governor::walkTo(ServerConnection connection,float x,float y) {
    std::cout << "WALK TO " << x << "," << y << std::endl;
    if(connection->player) {
        moveLocatable(connection->player.id(),x,y);
    }
}



void Governor::chooseOption(ServerConnection connection,int optionlist,int id) {
    reg<OptionList> list(optionlist);
    if(list.exists())
        list->options[id]->onChoose(connection->player);
}

void Governor::getLocatablePosition(ServerConnection connection,int id) {
    reg<Locatable> locatable(id);
    if(locatable.exists()) {
        reg<Map> map(locatable->getPosition().map);
        LocatablePosition position;
        position.set_id(id);
        position.set_map(map.id());
        position.set_x(locatable->getPosition().x);
        position.set_y(locatable->getPosition().y);
        connection->write<LOCATABLE_POSITION>(position);
    } else {
        std::cout << id << "doesnt exist" << std::endl;
    }
}
void Governor::getLocatableIntroduction(ServerConnection connection,int id) {
    IntroduceLocatable intro;
    intro.set_id(id);
    if(library.isPlayer(id)) {
        intro.set_type(2);
    } else if(library.isLivingObject(id)) {
        intro.set_type(1);
    } else if(library.isLocatable(id)) {
        intro.set_type(0);

    }
    reg<Locatable> locatable(id);
    if(locatable.exists()) {
        intro.set_name(locatable->getName());
        if(locatable->model().length()>0) {
            intro.set_model(locatable->model());
            if(locatable->texture().length()>0)
                intro.set_texture(locatable->texture());
        }
        connection->write<INTRODUCE_LOCATABLE>(intro);
    }
}


void Governor::login(ServerConnection connection,const std::string& username, const std::string& password) {
    db->playerLogin(username,password);
    if(username.compare("admin") == 0 || username.compare("beni") == 0) {
            if(password.compare("lol") == 0) {
                connection->write<LOGIN_VALID>();
                reg<Player> player;
                if(players.count(username) > 0 && library.isPlayer(players[username])) {
                    // disconnect old connection
                    player = library.getPlayer(players[username]);
                    connections[player.get()]->disconnect();
                } else {
                    player = library.createPlayer(username);
                    players[username] = player.id();
                }
                log << "Login: " << player.id();
                player->model() = "faerie.md2";
                player->texture() = "faerie2.bmp";
                connection->player = player.get();
                reg<Map> map(1);
                if(map.exists()) {
                    player->setPosition(Position(5,5,map.get()));

                    log << "Introduce all locatables" << std::endl;
                    introduceAllLocatables(connection,player.id());
                    log << "Send you are " << player.id() << std::endl;
                    YouAre youAre;
                    youAre.set_id(player.id());
                    connection->write<YOU_ARE>(youAre);

                    getLocatablePosition(connection,player.id());
                }
                connections[player.get()] = connection;
            } else {
                connection->write<LOGIN_PASSWORD_INVALID>();
            }
        } else {
            connection->write<LOGIN_USERNAME_INVALID>();
        }
}

void Governor::requestMap(ServerConnection connection,int id) {
    SendMapData msg;
    reg<Map> map(id);
    msg.set_id(id);
    msg.set_path(map->path);
    msg.set_heightmap(map->heightmappath);
    msg.set_width(map->width);
    msg.set_height(map->height);
    connection->write<SEND_MAP>(msg);
}

void Governor::sendTileOptionList(ServerConnection connection,reg<Map> map,int x,int y) {
/*    SendTileOptionList msg;
    Position pos(x,y,map.get());
    if(pos.isValid()) {
        Tile* tile = pos.getTile();
        reg<OptionList> optionlist(tile->getOptionList(connection->player));
        msg.set_map(map.id());
        msg.set_x(x);
        msg.set_y(y);
        int i=0;
        for(OptionList::Options::iterator iter=optionlist->options.begin();iter!=optionlist->options.end();++iter,++i) {
            OptionRef* optionRef = msg.add_options();
            optionRef->set_index(i);
            optionRef->set_description((*iter)->getDescription());
            optionRef->set_optionlist(optionlist.id());
        }
        connection->write<SEND_TILE_OPTIONLIST>(msg);
    }*/
}

void Governor::sendLocatableOptionList(ServerConnection connection,reg<Locatable> locatable) {
    SendLocatableOptionList msg;
    reg<OptionList> optionlist(locatable->getOptionList(connection->player));
    msg.set_id(locatable.id());
    int i=0;
    for(OptionList::Options::iterator iter=optionlist->options.begin();iter!=optionlist->options.end();++iter,++i) {
        OptionRef* optionRef = msg.add_options();
        optionRef->set_index(i);
        optionRef->set_description((*iter)->getDescription());
        optionRef->set_optionlist(optionlist.id());
    }
    connection->write<SEND_LOCATABLE_OPTIONLIST>(msg);
}

void Governor::introduceAllLocatables(ServerConnection connection,int id) {
    reg<Locatable> locatable(id);
    if(locatable.exists()) {
        Map* map = locatable->getPosition().map;
        if(map) {
            for(Map::Locatables::iterator i=map->locatables.begin();i!=map->locatables.end();++i) {
                reg<Locatable> loc(*i);
                getLocatableIntroduction(connection,loc.id());
            }
        }
    }
}


}
