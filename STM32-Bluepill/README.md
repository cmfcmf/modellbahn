---
layout: project
tags: [stm32, arduino, blue pill]
permalink: arduino/stm32-blue-pill
state: finished
---

# Anleitung zur Nutzung von generischen STM32 "Blue Pill" Boards mit Arduino

Bei ebay gibt es sehr günstige blaue Boards mit STM32F103C8T6 Prozessoren.
Diese lassen sich nach folgenden Schritten mit der Arduino-IDE programmieren.

Getestet habe ich das ganze mit Arduino 1.8.12. Die Anleitung basiert auf folgenden Quellen:

- <https://github.com/stm32duino/wiki/wiki/Getting-Started>
- <https://www.heise.de/developer/artikel/Keine-bittere-Pille-die-Blue-Pill-mit-ARM-Cortex-M3-4009580.html>

1. Arduino starten
2. Im Menü *Datei* -> *Voreinstellungen* unter *Zusätzliche Boardverwalter-URLs* folgendes eintragen:
   > https://raw.githubusercontent.com/stm32duino/BoardManagerFiles/master/STM32/package_stm_index.json
3. Im Menü *Werkzeuge* -> *Board* -> *Boardverwalter* wählen und nach "STM32 Cores" suchen und installieren.
4. Nach der Installation das richtige Board auswählen:
   - Dafür im Menü wieder auf *Werkzeuge* -> *Board* und dort "Generic STM32F1 Series" auswhälen.
   - Außerdem im Menü unter *Werkzeuge* -> *Board part number* "BluePill F103C8" auswählen.
5. Nun muss die Software STM32CubeProg heruntergeladen und installiert werden. Für den Download muss man eine E-Mail-Adresse angeben: <https://www.st.com/en/development-tools/stm32cubeprog.html#get-software5>
6. Nach der Installation das Board über einen USB->Serial-Adapter, der 3,3V-fähig ist, anschließen. Genauer: 3,3V muss mit 3,3V, GND mit GND, und TXD mit A10 sowie RXD mit A9 verbunden werden.
7. Die Power-LED auf dem Board sollte nun leuchten.
8. Nun setzt man den oberen gelben Jumper auf dem Board, BOOT0, auf 1, die rechte Position.
8. Im Menü *Werkzeuge* -> *Upload Method* *STM32CubeProgrammer (Serial)* auswählen.
9. Im Menü *Werkzeuge* -> *Port* den Anschluss wählen, an dem der USB->Serial-Adapter angeschlossen ist.
10. Im Menü *Datei* -> *Beispiele* -> *01. Basics* -> *Blink* auswählen.
11. Auf dem Board einmal die Reset-Taste drücken.
12. Programm kompilieren und herunterladen.


Sobald das funktioniert, kann man optional noch einen Bootloader installieren, um das Board in Zukunft direkt über USB und nicht mehr über den USB->Serial-Adapter programmieren zu können.


1. Unter Linux muss man noch ein Install-Skript ausführen, damit man auch ohne Admin-Rechte über USB auf das Board zugreifen kann. Dieses befindet sich in der über Arduino heruntergeladenen STM32 Bibliothek. Bei mir liegt das `install.sh`-Script in `$HOME/.arduino15/packages/STM32/tools/STM32Tools/1.4.0/tools/linux`
2. Den Bootloader lädt man hier runter: <https://github.com/rogerclarkmelbourne/STM32duino-bootloader/raw/master/bootloader_only_binaries/generic_boot20_pc13.bin>
3. Nun muss man den STM32CubeProgrammer öffnen und sich im rechten Bereich über einen Klick auf "Connect" mit dem Board verbinden.
4. Danach in der linken Seitenleiste das zweite Icon *Erasing & Programming* anklicken.
5. Unter *File path* den heruntergeladenen Bootloader auswählen.
6. *Verify programming* aktivieren, *Start address* bei `0x08000000` belassen.
7. *Start Programming* klicken
8. USB->Serial-Adapter abstöpseln, Board per USB anschließen.
8. Zurück zur Arduino-IDE wechseln
8. Im Menü *Werkzeuge* -> *Upload Method* *Maple DFU Bootloader 2.0* auswählen.
9. Im Menü *Werkzeuge* -> *Port* den Anschluss wählen, an dem das Board angeschlossen ist.
8. Beide Jumper auf dem Board auf 0 setzen (links).
10. Programm kompilieren und herunterladen. Sobald folgende Meldung kommt einmal den Reset-Knopf drücken:
    ```
    dfu-util: Invalid DFU suffix signature
    dfu-util: A valid DFU suffix will be required in a future dfu-util release!!!
    dfu-util: No DFU capable USB device available
    Trying 4 more time(s)
    ```

Aktuell gibt es leider keine Möglichkeit, automatisch, ohne Druck des Reset-Knopfs, ein neues Programm herunterzuladen. Das ist [aber in Arbeit](https://github.com/stm32duino/Arduino_Core_STM32/pull/710).