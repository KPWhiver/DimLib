set(CXXSOURCES
  textureproperties.cpp 
  dim.cpp scene.cpp 
  light.cpp 
  scenegraph.cpp 
  camera.cpp 
  shader.cpp 
  mesh.cpp 
  window.cpp 
  drawnode.cpp 
  mouse.cpp 
  button.cpp 
  context.cpp 
  menu.cpp 
  menuitem.cpp 
  listenarea.cpp 
  component.cpp 
  font.cpp 
  image2d.cpp 
  nodestoragebase.cpp 
  frame.cpp 
  tools.cpp 
  texture.cpp 
  surface.cpp 
  lex.cpp 
  switch.cpp
)

include_directories(
  ${PROJECT_SOURCE_DIR}/include
  /usr/include/freetype2
)

## Compiler flags
if(CMAKE_COMPILER_IS_GNUCXX)
  set(CMAKE_CXX_FLAGS "-std=c++0x -Wall")  ## Optimize
endif()

## Default build type is release
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

## Add profile build type
SET(CMAKE_CXX_FLAGS_PROFILE "-pg -g")

add_library(dim SHARED ${CXXSOURCES})
target_link_libraries(dim X11 pthread GL png freetype assimp GLEW glfw)

set(LIB_INSTALL_DIR lib)

if(CMAKE_BUILD_TYPE MATCHES Debug)
  set(LIB_INSTALL_DIR lib/debug/usr/lib)
endif()


install(TARGETS dim DESTINATION lib)
