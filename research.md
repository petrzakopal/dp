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
- supported FPGAs for HDL verifier [MathWorks](https://www.mathworks.com/help/supportpkg/alterafpgaboards/ug/altera-fpga-board-support-from-hdl-verifier.html)
- kde je možné také vyvíjet a simulovat věci na FPGA od intelu (Lite edice) [Intel](https://www.intel.com/content/www/us/en/collections/products/fpga/software/downloads.html?product=modelsim_ae&s=Newest&f:guidetm83741EA404664A899395C861EDA3D38B=%5BIntel%C2%AE%20Cyclone%C2%AE%5D)
- HDL coder do Matlabu [MathWorks](https://www.mathworks.com/products/hdl-coder.html)
- FPGA YouTube programming video for intel [YouTube](https://www.youtube.com/watch?v=vmraRVxKYss)
- programování FPGA [Design of FPGA-controlled power electronics and drives using MATLAB Simulink](https://ieeexplore-ieee-org.ezproxy.techlib.cz/document/6579155)
- Simulink Modeling and Design of an Efficient Hardware-Constrained FPGA-Based PMSM Speed Controller [Simulink Modeling and Design of an Efficient Hardware-Constrained FPGA-Based PMSM Speed Controller](https://ieeexplore-ieee-org.ezproxy.techlib.cz/document/6179536)
- FPGA Zybo Z7 Docs [Docs](https://digilent.com/reference/programmable-logic/zybo-z7/reference-manual?redirect=1)

# Random notes

- resolver - lepší než encoder, jak funguje? [YouTube](https://www.youtube.com/watch?v=7PKJ52b1Qvs)
- verilog programming [YouTube](https://www.youtube.com/watch?v=vmraRVxKYss)
- tyto kostky jsou řízeny pomocí FPGA [GitHub](https://github.com/kbob/LED-Cube) [Twitter](https://twitter.com/esden/status/1160309492896215040)
- IP znamená Intellectual Property = znovupoužitelý blok, jak už HW tak SW, který není závislý an výrobci a HW, jde použít univerzálněji nebo méně univerzálněji [Info](https://www.techtarget.com/whatis/definition/IP-core-intellectual-property-core)

# Development

- development kit [E-Shop](https://digilent.com/shop/zybo-z7-zynq-7000-arm-fpga-soc-development-board/)
- basic tutoriál na HDL coder [MathWorks](https://www.mathworks.com/help/hdlcoder/ug/getting-started-with-hardware-software-codesign-workflow-for-xilinx-zynq-platform.html)
- development in VIVADO [YouTube](https://www.youtube.com/watch?v=7HVpl1HiZoY)
- VERILOG and VHDL resources [Nandland](https://nandland.com/)
- VERILOG practice [HDLbits](https://hdlbits.01xz.net/wiki/Main_Page)
- C to Verilog article [Hackaday](https://hackaday.com/2015/12/17/xilinx-fpgas-in-c-for-free/)
- LegUp HLS C to verilog [LegUp](http://legup.eecg.utoronto.ca/) - free for non commercial
- C-like HDL use din game [GitHub](https://github.com/JulianKemmerer/PipelineC)
- Python convertsion library to VHDL [MyHDL](https://www.myhdl.org/)
- Vivado HLS Introduction [YouTube](https://www.youtube.com/watch?v=5lYq8_bqAcI)
- Getting started with Vivado HLS Tutorial [YouTube](https://www.youtube.com/watch?v=hZ2RGwLmXc0)
- Vivado HLS has been replaced by Vitis [Vitis Docs](https://www.xilinx.com/support/documentation-navigation/design-hubs/dh0090-vitis-hls-hub.html)

## Nápad jak udělat práci

- Nejdříve napsat v C HLS ve Xilix Vitis
- Exportovat IP
- vložit do Xilinx Vivado
- nahrát do FPGA

### Problémy 🔴

- nutnost naprogramování vlastních funkcí na transformace, na výpočty atd
- nutnost znalosti alespoň trochu VHDL nebo Verilog
- nutnost více SW
- nutnost ruční optimalizace

### Výhody 🟢

- open source ❤️
- možnost snadnějšího portu na jiné FPGA i jiné vendors
- lépe optimalizovat než Matlab
- i SW na managing je open source (matlab ne)
- když bude dobrá optimalizace v C, tak dobrá optimalizace i HDL, lepší efficiency
