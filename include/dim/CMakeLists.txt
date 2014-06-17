set(CXXHEADERS_SCENE
#  scene/iteratorbase.hpp
#  scene/nodebase.hpp
#  scene/nodestoragebase.hpp
#  scene/nodegrid.hpp
#  scene/nodegrid.inl
#  scene/scenegraph.hpp
#  scene/scenegraph.inl
  scene/scene.hpp
#  scene/filedrawnode.hpp
  scene/texturemanager.hpp
  scene/shadermanager.hpp
  scene/scenemanager.hpp
#  scene/bulletmanager.hpp
)

set(CXXHEADERS_CORE
  core/mesh.hpp
  core/buffer.hpp
  core/camera.hpp
  core/dim.hpp
  core/light.hpp
  core/shader.hpp
  core/surface.hpp
  core/surface.inl
  core/doublebufferedsurface.hpp
  core/doublebufferedsurface.inl
  core/texture.hpp
  core/texture.inl
  core/tools.hpp
  core/windowsurface.hpp
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
  gui/sprite.hpp
)

set(CXXHEADERS_UTIL
  util/copyptr.hpp
  util/ptrvector.hpp
  util/sortedvector.hpp
  util/onepair.hpp
  util/tupleforeach.hpp
)

set(USED_CXXHEADERS
  ${CXXHEADERS_CORE}
  ${CXXHEADERS_UTIL}
)

if(SCENE)
  set(USED_CXXHEADERS ${USED_CXXHEADERS} ${CXXHEADERS_SCENE})
  install(FILES ${CXXHEADERS_SCENE} DESTINATION include/dim/scene)
endif()

if(GUI)
  set(USED_CXXHEADERS ${USED_CXXHEADERS} ${CXXHEADERS_GUI})
  install(FILES ${CXXHEADERS_GUI} DESTINATION include/dim/gui)
endif()

if(FONT)
  set(USED_CXXHEADERS ${USED_CXXHEADERS} core/font.hpp)
  install(FILES core/font.hpp DESTINATION include/dim/core)
endif()

install(FILES ${CXXHEADERS_CORE} DESTINATION include/dim/core)
install(FILES ${CXXHEADERS_UTIL} DESTINATION include/dim/util)

set(CXXHEADERS
  ${USED_CXXHEADERS}
PARENT_SCOPE)
