CC=gcc
CFLAGS=-Wall
EXECUTABLE=cbhe
SOURCES=$(shell find src -name '*.cpp')
OBJS=$(patsubst src/%.cpp, bin/%.o, $(SOURCES))

$(EXECUTABLE) : $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $@ 

bin/%.o: src/%.cpp
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm $(EXECUTABLE)
	rm -r bin
