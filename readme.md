# About `soc-code`
Tato větev obsahuje zálohy aktuálních kódů, které vkládám do SoC. Toto je větev, která je primárně myšlena pro práci na serveru.

Tato větev obsahuje kódy pro `Digilent Zybo Zynq 7000` 1. verze.

Ruším kód na generování sinus signálu a jdu se pokoušet vložit vytvořený I-n model do kódu. Tato úprava readme je kvůli commitu.

V tomto commitu je to stejné asi co z noci. Podařilo se mi využít pouze cca 8,5 K LUTs, v kernelu vypočítávám pouze RK4.

Když se zadává hodnota ručně, tak nesedí moc výpočet, protože není zadána počáteční podmínka jinak než nulový tok.


## Kód na uschování git credentials
```bash
git config --global credential.helper store
```
Poté zadat jméno a heslo (Access token).
