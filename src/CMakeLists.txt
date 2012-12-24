set(CXXSOURCES
  core/textureproperties.cpp 
  core/dim.cpp 
  core/light.cpp   
  core/camera.cpp 
  core/shader.cpp   
  core/mouse.cpp 
  core/window.cpp 
  core/font.cpp 
  core/tools.cpp 
  core/texture.cpp 
  core/surface.cpp 
  core/lex.cpp 
  core/timer.cpp
  core/mesh.cpp  

  scene/scene.cpp 
  scene/scenegraph.cpp 
  scene/drawnode.cpp   
  scene/nodestoragebase.cpp 

  gui/button.cpp 
  gui/context.cpp 
  gui/menu.cpp 
  gui/menuitem.cpp 
  gui/listenarea.cpp 
  gui/component.cpp 
  gui/image2d.cpp 
  gui/frame.cpp 
  gui/switch.cpp
)

include_directories(
  ${PROJECT_SOURCE_DIR}/include
  /usr/include/freetype2
  /usr/local/include/bullet
)

if(CMAKE_CXX_COMPILER MATCHES ".*clang")
  set(CMAKE_COMPILER_IS_CLANGXX 1)
endif()

## Compiler flags
if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_COMPILER_IS_CLANGXX)
  set(CMAKE_CXX_FLAGS "-std=c++0x -Wall")
endif()

## Default build type is release
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

## Add profile build type
set(CMAKE_CXX_FLAGS_PROFILE "-pg -g")

add_library(dim SHARED ${CXXSOURCES})
target_link_libraries(dim X11 pthread GL png freetype assimp GLEW glfw)

set(LIB_INSTALL_DIR lib)

if(CMAKE_BUILD_TYPE MATCHES Debug)
  set(LIB_INSTALL_DIR lib/debug/usr/lib)
endif()


install(TARGETS dim DESTINATION lib)
