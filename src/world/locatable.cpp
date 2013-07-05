#include "world/locatable.hpp"
#include "world/tile.hpp"
#include "world/map.hpp"
#include "world/optionlist.hpp"
#include "misc/irrlicht.hpp"

namespace Ennovia
{


irr::ITimer*& timer()
{
    static irr::ITimer* myTimer = 0;
    return myTimer;
}

Locatable::Locatable(const Position& pos, const std::string& name_, const std::string& model, const std::string& texture)
    : actionEnd(0), name(name_), model_(model), texture_(texture), inventory_(this)
{
    //setPosition(pos);
}

Locatable::Locatable(const Position& pos, const std::string& name_) : actionEnd(0), name(name_), inventory_(this)
{
    //setPosition(pos);
}

Locatable::~Locatable()
{
    //if(position_.map)
    //    position_.map->getTile(position_.x,position_.y).leave(this);
}


std::string Locatable::getName()
{
    return name;
}

std::string Locatable::onExamine(LivingObject* o)
{
    std::cout << "examining" << std::endl;
    return name;
}



}
