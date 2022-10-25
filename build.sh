#!/bin/bash
g++ -c -o tme.o tme.cpp -std=c++20 -g
cd styles
g++ -c -o RoundedRectangleShape.o RoundedRectangleShape.cpp -std=c++20 -g
g++ -c -o Default.o Default.cpp -std=c++20 -g
cd ..
g++ -o tme tme.o styles/Default.o styles/RoundedRectangleShape.o -lsfml-system -lsfml-graphics -lyaml-cpp -g
