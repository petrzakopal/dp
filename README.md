# Diplomová práce

## Pracuji na

- řešení transferu dat v aplikaci host->kernel–>host pro class
- jak co nejefektivněji udělat model ASM, když nelze použít vektory a struktury jako v klasickém mikroprocesoru DSP - je to jiný přístup s FPGA, záleží i na funkčnosti FPGA, u zynq 7000 třeba je pevně definovaný počet argumentů kernelu, resp. nejde mi funkce když to chci změnit, nejdou používat snadno struktury a mix s polem, uložení proměnných v FPGA asi taky není možné, víc kernelů taky ne (pokud to PL nepodporuje)
- zprovoznění PMOD napojených na PL
- lze arduinem vytvořit nulu na PMOD při paralelním spojení nebo budu arduinem řídit MOSFET a vytvářet požadovaný signál

## Mé denní poznámky ✏️

👨🏻‍💻 [20.02.2023](/notes/20230220.md) <-- **povedlo se JB PMOD z FPGA!!!** (asi po 3 dnech konečně) takže budu zkoušet další PMOD a vložím do designu asi všechny, i když bude možné v budoucnu asi nějaký použít na SPI, I2C atd. to ještě nemám vytestované

---

👨🏻‍💻 [19.02.2023](/notes/20230219.md) <-- stále jsem zkoušel PMOD na PL ale nejde to, už to zkompilovalo v pohodě soubor, po odstranění ve vivado v `Sources/Utilities` soubor s koncovkou `dcp`, ale kernel nejede a ani tam nejsou vidět potřebné gpio výstupy v `/sys/class/gpio`, pracoval jsem na modelu

---

👨🏻‍💻 [16.02.2023](/notes/20230216.md) <-- přidán seznam, co je třeba vyzkoušet aby bylo možné pokračovat a tvořit pohodlně cmodel, napsal jsem něco do [dp.tex](./tex/dp.tex), něco vyscreenoval, zjistil. že je možné mít více akcelerovaných funkcí, resp. více kernelů, zprovoznil GPIO na switch, led, button, zprovoznil PMOD k PS, vyřešil jak je to s vstupem (dává se to na GND, nesmí být v breadbordu dioda, stačí R=120 Ohm), výstup taky v pohodě, na PS JF jsou totiž 200 Ohm rez. na ostatních ne, na to pozor, proto už radši rovnou vkládat rezistor automaticky

---

👨🏻‍💻 [15.02.2023](/notes/20230215.md) <-- povedlo se GPIO push buttons a LED, ale ještě se nepovedlo push buttons + LED + switch, práce na modelu v [cmodel](./code/test-program/cmodel/), začátek porozumívání class v c++ a snaha toho využít při tvorbě modelu, snaha tvořit cmodel v c++ na pc jako kdyby to bylo na fpga a SoC - alokace paměti na 4k boundary atd., změna, že pro kompilaci testovacího modelu není využíván makefile ale [script.sh](./code/test-program/cmodel/script.sh), protože makefile pořádně nerozumím

---

👨🏻‍💻 [14.02.2023](/notes/20230214.md) <-- pořád jsem zkoušel GPIO ale nejde to, zítra zkusím naposled bez návodu, kde se dává zvlášť gpio - použiji ty, co jsou v souboru hotové, pokud to nepůjde s testovacím souborem, zkusím zas odznova, zkoušel jsem připravovat model v C++ a rozvrhovat si to ve složce [cmodel](./code/test-program/cmodel/), nešel mi ale pořádně pochopit makefile, proto kompilaci zatím spouštím pomocí [script.sh](./code/test-program/cmodel/script.sh), dneska vivado a petalinuxy zabrali hodně času, u toho jsem dělal pořád něco jiného, dělal research proč nejde GPIO atd.

---

👨🏻‍💻 [13.02.2023](/notes/20230213.md) <-- - dnes hlavně pokusy o GPIO, začátky heldání optimalizace kernelu a host programu, zkouška kernelu a vstupního parametru funkce jako funkce + ke konci zkouška profilování

---

