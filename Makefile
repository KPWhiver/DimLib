STATICPROGRAM = libdim.a
DYNAMICPROGRAM = libdim.so
CONVERTPROGRAM = objtoheight

INCLUDEDIRS = -Iinclude
SRCDIR = src
OBJDIR = obj

INSTALL = /usr/bin/install -c
INSTALLDATA = /usr/bin/install -c -m 644

prefix = /usr/local
libdir = $(prefix)/lib
includedir = $(prefix)/include

vpath %.cpp src
vpath %.h include/DIM
vpath %.hpp include/DIM
vpath %.o obj

CXXSOURCES = drawstate.cpp surface.cpp light.cpp drawmap.cpp camera.cpp texture.cpp shader.cpp mesh.cpp window.cpp drawable.cpp mouse.cpp shaderbuffer.cpp button.cpp context.cpp menu.cpp menuitem.cpp listenarea.cpp component.cpp font.cpp image2d.cpp drawablewrapper.cpp frame.cpp tools.cpp
CXXHEADERS = base_iterator__.hpp component.hpp drawablewrapper.hpp drawstate.hpp light.hpp mesh.hpp surface.hpp buffer.hpp context.hpp font.hpp listenarea.hpp mouse.hpp shaderbuffer.hpp texture.hpp button.hpp dim.hpp drawablewrapper.inl frame.hpp menu.hpp onepair.hpp shader.hpp window.hpp camera.hpp drawable.hpp drawmap.hpp image2d.hpp menuitem.hpp wrapper_ptr.hpp tools.hpp texture.inl

CXXOBJECTS = $(CXXSOURCES:.cpp=.o)
CXXFLAGS = -Wall --std=c++0x $(INCLUDEDIRS)

CXX = g++

all: $(DYNAMICPROGRAM)

debug: CXXFLAGS += -g
debug: all

profile: CXXFLAGS += -pg
profile: debug

$(STATICPROGRAM): $(CXXOBJECTS)
	ar -rsv $(STATICPROGRAM) $(addprefix $(OBJDIR)/, $(CXXOBJECTS))
	ranlib $(STATICPROGRAM)
	
build = $(CXX) $(CXXFLAGS) -c $(SRCDIR)/$(1).cpp -o $(OBJDIR)/$(1).o

	
$(DYNAMICPROGRAM): CXXFLAGS += -fPIC
$(DYNAMICPROGRAM): $(CXXOBJECTS)
	g++ -shared -Wl,-soname,$(DYNAMICPROGRAM) -o $(DYNAMICPROGRAM) $(addprefix $(OBJDIR)/, $(CXXOBJECTS)) -lc
	
drawstate.o: drawstate.cpp $(DRAWSTATE)
	$(call build,drawstate)
	
DRAWSTATE: $(MESH) $(SHADER) $(TEXTURE) drawstate.hpp

MESH: $(BUFFER) mesh.hpp

BUFFER: dim.hpp buffer.hpp

SHADER: $(CAMERA) $(LIGHT) $(SHADERBUFFER) shader.hpp

CAMERA: dim.hpp camera.hpp

LIGHT: $(TEXTURE) light.hpp

TEXTURE: dim.hpp texture.hpp texture.inl

context.o: context.cpp $(CONTEXT)
	$(call build,context)

CONTEXT: $(MESH) $(FONT) context.hpp

FONT: $(TEXTURE) font.hpp

light.o: light.cpp $(LIGHT) $(SHADER) $(CAMERA)
	$(call build,light)
	
drawmap.o: drawmap.cpp $(DRAWMAP) $(SHADER)
	$(call build,drawmap)
	
DRAWMAP: $(DRAWABLEWRAPPER) $(DRAWSTATE) drawmap.hpp

DRAWABLEWRAPPER: $(DRAWABLE) $(BASE_ITERATOR__) onepair.hpp drawablewrapper.inl drawablewrapper.hpp

DRAWABLE: $(DRAWSTATE) onepair.hpp drawable.hpp

BASE_ITERATOR__: $(DRAWABLE) base_iterator__.hpp

camera.o: camera.cpp $(CAMERA) $(SHADER)
	$(call build,camera)
	
texture.o: texture.cpp $(TEXTURE) $(SHADER)
	$(call build,texture)

shader.o: shader.cpp $(SHADER)
	$(call build,shader)
	
mesh.o: mesh.cpp $(MESH) $(SHADER)
	$(call build,mesh)
	
window.o: window.cpp $(WINDOW)
	$(call build,window)
	
WINDOW: $(MOUSE) window.hpp

drawable.o: drawable.cpp $(DRAWABLE)
	$(call build,drawable)

mouse.o: mouse.cpp $(MOUSE)
	$(call build,mouse)
	
MOUSE: dim.hpp mouse.hpp

shaderbuffer.o: shaderbuffer.cpp $(SHADERBUFFER)
	$(call build,shaderbuffer)

SHADERBUFFER: dim.hpp shaderbuffer.hpp

button.o: button.cpp $(BUTTON) $(SHADER)
	$(call build,button)

BUTTON: $(TEXTURE) $(MENU) $(COMPONENT) button.hpp

MENU: $(TEXTURE) $(MENUITEM) $(COMPONENT) menu.hpp

MENUITEM: $(TEXTURE) $(COMPONENT) menuitem.hpp

COMPONENT: $(MOUSE) $(CONTEXT)

menu.o: menu.cpp $(MENU)
	$(call build,menu)
	
menuitem.o: menuitem.cpp $(MENUITEM) $(SHADER)
	$(call build,menuitem)
	
listenarea.o: listenarea.cpp $(LISTENAREA)
	$(call build,listenarea)
	
LISTENAREA: $(TEXTURE) $(COMPONENT) listenarea.hpp

component.o: component.cpp $(COMPONENT)
	$(call build,component)

font.o: font.cpp $(FONT)
	$(call build,font)
	
image2d.o: image2d.cpp $(IMAGE2D) $(SHADER)
	$(call build,image2d)

IMAGE2D: $(TEXTURE) $(COMPONENT) image2d.hpp

drawablewrapper.o: drawablewrapper.cpp $(DRAWABLEWRAPPER)
	$(call build,drawablewrapper)

frame.o: frame.cpp $(FRAME) $(MESH)
	$(call build,frame)

FRAME: $(COMPONENT) $(MOUSE) wrapper_ptr.hpp frame.hpp

surface.o: surface.cpp $(SURFACE) $(FONT)
	$(call build,surface)

SURFACE: $(TEXTURE)

tools.o: tools.cpp tools.hpp $(MESH)
	$(call build,tools)

define \n


endef

install: all
	$(INSTALL) $(DYNAMICPROGRAM) $(libdir)/$(DYNAMICPROGRAM)
	/sbin/ldconfig -l $(libdir)
	$(INSTALL) -d include/DIM $(includedir)/DIM
	$(addprefix $(INSTALL) -t $(includedir)/DIM/ include/DIM/,$(addsuffix ;$(\n),$(CXXHEADERS)))
  
clean:
	rm -f *~
	rm -f #*#
	rm -f $(OBJDIR)/*.o
	rm -f $(DYNAMICPROGRAM)
	rm -f Makefile.bak
	rm -f *.o
