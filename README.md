# Diplomová práce

## Pracuji na

- ⚡️ _zkoumám_ interrupts + timers
- přesun I-n modelu do FPGA
- ✅ _akvizize dat nejspíše v dalším vlákně programu pomocí timeru a jeho přerušení a mutexů, to vlákno bude nekonečné a spojí se až na konci programmu_ hledání jak funguje timer a přerušení a jak dělat akvizici dat do zybo pomocí PMOD GPIO a XADC pro otáčková resp. proudová čidla
- najít proudová a otáčková vhodná čidla nebo jak to naemulovat něčím dalším
- ✅ začít dělat regulátor a výstup pro měnič v cmodelu...
- ✅ momentální cíl -> model do FPGA, zkusit reakci na předpočítaná data, po úspěchu začít modelovat regulátory, SVM, výstup na měnič, výstup z pmod
- lze arduinem vytvořit nulu na PMOD při paralelním spojení nebo budu arduinem řídit MOSFET a vytvářet požadovaný signál (modelování vstupů do fpga / výstupů z motoru)
- a další...

## Mé denní poznámky ✏️

💻 [09.04.2023](/notes/20230409.md)

---

❌ [08.04.2023]()

---

💻 [07.04.2023](/notes/20230407.md) <-- snaha o rebuild s timerem, protože axi quad spi s spidev způsobil nenačtení kernelu, psaní v dp.tex ohledně RT patche atd

---

💻 [06.04.2023](/notes/20230406.md) <-- vyřešení timer generic-uio, ale problém s spi, psaní dp.tex

---

💻 [05.04.2023](/notes/20230405.md) <-- začalo mi fungovat generiq, zkouším jak to vylepšit abych mohl přejít na SPI

---

❌ [04.04.2023]()

---

❌ [03.04.2023]()

---

❌ [02.04.2023]()

---

❌ [01.04.2023]()

---

🐧 [31.03.2023](/notes/20230331.md) <-- řešení device tree a pl.dtsi

---

🐧 [30.03.2023](/notes/20230330.md) <-- řešení device tree a pl.dtsi a úprava a optimalizace kódu

---

🧪 [29.03.2023](/notes/20230329.md) <-- řešení pl overlay a preemptive real time + opravy kódu na modely + zprovoznění dvou kernelů (zatím nestreamovaná data)

---

🧪 & ⚙️ [28.03.2023](/notes/20230328.md) <-- snaha o patch linuxu a vytvoření modelu ASM a Invertoru do kernelu v test benchi a přesun do Kernelu

---

🐛 [27.03.2023](/notes/20230327.md) <-- debugging a řešení problémů + vytváření frameworku na testování v pc

---

💻 [26.03.2023](/notes/20230326.md) <--celý den snaha o zprovoznění modelu ASM v CPU a řízení v FPGA, problémy se segmentation fault asi kvůli xrt.ini ale když tam zas není ale includuju knihovny a mám v build settings v hw_link --config profile.cfg, tak to zas nejde a ani basic věc to nevydá správně a musím tam xrt.ini zase vložit, vrátil jsem se do nočníhoh kódu kter'ý totiž měl fungovat a najednou nefungoval... pak se zas zkusím vrátit dopředu, pomocí stažení kódu z commitu, nevím proč kód na regulátory, který funguje v pc nefunguje v fpga (array refactoring)

---

💻 [25.03.2023](/notes/20230325.md) <--celý den snaha o zprovoznění modelu ASM v CPU a řízení v FPGA + řešení SPI

---

💻 [24.03.2023](/notes/20230324.md) <--celý den snaha o zprovoznění modelu ASM v CPU a řízení v FPGA + řešení SPI

---

💻 [23.03.2023](/notes/20230323.md) <-- řešení SPI ADC, nového designu vivado, přidávání výpočtů do kernelu pro řízení pohonu, testování buildů

---

💻 [22.03.2023](/notes/20230322.md) <-- snaha optimalizovat, zásek v buildění, IDE už nefunguje, používám jen na tvorbu makefilů, vše jedu přes terminál - nejdřív jen kria-1-app, pak kria-1-app_kernels a nakonec kria-1-app_system_hw_link a vše jedu jako `make -j20 all`, po tom co si zítra změřím kolik času trval přenos old, kdy nebyly jen dva vektory a když jsou jen dva vektory, tak budu pokračovat v přepisování cmodelu do hls, na čase jsou regulátory..., opravoval jsem texty v dp.tex, v pc jsou ve složce two-vectors způsob pro dva vektory, který právě poměřím s mnoha vektory (arrays), myslím, že budou asi rychlejší i jen ty dva, i když to není logické, protože je možnost méně paralelismu, ale je třeba méně volat přesuny zas mezi paměťmi, nejelšpí by bylo přesouvat nejméně 2 MB dat aby byla linka využita, 2 GB i víc, ale to nejde v realtimu

