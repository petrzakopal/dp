# all: main modelFunctions parameterInitialization mathSolver

run: main
	./main



main: main.cpp
	gcc -std=c++11 main.cpp -o main -lstdc++ function/modelFunctions.cpp function/parameterInitialization.cpp function/mathSolver.cpp



# modelFunctions: function/modelFunctions.cpp
# 	gcc -std=c++11 main.cpp function/modelFunctions.cpp  -o ./../modelFunctions -lstdc++ -v

# parameterInitialization: function/parameterInitialization.cpp
# 	gcc -std=c++11 main.cpp function/parameterInitialization.cpp  -o ./../parameterInitialization -lstdc++

# mathSolver: function/mathSolver.cpp
# 	gcc -std=c++11 main.cpp function/mathSolver.cpp  -o ./../mathSolver -lstdc++	