APPNAME = $(shell basename $(shell pwd))

CC=cc
CXX=g++
FLAGS=-g -Wall -Wextra
CFLAGS =$(FLAGS)
CXXFLAGS=$(FLAGS)
LIBS=-pthread
CLIBS=$(LIBS)
CXXLIBS=$(LIBS)

all: $(APPNAME)

%: %.c
	$(CC) $(CFLAGS) $^ -o $@ $(CLIBS)

%: %.cpp
	$(CXX) $(CXXFLAGS) $^ -o $@ $(CXXLIBS)

.PHONY: gitignore
gitignore:
	echo $(APPNAME) > .gitignore

.PHONY: clean
clean:
	rm -f $(APPNAME)

#rules
#target: dependencias
#[tab]command
#make
# make clean
# make gitignore
# make CC=clang
# ./hello_w >> jiji.txt : usar eso como salida sin sobrescribir
# ./hello_w > jiji.txt : usar como salida estandar SOBRESCRIBIENDO
#
#
