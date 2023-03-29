gcc -std=c++14 main.cpp -o run/main -lstdc++ ./function/svmCore.cpp ./function/MotorModel.cpp ./function/transformation.cpp ./function/CurVelModel.cpp ./function/regulator.cpp ./function/invertor.cpp ./kernel.cpp

./run/main

python3 simulationGraph.py