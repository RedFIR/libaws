CC=g++
CFLAGS= -c -std=c++0x -I/usr/include/crypto++/ -lcryptopp -lcurl -std=c++0x `pkg-config libxml++-2.6 --cflags --libs` -IcurlIncludes -g3 -Iheaders/ -fPIC
SOURCES=$(wildcard src/*.cpp)
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=lib/libaws.so

all: $(SOURCES) $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJECTS) 
	gcc -shared -o $(EXECUTABLE) $(OBJECTS)

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

demo:
	g++ example/example1.cpp -L/usr/local/lib -laws -lcurlcpp -Iheaders/ -std=c++0x -lcurl `pkg-config libxml++-2.6 --cflags --libs` -lcryptopp -o demo

clean:
	rm -rf `find . -name "*.o"`

install:
	sudo cp lib/lib* /usr/local/lib
	sudo cp -r headers/ /usr/local/include/AWS/
