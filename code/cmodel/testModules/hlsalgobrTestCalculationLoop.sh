gcc -std=c++14 testCalculationLoopHlsAlgo.cpp -o run/testCalculationLoopHlsAlgo -lstdc++ ./../function/svmCore.cpp ./../function/MotorModel.cpp ./../function/transformation.cpp ./../function/CurVelModel.cpp ./../function/regulator.cpp ./../function/invertor.cpp

./run/testCalculationLoop

python3 simulationGraph.py