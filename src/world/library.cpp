#include "world/library.hpp"
#include "world/locatable.hpp"
#include "world/map.hpp"
#include "world/optionlist.hpp"
namespace Ennovia
{

SceneNodeContainer::SceneNodeContainer() {}
int SceneNodeContainer::create(int id,irr::scene::ISceneNode* node) {
    container.insert(container_type::value_type(id,node));
    return id;
}
irr::scene::ISceneNode* SceneNodeContainer::get(int id) {
    container_type::left_iterator i = container.left.find(id);
    if(i!=container.left.end()) {
        return i->second;
    } else {
        return 0;
    }
}
int SceneNodeContainer::get(irr::scene::ISceneNode* node) {
    container_type::right_iterator i = container.right.find(node);
    if(i!=container.right.end()) {
        return i->second;
    } else {
        return 0;
    }
}

bool SceneNodeContainer::exists(int id) {
    return get(id)!=0;
}
bool SceneNodeContainer::exists(irr::scene::ISceneNode* node) {
    return get(node)!=0;
}

LocatableContainer::LocatableContainer() : last_index(0) {}
int LocatableContainer::create(int id,const std::string& name) {
    container.insert(container_type::value_type(id,new Locatable(Position(),name)));
    return id;
}
int LocatableContainer::create(const std::string& name) {
    int new_id = ++last_index;
    container.insert(container_type::value_type(new_id,new Locatable(Position(),name)));
    return new_id;
}
int LocatableContainer::create(Locatable* loc) {
    int new_id = ++last_index;
    container.insert(container_type::value_type(new_id,loc));
    return new_id;
}
Locatable* LocatableContainer::get(int id) {
    container_type::left_iterator i = container.left.find(id);
    if(i!=container.left.end()) {
        return i->second;
    } else {
        return 0;
    }
}
int LocatableContainer::get(Locatable* loc) {
    container_type::right_iterator i = container.right.find(loc);
    if(i!=container.right.end()) {
        return i->second;
    } else {
        return 0;
    }
}

bool LocatableContainer::exists(int id) {
    return get(id)!=0;
}

MapContainer::MapContainer() {}

int MapContainer::create(int id,Map* map) {
    container.insert(container_type::value_type(id,map));
    return id;
}
Map* MapContainer::get(int id) {
    container_type::left_iterator i = container.left.find(id);
    if(i!=container.left.end()) {
        return i->second;
    } else {
        return 0;
    }
}
int MapContainer::get(Map* map) {
    container_type::right_iterator i = container.right.find(map);
    if(i!=container.right.end()) {
        return i->second;
    } else {
        return 0;
    }
}
bool MapContainer::exists(int id) {
    return get(id)!=0;
}

OptionListContainer::OptionListContainer() {}

int OptionListContainer::create(int id,OptionList* ol) {
    container.insert(container_type::value_type(id,ol));
    return id;
}
OptionList* OptionListContainer::get(int id) {
    container_type::left_iterator i = container.left.find(id);
    if(i!=container.left.end()) {
        return i->second;
    } else {
        return 0;
    }
}
int OptionListContainer::get(OptionList* ol) {
    container_type::right_iterator i = container.right.find(ol);
    if(i!=container.right.end()) {
        return i->second;
    } else {
        return 0;
    }
}
bool OptionListContainer::exists(int id) {
    return get(id)!=0;
}

}

