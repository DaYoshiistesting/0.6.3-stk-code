//  $Id: irr_driver.cpp 694 2006-08-29 07:42:36Z hiker $
//
//  SuperTuxKart - a fun racing game with go-kart
//  Copyright (C) 2009 Joerg Henrichs
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the GNU General Public License
//  as published by the Free Software Foundation; either version 3
//  of the License, or (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

#include "graphics/irr_driver.hpp"

#ifdef HAVE_GLUT
#  ifdef __APPLE__
#    include <GLUT/glut.h>
#  else
#    include <GL/glut.h>
#  endif
#endif

#include "config/user_config.hpp"
#include "graphics/material_manager.hpp"
#include "guiengine/engine.hpp"
#include "states_screens/state_manager.hpp"
#include "io/file_manager.hpp"
#include "items/item_manager.hpp"
#include "items/powerup_manager.hpp"
#include "items/attachment_manager.hpp"
#include "items/projectile_manager.hpp"
#include "karts/kart_properties_manager.hpp"
#include "modes/world.hpp"

IrrDriver *irr_driver = NULL;

IrrDriver::IrrDriver()
{
    file_manager->dropFileSystem();

    initDevice();
}   // IrrDriver

// ----------------------------------------------------------------------------
IrrDriver::~IrrDriver()
{
    m_device->drop();
}   // ~IrrDriver
// ----------------------------------------------------------------------------

void IrrDriver::initDevice()
{
    static bool firstTime = true;

    // ---- the first time, get a list of available video modes
    if(firstTime)
    {
        m_device = createDevice(EDT_NULL);
        
        video::IVideoModeList* modes = m_device->getVideoModeList();
        const int count = modes->getVideoModeCount();
        //std::cout << "--------------\n  allowed modes  \n--------------\n";
        //std::cout << "Desktop depth : " << modes->getDesktopDepth()  << std::endl;
        //std::cout << "Desktop resolution : " << modes->getDesktopResolution().Width << "," << modes->getDesktopResolution().Height << std::endl;
        
        //std::cout << "Found " << count << " valid modes\n";
        for(int i=0; i<count; i++)
        {
            // only consider 32-bit resolutions for now
            if(modes->getVideoModeDepth(i) >= 24)
            {
                VideoMode mode;
                mode.width = modes->getVideoModeResolution(i).Width;
                mode.height = modes->getVideoModeResolution(i).Height;
                m_modes.push_back( mode );
            }
            
            //std::cout <<
            //"bits : " << modes->getVideoModeDepth(i) <<
            //" resolution=" << modes->getVideoModeResolution(i).Width <<
            //"x" << modes->getVideoModeResolution(i).Height << std::endl;
        }
        m_device->closeDevice();
        
        firstTime = false;
    } // end if firstTime
    
    // ---- open device
    // Try different drivers: start with opengl, then DirectX
    for(int driver_type=0; driver_type<3; driver_type++)
    {
        video::E_DRIVER_TYPE type = driver_type==0
                                  ? video::EDT_OPENGL
                                  : (driver_type==1
                                    ? video::EDT_DIRECT3D9
                                    : video::EDT_DIRECT3D8);
        // Try 32 and, upon failure, 24 then 16 bit per pixels
        for(int bits=32; bits>15; bits -=8)
        {
            m_device = createDevice(type,
                                    core::dimension2d<s32>(UserConfigParams::m_width,
                                                           UserConfigParams::m_height ),
                                    bits, //bits per pixel
                                    UserConfigParams::m_fullscreen,
                                    false,  // stencil buffers
                                    false,  // vsync
                                    this    // event receiver
                                    );
            if(m_device) break;
        }   // for bits=24, 16
        if(m_device) break;
    }   // for edt_types
    if(!m_device)
    {
        fprintf(stderr, "Couldn't initialise irrlicht device. Quitting.\n");
        exit(-1);
    }

    m_device->getVideoDriver()->setTextureCreationFlag(video::ETCF_CREATE_MIP_MAPS,true);

    // Stores the new file system pointer.
    file_manager->setDevice(m_device);
    m_device->setWindowCaption(L"SuperTuxKart");
    m_scene_manager = m_device->getSceneManager();
    m_gui_env       = m_device->getGUIEnvironment();
    m_video_driver  = m_device->getVideoDriver();
    const std::string &font = file_manager->getFontFile("DomesticManners.xml");
    m_race_font     = m_gui_env->getFont(font.c_str());
}

