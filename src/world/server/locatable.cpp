#include "world/locatable.hpp"
#include "world/optionlist.hpp"
#include "world/position.hpp"
#include "world/map.hpp"
#include "governor.hpp"

namespace Ennovia {

    class LookAtOption : public Option {
    public:
        LookAtOption(Locatable* lo_) : lo(lo_) {}
        virtual ~LookAtOption() {}
        virtual std::string getDescription() {
            return "Look at";
        }
        virtual void onChoose(LivingObject* lo_) {
            reg<LivingObject> rlo(lo_);
            std::stringstream ss;
            ss << "You see a " << lo->getName();
            Governor::get().writeMessage(rlo.id(),ss.str());
        }
    private:
        Locatable* lo;
    };


    OptionList* Locatable::getOptionList(LivingObject* p)
    {
        OptionList* ol = new OptionList();
        ol->add(new LookAtOption(this));
        return ol;
    }

    void Locatable::setPosition(const Position& position)
    {
        //bool change = !(position==position_);
        //if(change && position_.isValid())
        //    position_.getTile()->leave(this);
        if(position.map != position_.map)
        {
            if(position_.map) {
                position_.map->locatables.erase(this);
            }
            if(position.map) {
                position.map->locatables.insert(this);
            }
            //Governor::get().updateLocatablePosition(this);
        }
        position_ = position;
        //if(change && position_.isValid())
        //    position_.getTile()->enter(this);
    }
}
