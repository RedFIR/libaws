CC=g++
CFLAGS= -c -Wall -std=c++0x -I/usr/include/crypto++/ -lcryptopp -lcurl -std=c++0x `pkg-config libxml++-2.6 --cflags --libs` -IcurlIncludes -g3
SOURCES=$(wildcard *.cpp)
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=a.out

all: $(SOURCES) $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@ -lcryptopp -lcurl -std=c++0x `pkg-config libxml++-2.6 --cflags --libs` libcurlcpp.a

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@





# CFLAGS = -g -Wall
# SRC = SQS.cpp \
# 	  Utils.cpp
# OBJ = $(src:.cpp=.o)

# LDLIBS=-lcryptopp -lcurl -I/usr/include/crypto++/ -std=c++0x `pkg-config libxml++-2.6 --cflags --libs` libcurlcpp.a -IcurlIncludes
# NAME = a.out

# all: $(OBJ)
# 	g++ -std=c++0x $(OBJ) $(LDLIBS) -o $(NAME)