//-----------------------------------------------------------------------------
void IrrDriver::showPointer()
{
    this->getDevice()->getCursorControl()->setVisible(true);
}   // showPointer

//-----------------------------------------------------------------------------
void IrrDriver::hidePointer()
{
    this->getDevice()->getCursorControl()->setVisible(true);
}   // hidePointer

//-----------------------------------------------------------------------------
void IrrDriver::changeResolution()
{
    // show black before resolution switch so we don't see OpenGL's buffer garbage during switch
    m_device->getVideoDriver()->beginScene(true, true, video::SColor(255,100,101,140));
    m_device->getVideoDriver()->draw2DRectangle( SColor(255, 0, 0, 0),
                                                core::rect<s32>(0, 0,
                                                                UserConfigParams::m_prev_width,
                                                                UserConfigParams::m_prev_height) );
    m_device->getVideoDriver()->endScene();

    // startScreen             -> removeTextures();
    attachment_manager      -> removeTextures();
    projectile_manager      -> removeTextures();
    item_manager            -> removeTextures();
    kart_properties_manager -> removeTextures();
    powerup_manager         -> removeTextures();
    GUIEngine::cleanUp();

    m_device->closeDevice();
    m_device->drop();
    initDevice();

    material_manager->reInit();

    powerup_manager         -> loadPowerups();
    kart_properties_manager -> loadKartData();
    item_manager            -> loadDefaultItems();
    projectile_manager      -> loadData();
    attachment_manager      -> loadModels();

    // Re-init GUI engine
    IrrlichtDevice* device = irr_driver->getDevice();
    video::IVideoDriver* driver = device->getVideoDriver();
    GUIEngine::init(device, driver, StateManager::get());
    GUIEngine::reshowCurrentScreen();
}   // changeResolution

// ----------------------------------------------------------------------------
/** Loads an animated mesh and returns a pointer to it.
 *  \param filename File to load.
 */
scene::IAnimatedMesh *IrrDriver::getAnimatedMesh(const std::string &filename)
{
    scene::IAnimatedMesh *m = m_scene_manager->getMesh(filename.c_str());
    if(m) setAllMaterialFlags(m);
    return m;
}   // getAnimatedMesh

// ----------------------------------------------------------------------------

/** Loads a non-animated mesh and returns a pointer to it.
 *  \param filename  File to load.
 */
scene::IMesh *IrrDriver::getMesh(const std::string &filename)
{
    scene::IAnimatedMesh *m = m_scene_manager->getMesh(filename.c_str());
    if(!m) return NULL;
    setAllMaterialFlags(m);
    return m->getMesh(0);
}   // getMesh

// ----------------------------------------------------------------------------
/** Sets the material flags in this mesh depending on the settings in
 *  material_manager.
 *  \param mesh  The mesh to change the settings in.
 */
void IrrDriver::setAllMaterialFlags(scene::IAnimatedMesh *mesh) const
{
    unsigned int n=mesh->getMeshBufferCount();
    for(unsigned int i=0; i<n; i++)
    {
        scene::IMeshBuffer *mb = mesh->getMeshBuffer(i);
        video::SMaterial &irr_material=mb->getMaterial();
        for(unsigned int j=0; j<video::MATERIAL_MAX_TEXTURES; j++)
        {
            video::ITexture* t=irr_material.getTexture(j);
            if(!t) continue;
            material_manager->setAllMaterialFlags(t, mb);
        }   // for j<MATERIAL_MAX_TEXTURES
    }  // for i<getMeshBufferCount()
}   // setAllMaterialFlags

// ----------------------------------------------------------------------------
/** Converts the mesh into a water scene node.
 *  \param mesh The mesh which is converted into a water scene node.
 *  \param wave_height Height of the water waves.
 *  \param wave_speed Speed of the water waves.
 *  \param wave_length Lenght of a water wave.
 */
