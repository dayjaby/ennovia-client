#include <boost/tuple/tuple.hpp>
#include "misc/CGUITTFont.h"
#include "ui/render.hpp"
#include "ui/graphics.hpp"
#include "ui/eventreceiver.hpp"
#include "ui/guiinventory.hpp"
#include "world/position.hpp"
#include "world/optionlist.hpp"
#include "world/map.hpp"
#include "world/library.hpp"
#include "world/locatable.hpp"
#include "mayor.hpp"

namespace Ennovia
{

struct RenderImpl
{
    RenderImpl(int width_, int height_,Mayor& mayor_) :
        width(width_), height(height_), mayor(mayor_), optionListContextMenu(0), currentMap(0)
    {
        device = createDevice(video::EDT_OPENGL, dimension2d<u32>(width,height),16,false,false,false);


        guienv = device->getGUIEnvironment();
        eventReceiver.gui = guienv;
        driver = device->getVideoDriver();
        smgr = device->getSceneManager();
        collision = smgr->getSceneCollisionManager();
        device->getFileSystem()->addFileArchive("media");
        device->setEventReceiver(&eventReceiver);
        moveCamera(0,0,0);

        camera = smgr->addCameraSceneNode(0,vector3df(0,20.f,0),vector3df(25,0,25));

        entityParent = smgr->addEmptySceneNode();
        std::cout << "Render constructed" << std::endl;
    }

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

    /** private **/
    Position getPositionUnderMouse(int mx,int my);
    void getLocatablesUnderMouse(std::vector<Locatable*>* locatables,int mx,int my);
    void processLeftClick();
    void processCtrlLeftClick();
    /** end private **/

    Position lastMousePosition;
    irr::core::position2di lastMouseLocation;

    Mayor& mayor;

    int width, height;
    EventReceiver eventReceiver;
    IrrlichtDevice* device;
    irr::gui::IGUIEnvironment* guienv;
    irr::video::IVideoDriver* driver;
    irr::scene::ISceneManager* smgr;
    scene::ISceneCollisionManager* collision;

    float cameraHeight, cameraAngle, cameraDistance;
    ISceneNode* cameraTarget;

    scene::ICameraSceneNode* camera;
    scene::ISceneNode* entityParent;
    ISceneNode* map;

    Map* currentMap;
    std::map<std::string,irr::video::ITexture*> textures;

    OptionList optionList;
    irr::gui::IGUIContextMenu* optionListContextMenu;
    std::map<Locatable*,OptionList*> optionLists;
    std::vector<Locatable*> leftClick;
    std::vector<Locatable*> ctrlLeftClick;

