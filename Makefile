STATICPROGRAM = libdim.a
DYNAMICPROGRAM = libdim.so
CONVERTPROGRAM = objtoheight

INCLUDEDIRS = -Iinclude -I/usr/include/freetype2
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
vpath %.d obj

CXXSOURCES = textureproperties__.cpp drawstate.cpp light.cpp drawmap.cpp camera.cpp shader.cpp mesh.cpp window.cpp drawable.cpp mouse.cpp shaderbuffer.cpp button.cpp context.cpp menu.cpp menuitem.cpp listenarea.cpp component.cpp font.cpp image2d.cpp drawablewrapper.cpp frame.cpp tools.cpp texture.cpp

CXXHEADERS = base_iterator__.hpp buffer.hpp button.hpp camera.hpp component.hpp context.hpp dim.hpp drawable.hpp drawablewrapper.hpp drawablewrapper.inl drawmap.hpp drawstate.hpp font.hpp frame.hpp image2d.hpp light.hpp listenarea.hpp menu.hpp menuitem.hpp mesh.hpp mouse.hpp onepair.hpp shaderbuffer.hpp shader.hpp surface.hpp surface.inl texture.hpp texture.inl tools.hpp window.hpp copyptr.hpp cloneptr.hpp

CXXOBJECTS = $(CXXSOURCES:.cpp=.o)
CXXDEPS = $(CXXSOURCES:.cpp=.d)
CXXFLAGS = -Wall --std=c++0x $(INCLUDEDIRS)

CXX = clang++

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
	$(CXX) -shared -Wl,-soname,$(DYNAMICPROGRAM) -o $(DYNAMICPROGRAM) $(addprefix $(OBJDIR)/, $(CXXOBJECTS)) -lc -lGLEW

-include $(addprefix $(OBJDIR)/, $(CXXDEPS))

%.o: %.cpp
	$(CXX) -c $(CXXFLAGS) $< -o $(OBJDIR)/$@
	$(CXX) -MM $(CXXFLAGS) $< > $(OBJDIR)/$*.d

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
