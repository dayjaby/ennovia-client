#ifndef ENNOVIA_RENDER_HPP
#define ENNOVIA_RENDER_HPP

#include <memory>
#include <vector>
#include "misc/irrlicht.hpp"
#include "world/position.hpp"

namespace Ennovia {
    class Map;
    class Mayor;
    class Locatable;
    class OptionList;
    struct RenderImpl;
    class Render {
    public:
        Render(int width_, int height_, Mayor& mayor_);
        ~Render();
        void loadMap(Map* map);
        void setMap(irr::scene::ISceneNode* node, irr::scene::ISceneNode* map);

        void processOptionList(Locatable* locatable,OptionList* optionlist);
        irr::scene::ISceneNode* loadAnimatedMesh(const std::string& path, const std::string& texture);
        irr::video::ITexture* getTexture(const std::string& texture);
        void setCameraTarget(irr::scene::ISceneNode* node);
        void setAnimation(irr::scene::ISceneNode* node,irr::scene::EMD2_ANIMATION_TYPE animation);
        void moveSceneNode(Locatable* locatable,irr::scene::ISceneNode* node,float x, float y);
        float getMapHeight(float x, float y);
        void draw();
        bool run();

        void moveMouse(int x,int y,bool left,bool right);
        void updateOptionList();
        void mouseLeftClick(int x,int y,bool ctrl);
        void mouseRightClick(int x,int y);
        void moveCamera(float x,float y,float z);
        irr::gui::IGUIEnvironment* getGUIEnvironment();
        irr::video::IVideoDriver* getVideoDriver();
        irr::gui::IGUIContextMenu* getOptionListContextMenu();
        void setOptionListContextMenu(irr::gui::IGUIContextMenu* olcm);

        irr::core::rect<irr::s32> getTopLeftRect(int w,int h);
        irr::core::rect<irr::s32> getTopRightRect(int w,int h);
        irr::core::rect<irr::s32> getBottomLeftRect(int w,int h);
        irr::core::rect<irr::s32> getBottomRightRect(int w,int h);
    private:
        Position getPositionUnderMouse(int mx,int my);
        void getLocatablesUnderMouse(std::vector<Locatable*>* locatables,int mx,int my);
        void processLeftClick();
        void processCtrlLeftClick();

        std::auto_ptr<RenderImpl> d;


    };

    #define RENDER Mayor::get().getRender()


}

#endif
