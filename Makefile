CC := gcc
CCFLAGS := -Wall -Wextra
CCLIB := -lm -lcurl -lcjson
SRCS := $(wildcard src/*.c)

run: build
	sh run.sh

build: $(SRCS)
	$(CC) -o bin/tasks $^ $(CCFLAGS) $(CCLIB)

debug: $(SRCS)
	$(CC) -o bin/tasks $^ $(CCFLAGS) $(CCLIB) -g

install: build
	chmod +x ./run.sh
	ln -s ./run.sh ~/.local/bin/hanaref

clean:
	rm ./bin/*
