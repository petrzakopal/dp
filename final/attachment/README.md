# Informace o struktuře archivu
- root (tento archiv)
    - build
    - legacy-app
    - python-script-for-graph-updated-app
    - updated-app
    - README.md (tento soubor)
    - hw

## build
- obsahuje soubory, spustitelné v Xilinx Kria KR260 na platformě, představené v práci

## legacy-app
- obsahuje složky projektu Vitis IDE
    - kria-1-2-rt-dp-legacy-curvel-app
    - kria-1-2-rt-dp-legacy-curvel-app_kernels
    - kria-1-2-rt-dp-legacy-curvel-app_system_hw_link

- tyto složky obsahují soubory, ze kterých je možné odvodit nastavení Application project souboru ve Vitis IDE a zajistit tak funkční build aplikace

## python-script-for-graph-updated-app
- python skript pro vykreslení grafu závislosti mechanické otáčivé rychlosti rotoru motoru a velikosti magnetického toku rotoru na čase

## updated-app
- obsahuje složky projektu Vitis IDE
    - kria-1-2-rt-dp-application
    - kria-1-2-rt-dp-application_kernels
    - kria-1-2-rt-dp-application_system_hw_link

## hw
- složka obsahující hw design platformy z Vivado
