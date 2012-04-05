STATICPROGRAM = libdim.a
DYNAMICPROGRAM = libdim.so
CONVERTPROGRAM = objtoheight

INCLUDEDIRS = -Iinclude
SRCDIR = src
OBJDIR = obj

vpath %.cpp src
vpath %.h include/DIM
vpath %.hpp include/DIM
vpath %.o obj

CXXSOURCES = drawstate.cpp surface.cpp light.cpp drawmap.cpp camera.cpp texture.cpp shader.cpp mesh.cpp window.cpp drawable.cpp mouse.cpp shaderbuffer.cpp button.cpp context.cpp menu.cpp menuitem.cpp listenarea.cpp component.cpp font.cpp image2d.cpp drawablewrapper.cpp frame.cpp
CXXOBJECTS = $(CXXSOURCES:.cpp=.o)
CXXFLAGS = -Wall --std=c++0x $(INCLUDEDIRS)
#-fpic
CXX = g++

MAINHEADER = $(CXXSOURCES:.cpp=.hpp)

all: $(STATICPROGRAM)

#dynamic: $(DYNAMICPROGRAM)

debug: CXXFLAGS += -g
debug: $(STATICPROGRAM)

profile: CXXFLAGS += -pg
profile: debug

$(STATICPROGRAM): $(CXXOBJECTS)
	ar -rsv $(STATICPROGRAM) $(addprefix $(OBJDIR)/, $(CXXOBJECTS))
	ranlib $(STATICPROGRAM)
	
build = $(CXX) $(CXXFLAGS) -c $(SRCDIR)/$(1).cpp -o $(OBJDIR)/$(1).o

	
$(DYNAMICPROGRAM): $(CXXOBJECTS)
	g++ -shared -Wl,-soname,$(DYNAMICPROGRAM) -o $(DYNAMICPROGRAM) $(addprefix $(OBJDIR)/, $(CXXOBJECTS)) -lc

#$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
#	$(CXX) $(CXXFLAGS) -c $(input) -o $(output)
	
drawstate.o: drawstate.cpp drawstate.hpp
	$(call build,drawstate)
	
drawstate.hpp: mesh.hpp shader.hpp texture.hpp

mesh.hpp: buffer.hpp

buffer.hpp: dim.hpp

shader.hpp: camera.hpp light.hpp shaderbuffer.hpp

camera.hpp: dim.hpp

light.hpp: texture.hpp

texture.hpp: dim.hpp

context.o: context.cpp context.hpp
	$(call build,context)

context.hpp: mesh.hpp font.hpp

font.hpp: texture.hpp

light.o: light.cpp light.hpp shader.hpp camera.hpp
	$(call build,light)
	
drawmap.o: drawmap.cpp drawmap.hpp shader.hpp
	$(call build,drawmap)
	
drawmap.hpp: drawablewrapper.hpp drawstate.hpp

drawablewrapper.hpp: drawable.hpp base_iterator__.hpp onepair.hpp drawablewrapperimplement.hpp

drawable.hpp: drawstate.hpp onepair.hpp

base_iterator__.hpp: drawable.hpp

camera.o: camera.cpp camera.hpp shader.hpp
	$(call build,camera)
	
texture.o: texture.cpp texture.hpp shader.hpp
	$(call build,texture)

shader.o: shader.cpp shader.hpp
	$(call build,shader)
	
mesh.o: mesh.cpp mesh.hpp shader.hpp
	$(call build,mesh)
	
window.o: window.cpp window.hpp
	$(call build,window)
	
window.hpp: mouse.hpp

drawable.o: drawable.cpp drawable.hpp
	$(call build,drawable)

mouse.o: mouse.cpp mouse.hpp
	$(call build,mouse)
	
mouse.hpp: dim.hpp

shaderbuffer.o: shaderbuffer.cpp shaderbuffer.hpp
	$(call build,shaderbuffer)

shaderbuffer.hpp: dim.hpp

button.o: button.cpp button.hpp shader.hpp
	$(call build,button)

button.hpp: texture.hpp menu.hpp component.hpp

menu.hpp: texture.hpp menuitem.hpp component.hpp

menuitem.hpp: texture.hpp component.hpp

component.hpp: mouse.hpp context.hpp

menu.o: menu.cpp menu.hpp
	$(call build,menu)
	
menuitem.o: menuitem.cpp menuitem.hpp shader.hpp
	$(call build,menuitem)
	
listenarea.o: listenarea.cpp listenarea.hpp
	$(call build,listenarea)
	
listenarea.hpp: texture.hpp component.hpp

component.o: component.cpp component.hpp
	$(call build,component)

font.o: font.cpp font.hpp
	$(call build,font)
	
image2d.o: image2d.cpp image2d.hpp shader.hpp
	$(call build,image2d)

image2d.hpp: texture.hpp component.hpp

drawablewrapper.o: drawablewrapper.cpp drawablewrapper.hpp
	$(call build,drawablewrapper)

frame.o: frame.cpp frame.hpp mesh.hpp
	$(call build,frame)

frame.hpp: component.hpp mouse.hpp wrapper_ptr.hpp

surface.o: surface.cpp surface.hpp font.hpp
	$(call build,surface)

surface.hpp: texture.hpp

#install:
  

clean:
	rm -f *~
	rm -f #*#
	rm -f $(OBJDIR)/*.o
	rm -f $(PROGRAM)
	rm -f Makefile.bak
	rm -f *.o