scene::ISceneNode* IrrDriver::addWaterNode(scene::IMesh *mesh,
                                           float wave_height,
                                           float wave_speed,
                                           float wave_length)
{
    return m_scene_manager->addWaterSurfaceSceneNode(mesh,
                                                     wave_height, wave_speed,
                                                     wave_length);
}   // addWaterNode

// ----------------------------------------------------------------------------
/** Adds a mesh that will be optimised using an oct tree.
 *  \param mesh Mesh to add.
 */
scene::ISceneNode *IrrDriver::addOctTree(scene::IMesh *mesh)
{
    return m_scene_manager->addOctTreeSceneNode(mesh);
}   // addOctTree

// ----------------------------------------------------------------------------
/** Adds a particle scene node.
 */
scene::IParticleSystemSceneNode *IrrDriver::addParticleNode(bool default_emitter)
{
    return m_scene_manager->addParticleSystemSceneNode(default_emitter);
}   // addParticleNode

// ----------------------------------------------------------------------------
/** Adds a static mesh to scene. This should be used for smaller objects,
 *  since the node is not optimised.
 *  \param mesh The mesh to add.
 */
scene::ISceneNode *IrrDriver::addMesh(scene::IMesh *mesh)
{
    return m_scene_manager->addMeshSceneNode(mesh);
}   // addMesh

// ----------------------------------------------------------------------------
/** Begins a rendering to a texture.
 *  \param dimension The size of the texture. 
 *  \param name Name of the texture.
 */
void IrrDriver::beginRenderToTexture(const core::dimension2di &dimension, 
                                     const std::string &name)
{
    m_render_target_texture = m_video_driver->addRenderTargetTexture(dimension, 
                                                                     name.c_str());
    m_device->getVideoDriver()->setRenderTarget(m_render_target_texture);
}   // beginRenderToTexture

// ----------------------------------------------------------------------------
/** Does the actual rendering to a texture, and switches the rendering system
 *  back to render on the screen.
 *  \return A pointer to the texture on which the scene was rendered.
 */
video::ITexture *IrrDriver::endRenderToTexture()
{
    m_scene_manager->drawAll();   
    m_device->getVideoDriver()->setRenderTarget(0, false, false);
    return m_render_target_texture;
}   // endRenderToTexture

// ----------------------------------------------------------------------------
/** Renders a given vector of meshes onto a texture. Parameters:
 *  \param mesh Vector of meshes to render.
 *  \param mesh_location For each mesh the location where it should be 
 *         positioned.
 *  \param target The texture to render the meshes to.
 *  \param angle Heading for all meshes.
 */
