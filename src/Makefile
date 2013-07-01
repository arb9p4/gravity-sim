# /*
# *	Makefile
# *
# *	Created on : Apr 10, 2013
# *	Course     : CS7610
# *	Instructor : Kannappan Palaniappan
# *	Author	   : Andrew Buck
# *	Purpose    : Builds the program executable
# *
# *	Copyright University of Missouri-Columbia
# */


########################################################
# Cross-platform makefile for FLTK (with OpenGL support)
#
# Ensure all of the compilation variables are correct
# and FLTK is installed!
#

########################################################
# Compilation vars
#

# Name/type of compiler
CC = g++

# Compiling flags to be used
CFLAGS = -c -Wall

# Directory which contains FLTK
FLTKDIR = /usr/local

# Name of executable
APP_NAME = gravity-sim

########################################################
# Source files (with extension)
#
SOURCES = main.cpp GlWindow.cpp Universe.cpp Body.cpp Camera.cpp

########################################################
# Included directories
#
INCLUDES = -I/. -I$(FLTKDIR)/include -I$(FLTKDIR)/FL/images

########################################################
# Libraries to be linked at compile-time
#
LIBS  = `fltk-config --cxxflags --ldflags --use-gl --use-images --use-glut`

########################################################
# Make rules
#
OBJECTS = $(addsuffix .o, $(basename $(SOURCES)))

all: $(SOURCES) $(APP_NAME)

$(APP_NAME): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@ $(LIBS)

%.o: %.cxx
	$(CC) $(CFLAGS) $(INCLUDES) $*.cxx -o $@

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) $*.c -o $@

%.o: %.cpp
	$(CC) $(CFLAGS) $(INCLUDES) $*.cpp -o $@

tidy:
	-rm -f *~ *.o

clean: tidy
	-rm -f $(APP_NAME)

remake: clean all

run: all
	./$(APP_NAME)

