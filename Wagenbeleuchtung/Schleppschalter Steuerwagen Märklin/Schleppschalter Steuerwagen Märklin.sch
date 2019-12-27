EESchema Schematic File Version 4
EELAYER 26 0
EELAYER END
$Descr User 8268 5827
encoding utf-8
Sheet 1 1
Title "Märklin Schleppschalter Anschluss"
Date ""
Rev ""
Comp ""
Comment1 ""
Comment2 "sondern über eine \"intelligente\" Platine."
Comment3 "Nur notwendig, wenn der Schleppschalter nicht direkt mit den LEDs verbunden ist,"
Comment4 "Anschluss einer alten Märklin-Schleppschalter-Platine an einen Funktionsdekoder."
$EndDescr
Text GLabel 1700 1050 0    79   Output ~ 0
Schleppschalter_1
Text GLabel 1700 1250 0    79   Output ~ 0
Schleppschalter_2
Text GLabel 1700 1800 0    79   Output ~ 0
Fahrstrom_Plus
Text GLabel 1700 2450 0    79   Output ~ 0
Fahrstrom_Minus
Wire Wire Line
	1700 1050 2750 1050
Wire Wire Line
	1700 1250 2200 1250
$Comp
L Device:D D1
U 1 1 5E0215BE
P 2200 2200
F 0 "D1" V 2154 2279 79  0000 L CNN
F 1 "unkritisch" V 2267 2279 50  0000 L CNN
F 2 "" H 2200 2200 50  0001 C CNN
F 3 "~" H 2200 2200 50  0001 C CNN
	1    2200 2200
	0    1    1    0   
$EndComp
$Comp
L Device:D D2
U 1 1 5E0216B8
P 2750 2200
F 0 "D2" V 2704 2279 79  0000 L CNN
F 1 "unkritisch" V 2817 2279 50  0000 L CNN
F 2 "" H 2750 2200 50  0001 C CNN
F 3 "~" H 2750 2200 50  0001 C CNN
	1    2750 2200
	0    1    1    0   
$EndComp
Wire Wire Line
	1700 2450 2200 2450
Wire Wire Line
	2200 2450 2200 2350
Wire Wire Line
	2200 2450 2750 2450
Wire Wire Line
	2750 2450 2750 2350
Connection ~ 2200 2450
Connection ~ 2200 1250
Wire Wire Line
	2200 1250 2950 1250
Connection ~ 2750 1050
Wire Wire Line
	2750 1050 3300 1050
Wire Wire Line
	2200 1250 2200 2050
Wire Wire Line
	2750 1050 2750 2050
Wire Wire Line
	1700 1800 2950 1800
$Comp
L Device:R R1
U 1 1 5E021A48
P 2950 1550
F 0 "R1" H 3020 1618 79  0000 L CNN
F 1 "10k" H 3020 1483 79  0000 L CNN
F 2 "" V 2880 1550 50  0001 C CNN
F 3 "~" H 2950 1550 50  0001 C CNN
	1    2950 1550
	1    0    0    -1  
$EndComp
$Comp
L Device:R R2
U 1 1 5E021ACA
P 3300 1550
F 0 "R2" H 3370 1618 79  0000 L CNN
F 1 "10k" H 3370 1483 79  0000 L CNN
F 2 "" V 3230 1550 50  0001 C CNN
F 3 "~" H 3300 1550 50  0001 C CNN
	1    3300 1550
	1    0    0    -1  
$EndComp
Wire Wire Line
	2950 1700 2950 1800
Connection ~ 2950 1800
Wire Wire Line
	2950 1800 3300 1800
Wire Wire Line
	3300 1700 3300 1800
Wire Wire Line
	2950 1400 2950 1250
Wire Wire Line
	3300 1400 3300 1050
Text Notes 550  2800 0    79   ~ 0
Schleppschalter Platine
Text Notes 3750 3550 0    79   ~ 0
Decoder Platine
Text GLabel 3750 1050 2    79   Input ~ 0
AUX1
Text GLabel 3750 1250 2    79   Input ~ 0
AUX2
Text GLabel 3750 1800 2    79   Input ~ 0
Rückleiter
Wire Notes Line
	5550 650  3650 650 
Wire Notes Line
	3650 3600 5550 3600
Wire Wire Line
	3300 1050 3750 1050
Connection ~ 3300 1050
Wire Wire Line
	3750 1250 2950 1250
Connection ~ 2950 1250
Wire Wire Line
	3300 1800 3750 1800
Connection ~ 3300 1800
Text GLabel 5350 2200 0    79   Input ~ 0
Fahrstrom_Plus
Text GLabel 5350 2450 0    79   Input ~ 0
Fahrstrom_Minus
Text GLabel 5350 1050 0    79   Input ~ 0
AUX3
Wire Wire Line
	5350 2200 6650 2200
Wire Wire Line
	5350 2450 5650 2450
Text GLabel 6700 1050 2    79   Output ~ 0
Kupplung
Text GLabel 6650 2450 2    79   Input ~ 0
Massefedern
Text GLabel 6650 2200 2    79   Input ~ 0
Schleifer
Wire Wire Line
	6700 1050 5650 1050
Wire Wire Line
	5650 2450 5650 1700
Wire Wire Line
	5650 1700 5900 1700
Connection ~ 5650 2450
Wire Wire Line
	5650 2450 6650 2450
Wire Wire Line
	5650 1050 5650 1600
Wire Wire Line
	5650 1600 5900 1600
Connection ~ 5650 1050
Wire Wire Line
	5650 1050 5350 1050
Text Notes 5650 1550 0    79   ~ 0
Hier LED-Beleuchtung anschließen\nwie im anderen Schaltplan gezeigt.
Text GLabel 5350 2800 0    79   Input ~ 0
Stützelko_Plus
Text GLabel 5350 3100 0    79   Input ~ 0
Stützelko_Minus
$Comp
L Device:CP C1
U 1 1 5E025595
P 5800 2950
F 0 "C1" H 5918 3018 79  0000 L CNN
F 1 "100µF-470µF, mind. 35V" H 5918 2883 79  0000 L CNN
F 2 "" H 5838 2800 50  0001 C CNN
F 3 "~" H 5800 2950 50  0001 C CNN
	1    5800 2950
	1    0    0    -1  
$EndComp
Wire Wire Line
	5350 2800 5800 2800
Wire Wire Line
	5800 3100 5350 3100
Wire Notes Line
	5550 650  5550 3600
Wire Notes Line
	3650 650  3650 3600
Wire Notes Line
	500  650  2000 650 
Wire Notes Line
	500  2850 2000 2850
Wire Notes Line
	2000 650  2000 2850
Wire Notes Line
	500  650  500  2850
$EndSCHEMATC
