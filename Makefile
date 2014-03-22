TARGET=lg.out
SRCS = main.cpp developCell.cpp developField.cpp \
	initialize.cpp printPBM.cpp setBound.cpp \
	deadOrAlive.cpp boxCount.cpp

OBJS = $(SRCS:%.cpp=%.o)
CXX = g++
CXXFLAGS = -O3 -std=c++0x -pthread 
LDFLAGS = -pthread 


all: $(TARGET)

$(TARGET): ${OBJS}
	$(CXX) ${OBJS} -o $@ ${LDFLAGS}

%.o: %.cpp etc.h
	$(CXX) $(CXXFLAGS) -c $<

clean:
	rm -f $(OBJS) $(TARGET)
