# ────────────────────────────────────────────────────────────────────────
#   Makefile for a simple Raylib project on macOS
#
# 1. Make sure Raylib and pkg‑config are installed:
#      brew install raylib pkg-config
#
# 2. Build:   make
# 3. Clean:   make clean
#
# ────────────────────────────────────────────────────────────────────────

CXX       := g++
CXXFLAGS  := -std=c++20 $(shell pkg-config --cflags raylib) -Wall -Wextra -Wno-missing-field-initializers
LDFLAGS   := $(shell pkg-config --libs raylib)

TARGET    := game
SRCS      := $(wildcard *.cpp)
OBJS      := $(SRCS:.cpp=.o)

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) -o $@ $^ $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)