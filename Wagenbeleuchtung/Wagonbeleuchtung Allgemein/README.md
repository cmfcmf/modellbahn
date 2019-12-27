---
layout: project
tags: [märklin, beleuchtung, wagenbeleuchtung, kicad]
permalink: beleuchtung/waggons/allgemein
latex: true
---

# Allgemeiner Aufbau einer Wagenbeleuchtung

Heutzutage bietet sich die Wagenbeleuchtung mittels preisgünstigen, selbstklebenden 12V-LED-Streifen an.
Diese können natürlich nicht direkt mit dem Digitalsignal der Schiene verbunden werden, das bei Märklin über 20V führt.
Auch wenn man einen Dekoder benutzt, ist der direkte Anschluss nur bei speziellen, auf 12V vorbereiteten Dekodern möglich.
Daher muss man in jedem Wagen eine kleine Schaltung einbauen, die das >20V Digitalsignal in 12V Gleichstrom umwandelt:

{%- include pdf_view.html name="Wagonbeleuchtung Allgemein.pdf" -%}

Nachdem das Digitalsignal über den Gleichrichter gleichgerichtet ist, folgt ein Elko zur Glättung und Überbrückung von dreckigen Schienenabschnitten.
Damit die Schaltung beim Einschalten nicht zu viel Strom verbraucht (der Dekoder könnte ggf. kaputt gehen) ist vor dem Elko auf Plusseite
noch ein Ladewiderstand eingebaut. Durch den eingebauten Ladewiderstand zieht der Kondensator maximal $$72mA$$. Damit der Kondernsator
seinen Strom ungehindert und ohne Widerstand an den LED-Streifen abgeben kann, ist parallel zum Ladewiderstand eine Diode verbaut.

{% raw %}
  $$U = {R}\cdot{I} \\ 24V = {330Ω}\cdot{I} \\ I = \frac{24V}{330Ω} = 0,072A = 72mA $$
{% endraw %}

Der Widerstand *R2* muss so gewählt werden, dass die gleichgerichteten >20V auf die notwendigen 12V (oder weniger, damit der Streifen nicht so hell leuchtet) für den LED-Streifen reduziert werden.
Je nach Streifenlänge und Stromverbrauch der LEDs muss der Widerstand angepasst werden.