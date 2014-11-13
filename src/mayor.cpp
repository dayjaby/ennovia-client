#include "mayor.hpp"
#include "core.hpp"
#include "ui/render.hpp"
#include "network/network.hpp"
#include "network/client.hpp"
#include "world/library.hpp"
#include "world/map.hpp"
#include "world/actions.hpp"
#include "world/locatable.hpp"
#include "misc/json/jsoncons.hpp"

namespace Ennovia {


struct MayorImpl {
    MayorImpl(Mayor& mayor_) :
    mayor(mayor_),
    tick_timer(io_service, boost::posix_time::milliseconds(TICK)),
    frame_timer(io_service, boost::posix_time::milliseconds(FRAME)),
    //client(io_service,std::string("djabby.dlinkddns.com"),PORT_S),
    //client(io_service,std::string("djabby.dlinkddns.com"),PORT_S),
    client(io_service,std::string("192.168.178.37"),PORT_S),
    connection(client.connection),
    render(1024,768,mayor)
    {
        std::cout << "Mayor constructed" << std::endl;
    }
    Mayor& mayor;
    bool alive;

    LocatablesInAction locatablesInAction;

    boost::asio::io_service io_service;
    boost::asio::deadline_timer tick_timer;
    boost::asio::deadline_timer frame_timer;
    int ticks;

    Client client;
    boost::shared_ptr<Connection> connection;
    SceneNodeContainer sceneNodes;
    LocatableContainer locatables;
    MapContainer maps;
    OptionListContainer optionLists;
    int localPlayer;

    Render render;

    void tick();
    void frame();

