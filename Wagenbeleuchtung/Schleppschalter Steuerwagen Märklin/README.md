---
layout: project
tags: [märklin, beleuchtung, wagenbeleuchtung, steuerwagen, kicad]
permalink: beleuchtung/waggons/märklin-schleppschalter
---

# Digitalisierung des Schleppschalters in Märklin Steuerwagen

Beim nachträglichen Einbau einer Beleuchtung in ältere Märklin Steuerwagen stellt sich schnell die Frage, wie man den Schleppschalter und die Schleppschalterplatine digitalisiert.
Grundsätzlich gibt es drei Optionen:
1. **Schleppschalter und -Platine so lassen wie sie sind.** Das Spitzensignal wird dann nicht über den neu eingebauten Dekoder gesteuert, sondern ist weiterhin fahrtrichtungsabhängig über den Schleppschalter realisiert. Der Nachteil dabei ist, dass man das Licht nicht ausschalten kann und die Schleppschalter manchmal auch unzuverlässig sind.
2. **Schleppschalter und -Platine ausbauen und die LEDs direkt an den neuen Dekoder anschließen.** Gegen diese Variante spricht, dass man dann mit den LEDs herumfriemeln muss und ggf. Vorwiderstände neu einbauen oder anpassen muss.
3. **Schleppschalter von der Schleppschalterplatine trennen (der Schleppschalter ist dann ohne Funktion) und den Dekoder an die Schleppschalterplatine anschließen.** Mit dieser Variante lässt sich das Spitzensignal korrekt mittels Dekoder an-, aus- und umschalten. Daher habe ich mich für diese Variante entschieden.

Der Anschluss des neuen Dekoders an die Schleppschalterplatine ist nicht trivial, wenn man das Spitzensignal nicht nur umschalten, sondern auch ausschalten können will.
Nach ein paar Experimenten habe ich dafür folgende minimale Schaltung ermittelt, die aus zwei Dioden und zwei Widerständen besteht:

{%- include pdf_view.html name="Schleppschalter Steuerwagen Märklin.pdf" -%}
