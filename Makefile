
GXX := g++
GCC := gcc

LDFLAGS := -ldl -lrt -lprotobuf -lpthread

CFLAGS := -g -O2 -Wall -std=c++11 
OUT := bin/jpsfinder

LIB := /usr/local/lib/libprotobuf.a

SOURCE_DIR := src
SOURCE_JPS_DIR := $(SOURCE_DIR)/jps
INCLUDE := -I$(SOURCE_JPS_DIR) -I$(SOURCE_DIR)
OBJ_DIR := bin/obj

SOURCE_CPP_DIR := $(wildcard $(SOURCE_JPS_DIR)/*.cc $(SOURCE_DIR)/*.cc)
OBJ := $(patsubst %.cc, $(OBJ_DIR)/%.o, $(SOURCE_CPP_DIR))


all: build

build: before out after

before: 
	test -d $(OBJ_DIR)/$(SOURCE_DIR) || mkdir -p $(OBJ_DIR)/$(SOURCE_DIR)
	test -d $(OBJ_DIR)/$(SOURCE_JPS_DIR) || mkdir -p $(OBJ_DIR)/$(SOURCE_JPS_DIR)

out: $(OBJ)

$(OBJ): $(OBJ_DIR)/%.o:%.cc
	$(GXX) $(CFLAGS) $(INCLUDE) -c $< -o $@

after:
	$(GXX) -o $(OUT) $(OBJ) $(LIB)  $(LDFLAGS)
	
clean:
	rm -rf bin/obj

.PHONY: all build before out after clean

