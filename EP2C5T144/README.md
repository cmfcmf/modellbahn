---
layout: project
tags: [fpga]
permalink: boards/EP2C5T144
---

# EP2C5T144 Development Board Tutorial

![Photo of the Board](board.jpg)

Das EP2C5T144 Development Board ist für unter 20€ unter anderem bei eBay und Amazon erhältlich. Leider ist der Einstieg in die Programmierung dieses Boards mit VHDL nicht ganz einfach. Diese Seite listet Ressourcen auf, die mir beim Einstieg geholfen haben.

- [land-boards.com](http://land-boards.com/blwiki/index.php?title=Cyclone_II_EP2C5_Mini_Dev_Board): Wiki mit ein paar allgemeinen Daten.
- [youtube.com](https://www.youtube.com/watch?v=le6Jo5DpLao): Einstiegsvideo in die Programmierung. Achtung: Enthält Fehler. Es ist hilfreich die Kommentare zu lesen!
- 
  [openimpulse.com](https://www.openimpulse.com/blog/products-page/product-category/ep2c5t144-altera-cyclone-ii-fpga-development-board/),
  [leonheller.com](http://www.leonheller.com/FPGA/EP2C5T144mini.pdf): Download des Schaltplans

<div class="toc" markdown="1">
#### Inhaltsverzeichnis
{:.no_toc}

1. TOC Placeholder
{:toc}
</div>
## Schaltplan und Pins

Eigentlich ist der Schaltplan recht selbsterklärend. Fast alle Pins sind auf die vier langen Pinheader rausgeführt. Allerdings gibt es folgende Besonderheiten:

### Besondere Pins

- `PIN144` ist über den Taster mit `GND` verbunden (falls der Taster benutzt werden soll, muss ein Pull-Up-Widerstand einprogrammiert werden).
- `PIN17` ist mit dem `50MHz`-Oszillator verbunden.
- `PIN3`, `PIN7`, `PIN9` sind jeweils über eine LED mit `VCC3.3` verbunden.

### Nicht nutzbare Pins

- `PIN26` ist über `R1`  (0Ω) fest mit `VCC1.2` verbunden.
- `PIN27` ist über `R2`  (0Ω) fest mit `GND`    verbunden.
- `PIN80` ist über `R9`  (0Ω) fest mit `GND`    verbunden.
- `PIN81` ist über `R10` (0Ω) fest mit `VCC1.2` verbunden.
- `PIN73` ist über `R13` (10kΩ) mit `VCC3.3` und über `C14` (10µF) mit `GND` verbunden.

Diese Pins können daher nicht als I/O Pins benutzt werden. Man kann die 0Ω-Widerstände mit etwas Geschick aber entfernen. Laut [land-boards.com](http://land-boards.com/blwiki/index.php?title=Cyclone_II_EP2C5_Mini_Dev_Board) sind diese Pins so komisch angeschlossen, um auch mit einem anderen FPGA, dem `EPC28`, kompatibel zu sein.

## Programmierung

Die Programmierung erfolgt mit der kostenlosen Quartus II Software. Allerdings unterstützten aktuelle Versionen den genutzten FPGA nicht mehr. **Die letzte Version, die den FPGA unterstützut, ist *Quartus 13.0SP1***. Die Software kann hier heruntergeladen werden: <https://fpgasoftware.intel.com/13.0sp1/?edition=subscription&platform=windows>

### Setup 

1. Klick auf "Create a New Project"
   ![]({{ "/EP2C5T144/screenshots/1-project.png" | relative_url }})
2. Klick auf "Next"
   ![]({{ "/EP2C5T144/screenshots/2-introduction.png" | relative_url }})
3. Namen und Speicherort für das Projekt vergeben, danach Klick auf "Next"
   ![]({{ "/EP2C5T144/screenshots/3-name.png" | relative_url }})
4. Klick auf "Next"
   ![]({{ "/EP2C5T144/screenshots/4-files.png" | relative_url }})
5. FPGA auswählen, danach Klick auf "Next"
   ![]({{ "/EP2C5T144/screenshots/5-device.png" | relative_url }})
6. Klick auf "Next"
   ![]({{ "/EP2C5T144/screenshots/6-tool-settings.png" | relative_url }})
7. Klick auf "Next"
   ![]({{ "/EP2C5T144/screenshots/7-summary.png" | relative_url }})
8. Das erstellte Projekt wird geöffnet.
   ![]({{ "/EP2C5T144/screenshots/8-done.png" | relative_url }})

Als nächstes sollte noch eine wichtige Option geändert werden. 
Standardmäßig werden alle nicht benutzten Pins nämlich mit `GND` verbunden.
Das führt aber zu einem Kurzschluss, da wie oben beschrieben einige Pins ja mit `VCC` verbunden sind.
Daher sollte man in den "Device"-Einstellungen einstellen, dass alle unbenutzten Pins als "input tri-stated" programmiert werden:

1. Rechtsklick auf das Projekt, dann auf "Device..."
   ![]({{ "/EP2C5T144/screenshots/9-device.png" | relative_url }})
2. Klick auf "Device and Pin Options..."
   ![]({{ "/EP2C5T144/screenshots/10-device-options.png" | relative_url }})
3. Klick auf "Unused Pins" und dann Auswahl von "As input tri-stated"
    ![]({{ "/EP2C5T144/screenshots/11-unused-pins.png" | relative_url }})

Desweiteren kann man in den Einstellungen noch den VHDL-Standard auf "VHDL 2008" stellen, um modernere VHDL-Features nutzen zu können:

1. Rechtsklick auf das Projekt, dann auf "Properties..."
   ![]({{ "/EP2C5T144/screenshots/12-settings.png" | relative_url }})
2. Auswahl von "VHDL 2008"
   ![]({{ "/EP2C5T144/screenshots/13-vhdl-version.png" | relative_url }})

### Top-Level Entity Erstellung und Pin Planner

1. Wir erstellen nun eine Top-Level Entity, die den gleichen Namen wie das Projekt hat:
   ![]({{ "/EP2C5T144/screenshots/14-new-file.png" | relative_url }})
2. Zum Testen lassen wir die LEDs mittels eines [Möbiuszählers](https://www.wikiwand.com/de/Ringz%C3%A4hler#/Johnson-Z%C3%A4hler) blinken. 
   Während der Knopf gedrückt wird, invertieren wir die Signale.
   ```vhdl
   library ieee;
   use ieee.std_logic_1164.all;
   use ieee.numeric_std.all;
 
   entity VHDLTest is
   port(
     clk		: in 	std_logic;
     btn_n		: in	std_logic; -- Per Konvention fügen wir an alle Signale mit negativer Logik ein _n suffix an.
     leds_n	: out	std_logic_vector(3 downto 1)
   );
   end VHDLTest;
 
   architecture arch of VHDLTest is
     signal led_state : std_logic_vector(3 downto 1) := "000";
   begin
     leds_n <= led_state when btn_n = '1' else not led_state;
 
     process(clk, led_state)
       variable counter : integer range 1 to 50000000 	:= 1;
     begin
       if rising_edge(clk) then
         counter := counter + 1;
         if counter = 50000000 then
           led_state <= led_state(2 downto 1) & not led_state(3);
         end if;
       end if;
     end process;
   end arch;
   ```
3. Wir speichern die neue Datei mit dem gleichen Namen wie das Projekt.
   ![]({{ "/EP2C5T144/screenshots/16-new-file-save.png" | relative_url }})
4. Nun können wir das Design testweise kompilieren:  
   ![]({{ "/EP2C5T144/screenshots/17-compile.png" | relative_url }})
5. Das sollte klappen und mit folgender Meldung bestätigt werden:
   ![]({{ "/EP2C5T144/screenshots/18-compile-success.png" | relative_url }})
6. Bevor wir das Design auf das Board übertragen können, müssen wir noch alle Ein- und Ausgangssignale der Top-Level Entity den entsprechenden Pins zu weisen. Dazu öffnen wir den Pin Planner:
   ![]({{ "/EP2C5T144/screenshots/19-pin-planner.png" | relative_url }})
7. Für uns sind nur die beiden Spalten "Location" und "Weak Pull-Up Resistor" relevant. Hier werden den Signalen die physischen Pins zugeordnet. Wir wählen die Pins natürlich so, wie sie auf unserem Board belegt sind. Danach schließen wir den Pin Planner wieder:
   ![]({{ "/EP2C5T144/screenshots/20-pin-planner.png" | relative_url }})
8. Nun können wir nochmal kompilieren und sollten eine Warnung weniger erhalten. Die verbleibenden Warnungen können wir getrost ignorieren:  
   ![]({{ "/EP2C5T144/screenshots/21-success.png" | relative_url }})
   ![]({{ "/EP2C5T144/screenshots/22-warnings.png" | relative_url }})

### Programmieren über JTAG

Die Programmierung über JTAG ist nicht persistent, d.h. nach dem nächsten aus- und anschalten des Board läuft wieder das im EEPROM auf dem Board gespeicherte Programm. 

1. Um das Design auf das Board zu übertragen, schließen wir den USB-Blaster an den Computer an und verbinden ihn mit dem rechten "JTAG" Sockel auf dem Board. Danach öffnen wir den Programmer.
   Dort stellen wir sicher, dass "JTAG" ausgewählt ist und klicken danach auf "Hardware Setup":
   ![]({{ "/EP2C5T144/screenshots/23-programmer.png" | relative_url }})
2. Hier sollte der USB-Blaster auftauchen, den wir unter "Currently selected hardware" auswählen:
   ![]({{ "/EP2C5T144/screenshots/25-hardware-selection.png" | relative_url }})
3. Nun können wir das Design per Klick auf "Start" auf das Board übertragen:
   ![]({{ "/EP2C5T144/screenshots/26-program.png" | relative_url }})

### Programmieren des EEPROM

Um ein Design dauerhaft auf dem FPGA zu speichern, müssen wir den EEPROM auf dem Board programmieren.

1. Zuerst stellen wir den "Mode" von "JTAG" auf "Active Serial Programming". Die ggf. erscheinende Warnung können wir ignorieren:
   ![]({{ "/EP2C5T144/screenshots/27-program-eeprom.png" | relative_url }})
2. Nun müssen wir über "Add File" die "VHDLTest.**pof**"-Datei auswählen:
   ![]({{ "/EP2C5T144/screenshots/28-add-file.png" | relative_url }})
3. Schließlich wählen wir sowohol "Program/Configure" als auch "Verify" aus und klicken abschließend auf "Start":
   ![]({{ "/EP2C5T144/screenshots/29-program.png" | relative_url }})

**Achtung:** Das Design wird erst gestartet, wenn man den USB-Blaster wieder in den JTAG-Port steckt oder ausstöpselt.
