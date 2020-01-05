---
layout: project
tags: [arduino, arduino uno, dccpp, dcc]
permalink: informationen/dccplusplus
related_projects: []
---

# DCC++ Basisstation als Zentrale zum Testen von Selbstbau-DCC-Zubehördecodern verwenden

DCC++ ist eine Modellbahnsteuerungszentrale basierend auf einem Arduino Nano.
Wie der Name schon deutlich macht, unterstützt sie nur DCC-Decoder. Alle
Informationen gibt es (auf Englisch) im [GitHub Repository](https://github.com/DccPlusPlus/BaseStation)
und [GitHub Wiki von DCC++](https://github.com/DccPlusPlus/BaseStation/wiki/What-is-DCC--Plus-Plus).
Normalerweise benötigt man zur Benutzung neben einem Arduino Uno auch ein
Motorshield, da der Arduino direkt nicht genug Strom und Spannung zum Betrieb
der Lokomotiven zur Verfügung stellen kann.

Hier möchte ich mich allerdings darauf fokussieren, wie man DCC++ ohne
Motorshield benutzen kann, um selbstgebaute DCC-Zubehördecoder zu testen.
Dafür muss der DCC-Eingang des Zubehördecoders auch mit nur 5V statt den
üblichen 20V funktionieren. Das ist z.B. der Fall, wenn man den DCC-Eingang
eines auf einem Arduino basierenden Zubehördecoders direkt mit dem Arduino
von DCC++ verbindet.

Folgende Pins müssen verbunden werden:
1. Pin `D10` ist das DCC-Signal.
2. Pin `A0` sollte direkt mit Masse verbunden werden, da über diesen eigentlich
   der vom Motorshield verbrauchte Strom gemessen wird. 3,3V ensprechen dort
   2A. Die Verbindung mit Masse sorgt dafür, dass DCC++ durchgängig 0A misst
   und daher das DCC-Signal nicht wegen zu hohem Stromverbrauch abschaltet.
3. Pin `D3` kann optional über LED und Vorwiderstand nach Masse verbunden
   werden und zeigt an, ob das DCC-Signal aktiv ist.
4. GND

Um den angeschlossenen Decoder zu testen, muss die DCC++-Zentrale natürlich
auch Schaltbefehle senden. Das geschieht, indem man über den seriellen
Monitor Befehle an DCC++ schickt. Hier die dafür notwendigen Befehle; eine
vollständige Auflistung findet man [im DCC++ Wiki](https://github.com/DccPlusPlus/BaseStation/wiki/Commands-for-DCCpp-BaseStation).

1. `<1>` schaltet den Strom und damit das DCC-Signal an. Eine evtl. an `D3`
   angeschlossene LED sollte nun leuchten. `<0>` schaltet den Strom wieder
   aus.
2. `<a ADRESSE WEICHE RICHTUNG>` sendet einen Weichenschaltbefehl. Der Befehl wird
   wie bei Zentralen üblich ca. 4 mal nacheinander gesendet.
   - `ADRESSE` (0-511): Die Decoderadresse. 4 Weichen gehören zu einer Decoderadresse.
   - `WEICHE` (0-3): Die Weiche innerhalb des Decoders, die geschaltet werden soll.
   - `RICHTUNG` (0-1): 0 entspricht "rot" oder "gerade", 1 entspricht "grün" oder "abzweigend".