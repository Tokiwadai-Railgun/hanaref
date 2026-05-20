CC := gcc
CCFLAGS := -Wall -Wextra
CCLIB := -lm -lcurl
SRCS := $(wildcard src/*.c)

run: build
	sh run.sh

build: $(SRCS)
	$(CC) -o bin/tasks $^ $(CCFLAGS) $(CCLIB)

clean:
	rm ./bin/*
