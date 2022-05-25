CC=g++
LDFLAGS=-std=c++11 -O3 -lm
SOURCES=src/bstar.cpp src/main.cpp
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=fp
INCLUDES=src/bstar.h src/module.h


all: $(SOURCES) bin/$(EXECUTABLE)

bin/$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

%.o:  %.c  ${INCLUDES}
	$(CC) $(CFLAGS) $< -o $@


clean:
	rm -rf *.o bin/$(EXECUTABLE)