    template <class Type>
    void write(Type t) {
        client.write(t);
    }
};

Mayor::Mayor() : log("client_log.txt"),d(new MayorImpl(*this))
{
}

int Mayor::getTicks() { return d->ticks; }


void Mayor::run() {
    d->alive = true;
    d->tick();
    d->frame();
    d->io_service.run();
}

void MayorImpl::tick()
{
    mayor.log << "Mayor.Tick" << std::endl;
    mayor.log << "<Tick " << std::flush;
    if(alive) {
        ticks++;
        mayor.log << "| alive | " << ticks << std::flush;
        tick_timer.expires_at(tick_timer.expires_at() + boost::posix_time::milliseconds(TICK));
        tick_timer.async_wait(boost::bind(&MayorImpl::tick, this));
        for(LocatablesInAction::iterator i = locatablesInAction.begin();i!=locatablesInAction.end();++i) {
            (*i)->actions.tick();
        }
    }
    mayor.log << ">" << std::endl;
}

void MayorImpl::frame()
{
    mayor.log << "Mayor.Frame" << std::endl;
    mayor.log << "<Frame " << std::flush;
    if(alive) {
        mayor.log << "| alive" << std::flush;
        frame_timer.expires_at(tick_timer.expires_at() + boost::posix_time::milliseconds(FRAME));
        frame_timer.async_wait(boost::bind(&MayorImpl::frame, this));

        if(render.run()) render.draw();
        mayor.log << "| draw " << std::flush;
    }
    mayor.log << ">" << std::endl;
}

Render& Mayor::getRender() { return d->render; }
Locatable* Mayor::getPlayer() {
    return getLocatables().get(d->localPlayer);
}

SceneNodeContainer& Mayor::getSceneNodes() { return d->sceneNodes; }
LocatableContainer& Mayor::getLocatables() { return d->locatables; }
MapContainer& Mayor::getMaps() { return d->maps; }
OptionListContainer& Mayor::getOptionLists() { return d->optionLists;}


LocatablesInAction& Mayor::getLocatablesInAction() { return d->locatablesInAction; }

/*
void Mayor::getTileOptionList(Tile& tile) {
    if(tile.getPosition().map) {
        reg<Map> map = Library::getMap(tile.getPosition().map);
        GetTileOptionList msg;
        msg.set_map(map.id());
        msg.set_x(tile.getPosition().x);
        msg.set_y(tile.getPosition().y);
        client.write<GET_TILE_OPTIONLIST>(msg);
    }
}*/

void Mayor::getLocatableOptionList(int id) {
    d->write(Json::Object(Json::Val("msg",GET_LOCATABLE_OPTIONLIST),Json::Val("id",id)));
}

void Mayor::walkTo(float x, float y) {
    d->write(Json::Object(Json::Val("msg",WALK_TO),Json::Val("x",x),Json::Val("y",y)));
}

void Mayor::chooseOption(int optionlist,int id) {
    d->write(Json::Object(Json::Val("msg",CHOOSE_OPTION),Json::Val("optionlist",optionlist),Json::Val("id",id)));
}

void Mayor::getLocatablePosition(int id) {
    d->write(Json::Object(Json::Val("msg",GET_LOCATABLE_POSITION),Json::Val("id",id)));
}

void Mayor::getLocatableIntroduction(int id) {
    d->write(Json::Object(Json::Val("msg",GET_LOCATABLE_INTRODUCTION),Json::Val("id",id)));
    getLocatablePosition(id);
}

void Mayor::requestMapData(int mapid) {
    d->write(Json::Object(Json::Val("msg",REQUEST_MAP),Json::Val("id",mapid)));
}

void Mayor::receiveMessage(const std::string& msg) {
}

void Mayor::receiveLocatableOptionList(int id,OptionList* optionlist) {
    Locatable* locatable = getLocatables().get(id);
    if(locatable) {
        getRender().processOptionList(locatable,optionlist);
    } else {
        getLocatableIntroduction(id);
    }
}

void Mayor::moveTo(int id,float x, float y) {
    irr::scene::ISceneNode* sceneNode = getSceneNodes().get(id);
    Locatable* locatable = getLocatables().get(id);
    if(sceneNode && locatable) {
        getRender().moveSceneNode(locatable,sceneNode,x,y);
    } else {
        getLocatableIntroduction(id);
    }
}


void Mayor::onRequestCredentials() {
    std::string username,password;
    std::cout << "Username: ";
    getline(std::cin,username);
    std::cout << "Password: ";
    getline(std::cin,password);
    d->write(Json::Object(Json::Val("msg",CREDENTIALS),Json::Val("username",username),Json::Val("password",password)));
}


void Mayor::onLoginUsernameInvalid() {
    log << "Username invalid" << std::endl;
}

void Mayor::onLoginPasswordInvalid() {
    log << "Password invalid" << std::endl;
}

void Mayor::onLoginValid() {
    log << "Login successful!" << std::endl;
}

void Mayor::youAre(int yourId) {
    d->localPlayer = yourId;
    d->write(Json::Object(Json::Val("msg",GET_LOCATABLE_INTRODUCTION),Json::Val("id",yourId)));
    log << "Local player: " << yourId << std::endl;
}

void Mayor::setLocatablePosition(int id, int mapid, float x, float y) {
    log << "Set Locatable Position" << std::endl;
    Locatable* locatable = getLocatables().get(id);
    if(locatable) {
        log << "Locatable exists" << std::endl;
        Map* map = getMaps().get(mapid);
        if(map) {
            log << "Map exists" << std::endl;
            log << id << "," << d->localPlayer << std::endl;
            Map* oldMap = locatable->getPosition().map;
            locatable->setPosition(Position(x,y,map));
            log << "Locatable repositioned" << std::endl;
            irr::scene::ISceneNode* node = getSceneNodes().get(id);
            if(node) {
                log << "Is scene node" << std::endl;
                getRender().moveSceneNode(locatable,node,x,y);
                log << "Scene node moved" << std::endl;
            }
            if(id == d->localPlayer && oldMap != map) {
                log << "Render new map" << std::endl;
                getRender().loadMap(map);
            }
        } else {
            log << "Map doesnt exist" << std::endl;
            requestMapData(mapid);
            getLocatablePosition(id);
        }
    } else {
        log << "Locatable " << id << " doesnt exist" << std::endl;
        getLocatableIntroduction(id);
    }
    log << "Done"<< std::endl;
}

void Mayor::introduceLocatable(int id, const std::string& name) {
    Locatable* locatable = getLocatables().get(id);
    if(!locatable)
        getLocatables().create(id,name);
}

void Mayor::setLocatableModel(int id, const std::string& model, const std::string& texture) {
    irr::scene::ISceneNode* node = getSceneNodes().get(id);
    if(!node) {
        node = getRender().loadAnimatedMesh(model,texture);
        getSceneNodes().create(id,node);
        getLocatablePosition(id);
    }
    if(id == d->localPlayer) getRender().setCameraTarget(node);
}

void Mayor::sendMapData(int id,const std::string& path, const std::string& heightmap, int width, int height) {
    Map* map = getMaps().get(id);
    if(!map)
        getMaps().create(id,new Map(id,path,heightmap,width,height));
}


}

