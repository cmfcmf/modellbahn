EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr User 5827 8268
encoding utf-8
Sheet 1 1
Title "MobaLedLib Bipolar Signal"
Date "2021-03-27"
Rev ""
Comp "Christian Flach @cmfcmf"
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L Driver_LED:WS2811 U3
U 1 1 6060DE64
P 3150 3200
F 0 "U3" H 2900 3450 50  0000 C CNN
F 1 "WS2811" H 3300 3450 50  0000 C CNN
F 2 "Package_DIP:DIP-8_W7.62mm_Socket_LongPads" H 2850 3350 50  0001 C CNN
F 3 "https://cdn-shop.adafruit.com/datasheets/WS2811.pdf" H 2950 3450 50  0001 C CNN
	1    3150 3200
	1    0    0    -1  
$EndComp
$Comp
L MobaLedLib_BiPolar-rescue:PFS173-S16-Padauk U4
U 1 1 6060DE6A
P 3150 4450
F 0 "U4" H 3150 5065 50  0000 C CNN
F 1 "PFS173-S16" H 3150 4974 50  0000 C CNN
F 2 "Display_7Segment:DA04-11SEKWA" H 3150 4450 50  0001 C CNN
F 3 "" H 3150 4450 50  0001 C CNN
	1    3150 4450
	1    0    0    -1  
$EndComp
$Comp
L power:GNDD #PWR012
U 1 1 6060DE71
P 3800 5700
F 0 "#PWR012" H 3800 5450 50  0001 C CNN
F 1 "GNDD" H 3804 5545 50  0000 C CNN
F 2 "" H 3800 5700 50  0001 C CNN
F 3 "" H 3800 5700 50  0001 C CNN
	1    3800 5700
	1    0    0    -1  
$EndComp
Wire Wire Line
	3800 5700 3800 5200
Wire Wire Line
	3800 4500 3550 4500
$Comp
L Connector_Generic:Conn_01x02 J6
U 1 1 6060DE7E
P 2150 4200
F 0 "J6" H 2068 3875 50  0000 C CNN
F 1 "Conn_01x02" H 2068 3966 50  0000 C CNN
F 2 "TerminalBlock:TerminalBlock_bornier-2_P5.08mm" H 2150 4200 50  0001 C CNN
F 3 "~" H 2150 4200 50  0001 C CNN
	1    2150 4200
	-1   0    0    1   
$EndComp
$Comp
L Device:R R7
U 1 1 6060DE8B
P 2750 2800
F 0 "R7" V 2543 2800 50  0000 C CNN
F 1 "150" V 2634 2800 50  0000 C CNN
F 2 "Resistor_THT:R_Axial_DIN0204_L3.6mm_D1.6mm_P7.62mm_Horizontal" V 2680 2800 50  0001 C CNN
F 3 "~" H 2750 2800 50  0001 C CNN
	1    2750 2800
	0    1    1    0   
$EndComp
Wire Wire Line
	3150 2900 3150 2800
Wire Wire Line
	3150 2800 2900 2800
$Comp
L Device:C C4
U 1 1 6060DE93
P 3500 2800
F 0 "C4" V 3248 2800 50  0000 C CNN
F 1 "100n" V 3339 2800 50  0000 C CNN
F 2 "Capacitor_THT:C_Disc_D4.7mm_W2.5mm_P5.00mm" H 3538 2650 50  0001 C CNN
F 3 "~" H 3500 2800 50  0001 C CNN
	1    3500 2800
	0    1    1    0   
$EndComp
Wire Wire Line
	3650 2800 3800 2800
Wire Wire Line
	3350 2800 3150 2800
Connection ~ 3150 2800
$Comp
L power:+5V #PWR06
U 1 1 6060DE9E
P 1600 2800
F 0 "#PWR06" H 1600 2650 50  0001 C CNN
F 1 "+5V" V 1615 2928 50  0000 L CNN
F 2 "" H 1600 2800 50  0001 C CNN
F 3 "" H 1600 2800 50  0001 C CNN
	1    1600 2800
	0    -1   -1   0   
