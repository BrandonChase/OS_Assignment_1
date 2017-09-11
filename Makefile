CXX= g++ -std=c++11
CXXFLAGS= -Wall -g
LDFLAGS= 
INCLUDE= 

SRCDIR= ./src
BINDIR= .
TARGET= $(BINDIR)/ts

SOURCE= $(wildcard $(SRCDIR)/*.cpp)

all:
	$(CXX) $(CXXFLAGS) $(INCLUDE) $(SOURCE) $(LDFLAGS) -o $(TARGET)

clean:
	rm $(TARGET)