❌ _12.02.2023_ <-- nic jsem neudělal do DP, ani na státnice, musel jsem dodělat jeden web, ach jo... [Andělská Rapsodie](https://andelskarapsodie.cz) - pokud není žádný obsah, buďto jsem vypl hosting, expirovala domána, nebo ještě nebyl obsah publikován

---

👨🏻‍💻 [11.02.2023](/notes/20230211.md) <--jen úschova odkazů, večer možná na něčem zapracuji, chci zkusit zprovoznit GPIO na LED, možná bude nejlepší zkusit přímo flow dle návodů a ne flow od accelerated aplikace, problém je prostě s importem knihoven

---

👨🏻‍💻 [10.02.2023](/notes/20230210.md) <-- docela veliký todo list, který chci splnit

---

👨🏻‍💻 [09.02.2023](/notes/20230209.md)

---

👨🏻‍💻 [08.02.2023](/notes/20230208.md) <-- stále snaha o přenos dat, testování basic kódů c++, už trochu začínám chápat princip, pozor na omezení max 4 argumentů do kernelu v ZCU\_\_\_\_ (zynq 7000), přišel jsem na to omylem, v nějakých SoC je i více..., připsal jsem poznámky a testovací kódy, YES -> „už to nefunguje, když to nemá fungovat, ale je třeba zjistit, zda to bude fungovat i když to má fungovat, to je na další den ⬇️“ – je třeba dodávat soubory z HW buildu - `binary_container_1.xclbin image.ub BOOT.BIN zybo-dp-test-data`

---

👨🏻‍💻 [07.02.2023](/notes/20230207.md) <-- snaha o rozchození a pochopení migrace dat, vektory atd. atd.

---

👨🏻‍💻 [06.02.2023](/notes/20230206.md)

---

👨🏻‍💻 [29.11.2022](/notes/20221129.md)

---

👨🏻‍💻 [27.11.2022](/notes/20221127.md) <-- poslední snaha rozjet kria a debugger, popisování vivado flow, vytváření MindMap ohledně tvorby modelu ASM v C++ a jeho konverze na akcelerovanou aplikaci

---

👨🏻‍💻 [26.11.2022](/notes/20221126.md) <-- snaha opět rozjet debugger, vzal jsem rozjetí debuggeru nebo jinou platformu, nelze to, SW qemu emulace nespustí FPGA logic, rkia HW ve vitis nelze udělat dle návodu, popsal jsem flow, jak to budu asi muset udělat, jen pár věcí do LaTeXu --> PDF 📄 [Dip.Práce ✏️](/tex/dp.pdf)

---

👨🏻‍💻 [20.11.2022](/notes/20221120.md) <-- převážně out of order den, ale napsal jsem něco do DP LaTeXu [PDF](/tex/dp.pdf), zkouším nápady z [19.11.2022](/notes/20221119.md)

---

👨🏻‍💻 [19.11.2022](/notes/20221119.md) <-- snaha opět rozjet debugger

---

👨🏻‍💻 [18.11.2022](/notes/20221118.md) <-- kódící den + řešení debuggingu + přišel jsem na to, jak má asi fungovat debugging ale mně nefunguje, nice, pozitivních 13 hodin stráveno nad nulovým posunem, jako vždy

---

👨🏻‍💻 [15.11.2022](/notes/20221115.md) | [krnl_vadd.cpp](/code/simple_vadd/src/krnl_vadd.cpp) | [vadd.cpp](/code/simple_vadd/src/vadd.cpp) <-- example code reverse engineering and commenting

[OpenCL 1.0 refference pages](https://registry.khronos.org/OpenCL/sdk/1.0/docs/man/xhtml/) | [cplusplus.com](https://cplusplus.com/) | [en.cppreference.com](https://en.cppreference.com/w/) | [OpenCL C++ Wrapper API 1.1](https://kosobucki.pl/cl_doc/annotated.html) | [OpenCL man](https://github.com/OpenCL/man)

**Comment:**
_prostě jsem hledal jaké věci co znamenají v dokumentacích na internetu a popisoval jsem si to do example kódu, udělám takto více example souborů abych znal možnosti, co jde a jak jde udělat_

---

📄 [11.11.2022](/tex/dp.pdf) <-- převážně psací den

---

📄 [06.11.2022](/tex/dp.pdf) <-- psací den

---

📄 [05.11.2022](/tex/dp.pdf) <-- psací den

---

🔗 [03.11.2022](notes/20221103.md) <-- spíše příspěvky do LaTeXu; 📄 [LaTeX dokument DP](/tex/dp.pdf)

---

🔗 [01.11.2022](notes/20221101.md) <-- významný pokrok

---

🔗 [30.10.2022](notes/20221030.md) <-- významný pokrok

---

🔗 [29.10.2022](notes/20221029.md)

---

🔗 [14.10.2022](notes/20221014.md)

---

🔗 [13.10.2022](notes/20221013.md)

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
