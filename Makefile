CXX := c++
CC := gcc
STRIP := strip
#FLAGS := -std=c++14 -O2 -Wall -I ./include
FLAGS := -std=c++14 -O0 -g -Wall -I ./include
LDFLAGS := -rdynamic -static-libstdc++ libfmt.a

TARGET = cppstation.exe
CPPSOURCES = $(shell echo cpu/*.cpp memory/*.cpp *.cpp)
#CSOURCES = $(shell echo *.c)

OBJECTS = $(CPPSOURCES:.cpp=_cpp.o)
#OBJECTS += $(CSOURCES:.c=_c.o)

all: $(TARGET)

%_cpp.o: %.cpp
	$(CXX) $(FLAGS) -c $< -o $@
	
%_c.o: %.c
	$(CXX) $(FLAGS) -c $< -o $@

$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) $(LDFLAGS) -o $(TARGET)

clean:
	rm $(OBJECTS) $(TARGET)
