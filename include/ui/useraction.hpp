#ifndef ENNOVIA_USERACTION_HPP
#define ENNOVIA_USERACTION_HPP

#include "misc/irrlicht.hpp"
#include <map>

namespace Ennovia {
    struct UserAction {
        void perform(SEvent& event) {}
        bool hold() { return false; }
    };

    class UserActionFactory {
    public:
        UserActionFactory();
        ~UserActionFactory();

        static UserActionFactory& get();
        static UserAction* create(const std::string& name);

        std::map<std::string,UserAction*> actions;
    };

    struct UserInput {
        UserInput(SEvent& event);
    };
}

#endif // ENNOVIA_USERACTION_HPP
