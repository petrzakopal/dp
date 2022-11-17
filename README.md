# Diplomová práce

## Pracuji na

- research ohledně FPGA (programování v HLS (C/Matlab/Simlink) nevo HDL/Verilog [research.md](/research.md)
- rozfungování serveru a spojení se zybo
  - jaký bude workflow tvoření programů na FPGA s použitím serveru
  - jak se bude build exportovat, přenáše to lokální virtuálky a flashovat do sd karty - návod je na [github.com](https://github.com/Digilent/Petalinux-Zybo) jen na linux, vyzkoušet, jak moc safe je to na macOS
  - šlo by vybuildit na serveru a přenést do macOS a flashnout SD kartu

## Mé denní poznámky ✏️

🔗 [13.10.2022](notes/20221013.md)

---

🔗 [14.10.2022](notes/20221014.md)

---

🔗 [29.10.2022](notes/20221029.md)

---

🔗 [30.10.2022](notes/20221030.md) <-- významný pokrok

---

🔗 [01.11.2022](notes/20221101.md) <-- významný pokrok

---

🔗 [03.11.2022](notes/20221103.md) <-- spíše příspěvky do LaTeXu; 📄 [LaTeX dokument DP](/tex/dp.pdf)

---

📄 [05.11.2022](/tex/dp.pdf) <-- psací den

---

📄 [06.11.2022](/tex/dp.pdf) <-- psací den

---

📄 [11.11.2022](/tex/dp.pdf) <-- převážně psací den

---

👨🏻‍💻 [15.11.2022](/notes/20221115.md) | [krnl_vadd.cpp](/code/simple_vadd/src/krnl_vadd.cpp) | [vadd.cpp](/code/simple_vadd/src/vadd.cpp) <-- example code reverse engineering and commenting

[OpenCL 1.0 refference pages](https://registry.khronos.org/OpenCL/sdk/1.0/docs/man/xhtml/) | [cplusplus.com](https://cplusplus.com/) | [en.cppreference.com](https://en.cppreference.com/w/) | [OpenCL C++ Wrapper API 1.1](https://kosobucki.pl/cl_doc/annotated.html) | [OpenCL man](https://github.com/OpenCL/man)

**Comment:**
_prostě jsem hledal jaké věci co znamenají v dokumentacích na internetu a popisoval jsem si to do example kódu, udělám takto více example souborů abych znal možnosti, co jde a jak jde udělat_

---

👨🏻‍💻 [18.11.2022](/notes/20221118.md) <-- kódící den + řešení debuggingu

---

## Progress

_Coming soon..._

🟩 🟨 🟨 ⬜️ ⬜️ ⬜️ ⬜️ ⬜️ ⬜️ ⬜️ ⬜️ ⬜️ ⬜️ ◻️ ◻️ ◻️ ▫️ ▫️ ▫️

0. Nápad
1. Research
2. Začal jsem pracovat na popsání FPGA, jejich bloků a částí, obecný teoretický základ nutný k pochopení FPGA, které bude dále programováno

## Tech Stack

- PetaLinux - běžící na ARM vývojové desky Digilent
- Vitis - pro programování kernelů a host aplikací
- Vivado - pro design FPGA
- Linux - běžící na serveru, kde dochází k syntéze a tvorbě aplikace
- VS Code - v něm momentálně píši veškeré texty, poznámky a LaTeX dokument
- Figma - pro vytváření názorných obrázků
- LaTeX - pro sázení textu práce
- AutoCAD - pro vytváření schémat
