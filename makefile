# build an executable named cacheSim from cache.c and cache.h
CC = gcc
# compiler flags:
# - lm: allows use of math library
# - std=c99: compile for less recent GNU machines
CFLAGS = -std=c99 -lm

# what this will build
TARGET = cacheSim

all: $(TARGET)

$(TARGET): cache.c
	$(CC) $(CFLAGS) -o $(TARGET) cache.c

clean:
	$(RM) $(TARGET)