$EndComp
Wire Wire Line
	1600 2800 1800 2800
Connection ~ 3800 5200
Wire Wire Line
	3800 5200 3800 4500
Wire Wire Line
	2650 4500 2750 4500
$Comp
L Device:LED D2
U 1 1 6060DEA8
P 4100 4300
F 0 "D2" V 4139 4182 50  0000 R CNN
F 1 "LED" V 4048 4182 50  0000 R CNN
F 2 "LED_THT:LED_D3.0mm" H 4100 4300 50  0001 C CNN
F 3 "~" H 4100 4300 50  0001 C CNN
	1    4100 4300
	0    -1   -1   0   
$EndComp
Wire Wire Line
	3550 4700 4100 4700
Wire Wire Line
	4100 4700 4100 4450
$Comp
L Device:R R8
U 1 1 6060DEB0
P 4100 3750
F 0 "R8" H 4030 3704 50  0000 R CNN
F 1 "1k" H 4030 3795 50  0000 R CNN
F 2 "Resistor_THT:R_Axial_DIN0204_L3.6mm_D1.6mm_P2.54mm_Vertical" V 4030 3750 50  0001 C CNN
F 3 "~" H 4100 3750 50  0001 C CNN
	1    4100 3750
	-1   0    0    1   
$EndComp
$Comp
L power:+5V #PWR013
U 1 1 6060DEB7
P 4100 3500
F 0 "#PWR013" H 4100 3350 50  0001 C CNN
F 1 "+5V" H 4115 3673 50  0000 C CNN
F 2 "" H 4100 3500 50  0001 C CNN
F 3 "" H 4100 3500 50  0001 C CNN
	1    4100 3500
	1    0    0    -1  
$EndComp
Wire Wire Line
	4100 3500 4100 3600
$Comp
L Connector_Generic:Conn_01x02 J7
U 1 1 6060DEBE
P 4200 2300
F 0 "J7" H 4280 2292 50  0000 L CNN
F 1 "Conn_01x02" H 4280 2201 50  0000 L CNN
F 2 "TerminalBlock:TerminalBlock_bornier-2_P5.08mm" H 4200 2300 50  0001 C CNN
F 3 "~" H 4200 2300 50  0001 C CNN
	1    4200 2300
	1    0    0    -1  
$EndComp
$Comp
L Device:C C3
U 1 1 6060DEDC
P 2650 5050
F 0 "C3" H 2765 5096 50  0000 L CNN
F 1 "100n" H 2765 5005 50  0000 L CNN
F 2 "Capacitor_THT:C_Disc_D4.7mm_W2.5mm_P5.00mm" H 2688 4900 50  0001 C CNN
F 3 "~" H 2650 5050 50  0001 C CNN
	1    2650 5050
	1    0    0    -1  
$EndComp
Wire Wire Line
	2650 4900 2650 4500
Wire Wire Line
	2650 5200 3800 5200
$Comp
L Connector_Generic:Conn_02x03_Odd_Even J5
U 1 1 6060DEED
P 2000 3200
F 0 "J5" H 2050 3517 50  0000 C CNN
F 1 "Conn_02x03_Odd_Even" H 2050 3426 50  0000 C CNN
F 2 "Connector_IDC:IDC-Header_2x03_P2.54mm_Vertical" H 2000 3200 50  0001 C CNN
F 3 "~" H 2000 3200 50  0001 C CNN
	1    2000 3200
	1    0    0    -1  
$EndComp
Wire Wire Line
	2300 3100 2750 3100
Wire Wire Line
	2750 3200 2300 3200
Wire Wire Line
	1800 3200 1800 3300
Wire Wire Line
	1800 3100 1800 2800
Connection ~ 1800 2800
Wire Wire Line
	1800 2800 2600 2800
