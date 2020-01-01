---
layout: project
tags: [arduino, arduino nano, decoder, signale, viessmann, dcc]
permalink: decoder/multiplex
state: wip
related_projects: [signale/multiplex]
---

# Decoder für Viessmann Multiplex-Signale

Es handelt sich um einen DCC-Decoder für 4 Multiplex-Signale von Viessmann.
Der Decoder basiert auf einem Arduino Nano.

## Funktionen

- Ansteuerung von bis zu 4 HV-Signalen, jeweils optional mit Vorsignal am Mast
  - KS-Signale wären theoretisch auch möglich, sind aber noch nicht implementiert, da ich keine zum Testen da habe
- Automatische Erkennung der Signaltypen beim Einschalten
- Alle Signalbilder schaltbar, inklusive "dunkel"
- Sanftes Überblenden zwischen den Signalbildern
- Steuerung über DCC-Weichenbefehle
- Separate Adressen pro Signal und Vorsignal

### Adresseinstellung

1. ggf. neue Signale anschließen und den Dekoder neu starten,
   indem der Reset-Knopf auf der Arduino-Platine gedruckt wird.
   Dadurch werden die neu angeschlossenen Signale erkannt.
2. Es darf nun kein Fahrbetrieb mehr stattfinden und keine (anderen) Weichen und Signale geschaltet werden!
3. Den Adressprogrammierknopf einmal drücken -> die LED auf der Platine fängt an zu blinken.
4. Das erste angeschlossene Hauptsignal fängt an, nacheinander alle Signalbilder durchzuschalten.
5. Möchte man die Adresse des Signals nicht ändern, drückt man einfach den Adressprogrammierknopf nochmal.
   Andernfalls sendet man einen Weichenbefehl mit der Digitalzentrale. Das Signal übernimmt dann die gesendete Adresse.
6. Jetzt fängt das nächste Signal an alle Signalbilder durchzuschalten. War am Mast des Hauptsignals ein
   Vorsignal angeschlossen, fängt dieses an die Signalbilder durchzuschalten.
7. Nun sendet man die Adresse des nächsten Signals wie in Schritt 5 beschrieben.
8. Wenn alle angeschlossenen Signale neue Adressen erhalten haben, hört die LED auf der Platine auf zu blinken.

## Hardware

TODO

## Software

Die Software ist für die Arduino-IDE geschrieben und muss mittels dieser in den
Arduino Nano übertragen werden. Falls die Übertragung nicht funktioniert, sollte
man nochmal sichergehen, dass im Menü unter `Werkzeuge -> Board` `Arduino Nano`
ausgewählt und unter `Werkzeuge -> Prozessor` `Atmega328P` oder `Atmega328P (Old Bootloader)`
ausgewählt ist. Ob der Bootloader des Arduinos neu oder "old" ist, lässt sich
am besten durch probieren herausfinden; wenn die Übertragung klappt, hat man
den richtigen Eintrag ausgewählt.

### Konfigurationsoptionen

Einige Optionen sind nur beim Programmieren des Arduinos und nicht im
späteren Betrieb einstellbar. Die notwendigen Einstellungen befinden sich
allesamt in der `Config.h`-Datei und müssen angepasst werden, bevor der
Arduino programmiert wird.

- **DEMO**
  - **0**: Normaler Betrieb
  - **1**: Demomodus aktiv: Alle Signale schalten nacheinander automatisch ihre Signalbegriffe durch.
  - **2**: Demomodus aktiv: Alle Signale schalten zufällige Signalbegriffe durcheinander.
- **DARK_MAIN_SIGNAL_ASPECT**
  - **0**: Hauptsignale können nicht dunkel geschaltet werden.
  - **1**: Hauptsignale können zusätzlich dunkel geschaltet werden.
- **DARK_DISTANT_SIGNAL_ASPECT**
  - **0**: Vorsignale können nicht dunkel geschaltet werden
    (**steht das Hauptsignal auf rot, wird das Vorsignal aber in jedem Fall dunkel geschaltet!**).
  - **1**: Vorsignale können zusätzlich dunkel geschaltet werden.
- **USE_SERIAL**
  - **0**: Keine Debug-Ausgaben über den seriellen Anschluss.
  - **1**: Zusätzliche Debug-Ausgaben über den seriellen Anschluss.

{%- include code_view.html name="Config.h" from_line=4 to_line=8 -%}