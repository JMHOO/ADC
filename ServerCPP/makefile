SourceDir = ./src
IncludeDir = ./src
ObjectDir = ./obj
BinaryDir = ../bin

SourceFile = $(wildcard ${SourceDir}/*.cpp)
objects = $(patsubst %.cpp,${ObjectDir}/%.o,$(notdir ${SourceFile}))

TARGET = adcserver

BIN_TARGET = ${BinaryDir}/${TARGET}

CXX = g++
CFLAGS = -g -Wall -std=c++11 -I${IncludeDir}

${BIN_TARGET} : ${objects}
	$(CXX) $(objects) -pthread -lrt -o $@

${ObjectDir}/%.o:${SourceDir}/%.cpp
	$(CXX) $(CFLAGS) -c $< -o $@

.PHONY : clean
clean :
	find ${ObjectDir} -name *.o -delete
