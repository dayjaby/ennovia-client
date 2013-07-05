#include "world/locatable.hpp"
#include "world/position.hpp"
#include "world/map.hpp"
#include "world/optionlist.hpp"
#include "mayor.hpp"

namespace Ennovia {
    OptionList* Locatable::getOptionList(LivingObject* p)
    {
        Library& library = Mayor::get().getLibrary();
        reg<Locatable> locatable = library.createLocatable(this);
        reg<OptionList> optionList = library.getOptionList(locatable.id());
        if(optionList.exists())
        {
            return optionList.get();
        }
        else
        {
            return library.createOptionList(new OptionList(),locatable.id()).get();
        }
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