---

💻 [21.03.2023](/notes/20230321.md) <-- snaha vylepšit curvel model a přijít na to, proč kernel tak dlouho trvá, kernel je rychlý, ale trvá přenos dat a enqueue věcí, dodělání excelu na pmod, otestoval jsem i vec-add, taky to nebylo nic moc s tím přenosem dat, musím zmenšit počet přenášených bufferů

---

💻 [20.03.2023](/notes/20230320.md) <-- rozchození starší verze curvel model, snaha o optimalizaci - bude muset přijít na řadu optimalizace v podobě hls::stream, popis dole, vytvoření skriptů na transfer dat server -> pc -> kria a na transport analytik kria -> pc -> server, začátek lepší přípravy pinoutu v excelu

---

💻 [19.03.2023](/notes/20230319.md) <-- povedlo se rozchodit linux, testovací appka, pak jsem začal dělat novou strukturu projektů na serveru a přestalo mi fungovat napojení věcí na jednotlivé buildy, začínám znovu, popsal jsem / vypsal jsem z hackster.io postup tvorby a opravil tam chyby, začínám dělat v excelu popis na piny package FPGA a schematics pro PMOD atd., inspirace z hackster, zjištěn problém s eth0, musí se dělat zatím vždycky `ifup eth0` a pak až kria dashboard

---

💻 [18.03.2023](/notes/20230318.md) <-- řešení problému xdc pro mapování pinů (větrák), buildění petalinuxu, potřebuje to víc místa na disku než zybo, nějak jsem opravoval [dp.tex](./tex/dp.tex)

---

💻 [17.03.2023](/notes/20230317.md) <-- přestavba pracoviště a začátek kria

---

💻 [16.03.2023](/notes/20230316.md) <-- snaha o linux kernel driver, rozchodil jsem tutorial driver (character), zkkopíroval uio generic a zksuil upravit a vyhodit nějaké věci, to fungovalo částečně, po restartu pak spadlo a zaseklo se, driver example z petalinuxu se sekne a je CPU stall, zkusím ještě jeden poslední, pak vše budu řešit v userspacu a změřím si to asi, četl jsem si nějaké info o kria kr260, že to bude jiné a bude jiný flow, je tam rozdíl v bootovaní a image atd., bude to ještě hell, ale je to progresivnější než zybo od digilentu, je to dobré napsat do DP a porovnat, protože kria KV260 myslím dle press release vyšla 2021 a kr260 v 2022, rozdíl je hlavně v carrier board, jinak SOM by mělo být stejné

---

