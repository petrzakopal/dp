# Informace o struktuře příloh

## Struktura

```
.
├── attachment-1
│   ├── build
│   │   ├── kria-1-2-rt-dp-application
│   │   └── kria-1-2-rt-dp-legacy
│   ├── legacy-app
│   │   ├── kria-1-2-rt-dp-legacy-curvel-app
│   │   ├── kria-1-2-rt-dp-legacy-curvel-app_kernels
│   │   └── kria-1-2-rt-dp-legacy-curvel-app_system_hw_link
│   ├── mappings
│   ├── python-script-for-graph-updated-app
│   └── updated-app
│       ├── kria-1-2-rt-dp-application
│       ├── kria-1-2-rt-dp-application_kernels
│       └── kria-1-2-rt-dp-application_system_hw_link
├── attachment-2
│   └── hw
│       ├── kria-1.cache
│       ├── kria-1.gen
│       ├── kria-1.hw
│       ├── kria-1.ip_user_files
│       ├── kria-1.sim
│       └── kria-1.srcs
└── attachment-3
    └── hw
        └── kria-1.runs
```

### build

- obsahuje soubory, spustitelné v Xilinx Kria KR260 na platformě, představené v práci

### legacy-app

- obsahuje složky projektu Vitis IDE

  - kria-1-2-rt-dp-legacy-curvel-app
  - kria-1-2-rt-dp-legacy-curvel-app_kernels
  - kria-1-2-rt-dp-legacy-curvel-app_system_hw_link

- tyto složky obsahují soubory, ze kterých je možné odvodit nastavení Application project souboru ve Vitis IDE a zajistit tak funkční build aplikace

### mappings

- obsahuje excel soubor s popisem mapování PMOD a Raspberry Pi HAT konektorů pro Xilinx Kria KR260

### python-script-for-graph-updated-app

- python skript pro vykreslení grafu závislosti mechanické otáčivé rychlosti rotoru motoru a velikosti magnetického toku rotoru na čase

### updated-app

- obsahuje složky projektu Vitis IDE
  - kria-1-2-rt-dp-application
  - kria-1-2-rt-dp-application_kernels
  - kria-1-2-rt-dp-application_system_hw_link

### hw

- složka obsahující hw design platformy z Vivado
