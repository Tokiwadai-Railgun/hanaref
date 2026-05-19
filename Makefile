CC := gcc
CCFLAGS := -Wall -Wextra
CCLIB := -lm
SRCS := $(wildcard src/*.c)

run: build
	./bin/tasks

build: $(SRCS)
	$(CC) -o bin/tasks $^ $(CCFLAGS) $(CCLIB)

clean:
	rm ./bin/*
