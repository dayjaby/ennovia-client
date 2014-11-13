#include "ui/useraction.hpp"
#include "ui/eventreceiver.hpp"
#include "mayor.hpp"

namespace Ennovia {
    struct RotateCamera : UserAction {
        RotateCamera(float xSpeed_, float ySpeed_, float zSpeed_) : xSpeed(xSpeed_), ySpeed(ySpeed_), zSpeed(zSpeed_) {
        }

        void perform(SEvent& event) {
            Mayor::get().getRender().moveCamera(xSpeed,ySpeed,zSpeed);
        }

        bool hold() { return true; }

        float xSpeed, ySpeed, zSpeed;
    };


    UserActionFactory::UserActionFactory() {
        actions["none"] = new UserAction();
        actions["RotateCameraLeft"] = new RotateCamera(-cameraXSpeed,0,0);
        actions["RotateCameraRight"] = new RotateCamera(+cameraXSpeed,0,0);
        actions["RotateCameraUp"] = new RotateCamera(0,-cameraYSpeed,0);
        actions["RotateCameraDown"] = new RotateCamera(0,+cameraYSpeed,0);
    }

    UserActionFactory::~UserActionFactory() {
        for(std::map<std::string,UserAction*>::iterator i=actions.begin();i!=actions.end();++i) {
            delete(i->second);
        }
    }

    UserActionFactory& UserActionFactory::get() {
        static UserActionFactory uaf;
        return uaf;
    }

    UserAction* UserActionFactory::create(const std::string& name) {
        std::map<std::string,UserAction*>::iterator i = get().actions.find(name);
        if(i != get().actions.end()) {
            return i->second;
        } else {
            return get().actions["none"];
        }
    }

}
