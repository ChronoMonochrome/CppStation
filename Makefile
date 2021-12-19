CXX := c++
STRIP := strip
FLAGS := -std=c++2a -O2 -Wall -I ./include
LDFLAGS := -static-libstdc++

TARGET = cppstation.exe
CSOURCES = $(shell echo memory/*.cpp *.cpp)

OBJECTS = $(CSOURCES:.cpp=_cpp.o)

all: $(TARGET)

%_cpp.o: %.cpp
	$(CXX) $(FLAGS) -c $< -o $@

$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) $(LDFLAGS) -o $(TARGET)
	$(STRIP) $(TARGET)

clean:
	rm $(OBJECTS) $(TARGET)
