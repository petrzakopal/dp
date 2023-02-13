# Diplomová práce

## Pracuji na

- řešení transferu dat v aplikaci host->kernel–>host
- jak co nejefektivněji udělat model ASM, když nelze použít vektory a struktury jako v klasickém mikroprocesoru DSP - je to jiný přístup s FPGA, záleží i na funkčnosti FPGA, u zynq 7000 třeba je pevně definovaný počet argumentů kernelu, resp. nejde mi funkce když to chci změnit, nejdou používat snadno struktury a mix s polem, uložení proměnných v FPGA asi taky není možné, víc kernelů taky ne (pokud to PL nepodporuje)

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

👨🏻‍💻 [18.11.2022](/notes/20221118.md) <-- kódící den + řešení debuggingu + přišel jsem na to, jak má asi fungovat debugging ale mně nefunguje, nice, pozitivních 13 hodin stráveno nad nulovým posunem, jako vždy

---

👨🏻‍💻 [19.11.2022](/notes/20221119.md) <-- snaha opět rozjet debugger

---

👨🏻‍💻 [20.11.2022](/notes/20221120.md) <-- převážně out of order den, ale napsal jsem něco do DP LaTeXu [PDF](/tex/dp.pdf), zkouším nápady z [19.11.2022](/notes/20221119.md)

---

👨🏻‍💻 [26.11.2022](/notes/20221126.md) <-- snaha opět rozjet debugger, vzal jsem rozjetí debuggeru nebo jinou platformu, nelze to, SW qemu emulace nespustí FPGA logic, rkia HW ve vitis nelze udělat dle návodu, popsal jsem flow, jak to budu asi muset udělat, jen pár věcí do LaTeXu --> PDF 📄 [Dip.Práce ✏️](/tex/dp.pdf)

---

👨🏻‍💻 [27.11.2022](/notes/20221127.md) <-- poslední snaha rozjet kria a debugger, popisování vivado flow, vytváření MindMap ohledně tvorby modelu ASM v C++ a jeho konverze na akcelerovanou aplikaci

---

👨🏻‍💻 [29.11.2022](/notes/20221129.md)

---

👨🏻‍💻 [06.02.2023](/notes/20230206.md)

---

👨🏻‍💻 [07.02.2023](/notes/20230207.md) <-- snaha o rozchození a pochopení migrace dat, vektory atd. atd.

---

👨🏻‍💻 [08.02.2023](/notes/20230208.md) <-- stále snaha o přenos dat, testování basic kódů c++, už trochu začínám chápat princip, pozor na omezení max 4 argumentů do kernelu v ZCU\_\_\_\_ (zynq 7000), přišel jsem na to omylem, v nějakých SoC je i více..., připsal jsem poznámky a testovací kódy, YES -> „už to nefunguje, když to nemá fungovat, ale je třeba zjistit, zda to bude fungovat i když to má fungovat, to je na další den ⬇️“ – je třeba dodávat soubory z HW buildu - `binary_container_1.xclbin image.ub BOOT.BIN zybo-dp-test-data`

---

👨🏻‍💻 [09.02.2023](/notes/20230209.md)

---

👨🏻‍💻 [10.02.2023](/notes/20230210.md) <-- docela veliký todo list, který chci splnit

---

👨🏻‍💻 [11.02.2023](/notes/20230211.md) <--jen úschova odkazů, večer možná na něčem zapracuji, chci zkusit zprovoznit GPIO na LED, možná bude nejlepší zkusit přímo flow dle návodů a ne flow od accelerated aplikace, problém je prostě s importem knihoven

---

_12.02.2023_ <-- nic jsem neudělal do DP, ani na státnice, musel jsem dodělat jeden web, ach jo... [Andělská Rapsodie](https://andelskarapsodie.cz) - pokud není žádný obsah, buďto jsem vypl hosting, expirovala domána, nebo ještě nebyl obsah publikován

👨🏻‍💻 [13.02.2023](/notes/20230213.md)

## Tech Stack

- PetaLinux - běžící na ARM vývojové desky Digilent
- Vitis - pro programování kernelů a host aplikací
- Vivado - pro design FPGA
- Linux - běžící na serveru, kde dochází k syntéze a tvorbě aplikace
- VS Code - v něm momentálně píši veškeré texty, poznámky a LaTeX dokument
- Figma - pro vytváření názorných obrázků
- LaTeX - pro sázení textu práce
- AutoCAD - pro vytváření schémat
- ExcaliDraw - na občasné interní nákresy / MindMap
- iTerm2 - Terminal [iterm2](https://iterm2.com/)
- Oh My ZSH - konfigurace terminálu zsh [ohmyz](https://ohmyz.sh/)
- powerlevel10k - nejlepší théma na zsh [powerlevel10k](https://github.com/romkatv/powerlevel10k)
