#include "world/locatable.hpp"
#include "world/tile.hpp"
#include "world/map.hpp"
#include "world/optionlist.hpp"
#include "misc/irrlicht.hpp"
#include "world/library.hpp"
#include "mayor.hpp"

namespace Ennovia
{


irr::ITimer*& timer()
{
    static irr::ITimer* myTimer = 0;
    return myTimer;
}

Locatable::Locatable(const Position& pos, const std::string& name_, const std::string& model, const std::string& texture)
    : actionEnd(0), name(name_), model_(model), texture_(texture)
{
}

Locatable::Locatable(const Position& pos, const std::string& name_) : actionEnd(0), name(name_)
{
}

Locatable::~Locatable()
{
}


std::string Locatable::getName()
{
    return name;
}

std::string Locatable::onExamine(Locatable* o)
{
    std::cout << "examining" << std::endl;
    return name;
}

OptionList* Locatable::getOptionList(Locatable* p)
{
    int id = Mayor::get().getLocatables().get(this);
    OptionList* optionlist = Mayor::get().getOptionLists().get(id);
    if(!optionlist) {
        optionlist = new OptionList();
        Mayor::get().getOptionLists().create(id,optionlist);
    }
    return optionlist;
}

void Locatable::setPosition(const Position& position)
{
    //bool change = !(position==position_);
    //if(change && position_.isValid())
    //    position_.getTile()->leave(this);
    if(position.map != position_.map)
    {
        if(position_.map) {
            Mayor::get().log << "Erase locatable from map" << std::endl;
            position_.map->locatables.erase(this);
        }
        if(position.map) {
            Mayor::get().log << "Add locatable to map" << std::endl;
            position.map->locatables.insert(this);
        }
    }
    position_ = position;
    //if(change && position_.isValid())
    //    position_.getTile()->enter(this);
}



}
