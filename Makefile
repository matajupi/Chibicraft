CXX = g++
CXXFLAGS = -Iinc -lSDL -lX11 -lglm -Wall -fopenmp
LINT = cpplint

B = bin
S = src

SRCS 	= $(S)/main.cc $(S)/quickcg.cpp $(S)/game.cc
TARGET 	= $(B)/chibi

.PHONY: clean prebuild all
all: clean prebuild $(TARGET)

clean:
	rm -rf $(B)

prebuild:
	mkdir -p $(B)

$(TARGET): $(SRCS)
	$(CXX) -o $@ $^ $(CXXFLAGS)