void IrrDriver::renderToTexture(ptr_vector<scene::IMesh, REF>& mesh, 
                                std::vector<Vec3>& mesh_location,
                                ITexture* target, float angle)
{      
    scene::ISceneNode* main_node = NULL;
    
    const int mesh_amount = mesh.size();
    for(int n=0; n<mesh_amount; n++)
    {
        scene::ISceneNode* node = addMesh(mesh.get(n));
        
        if(main_node == NULL)
        {
            main_node = node;
        }
        else
        {
            node->setParent(main_node);
        }
        
        node->setPosition( mesh_location[n].toIrrVector() );
        node->updateAbsolutePosition();
    }

    main_node->setScale( core::vector3df(50.0f, 50.0f, 50.0f) );
    main_node->setRotation( core::vector3df(0, angle, 0) );
    
    //vector3d< f32 > modelsize = mesh->getBoundingBox().getExtent();
    //std::cout << "box size " << modelsize.X*50.0 << ", " << modelsize.Y*50.0 << ", " << modelsize.Z*50.0 << std::endl;
    
    getSceneManager()->setAmbientLight(video::SColor(255, 120, 120, 120));
    
    const core::vector3df &sun_pos = core::vector3df( 0, 200, 100.0f );
    scene::ILightSceneNode* light = getSceneManager()->addLightSceneNode(NULL, sun_pos, video::SColorf(1.0f,1.0f,1.0f), 10000.0f /* radius */);
    //light->setLightType(ELT_DIRECTIONAL); // ELT_DIRECTIONAL , ELT_POINT
    //light->getLightData().AmbientColor = irr::video::SColorf(0.5f, 0.5f, 0.5f, 1.0f);
    //light->getLightData().Attenuation = core::vector3df(10, 10, 10);
    light->getLightData().DiffuseColor = irr::video::SColorf(1.0f, 1.0f, 1.0f, 1.0f);
    light->getLightData().SpecularColor = irr::video::SColorf(1.0f, 1.0f, 1.0f, 1.0f);
     
    main_node->setMaterialFlag(EMF_GOURAUD_SHADING , true);
    main_node->setMaterialFlag(EMF_LIGHTING, true);
   // node->setMaterialFlag(EMF_LIGHTING, true);
    
    const int materials = main_node->getMaterialCount();
    for(int n=0; n<materials; n++)
    {
        main_node->getMaterial(n).setFlag(EMF_LIGHTING, true);

        main_node->getMaterial(n).Shininess = 200.0f; // set size of specular highlights
        main_node->getMaterial(n).SpecularColor.set(255,150,150,150); 
        main_node->getMaterial(n).DiffuseColor.set(255,150,150,150);
         
        //node->getMaterial(n).setFlag(EMF_NORMALIZE_NORMALS , true);
        main_node->getMaterial(n).setFlag(EMF_GOURAUD_SHADING , true);
        main_node->getMaterial(n).GouraudShading = true;
    }
    
    ICameraSceneNode* camera =	m_scene_manager->addCameraSceneNode();
    
    camera->setPosition( core::vector3df(0.0, 30.0f, 70.0f) );
    camera->setUpVector( core::vector3df(0.0, 1.0, 0.0) );
    camera->setTarget( core::vector3df(0, 10, 0.0f) );
    camera->updateAbsolutePosition();
    
    m_device->getVideoDriver()->setRenderTarget(target);
    //m_device->getVideoDriver()->beginScene(true, true, video::SColor(0,0,0,0));
    m_scene_manager->drawAll();
    //m_device->getVideoDriver()->endScene();
    
    removeNode(main_node);
    removeNode(camera);
    removeNode(light);
    
    m_device->getVideoDriver()->setRenderTarget(0, false, false);
}

// ----------------------------------------------------------------------------
/** Creates a quad mesh buffer and adds it to the scene graph.
 */
scene::IMesh *IrrDriver::createQuadMesh(const video::SMaterial *material,
                                        bool create_one_quad)
{
    scene::SMeshBuffer *buffer = new scene::SMeshBuffer();
    if(create_one_quad)
    {
        video::S3DVertex v;
        v.Pos    = core::vector3df(0,0,0);
        v.Normal = core::vector3df(1/sqrt(2.0f), 1/sqrt(2.0f), 0);

        // Add the vertices
        // ----------------
        buffer->Vertices.push_back(v);
        buffer->Vertices.push_back(v);
        buffer->Vertices.push_back(v);
        buffer->Vertices.push_back(v);

        // Define the indices for the triangles
        // ------------------------------------
        buffer->Indices.push_back(0);
        buffer->Indices.push_back(1);
        buffer->Indices.push_back(2);

        buffer->Indices.push_back(0);
        buffer->Indices.push_back(2);
        buffer->Indices.push_back(3);
    }
    if(material)
        buffer->Material = *material;
    SMesh *mesh       = new SMesh();
    mesh->addMeshBuffer(buffer);
    mesh->recalculateBoundingBox();
    buffer->drop();
    return mesh;
}   // createQuadMesh

// ----------------------------------------------------------------------------
/** Removes a scene node from the scene.
 *  \param node The scene node to remove.
 */
void IrrDriver::removeNode(scene::ISceneNode *node)
{
    node->remove();
}   // removeMesh

// ----------------------------------------------------------------------------
/** Removes a mesh from the mesh cache, freeing the memory.
 *  \param mesh The mesh to remove.
 */
void IrrDriver::removeMesh(scene::IMesh *mesh)
{
    m_scene_manager->getMeshCache()->removeMesh(mesh);
}   // removeMesh
// ----------------------------------------------------------------------------
/** Removes a texture from irrlicht's texture cache.
 *  \param t The texture to remove.
 */
