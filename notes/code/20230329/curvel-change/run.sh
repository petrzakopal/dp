gcc -std=c++14 main.cpp -o run/main -lstdc++ ./function/svmCore.cpp ./function/MotorModel.cpp ./function/transformation.cpp ./function/CurVelModel.cpp ./function/regulator.cpp ./function/invertor.cpp krnl_main.cpp krnl_inv_asm.cpp

./run/main

python3 simulationGraph.py