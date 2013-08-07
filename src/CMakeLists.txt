set(CXXSOURCES
  core/textureproperties.cpp
  core/dim.cpp
  core/light.cpp
  core/camera.cpp
  core/shader.cpp
  core/font.cpp
  core/tools.cpp
  core/texture.cpp
  core/surface.cpp
  core/windowsurface.cpp
  core/lex.cpp
  core/timer.cpp
  core/mesh.cpp

  scene/scene.cpp
  scene/nodebase.cpp
  scene/filedrawnode.cpp
  scene/nodestoragebase.cpp
  scene/resourcemanager.cpp
  scene/scenegraph.cpp

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

find_package(Bullet REQUIRED)
if(BULLET_FOUND)
  include_directories(${BULLET_INCLUDE_DIRS})
endif()

find_package(Freetype REQUIRED)
if(FREETYPE_FOUND)
  include_directories(${FREETYPE_INCLUDE_DIRS})
endif()

include_directories(
  ${PROJECT_SOURCE_DIR}/include
)

#if(CMAKE_CXX_COMPILER MATCHES ".*Clang.*")
#  set(CMAKE_COMPILER_IS_CLANGXX 1)
#endif()

## Compiler flags
#if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_COMPILER_IS_CLANGXX)
  set(CMAKE_CXX_FLAGS "-std=c++0x -Wall")#-Wpadded
#endif()

## Default build type is release
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

## Add profile build type
set(CMAKE_CXX_FLAGS_PROFILE "-pg -g")

## Get headers
add_library(dim STATIC ${CXXSOURCES} ${CXXHEADERS})
#target_link_libraries(dim GL png freetype assimp GLEW yaml-cpp)

set(LIB_INSTALL_DIR lib)

if(CMAKE_BUILD_TYPE MATCHES Debug)
  set(LIB_INSTALL_DIR lib/debug/usr/lib)
endif()


install(TARGETS dim DESTINATION lib)
