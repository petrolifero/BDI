all : 
	g++ -std=c++11 -march=nehalem -g Select.cpp -o Select
	./Select README.md
