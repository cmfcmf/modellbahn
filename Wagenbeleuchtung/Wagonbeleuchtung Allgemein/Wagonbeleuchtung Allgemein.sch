EESchema Schematic File Version 4
LIBS:power
LIBS:74xx
LIBS:Wagonbeleuchtung Allgemein-cache
EELAYER 26 0
EELAYER END
$Descr User 8268 5827
encoding utf-8
Sheet 1 1
Title "Wagonbeleuchtung Allgemein"
Date ""
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L Diode_Bridge:B80C1500G Gleichrichter
U 1 1 5E01E43A
P 3150 1100
F 0 "Gleichrichter" V 3196 759 79  0000 R CNN
F 1 "unkritisch" V 3105 759 50  0000 R CNN
F 2 "Diode_THT:Diode_Bridge_Round_D9.8mm" H 3300 1225 79  0001 L CNN
F 3 "https://www.vishay.com/docs/88501/b40c1500g.pdf" H 3150 1100 50  0001 C CNN
	1    3150 1100
	0    -1   -1   0   
$EndComp
Wire Wire Line
	3450 1100 3450 1500
Text GLabel 1350 2850 0    79   Input ~ 0
Massefeder
Text Label 3650 1700 0    79   ~ 0
Gleichrichter_Minus
Text Label 3650 800  0    79   ~ 0
Gleichrichter_Plus
Wire Wire Line
	3150 1700 5200 1700
$Comp
L Device:R R2
U 1 1 5E01EA35
P 5950 800
F 0 "R2" V 5743 800 79  0000 C CNN
F 1 "1k bis 3k" V 5834 800 79  0000 C CNN
F 2 "" V 5880 800 50  0001 C CNN
F 3 "~" H 5950 800 50  0001 C CNN
	1    5950 800 
	0    1    1    0   
$EndComp
$Comp
L Device:CP C1
U 1 1 5E01EBCF
P 5200 1450
F 0 "C1" H 5318 1512 79  0000 L CNN
F 1 "220µF bis 1000µF, mind. 35V" H 5318 1405 79  0000 L CNN
F 2 "" H 5238 1300 50  0001 C CNN
F 3 "~" H 5200 1450 50  0001 C CNN
	1    5200 1450
	1    0    0    -1  
$EndComp
$Comp
L Device:D D2
U 1 1 5E01EDD5
P 5400 1000
F 0 "D2" V 5354 1079 79  0000 L CNN
F 1 "unkritisch" V 5445 1079 50  0000 L CNN
F 2 "" H 5400 1000 50  0001 C CNN
F 3 "~" H 5400 1000 50  0001 C CNN
	1    5400 1000
	0    1    1    0   
$EndComp
$Comp
L Device:R R1
U 1 1 5E01EF07
P 5000 1000
F 0 "R1" H 5070 1065 79  0000 L CNN
F 1 "330" H 5070 955 79  0000 L CNN
F 2 "" V 4930 1000 50  0001 C CNN
F 3 "~" H 5000 1000 50  0001 C CNN
	1    5000 1000
	1    0    0    -1  
$EndComp
Wire Wire Line
	5400 1250 5400 1150
Wire Wire Line
	5400 850  5400 800 
Connection ~ 5400 800 
Wire Wire Line
	5400 800  5800 800 
Wire Wire Line
	5200 1250 5400 1250
Connection ~ 5200 1250
Wire Wire Line
	3150 1400 3150 1700
Wire Wire Line
	5200 1250 5200 1300
Wire Wire Line
	5200 1600 5200 1700
Wire Wire Line
	5000 1250 5000 1150
Wire Wire Line
	5000 1250 5200 1250
Wire Wire Line
	3150 800  5000 800 
Wire Wire Line
	5000 850  5000 800 
Connection ~ 5000 800 
Wire Wire Line
	5000 800  5400 800 
Connection ~ 5200 1700
Text GLabel 6550 800  2    79   Output ~ 0
LED_Streifen_Plus
Text GLabel 6550 1700 2    79   Output ~ 0
LED_Streifen_Minus
Wire Wire Line
	6100 800  6550 800 
Wire Wire Line
	5200 1700 6550 1700
Text Notes 700  3350 0    79   ~ 0
Wenn mehrere LED-Streifen in einem Waggon angeschlossen werden sollen,\nwerden alle Pluspole der LED-Streifen gemeinsam an LED_Streifen_Plus\nangeschlossen und alle Minuspole gemeinsam an LED_Streifen_Minus.
Text GLabel 6850 2850 2    79   Input ~ 0
Massefeder
Text GLabel 6850 2600 2    79   Input ~ 0
Kupplung
Text GLabel 1350 2600 0    79   Input ~ 0
Kupplung
Wire Wire Line
	1350 2850 1950 2850
Wire Wire Line
	1350 2600 1650 2600
Wire Wire Line
	1650 2600 1650 1100
Wire Wire Line
	1650 1100 2850 1100
Connection ~ 1650 2600
Wire Wire Line
	1650 2600 6850 2600
Wire Wire Line
	1950 2850 1950 1500
Wire Wire Line
	1950 1500 3450 1500
Connection ~ 1950 2850
Wire Wire Line
	1950 2850 6850 2850
$EndSCHEMATC
