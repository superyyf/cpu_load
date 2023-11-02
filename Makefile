CXX ?= g++
CXXFLAGS = -I $(INC_PATH)
COBJFLAGS = $(CXXFLAGS) -c

TARGET = demo

CURDIR := $(shell pwd)
SRC_PATH := $(CURDIR)/src
INC_PATH := $(CURDIR)/include
BUILD_PATH := $(CURDIR)/build

SRC := $(foreach x, $(SRC_PATH), $(wildcard $(x)/*.c*))
OBJ := $(addprefix $(BUILD_PATH)/, $(addsuffix .o, $(notdir $(basename $(SRC)))))

CLEAN_LIST := $(TARGET) $(OBJ)

default: mkdirs all

$(TARGET): $(OBJ)
	$(CXX) -o  $@ $(OBJ) $(CXXFLAGS)

$(BUILD_PATH)/%.o: $(SRC_PATH)/%.c*
	$(CXX) $(COBJFLAGS) $< -o $@


.PHONY: mkdirs all clean
mkdirs:
	@mkdir -p $(BUILD_PATH)

all: $(TARGET)

clean:
	@echo clean $(CLEAN_LIST)
	@rm -f $(CLEAN_LIST)