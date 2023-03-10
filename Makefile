FLAGS= -Wall
CC= gcc
LIBS=-lpthread -lmsq-utils
OBJECTS=objects
BUILD=build
SRC=src
INCLUDE=include
OBJECTS_FILES= $(OBJECTS)/main.o $(OBJECTS)/scanner.o

all: $(BUILD)/main

$(BUILD)/main: $(OBJECTS_FILES)
	$(CC) $(FLAGS) -o $(BUILD)/main $^  $(LIBS)

$(OBJECTS)/%.o: $(SRC)/%.c $(INCLUDE)/*.h
	$(CC) $(FLAGS) -c $< -o $@ $(LIBS)

test: test.c
	gcc `pkg-config --cflags gtk+-3.0` test.c -o test `pkg-config --libs gtk+-3.0` -rdynamic

clean:
	rm -f $(OBJECTS)/*
	rm -f $(BUILD)/*