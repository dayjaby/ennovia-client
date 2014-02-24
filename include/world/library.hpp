#ifndef ENNOVIA_LIBRARY_HPP
#define ENNOVIA_LIBRARY_HPP

#include <boost/bimap.hpp>
#include "core.hpp"

namespace Ennovia {

class Locatable;
class LivingObject;
class Player;
class Map;
class OptionList;



class SceneNodeContainer {
public:
    SceneNodeContainer();
    typedef boost::bimap<int,irr::scene::ISceneNode*> container_type;

    int create(int id,irr::scene::ISceneNode* node);
    irr::scene::ISceneNode* get(int id);
    int get(irr::scene::ISceneNode* node);

    bool exists(int id);
    bool exists(irr::scene::ISceneNode* node);
private:
    container_type container;
};

class LocatableContainer {
public:
    LocatableContainer();
    typedef boost::bimap<int,Locatable*> container_type;

    int create(int id,const std::string& name);
    int create(const std::string& name);
    int create(Locatable* loc);
    Locatable* get(int id);
    int get(Locatable* loc);

    bool exists(int id);
private:
    int last_index;
    container_type container;
};

class MapContainer {
public:
    MapContainer();
    typedef boost::bimap<int,Map*> container_type;

    int create(int id,Map* map);
    Map* get(int id);
    int get(Map* map);

    bool exists(int id);
private:
    container_type container;
};

class OptionListContainer {
public:
    OptionListContainer();
    typedef boost::bimap<int,OptionList*> container_type;

    int create(int id,OptionList* ol);
    OptionList* get(int id);
    int get(OptionList* ol);

    bool exists(int id);
private:
    container_type container;
};

}


#endif
