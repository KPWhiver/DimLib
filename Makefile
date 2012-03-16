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

CXXSOURCES = drawstate.cpp surface.cpp light.cpp drawmap.cpp camera.cpp texture.cpp shader.cpp mesh.cpp window.cpp drawable.cpp mouse.cpp shaderbuffer.cpp button.cpp context.cpp menu.cpp menuitem.cpp listenarea.cpp component.cpp font.cpp image2d.cpp drawablewrapper.cpp
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
	ar -rsv $(STATICPROGRAM) $(CXXOBJECTS)
	ranlib $(STATICPROGRAM)
	
#$(DYNAMICPROGRAM): $(CXXOBJECTS)
#  g++ -shared -Wl,-soname,$(DYNAMICPROGRAM) -o $(DYNAMICPROGRAM) $(addprefix $(OBJDIR)/, $(CXXOBJECTS)) -lc

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $(input) -o $(output)

install:
  

clean:
	rm -f *~
	rm -f #*#
	rm -f $(OBJDIR)/*.o
	rm -f $(PROGRAM)
	rm -f Makefile.bak
	rm -f *.o
