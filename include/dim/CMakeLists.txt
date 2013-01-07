set(CXXHEADERS_SCENE
  scene/iteratorbase.hpp   
  scene/drawnode.hpp 
  scene/nodestoragebase.hpp 
  scene/nodegrid.hpp 
  scene/nodegrid.inl 
  scene/scenegraph.hpp 
  scene/scene.hpp
  scene/filedrawnode.hpp
  scene/texturemanager.hpp
  scene/shadermanager.hpp
  scene/scenemanager.hpp
  scene/bulletmanager.hpp
)

set(CXXHEADERS_CORE
  core/mesh.hpp  
  core/buffer.hpp 
  core/camera.hpp 
  core/dim.hpp 
  core/font.hpp 
  core/light.hpp
  core/mouse.hpp 
  core/shader.hpp 
  core/surface.hpp 
  core/surface.inl 
  core/doublebufferedsurface.hpp
  core/doublebufferedsurface.inl
  core/texture.hpp 
  core/texture.inl 
  core/tools.hpp 
  core/window.hpp 
  core/timer.hpp
)  
  
set(CXXHEADERS_GUI
  gui/switch.hpp
  gui/listenarea.hpp 
  gui/menu.hpp 
  gui/menuitem.hpp   
  gui/frame.hpp 
  gui/image2d.hpp 
  gui/button.hpp   
  gui/component.hpp 
  gui/context.hpp 
)      
      
set(CXXHEADERS_UTIL 
  util/copyptr.hpp 
  util/ptrvector.hpp  
  util/onepair.hpp 
)

install(FILES ${CXXHEADERS_SCENE} DESTINATION include/dim/scene)
install(FILES ${CXXHEADERS_CORE} DESTINATION include/dim/core)
install(FILES ${CXXHEADERS_GUI} DESTINATION include/dim/gui)
install(FILES ${CXXHEADERS_UTIL} DESTINATION include/dim/util)
