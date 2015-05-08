all: visualizer generator counter
visualizer: visualizer.cpp
	icpc -std=c++11 visualizer.cpp -o visualizer
generator: generator.cpp
	icpc -std=c++11 generator.cpp -o generator
counter: counter.cpp
	icpc -std=c++11 counter.cpp -I../../src/ ../../src/util.cpp -o counter

clean: 
	rm visualizer generator counter
.PHONY: all clean
