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
Angeschlossene Signale werden automatisiert erkannt (allerdings derzeit nur
HV-Signale, KS-Signale wären theoretisch auch möglich, habe ich aber nicht
zum testen da).

Wenn man das Projekt in der Arduino IDE öffnet, muss man in der "Config.h"-Datei
noch den Demo-Modus abschalten, indem man die Zeile auf `#define DEMO 0` ändert.
Im Demo-Modus schalten die einzelnen Signalbegriffe auch ohne DCC-Signal im
Wechsel durch.

{%- include code_view.html name="Config.h" from_line=4 to_line=4 -%}