💻 [15.03.2023](/notes/20230315.md) <-- práce a snaha ohledně rozchození linux kernel driveru, ale asi to zatím nejde, problém s petalinuxem, problém s patchnutým linuxem, že má jiný source a já to ještě v makefile neumím, snaha zajistit interrupt v example souboru mymodule, třeba dodtat informace do device-tree atd., na další den bude postup takový, že se vybuildil linux bez interruptů a bude se zkoušet jen testovací [mymodule_edited.c](./notes/code/20230314/mymodule_edited.c) a také ze stránky ohledně tutoriálu ohledně device zápisu [http://derekmolloy.ie/writing-a-linux-kernel-module-part-2-a-character-device/](http://derekmolloy.ie/writing-a-linux-kernel-module-part-2-a-character-device/), je třeba zjistit, zda nemusí být interrupt v HDF udělán aby se na něj mohl dle mymodule_edited.c připojit ale byl stále vidět jako gpio a nebo to nejde a bude muset být jako interrupt, ale pak zase nebude jasné jeho číslo..., také jsem si nakreslil možné varianty akvizice dat, další den je třeba se pobavit s vedoucím

---

💻 🐧 [14.03.2023](/notes/20230314.md) <-- práce na rozchození pracoviště se serverem ve škole, proxmox nastavení a virtuálky matlabu pro pracovníky katedry, vytvoření účtů, nastavení port routingu..., vytvoření prvního testovacího kernel modulu, snaha o jeho build, ještě není otestován [mymodule_edited.c](./notes/code/20230314/mymodule_edited.c), popsání postupu

---

🚗 [13.03.2023](/notes/20230313.md) <-- přesun pracoviště na fakultu, zkoušení pointerů, todo na další dny než přijde deska je začít psát linux driver na interrupts a začít zkoumat kria a začít kontrolovat text v [dp.tex](./tex/dp.tex) a začít začleňovat více KRIA

---

❌ [12.03.2023](/) <- dělal jsem na zakázce a zkoušel jsem dostat se zas z kernelu do registrů, dostal jsem se z PS zas do registru, resp. do adresy, ale z PL to nejde, protože to nejde brát jako subprogram ale jako IP ve vivadu, kde i když vložím adresu jako argument a pak dereferencuji v kernelu a vložím hodnotu, tak že dojde ke změně hodnoty v PS procesoru, musel bych si udělat proměnnou, jejíž hodnota by se pak v PS vkládala do té hodnoty na adrese

---

❌ [11.03.2023](/) <-- snažil jsem se použít registry v kernelu ale nějak to nejde, zkusil jsem požáda o informaci AI a stejně nic, poté jsem musel jít pryč

---

💻 [10.03.2023](/notes/20230310.md) <-- snaha o namapování registrů a rozchození dalšího kernelu

---

💻 [09.03.2023](/notes/20230309.md) <-- snaha buildu velikého designu a hledání informací o tom jak zrychli kernel a udělat free running kernel

---

🐧 [08.03.2023](/notes/20230308.md) <-- linux patch se povedl, sepsán postup a snaha tvorby většího vivado designu a refactoring kódu na SoC

---

🐧 [07.03.2023](/notes/20230307.md) <-- snaha o RT patch linuxu, nepodařilo se, snaha vyzkoušet několik variant, nevyšlo to, zítra ještě jedna varianta a pak prohlásím, jako Xilinx, že neposkytují na toto podporu i když se to někomu podařilo...., vylepšení kódu na I-n model v zybo - udělán víc basic jen s pomocí arrays..., je divné že i při vitis_analyzer syntéze atan2f a sqrtf v kernelu ukazuje mnoho využití LUTs ale v realitě to funguje a syntetizovalo se to, demo na [YouTube](https://youtu.be/MlzqgY1zWvk)

---

👨🏻‍💻 🔎 [06.03.2023](/notes/20230306.md) <-- dělal jsem research ohledně zrychlení linuxu a běhu apliakcí fpga + ps, bude třeba patch RT na petalinux - vyzkoušet to, pro nejrychlejší obsluhu přerušení bude nejlepší si napsat svůj linux driver, v petalinuxu se to nazývá `kernel module` do aplikace na host (host.cpp) je třeba přidat část, která bude řešit že při vyslání `SIGTERM` neboli `ctrl+c` tak nastaví vše na nulu a program zastaví/motor zastaví

---

❌ [05.03.2023](/) <-- práce na zakázce, oddech od diplomky

---

👨🏻‍💻 🐛 [04.03.2023](/notes/20230304.md) <-- debugging [testCalculationLoop](./code/cmodel/testModules/testCalculationLoop.cpp), byl problém s regulátory, chyby ve vložení hodnot do transformací vlivem kopírování nebo nepozornosti, polish kódu

---

💻 [03.03.2023](/notes/20230303.md) <-- úprava a kompletace c++ modelu v [cmodel](./code/cmodel/testModules/testCalculationLoop.cpp), příprava kontrolních grafů cmodelu v pythonu [simulationGraph](./code/cmodel/testModules/simulationGraph.py)

---

👨🏻‍💻 🐍 [02.03.2023](/notes/20230302.md) <-- práce na FOC SVM, min max metody, komparační úrovně, testové příklady v [cmodel/testmodules](./code/cmodel/testModules/), vyřešení testovího plotění grafů, protože mathematika na 100 k hodnot je na zobrazení moc pomalá a matlab musí být aktuálně nasměrován myslím do adresáře odkud má část data, takže využit `python` [graph.py](./code/cmodel/testModules/graph.py)

---

👨🏻‍💻 🧑🏻‍🎓 [01.03.2023](/notes/20230301.md) <-- řešení interruptu, proxmox schování MAC adresy VM za Proxmox server, řešení možnosti využití multithreadingu, konzultace na katedře

---

👨🏻‍💻 [28.02.2023](/notes/20230228.md) <-- stále snaha o interrupt ohledně timeru - na pravidelnou akvizici dat, řešil jsem interrupty, buildil jsem petalinux a design, protože se zase oběvil `dcp` soubor a já si ho nevšiml, ztratil jsem 3 hodiny, problém s interrupty, XADC atd., **POVEDLO SE** jako rozchození interruptu a běhu timeru v linuxu, viz video [YouTube](https://youtu.be/3upD2LAqUCk), nepsal jsem ještě nikam postup, ale ve videu o něm trochu mluvím, ale určitě to bude třeba si zapsat, už je toho moc...

---

👨🏻‍💻 [27.02.2023](/notes/20230227.md) <-- celý den řeším interrupty, tudíž vlastně i timer, problém je v tom, že v bare-metal aplikaci je to docela easy a knihovny jdou naimportovat, v linuxu - tudíž aby mohlo být využito FPGA jako akcelerace, to není tak easy a je třeba využít buďto custom Linux driverů a nebo UIO a různých vrstev linuxu a registrů a dalšího designu ve vivado atd., našel jsem fajn článek na LinkedIn o tom, ale chybí tam část, jak si člověk dělá vlastní IP blok do vivado, to neumím pořádně a nerozumím tomu, ta část bez custom ip bloku nějak fungovala, ještě jsem nezkoumal pořádně C++ kód, resp. hledal jsem jak to funguje a nějaké odkazy, ale pořádně to nemám rozmyšleno, zatím tedy přerušení se nějak rozchodilo od gpio ale ne od timeru, to budu muset vyřešit..., vypadá to jako málo práce, ale je to docela dost na pochopení náročné, ne jako PIC, v návodu jsem jednu větu četl asi 15x a stejně to nechápu (ohledně custom IP ve vivado)

---

👨🏻‍💻 [26.02.2023](/notes/20230226.md) <-- pracoval jsem opět na vylepšení kernelu, snaha o větší optimalizaci ale nepovedla se, problém, že i poté po vrácení kódu nešla vůbec syntéza - moc LUTs, hledal jsem opět interrupts, připravil si bloky regulátoru, připravil design timeru, brainstorming obr. [program brainstorm flow](./notes/images/20230226/20230226_brainstorm-program-flow.png), zjišťoval více informací o profilování a vyprofiloval dva druhy aplikace, problém je, že když se poté udělal znovu kernel s profilováním, nebylo dostatek LUTs a bylo třeba profilování kernelu zrušit, do host programu atan2f, vyzkoušel fixed pointy atd., připravil simulační flow v [cmodel](./code/cmodel/) jako v SoC.

---

👨🏻‍💻 [25.02.2023](/notes/20230225.md) <-- zjištění dne **DIGILENT ZYBO SE ZÁKLADNÍM ZYNQ 7000 NENÍ VHODNÉ NA AKCELEROVÁNÍ NĚJAKÝCH SLOŽITÝCH APLIKACÍ A MODELŮ, SPÍŠE NA SEZNÁMENÍ SE S PRINCIPEM, JAK TAKOVÉ PROGRAMOVÁNÍ, HLS, MODELOVÁNÍ A PŘÍSTUPY FUNGUJÍ**, vidím jako nemožné akcelerovat více než 20 % celkového modelu a bloků pro řízení, - zkusil jsem ještě udělat I-n model, v kernelu je jen RK4 a v procesoru vypočítávám amplitudu a atan2, do FPGA se to vejde jen tak tak, zkoušel jsem udělat také streamy a snažil se udělat optimalizaci čtení dat aby jednotlivé děje mohli být paralelně, ale moc to nešlo a bylo to ještě horší, pomocí různých optimalizací kódů a postupů se tam fakt vešel jen I-n model RK4, což je slabota, pokud by to bylo možné použít v průmyslu, chtělo by to pak paralelně počítat třeba regulátory, odvazbení, zrychleně SVM atd. atd. na to tento ZynQ SoC FPGA PL je relativně slabý..., ztratil jsem ale část kódu a model nefunguje tak, jak by měl => zatím to vypadá, že jsem se spletl zase v kernelu (přepsal jsem se, no tak to nechám buildit asi teďka do 5 hodin do rána... FPGA 💛), už mi z toho, jak pořád dělám na diplomce s malým posunem docela hrabe... 🤓, pardon.

---

👨🏻‍💻 [24.02.2023](/notes/20230224.md) <-- zjištěno, že opravdu ASM model nebude moci být v zybo, nevejde se tam ani generování napětí s mojí všemožnou optimalizací pomocí pragma direktiv a rozdělení funkce na prvočinitele, udělal jsem [cmodel](/code/cmodel/) I-n model a nasimuloval data z ASM přímo do I-n modelu v alpha beta, oddebugoval, poté vyexportoval data z ASM modelu do 123 (ABC) souřadnic a poté zpět nahrál do programu jako vstupní data a choval se k nim jako data acquired z čidel a zkontroloval další funkci I-n modelu a Clark transformace, výsledky / rozdíly v hodnotách jsou zobrazeny v grafech v [mathematika diff.nb](/misc/diff.nb), **POKUD** požadavek na ASM model v FPGA akcelerovaný, je třeba využít např. [Xilinx Kria Robotics – dražší ale vhodnější pro řízení pohonů](https://cz.mouser.com/ProductDetail/Xilinx/SK-KR260-G?qs=t7xnP681wgWGuCVg0VyR9A%3D%3D), asi by bylo nejvíce vhodné...

---

👨🏻‍💻 [23.02.2023](/notes/20230223.md) <-- vypadá to, jako že jsem toho moc neudělal, ale řešil jsem problém s prázdnou větví repozitáře a zabuggovaného Vitisu, když už to skoro bylo vše v pohodě, vznikl problém, že v ZynQ je příliš málo LUTS a API motoru, které jsem udělal je k ničemu, protože musím rozdělat ještě více funkce na další části a hledat další možné optimalizace, snad to půjde, je taky divný problém s tím, že se nevygenerují všechny samply napětí nebo když se změní jejich počet tak je problém s čištění pamětí, algoritmy jsou stále stejné a po debuggingu pomocí `leaks` v macOS není žádná chyba. toto je asiu random chyba optimalizace pomocí pragma, budu řešit později, zkusím udělat co nejdříve pouze I-n model a ten akcelerovat.... **pro lepší výpočty a a akceleraci aplikací a modelu doporučuji [Xilinx Kria – Vision AI - levnější ale málo PMOD](https://cz.mouser.com/ProductDetail/Xilinx/SK-KV260-G?qs=DRkmTr78QATF92lTPoHh8Q%3D%3D), [Xilinx Kria Robotics – dražší ale vhodnější pro řízení pohonů](https://cz.mouser.com/ProductDetail/Xilinx/SK-KR260-G?qs=t7xnP681wgWGuCVg0VyR9A%3D%3D)** => obě desky mají veliký počet LUTs - 256K oproti Zybo, které používáme 17,6 K a vyšší zybo s cenou jako kria poté 53,2 K..., vyřešil jsem také skoro prakticky headless workflow na buildění, pomáhám si však ještě někdy IDE...

---

🐛 [22.02.2023](/notes/20230222.md) <-- pozor PŘESUN CMODEL NA [cmodel](./code/cmodel/), pracoval jsema opravil jsem c model v pc, udělal jsem GPIO JB->JE na PL v block design, přenáším kód do SoC ohledně modelu, to je i todo na zítra, todo je udělat krom toho, že jsem zatím pro dnešek udělal backup pro kód [proxmox-backup](./code/proxmox-backup/) z SoC, tak udělat čistou branch a to mít na proxmoxu a pushovat asi odtamtud... vyzkoušet si nejdřív na novém repo... návod je popsán třeba zde [stack overflow](https://stackoverflow.com/questions/61906358/can-i-create-a-new-branch-which-does-not-have-any-files-from-the-master-branch) nebo na [stack overflow 2](https://stackoverflow.com/questions/13969050/creating-a-new-empty-branch-for-a-new-project/13969482#13969482)

---

👨🏻‍💻 [21.02.2023](/notes/20230221.md) <-- konečně jsem zas přišel na PMOD z PL, dodělal jsem (NOVÝ ODKAZ 20220222 [cmodel](./code/cmodel/)) , starý odkaz -> [cmodel](./code/test-program/cmodel/) ale je tam nějaká chyba asi v RK nebo nějak tak, je třeba to vyřešit, než to půjde do zynq, leaky memory by měli být odstraněny (to mi něco házelo jako lepší hodnoty, ale pak to ustřelilo a nebo byl segmentation fault), kontroluji to pomocí mathemtiky [mathematika](./misc/diff.nb), stačí spustit a nahraje to vyexportovaný soubor z c++, na další dny je hlavní úkol design pro všechny pmod, v budoucnu třeba přerušení, timery, SPI a I2C (lepší SPI, I2C není na kritické operace), také napsat do hostu driver na zápis do GPIO a čtení... v přechozích dnech byl odkaz na wiki attlasian xilinx, hlavní úkol je však **debugging cmodelu**

---

👨🏻‍💻 ⚙️ [20.02.2023](/notes/20230220.md) <-- **povedlo se JB PMOD z FPGA!!!** (asi po 3 dnech konečně) takže budu zkoušet další PMOD a vložím do designu asi všechny, i když bude možné v budoucnu asi nějaký použít na SPI, I2C atd. to ještě nemám vytestované, ráce na cmodelu [cmodel](./code/test-program/cmodel/), vytváření `MotorModel` API ⚙️

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
- tmux - terminal multiplexer [tmux](https://github.com/tmux/tmux/wiki), [cheatsheet](https://tmuxcheatsheet.com/)
