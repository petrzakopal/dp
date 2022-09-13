# Research of a subject

_Advanced numerical model of permanent magnet synchronous motor implemented on FPGA for Hardware-in-in-the-loop systems_

Where to take inspiration

- [Conference paper](http://poseidon2.feld.cvut.cz/conf/poster/)
- [IEEE](https://ieeexplore.ieee.org/document/9432517)
- [Diploma Thesis ČVUT](https://dspace.cvut.cz/bitstream/handle/10467/89815/F3-DP-2020-Sterba-Vojtech-VojtechSterba.pdf?sequence=-1&isAllowed=y)
- [YouTube HIL Simulation](https://www.youtube.com/watch?v=vrRq6YovMtk)

## Hardware in the loop systems - HIL

- slouží k testování komplexních embeded real time systémů

## FPGA

- Rychlejší switching events
- Procesuje paralelně oproti CPU, které sériově, ale nutnost převést model do diskrétního času, jed dražší
- VHDL nebo HDL programování, docela hard, na to aby se tam naprogramoval v čistém kódu nějaký matematický model
- asi bude programováno v simulinku s HDL compatible blocks a pak převedeno na Xilinx VHDL a implementováno
- FPGA programming in matlab [MathWorks](https://www.mathworks.com/discovery/fpga-programming.html)
- Getting Started With Matlab Programming [MathWorks](https://www.mathworks.com/solutions/fpga-asic-soc-development/resources.html)
- jediné dostupné Intel FPGA, které připadá v úvahu [Mouser](https://cz.mouser.com/ProductDetail/Intel/EK-10CL025U256?qs=HXFqYaX1Q2zh%2FEGiixG4LQ%3D%3D)

# Random notes

- resolver - lepší než encoder, jak funguje? [YouTube](https://www.youtube.com/watch?v=7PKJ52b1Qvs)
