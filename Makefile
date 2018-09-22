CC=gcc
CFLAGS=-Wall -g
EXECUTABLE=cbhe
SOURCES=$(shell find src -name '*.c')
OBJS=$(patsubst src/%.c, bin/%.o, $(SOURCES))

$(EXECUTABLE) : $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $@ 

bin/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm $(EXECUTABLE)
	rm -r bin
