BIN_DIR = bin
TEMP_DIR = intermediate

BIN = $(BIN_DIR)/SensorsServer

HDRS = Sensors.h SensorsServer.h

SRC_SRV = SensorsServer.cpp
OBJ_SRV = $(TEMP_DIR)/SensorsServer.o

SRC_MAIN = Main.cpp
OBJ_MAIN = $(TEMP_DIR)/Main.o

CPP_FLAGS = -std=c++17 -Wall -c
LD_FLAGS = -std=c++17

.PHONY: all clean build

all: clean build

build: $(BIN_DIR) $(TEMP_DIR) $(BIN)

clean:
	rm -rf $(TEMP_DIR)
	rm -rf $(BIN_DIR)

$(BIN_DIR):
	mkdir -p $@

$(TEMP_DIR):
	mkdir -p $@

$(BIN): $(OBJ_SRV) $(OBJ_MAIN)
	g++ $(LD_FLAGS) $^ -o $@

$(OBJ_SRV): $(SRC_SRV)
	g++ $(CPP_FLAGS) -c $^ -o $@

$(OBJ_MAIN): $(SRC_MAIN)
	g++ $(CPP_FLAGS) -c $^ -o $@
