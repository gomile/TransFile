CXX = g++
CXXFLAGS = -Wall -g -std=c++11
COMPILER = $(CXX) $(CXXFLAGS) -c -o
LINK = $(CXX) $(CXXFLAGS) -o

SRCS = $(wildcard *.cpp)
OBJS = $(patsubst %.cpp, %.o, $(SRCS))
EXE = TransFileCli

all:$(EXE)

$(EXE):$(OBJS)
	$(LINK) $@ $^
%.o:%.cpp
	$(COMPILER) $@ $<

clean:
	rm -f $(OBJS) $(EXE)