    irr::gui::IGUIWindow* inventory;
};


void RenderImpl::draw()
{
    video::SMaterial material;
    material.Lighting = false;
    material.Wireframe = false;
    mayor.log << "Render Draw " << std::endl;
    if(cameraTarget)
    {
        mayor.log << "CT" << std::flush;
        float theta = cameraHeight*PI/180;
        float phi = cameraAngle*PI/180;
        camera->setPosition(cameraTarget->getPosition()
                            +vector3df(sin(theta)*cos(phi)*cameraDistance,
                                       cos(theta)*cameraDistance,
                                       sin(theta)*sin(phi)*cameraDistance));
        camera->setTarget(cameraTarget->getPosition());
    }
    mayor.log << "Get Player " << std::endl;

    /* reg<Player> player = mayor.getPlayer();
     if(!inventory && player.exists()) {
         inventory = guienv->addWindow(getBottomRightRect(192+8,300));
         new GUIInventory(inventory,player->inventory());
     }*/
    mayor.log << "Begin Scene" << std::endl;
    driver->beginScene(true,true,SColor(255,100,101,140));
    smgr->drawAll();
    guienv->drawAll();
    driver->endScene();
    driver->setMaterial(material);
}

bool RenderImpl::run()
{
    return device->run();
}

void RenderImpl::moveMouse(int x,int y,bool left,bool right)
{
    if(currentMap)
    {
        Position pos = getPositionUnderMouse(x,y);
        mayor.log << "Mouse moved and position fetched!" << std::endl;
        lastMousePosition = pos;
        lastMouseLocation = core::position2di(x,y);
    }
}

void RenderImpl::updateOptionList()
{
    if(optionList.options.size()>0)
    {
        optionListContextMenu = guienv->addContextMenu(core::rect<s32>(lastMouseLocation,core::dimension2di(250,200)),0,-2);
        for(int i=0; i<optionList.options.size(); i++)
        {
            optionListContextMenu->addItem(core::stringw(optionList.options[i]->getDescription().c_str()).c_str());
        }
    }
}

void RenderImpl::mouseLeftClick(int x,int y,bool ctrl)
{
    optionList.options.clear();
    irr::gui::IGUIElement* element = guienv->getRootGUIElement()->getElementFromPoint(irr::core::position2d<s32>(x,y));
    if(element && element->getParent() && element->getID()>0)
    {
        switch(element->getID())
        {
        case 1: //  Item
            /*GUIDataElement<Item>* itemData = static_cast<GUIDataElement<Item>*>(element);
            Item* item = itemData->getData();
            OptionList* ol = item->getOptionList(mayor.getPlayer());
            if(ol) optionList.addList(*ol);
            updateOptionList();
            */
            break;
        }
    }
    else if(element && element->getID()<0)
    {
        switch(element->getID())
        {
        case -2:
            IGUIContextMenu* contextMenu = (IGUIContextMenu*) element;
            Locatable* player = mayor.getPlayer();
            optionList.options[contextMenu->getSelectedItem()]->onChoose(player);
            leftClick.clear();
            ctrlLeftClick.clear();
            break;
        }
    }
    else
    {
        if(ctrl)
        {
            getLocatablesUnderMouse(&ctrlLeftClick,x,y);
            processCtrlLeftClick();
        }
        else
        {
            getLocatablesUnderMouse(&leftClick,x,y);
            processLeftClick();
        }
    }
}

void RenderImpl::mouseRightClick(int x,int y)
{
    mayor.log << "Mouse right click" << std::endl;
    //getOptionListsUnderMouse(x,y);
    Position end = getPositionUnderMouse(x,y);
    Locatable* player = mayor.getPlayer();
    mayor.log << "Does the player exist?" << std::endl;
    if(!player) return;
    mayor.log << "Yes he does!" << std::endl;
    Position start = player->getPosition();
    if(end.isValid())
    {
        core::line3d<f32> ray;
        ray.start = vector3df(start.x,getMapHeight(start.x,start.y)+0.1,start.y);
        float height = getMapHeight(end.x,end.y);
        ray.end = vector3df(end.x,height+0.1,end.y);
        core::vector3df intersection;
        core::triangle3df hitTriangle;
        if(!smgr->getSceneCollisionManager()->
                getSceneNodeAndCollisionPointFromRay(ray,
                        intersection,
                        hitTriangle,
                        4))
        {
            mayor.walkTo(end.x,end.y);
            //reg<ISceneNode> node(player.id());
            //if(node.exists())
            //    walkTo(node.get(),end.x,height,end.y);
        }
    }
}

void RenderImpl::moveCamera(float x,float y,float z)
{
    cameraAngle += x;
    cameraHeight += y;
    cameraDistance += z;
    if(cameraHeight<1) cameraHeight = 1;
    if(cameraHeight>60) cameraHeight = 60;
    if(cameraDistance<1) cameraDistance = 1;
    if(cameraDistance>30) cameraDistance = 30;
}

irr::gui::IGUIEnvironment* RenderImpl::getGUIEnvironment()
{
    return guienv;
}
irr::video::IVideoDriver* RenderImpl::getVideoDriver()
{
    return driver;
}
irr::gui::IGUIContextMenu* RenderImpl::getOptionListContextMenu()
{
    return optionListContextMenu;
}
void RenderImpl::setOptionListContextMenu(irr::gui::IGUIContextMenu* olcm)
{
    optionListContextMenu = olcm;
}


irr::core::rect<s32> RenderImpl::getTopLeftRect(int w,int h)
{
    return irr::core::rect<s32>(0,0,w,h);
}
irr::core::rect<s32> RenderImpl::getTopRightRect(int w,int h)
{
    return irr::core::rect<s32>(width-w,0,width,h);
}
irr::core::rect<s32> RenderImpl::getBottomLeftRect(int w,int h)
{
    return irr::core::rect<s32>(0,height-h,w,height);
}
irr::core::rect<s32> RenderImpl::getBottomRightRect(int w,int h)
{
    return irr::core::rect<s32>(width-w,height-h,width,height);
}

Position RenderImpl::getPositionUnderMouse(int mx,int my)
{
    mayor.log << "Get position under mouse" << std::endl;
    core::line3df ray =
        collision->getRayFromScreenCoordinates(core::position2di(mx,my),
                camera);
    mayor.log << "Get position under mouse" << std::endl;
    core::vector3df intersection;
    core::triangle3df hitTriangle;
    if(currentMap)
    {
        mayor.log << "Ray calculated " << currentMap << std::endl;
        mayor.log << "Ray SceneNode Collision" << std::endl;
        if(collision->getSceneNodeAndCollisionPointFromRay(ray,intersection,hitTriangle,1))
        {
            float x = intersection.X;
            float y = intersection.Z;
            mayor.log << "Finished" << std::endl;
            return Position(x,y,currentMap);
        }
    }
    return Position();
}

void RenderImpl::getLocatablesUnderMouse(std::vector<Locatable*>* locatables,int mx,int my)
{
    mayor.log << "Get Locatables under mouse" << std::endl;
    core::line3df ray =
        collision->getRayFromScreenCoordinates(core::position2di(mx,my),
                camera);
    core::vector3df intersection;
    core::triangle3df hitTriangle;
    std::set<Locatable*> locatableSet;
    while(ISceneNode* node = collision->getSceneNodeAndCollisionPointFromRay(ray,intersection,hitTriangle,2,entityParent))
    {
        int id = mayor.getSceneNodes().get(node);
        Locatable* locatable = mayor.getLocatables().get(id);
        if(locatable)
        {
            if(locatableSet.count(locatable)==0)
            {
                mayor.getLocatableOptionList(id);
                optionLists.erase(locatable);
            }
            locatableSet.insert(locatable);

        }
        ray.start = intersection + ROUNDING_ERROR_f32 * (ray.end - ray.start);
    }
    if(locatables)
        *locatables = std::vector<Locatable*>(locatableSet.begin(),locatableSet.end());
    if(currentMap)
    {
        if(collision->getSceneNodeAndCollisionPointFromRay(ray,intersection,hitTriangle,1))
        {
            int x = intersection.X;
            int y = intersection.Z;
            Position pos(x,y,currentMap);
            if(pos.isValid())
            {
                /*                Tile* tile = pos.getTile();
                                if(locatables)
                                    locatables->push_back(tile);
                                mayor.getTileOptionList(*tile);*/
            }
        }
    }
}


void RenderImpl::loadMap(Map* newmap)
{
    mayor.log << "Load map" << std::endl;
    if(!newmap) return;
    currentMap = newmap;
    int mwidth = newmap->width, mheight = newmap->height;
    scene::IAnimatedMesh* mesh = smgr->getMesh(newmap->path.c_str());
    //scene::IAnimatedMesh* mesh = smgr->getMesh("media/tree_scene.3ds");
    if (mesh)
    {
        mayor.log << "Mesh loaded" << std::endl;
        map = smgr->addOctreeSceneNode(mesh->getMesh(0), 0, 1, 1024);
        map->setMaterialFlag(video::EMF_LIGHTING, false);
        ITriangleSelector* selector = smgr->createOctreeTriangleSelector(
                                          mesh, map, 128);
        map->setTriangleSelector(selector);
        selector->drop();
        for(int x=0; x<mwidth+1; x++)
        {
            for(int y=0; y<mheight+1; y++)
            {
                currentMap->heightmap[x][y] = (x*x+y*y)/150.0;
            }
        }
        if(cameraTarget) mayor.log << "camera target exists" << std::endl;
        int id = mayor.getSceneNodes().get(cameraTarget);
        if(cameraTarget && id)
        {
            mayor.log << "camera target is scene node" << std::endl;
                mayor.log << "camera target is locatable" << std::endl;
                Locatable* locatable = mayor.getLocatables().get(id);
                if(locatable && locatable->getPosition().map == currentMap)
                {
                        mayor.log << "camera target is on this map" << std::endl;
                        float x = locatable->getPosition().x;
                        float y = locatable->getPosition().y;
                        moveSceneNode(locatable,cameraTarget,x,y);
                        setMap(cameraTarget,map);
                }
        }
        mayor.log << "after?" << std::endl;
    }
}

void RenderImpl::setMap(irr::scene::ISceneNode* node, irr::scene::ISceneNode* map)
{
    mayor.log << "SET MAP" << std::endl;
    const core::aabbox3d<f32>& box = node->getBoundingBox();
    core::vector3df radius = box.MaxEdge - box.getCenter();
    scene::ISceneNodeAnimator* anim = smgr->createCollisionResponseAnimator(
                                          map->getTriangleSelector(), node, radius);
    node->addAnimator(anim);
    anim->drop();

}

void RenderImpl::processOptionList(Locatable* locatable,OptionList* optionlist)
{
    optionLists[locatable] = optionlist;
    typedef OptionList::Options::iterator Iterator;
    int index = 0;
    for(Iterator i = optionlist->options.begin(); i != optionlist->options.end(); ++i,++index)
    {
        mayor.log << index << ": " << (*i)->getDescription();
    }

    if(std::find(leftClick.begin(),leftClick.end(),locatable) != leftClick.end()) processLeftClick();
    if(std::find(ctrlLeftClick.begin(),ctrlLeftClick.end(),locatable) != ctrlLeftClick.end()) processCtrlLeftClick();
}

void RenderImpl::processLeftClick()
{
    typedef std::vector<Locatable*>::iterator Iterator;
    mayor.log << "elements=" << leftClick.size() << std::endl;
    for(Iterator i = leftClick.begin(); i != leftClick.end(); ++i)
    {
        Locatable* locatable = *i;
        if(optionLists.count(locatable)>0)
        {
            mayor.log << "contains optionlist" << std::endl;
            OptionList* optionList = optionLists[locatable];
            if(optionList->options.size()>0)
            {
                mayor.log << "contains option" << std::endl;
                Locatable* player = mayor.getPlayer();
                optionList->options[0]->onChoose(player);
                leftClick.clear();
                ctrlLeftClick.clear();
                return;
            }
        }
        else
        {
            return;
        }
    }
}

void RenderImpl::processCtrlLeftClick()
{
    optionList.options.clear();
    typedef std::vector<Locatable*>::iterator Iterator;
    mayor.log << "elements=" << ctrlLeftClick.size() << std::endl;
    for(Iterator i = ctrlLeftClick.begin(); i != ctrlLeftClick.end(); ++i)
    {
        Locatable* locatable = *i;
        if(optionLists.count(locatable)==0)
        {
            return;
        }
    }
    for(Iterator i = ctrlLeftClick.begin(); i != ctrlLeftClick.end(); ++i)
    {
        Locatable* locatable = *i;
        //if(optionLists.count(locatable)>0) {
        mayor.log << "contains optionlist" << std::endl;
        OptionList* ol = optionLists[locatable];
        optionList.addList(*ol);

        //} else {
        //    return;
        //}
    }
    updateOptionList();
    leftClick.clear();
    ctrlLeftClick.clear();

}

irr::scene::ISceneNode* RenderImpl::loadAnimatedMesh(const std::string& path, const std::string& texture)
{
    mayor.log << path << std::endl;
    irr::scene::IAnimatedMeshSceneNode* mesh = smgr->addAnimatedMeshSceneNode(smgr->getMesh(path.c_str()),entityParent,2,core::vector3df(0,0,0));
    if(mesh)
    {
        mayor.log << "create tri selector" << std::endl;
        ITriangleSelector* selector = smgr->createTriangleSelectorFromBoundingBox(mesh);
        if(map)
        {
            mayor.log << "set map" << std::endl;
            setMap(mesh,map);
        }
        mesh->setMD2Animation(scene::EMAT_STAND);
        mesh->setMaterialTexture(0,getTexture(texture));
        if(path.compare("faerie.md2")==0)
            mesh->setScale(vector3df(0.02,0.02,0.02));
        mesh->setMaterialFlag(video::EMF_LIGHTING,false);
        mesh->setTriangleSelector(selector);
        mayor.log << "done" << std::endl;
        selector->drop();
        return mesh;
    }
    else
    {
        return 0;
    }
}

irr::video::ITexture* RenderImpl::getTexture(const std::string& texture)
{
    return driver->getTexture(texture.c_str());
}
void RenderImpl::setCameraTarget(ISceneNode* node)
{
    mayor.log << "Set camera target" << std::endl;
    if(node) cameraTarget = node;
    getMapHeight(1,1);
}

void RenderImpl::setAnimation(ISceneNode* node,EMD2_ANIMATION_TYPE animation)
{
    mayor.log << "Set animation" << std::endl;
    irr::scene::IAnimatedMeshSceneNode* mnode = (irr::scene::IAnimatedMeshSceneNode*)node;
    mnode->setMD2Animation(animation);
}

void RenderImpl::moveSceneNode(Locatable* locatable,irr::scene::ISceneNode* node,float x, float y)
{
    mayor.log << "Move scene node" << std::endl;
    getMapHeight(x,y);
    irr::scene::IAnimatedMeshSceneNode* mnode = (irr::scene::IAnimatedMeshSceneNode*)node;
    mayor.log << "map height " << x << "," << y << " = " << getMapHeight(x,y) << std::endl;
    if(node->getPosition().X == 0 && node->getPosition().Z == 0)
        node->setPosition(vector3df(x,getMapHeight(x,y)+0.5,y));
    else if(locatable)
    {
        Position currentPos(node->getPosition().X,node->getPosition().Z,locatable->getPosition().map);
        Position targetPos(x,y,locatable->getPosition().map);
        Move* move = new Move(locatable,currentPos,targetPos);
        mayor.getLocatablesInAction().insert(locatable);
        locatable->actions.clear();
        locatable->actions.add(move);
        mayor.log << (targetPos.x - currentPos.x) << "|" << (targetPos.y - currentPos.y);
        mnode->setMD2Animation(irr::scene::EMAT_RUN);
        vector3df myRotation;
        myRotation.Y = std::atan2(-move->getDY(),move->getDX()) * (180.0f / irr::core::PI);
        node->setRotation(myRotation);
        vector3df target = vector3df(x,getMapHeight(x,y)+0.5,y);
        mayor.log << "Target = " << target.X << "," << target.Y << "," << target.Z << std::endl;
        irr::scene::ISceneNodeAnimator* anim =
            node->getSceneManager()->createFlyStraightAnimator(node->getPosition(),
                    target,move->getDuration()*100);
        if(anim)
        {
            mayor.log << "animated !!!!!!!!!!" << std::endl;
            node->addAnimator(anim);
            //anim->drop();
            anim = 0;
        }
    }
}

float RenderImpl::getMapHeight(float x, float y)
{
    core::line3d<f32> ray;
    ray.start = vector3df(x,3,y);
    ray.end = vector3df(x,-3,y);
    core::vector3df intersection;
    core::triangle3df hitTriangle;
    smgr->getSceneCollisionManager()->
    getSceneNodeAndCollisionPointFromRay(ray,
                                         intersection,
                                         hitTriangle,
                                         1);

    return (hitTriangle.pointA.Y + hitTriangle.pointB.Y + hitTriangle.pointC.Y) / 3;

}

Render::Render(int width_, int height_,Mayor& mayor_) :
    d(new RenderImpl(width_,height_,mayor_))
{
}

Render::~Render()
{
    if(d->map) d->map->drop();
    d->device->drop();
}
void Render::loadMap(Map* map)
{
    d->loadMap(map);
}
void Render::setMap(irr::scene::ISceneNode* node, irr::scene::ISceneNode* map)
{
    d->setMap(node,map);
}
void Render::processOptionList(Locatable* locatable,OptionList* optionlist)
{
    d->processOptionList(locatable,optionlist);
}
irr::scene::ISceneNode* Render::loadAnimatedMesh(const std::string& path, const std::string& texture)
{
    return d->loadAnimatedMesh(path,texture);
}
irr::video::ITexture* Render::getTexture(const std::string& texture)
{
    return d->getTexture(texture);
}
void Render::setCameraTarget(irr::scene::ISceneNode* node)
{
    d->setCameraTarget(node);
}
void Render::setAnimation(irr::scene::ISceneNode* node,irr::scene::EMD2_ANIMATION_TYPE animation)
{
    d->setAnimation(node,animation);
}
void Render::moveSceneNode(Locatable* locatable,irr::scene::ISceneNode* node,float x, float y)
{
    d->moveSceneNode(locatable,node,x,y);
}
float Render::getMapHeight(float x, float y)
{
    return d->getMapHeight(x,y);
}
void Render::draw()
{
    d->draw();
}
bool Render::run()
{
    return d->run();
}

void Render::moveMouse(int x,int y,bool left,bool right)
{
    d->moveMouse(x,y,left,right);
}
void Render::updateOptionList()
{
    d->updateOptionList();
}
void Render::mouseLeftClick(int x,int y,bool ctrl)
{
    d->mouseLeftClick(x,y,ctrl);
}
void Render::mouseRightClick(int x,int y)
{
    d->mouseRightClick(x,y);
}
void Render::moveCamera(float x,float y,float z)
{
    d->moveCamera(x,y,z);
}
irr::gui::IGUIEnvironment* Render::getGUIEnvironment()
{
    return d->getGUIEnvironment();
}
irr::video::IVideoDriver* Render::getVideoDriver()
{
    return d->getVideoDriver();
}
irr::gui::IGUIContextMenu* Render::getOptionListContextMenu()
{
    return d->getOptionListContextMenu();
}
void Render::setOptionListContextMenu(irr::gui::IGUIContextMenu* olcm)
{
    return d->setOptionListContextMenu(olcm);
}

irr::core::rect<irr::s32> Render::getTopLeftRect(int w,int h)
{
    return d->getTopLeftRect(w,h);
}
irr::core::rect<irr::s32> Render::getTopRightRect(int w,int h)
{
    return d->getTopRightRect(w,h);
}
irr::core::rect<irr::s32> Render::getBottomLeftRect(int w,int h)
{
    return d->getBottomLeftRect(w,h);
}
irr::core::rect<irr::s32> Render::getBottomRightRect(int w,int h)
{
    return d->getBottomRightRect(w,h);
}

}
