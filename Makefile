CXX = g++
CXXFLAGS = -Iinc -lSDL -lX11 -lglm -Wall -std=c++20
LINT = cpplint

B = bin
S = src

SRCS 	= $(S)/main.cc $(S)/quickcg.cpp $(S)/game.cc \
	$(S)/world.cc $(S)/chunk.cc $(S)/block.cc $(S)/texture.cc \
	$(S)/screen.cc
TARGET 	= $(B)/chibi

.PHONY: clean prebuild all
all: clean prebuild $(TARGET)

clean:
	rm -rf $(B)

prebuild:
	mkdir -p $(B)

$(TARGET): $(SRCS)
	$(CXX) -o $@ $^ $(CXXFLAGS)