$Comp
L Device:R R6
U 1 1 6060DEFE
P 2550 4200
F 0 "R6" V 2343 4200 50  0000 C CNN
F 1 "680" V 2434 4200 50  0000 C CNN
F 2 "Resistor_THT:R_Axial_DIN0204_L3.6mm_D1.6mm_P2.54mm_Vertical" V 2480 4200 50  0001 C CNN
F 3 "~" H 2550 4200 50  0001 C CNN
	1    2550 4200
	0    1    -1   0   
$EndComp
$Comp
L power:+5V #PWR010
U 1 1 6060DF0C
P 3800 2400
F 0 "#PWR010" H 3800 2250 50  0001 C CNN
F 1 "+5V" V 3815 2528 50  0000 L CNN
F 2 "" H 3800 2400 50  0001 C CNN
F 3 "" H 3800 2400 50  0001 C CNN
	1    3800 2400
	0    -1   -1   0   
$EndComp
$Comp
L power:GNDD #PWR07
U 1 1 6063B179
P 1800 3400
F 0 "#PWR07" H 1800 3150 50  0001 C CNN
F 1 "GNDD" H 1804 3245 50  0000 C CNN
F 2 "" H 1800 3400 50  0001 C CNN
F 3 "" H 1800 3400 50  0001 C CNN
	1    1800 3400
	1    0    0    -1  
$EndComp
Wire Wire Line
	1800 3400 1800 3300
Connection ~ 1800 3300
$Comp
L power:+5V #PWR08
U 1 1 6065AB72
P 2550 4500
F 0 "#PWR08" H 2550 4350 50  0001 C CNN
F 1 "+5V" V 2565 4628 50  0000 L CNN
F 2 "" H 2550 4500 50  0001 C CNN
F 3 "" H 2550 4500 50  0001 C CNN
	1    2550 4500
	0    -1   -1   0   
$EndComp
Wire Wire Line
	2550 4500 2650 4500
Connection ~ 2650 4500
$Comp
L power:GNDD #PWR09
U 1 1 606642D3
P 3150 3500
F 0 "#PWR09" H 3150 3250 50  0001 C CNN
F 1 "GNDD" H 3154 3345 50  0000 C CNN
F 2 "" H 3150 3500 50  0001 C CNN
F 3 "" H 3150 3500 50  0001 C CNN
	1    3150 3500
	1    0    0    -1  
$EndComp
Wire Wire Line
	3800 2800 3800 4500
Connection ~ 3800 4500
Wire Wire Line
	4100 3900 4100 4150
Wire Wire Line
	3550 3200 3650 3200
Wire Wire Line
	3650 3200 3650 4300
Wire Wire Line
	3650 4300 3550 4300
Wire Wire Line
	3550 4200 3700 4200
Wire Wire Line
	3700 4200 3700 3100
Wire Wire Line
	3700 3100 3550 3100
Wire Wire Line
	3550 3300 3900 3300
Wire Wire Line
	2750 4200 2700 4200
Wire Wire Line
	2750 4100 2350 4100
Wire Wire Line
	2350 4200 2400 4200
Wire Wire Line
	3900 2300 3900 3300
Wire Wire Line
	3900 2300 4000 2300
Wire Wire Line
	3800 2400 4000 2400
$Comp
L Mechanical:MountingHole H1
U 1 1 60630A01
P 4300 4950
F 0 "H1" H 4400 4996 50  0000 L CNN
F 1 "MountingHole" H 4400 4905 50  0000 L CNN
F 2 "MountingHole:MountingHole_3.2mm_M3_ISO7380" H 4300 4950 50  0001 C CNN
F 3 "~" H 4300 4950 50  0001 C CNN
	1    4300 4950
	1    0    0    -1  
$EndComp
$Comp
L Mechanical:MountingHole H2
U 1 1 60631085
P 4300 5150
F 0 "H2" H 4400 5196 50  0000 L CNN
F 1 "MountingHole" H 4400 5105 50  0000 L CNN
F 2 "MountingHole:MountingHole_3.2mm_M3_ISO7380" H 4300 5150 50  0001 C CNN
F 3 "~" H 4300 5150 50  0001 C CNN
	1    4300 5150
	1    0    0    -1  
$EndComp
$EndSCHEMATC
