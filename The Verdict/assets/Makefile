CC      = gcc
CFLAGS  = -std=c99 -Wall -Wextra -O2 -Iinclude
SRC     = $(wildcard src/*.c)
TARGET  = TheVerdict

ifeq ($(OS),Windows_NT)
    TARGET := $(TARGET).exe
    LDLIBS  = -lraylib -lopengl32 -lgdi32 -lwinmm
else
    LDLIBS  = -lraylib -lm -lpthread -ldl -lrt -lX11
endif

all: $(TARGET)

$(TARGET): $(SRC) $(wildcard include/*.h)
	$(CC) $(CFLAGS) $(SRC) -o $@ $(LDLIBS)

clean:
	rm -f $(TARGET)

.PHONY: all clean