CC   = gcc
CXX  = g++
AR   = ar
C_FLAGS= -Wall -O2 -g

LIB = -lprotobuf -lprotoc

TARGET= cpp_service
OBJECTS= cpp_service_generator.o cpp_service_helpers.o cpp_service.o

all : $(TARGET)

$(TARGET) : $(OBJECTS)
	$(CXX) -o $(TARGET) $(OBJECTS) $(LIB)

.cpp.o:
	$(CXX) -c -o $*.o $(C_FLAGS) $(INC) $*.cpp
.cc.o:
	$(CXX) -c -o $*.o $(C_FLAGS) $(INC) $*.cc

clean:
	rm -f $(OBJECTS) $(TARGET)