void IrrDriver::removeTexture(video::ITexture *t)
{
    m_video_driver->removeTexture(t);
}   // removeTexture

// ----------------------------------------------------------------------------
/** Adds an animated mesh to the scene.
 *  \param mesh The animated mesh to add.
 */
scene::IAnimatedMeshSceneNode *IrrDriver::addAnimatedMesh(scene::IAnimatedMesh *mesh)
{
    return m_scene_manager->addAnimatedMeshSceneNode(mesh);
}   // addAnimatedMesh


// ----------------------------------------------------------------------------
/** Adds a sky dome. Documentation from irrlicht:
 *  A skydome is a large (half-) sphere with a panoramic texture on the inside
 *  and is drawn around the camera position.
 *  \param texture: Texture for the dome.
 *  \param horiRes: Number of vertices of a horizontal layer of the sphere.
 *  \param vertRes: Number of vertices of a vertical layer of the sphere.
 *  \param texturePercentage: How much of the height of the texture is used.
 *         Should be between 0 and 1.
 *  \param spherePercentage: How much of the sphere is drawn. Value should be
 *          between 0 and 2, where 1 is an exact half-sphere and 2 is a full
 *          sphere.
 */
scene::ISceneNode *IrrDriver::addSkyDome(const std::string &texture_name,
                                         int hori_res, int vert_res,
                                         float texture_percent,
                                         float sphere_percent)
{
    ITexture *texture = getTexture(texture_name);
    return m_scene_manager->addSkyDomeSceneNode(texture, hori_res, vert_res,
                                                texture_percent,
                                                sphere_percent);
}   // addSkyDome

// ----------------------------------------------------------------------------
/** Adds a skybox using. Irrlicht documentation:
 *  A skybox is a big cube with 6 textures on it and is drawn around the camera
 *  position.
 *  \param top: Texture for the top plane of the box.
 *  \param bottom: Texture for the bottom plane of the box.
 *  \param left: Texture for the left plane of the box.
 *  \param right: Texture for the right plane of the box.
 *  \param front: Texture for the front plane of the box.
 *  \param back: Texture for the back plane of the box.
 */
scene::ISceneNode *IrrDriver::addSkyBox(const std::vector<std::string> &texture_names)
{
    std::vector<video::ITexture*> t;
    for(unsigned int i=0; i<texture_names.size(); i++)
    {
        t.push_back(getTexture(texture_names[i]));
    }
    return m_scene_manager->addSkyBoxSceneNode(t[0], t[1], t[2], t[3], t[4], t[5]);
}   // addSkyBox

// ----------------------------------------------------------------------------
/** Adds a camera to the scene.
 */
scene::ICameraSceneNode *IrrDriver::addCamera()
 {
     return m_scene_manager->addCameraSceneNode();
 }   // addCamera

// ----------------------------------------------------------------------------
/** Loads a texture from a file and returns the texture object.
 *  \param filename File name of the texture to load.
 */
video::ITexture *IrrDriver::getTexture(const std::string &filename)
{
    return m_scene_manager->getVideoDriver()->getTexture(filename.c_str());
}   // getTexture

// ----------------------------------------------------------------------------
/** Sets the ambient light.
 *  \param light The colour of the light to set.
 */
void IrrDriver::setAmbientLight(const video::SColor &light)
{
    m_scene_manager->setAmbientLight(light);
}   // setAmbientLight

// ----------------------------------------------------------------------------
/** Renders the bullet debug view using glut.
 */
