#ifndef ENNOVIA_MAYOR_HPP
#define ENNOVIA_MAYOR_HPP

#include <fstream>
#include <memory>
#include <set>

namespace Ennovia {
struct MayorImpl;
class Render;
class Player;
class Locatable;
class OptionList;
class LocatableContainer;
class OptionListContainer;
class SceneNodeContainer;
class MapContainer;

typedef std::set<Locatable*> LocatablesInAction;
class Mayor {
public:
    Mayor();

    static Mayor& get() {
        static Mayor instance;
        return instance;
    }

    void run();
    /** tick() is called once every 0.1 seconds **/
    int getTicks();

    Render& getRender();
    Locatable* getPlayer();

    LocatablesInAction& getLocatablesInAction();

    /** container accessors **/
    SceneNodeContainer& getSceneNodes();
    LocatableContainer& getLocatables();
    MapContainer& getMaps();
    OptionListContainer& getOptionLists();

    /** Interface for the view **/

    void sendMessage(const std::string& msg);
    void getLocatableOptionList(int id);
    void walkTo(float x, float y);
    void chooseOption(int optionlist,int id);
    void getLocatablePosition(int id);
    void getLocatableIntroduction(int id);
    void login(const std::string& username, const std::string& password);
    // void sendAESKey...
    void requestMapData(int mapid);
    // ...

    /** Interface for the network **/
    void receiveMessage(const std::string& msg);
    void receiveLocatableOptionList(int id,OptionList* optionlist);
    void moveTo(int id,float x,float y);
    void onRequestCredentials();
    void onLoginUsernameInvalid();
    void onLoginPasswordInvalid();
    void onLoginValid();
    // void sendRSAKey...
    void youAre(int yourId);
    void setLocatablePosition(int id, int mapid, float x, float y);
    void introduceLocatable(int id, const std::string& name);
    void setLocatableModel(int id, const std::string& model, const std::string& texture);
    void sendMapData(int id,const std::string& path, const std::string& heightmap, int width, int height);

    std::ofstream log;

private:
    std::auto_ptr<MayorImpl> d;

};


}

#endif

