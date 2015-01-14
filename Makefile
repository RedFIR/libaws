CC=g++
CFLAGS= -c -std=c++0x -I/usr/include/crypto++/ -lcryptopp -lcurl -std=c++0x `pkg-config libxml++-2.6 --cflags --libs` -IcurlIncludes -g3 -Iheaders/
SOURCES=$(wildcard src/*.cpp)
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=lib/libaws.a

all: $(SOURCES) $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJECTS) 
	ar crf $(EXECUTABLE) $(OBJECTS)

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

demo:
	g++ example/example1.cpp lib/lib*.a -Iheaders/ -std=c++0x -lcurl -std=c++0x `pkg-config libxml++-2.6 --cflags --libs` -lcryptopp -o demo

clean:
	rm -rf `find . -name "*.o"`

