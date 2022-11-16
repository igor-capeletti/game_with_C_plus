#!/bin/bash
echo "Algoritmo para gerar um cenario em 3D e navegar pelo cenario atraves de um drone(ficticio): --------------------------------------"
echo ""

g++ jogo_igor_capeletti.cpp -o executavel -lGL -lGLU -lglut -lm      #compilacao do codigo em C++

./executavel