void IrrDriver::renderBulletDebugView()
{
#ifdef HAVE_GLUT
    // Use bullets debug drawer
    GLfloat light_ambient[] = { 0.0, 0.0, 0.0, 1.0 };
    GLfloat light_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };
    /*	light_position is NOT default value	*/
    GLfloat light_position0[] = { 1.0, 1.0, 1.0, 0.0 };
    GLfloat light_position1[] = { -1.0, -1.0, -1.0, 0.0 };

    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position0);

    glLightfv(GL_LIGHT1, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT1, GL_SPECULAR, light_specular);
    glLightfv(GL_LIGHT1, GL_POSITION, light_position1);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHT1);

    glShadeModel(GL_SMOOTH);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    glClearColor(0.8f,0.8f,0.8f,0);

    glCullFace(GL_BACK);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    float f=2.0f;
    glFrustum(-f, f, -f, f, 1.0, 1000.0);

    Vec3 xyz = RaceManager::getKart(race_manager->getNumKarts()-1)->getXYZ();
    gluLookAt(xyz.getX(), xyz.getY()-5.f, xyz.getZ()+4,
        xyz.getX(), xyz.getY(),     xyz.getZ(),
        0.0f, 0.0f, 1.0f);
    glMatrixMode(GL_MODELVIEW);

    for (unsigned int i = 0 ; i < race_manager->getNumKarts(); ++i)
    {
        Kart *kart=RaceManager::getKart((int)i);
        if(!kart->isEliminated()) kart->draw();
    }
    RaceManager::getWorld()->getPhysics()->draw();
#endif
}   // renderBulletDebugView

// ----------------------------------------------------------------------------
/** Displays the FPS on the screen.
 */
void IrrDriver::displayFPS()
{
    gui::IGUIFont* font = getRaceFont();
    const int fps       = m_device->getVideoDriver()->getFPS();

    // Min and max info tracking, per mode, so user can check game vs menus
    bool current_state     = StateManager::get()->isGameState();
    static bool prev_state = false;
    static int min         = 999; // Absurd values for start will print first time
    static int max         = 0;   // but no big issue, maybe even "invisible"

    // Reset limits if state changes
    if (prev_state != current_state)
    {
        min = 999;
        max = 0;
        prev_state = current_state;
    }

    if (min > fps && fps > 1) min = fps; // Start moments always give useless 1
    if (max < fps) max = fps;

    static char buffer[32];
    sprintf(buffer, "FPS : %i/%i/%i", min, fps, max);

    core::stringw fpsString = buffer;

    static video::SColor fpsColor = video::SColor(255, 255, 0, 0);
    font->draw( fpsString.c_str(), core::rect< s32 >(0,0,600,200), fpsColor, true );
}   // updateFPS

// ----------------------------------------------------------------------------
/** Update, called once per frame.
 *  \param dt Time since last update
 */
void IrrDriver::update(float dt)
{
    if(!m_device->run()) return;
    
    m_device->getVideoDriver()->beginScene(true, true, video::SColor(255,100,101,140));

    {    // just to mark the beding/end scene block
        if(!StateManager::get()->isGameState())
        {
            // this code needs to go outside beginScene() / endScene() since
            // the model view widget will do off-screen rendering there
            const int updateAmount = GUIEngine::needsUpdate.size();
            for(int n=0; n<updateAmount; n++)
            {
                GUIEngine::needsUpdate[n].update(dt);
            }
        }

        if(race_manager->raceIsActive())
        {
            if(UserConfigParams::m_bullet_debug) renderBulletDebugView();
            m_scene_manager->drawAll();
            RaceManager::getWorld()->render();
        }
        else   // GUI is active
            GUIEngine::render(dt);

        // draw FPS if enabled
        if ( UserConfigParams::m_display_fps ) displayFPS();

    }   // just to makr the begin/end scene block
    m_device->getVideoDriver()->endScene();
    
}   // update

// ----------------------------------------------------------------------------
// Irrlicht Event handler.
bool IrrDriver::OnEvent(const irr::SEvent &event)
{
    switch (event.EventType)
    {
    case   irr::EET_KEY_INPUT_EVENT: {
        printf("key input event\n");
               break;
           }  // keyboard
      case irr::EET_GUI_EVENT:         {
               return false;           }
      case irr::EET_MOUSE_INPUT_EVENT: {
               return false;           }
      case irr::EET_LOG_TEXT_EVENT:
          {
              // Ignore 'normal' messages
              if(event.LogEvent.Level>0)
              {
                  printf("Level %d: %s\n",
                         event.LogEvent.Level,event.LogEvent.Text);
              }
              return true;
           }
      default:
          printf("Event: %d -> ",event.EventType);
          return false;
    }   // switch
    return false;
}   // OnEvent

// ----------------------------------------------------------------------------
