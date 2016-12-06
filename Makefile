all : 
	g++ -std=c++11 -march=native -g Select.cpp -o Select
	./Select README.md
