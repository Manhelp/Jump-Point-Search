
GXX := g++
GCC := gcc

LDFLAGS := #-ldl -lrt

CFLAGS := -g -O2 -Wall -std=c++11
OUT := bin/jpsfinder

SOURCE_DIR := src
OBJ_DIR := bin/obj

SOURCE_CPP_DIR := $(wildcard $(SOURCE_DIR)/*.cpp)
OBJ := $(patsubst %.cpp, $(OBJ_DIR)/%.o, $(SOURCE_CPP_DIR))


all: build

build: before out after

before: 
	test -d $(OBJ_DIR)/src || mkdir -p $(OBJ_DIR)/src

out: $(OBJ)

$(OBJ): $(OBJ_DIR)/%.o:%.cpp 
	$(GXX) $(CFLAGS) -c $< -o $@

after:
	$(GXX) -o $(OUT) $(OBJ)  $(LDFLAGS)
	
clean:
	rm -rf bin/obj

.PHONY: all build before out after clean

