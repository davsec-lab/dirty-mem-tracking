CXX=g++
CXXFLAGS=-fpic -O2

TARGET=test_sd

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(TARGET): test_sd.o sd.o
	$(CXX) -pie test_sd.o sd.o -o test
