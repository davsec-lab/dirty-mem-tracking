CXX=g++
CXXFLAGS=-fpic -O0 -ggdb

TARGET=test_sd

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(TARGET): test_sd.o sd.o
	$(CXX) -O0 -pie test_sd.o sd.o -o test

clean:
	rm *.o test
