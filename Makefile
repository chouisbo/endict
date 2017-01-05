CXX ?= g++
CFLAGS = -Wall -Wconversion -O3 -fPIC -fpermissive
INCLUDES = `pkg-config --cflags mysqlclient`
INCLUDES += `pkg-config --cflags glib-2.0`
INCLUDES += -I./include 
LIBS = `pkg-config --libs mysqlclient`
LIBS += `pkg-config --libs glib-2.0`
LIBS += -L./lib -ldatrie

all: endict

endict: src/endict.c src/main.c
	$(CXX) $(CFLAGS) $(INCLUDES) src/endict.c src/main.c $(LIBS) -o endict 

clean:
	rm -rf endict *.o

