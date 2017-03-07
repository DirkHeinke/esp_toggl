EESchema Schematic File Version 2
LIBS:power
LIBS:device
LIBS:transistors
LIBS:conn
LIBS:linear
LIBS:regul
LIBS:74xx
LIBS:cmos4000
LIBS:adc-dac
LIBS:memory
LIBS:xilinx
LIBS:microcontrollers
LIBS:dsp
LIBS:microchip
LIBS:analog_switches
LIBS:motorola
LIBS:texas
LIBS:intel
LIBS:audio
LIBS:interface
LIBS:digital-audio
LIBS:philips
LIBS:display
LIBS:cypress
LIBS:siliconi
LIBS:opto
LIBS:atmel
LIBS:contrib
LIBS:valves
LIBS:Worldsemi
LIBS:togglbutton-cache
EELAYER 25 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title ""
Date ""
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L CONN_02X04 P4
U 1 1 58BDD496
P 4200 3250
F 0 "P4" H 4200 3500 50  0000 C CNN
F 1 "CONN_02X04" H 4200 3000 50  0000 C CNN
F 2 "Socket_Strips:Socket_Strip_Straight_2x04_Pitch2.54mm" H 4200 3550 50  0001 C CNN
F 3 "" H 4200 2050 50  0000 C CNN
	1    4200 3250
	1    0    0    -1  
$EndComp
$Comp
L USB_B P3
U 1 1 58BDD53A
P 3100 2250
F 0 "P3" H 3300 2050 50  0000 C CNN
F 1 "USB_B" H 3050 2450 50  0000 C CNN
F 2 "Connect:USB_B" V 3050 2150 50  0001 C CNN
F 3 "" V 3050 2150 50  0000 C CNN
	1    3100 2250
	0    -1   1    0   
$EndComp
$Comp
L NCP1117ST33T3G U1
U 1 1 58BDD95C
P 4200 2050
F 0 "U1" H 4200 2275 50  0000 C CNN
F 1 "NCP1117ST33T3G" H 4200 2200 50  0000 C CNN
F 2 "TO_SOT_Packages_SMD:SOT-223" H 4250 1800 50  0001 L CNN
F 3 "" H 4200 2050 50  0000 C CNN
	1    4200 2050
	1    0    0    -1  
$EndComp
$Comp
L R R2
U 1 1 58BDDB38
P 5000 3300
F 0 "R2" V 5050 3450 50  0000 C CNN
F 1 "10k" V 5000 3300 50  0000 C CNN
F 2 "Resistors_ThroughHole:R_Axial_DIN0207_L6.3mm_D2.5mm_P2.54mm_Vertical" V 5150 4800 50  0001 C CNN
F 3 "" H 5000 3300 50  0000 C CNN
	1    5000 3300
	0    1    1    0   
$EndComp
$Comp
L R R1
U 1 1 58BDDC0D
P 5000 3200
F 0 "R1" V 4950 3350 50  0000 C CNN
F 1 "10k" V 5000 3200 50  0000 C CNN
F 2 "Resistors_ThroughHole:R_Axial_DIN0207_L6.3mm_D2.5mm_P2.54mm_Vertical" V 5150 4700 50  0001 C CNN
F 3 "" H 5000 3200 50  0000 C CNN
	1    5000 3200
	0    1    1    0   
$EndComp
Wire Wire Line
	4850 3300 4450 3300
Wire Wire Line
	4450 3200 4850 3200
Wire Wire Line
	5350 3200 5150 3200
Connection ~ 5350 3200
Wire Wire Line
	5350 3300 5150 3300
Connection ~ 5350 3300
Wire Wire Line
	4500 2050 5350 2050
Wire Wire Line
	5350 2050 5350 3400
Wire Wire Line
	3400 2050 3900 2050
Wire Wire Line
	3400 2350 4200 2350
$Comp
L GND #PWR01
U 1 1 58BDE4A3
P 2900 3400
F 0 "#PWR01" H 2900 3150 50  0001 C CNN
F 1 "GND" H 2900 3250 50  0000 C CNN
F 2 "" H 2900 3400 50  0000 C CNN
F 3 "" H 2900 3400 50  0000 C CNN
	1    2900 3400
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR02
U 1 1 58BDE4E8
P 3600 2700
F 0 "#PWR02" H 3600 2450 50  0001 C CNN
F 1 "GND" H 3600 2550 50  0000 C CNN
F 2 "" H 3600 2700 50  0000 C CNN
F 3 "" H 3600 2700 50  0000 C CNN
	1    3600 2700
	1    0    0    -1  
$EndComp
Wire Wire Line
	3600 2350 3600 2700
Connection ~ 3600 2350
$Comp
L +5V #PWR03
U 1 1 58BDE532
P 3650 1950
F 0 "#PWR03" H 3650 1800 50  0001 C CNN
F 1 "+5V" H 3650 2090 50  0000 C CNN
F 2 "" H 3650 1950 50  0000 C CNN
F 3 "" H 3650 1950 50  0000 C CNN
	1    3650 1950
	1    0    0    -1  
$EndComp
Wire Wire Line
	3650 2050 3650 1950
Connection ~ 3650 2050
$Comp
L GND #PWR04
U 1 1 58BDE7D8
P 3850 3100
F 0 "#PWR04" H 3850 2850 50  0001 C CNN
F 1 "GND" H 3850 2950 50  0000 C CNN
F 2 "" H 3850 3100 50  0000 C CNN
F 3 "" H 3850 3100 50  0000 C CNN
	1    3850 3100
	0    1    1    0   
$EndComp
$Comp
L GND #PWR05
U 1 1 58BDEB55
P 3250 4000
F 0 "#PWR05" H 3250 3750 50  0001 C CNN
F 1 "GND" H 3250 3850 50  0000 C CNN
F 2 "" H 3250 4000 50  0000 C CNN
F 3 "" H 3250 4000 50  0000 C CNN
	1    3250 4000
	1    0    0    -1  
$EndComp
Wire Wire Line
	3000 2550 3000 2600
Wire Wire Line
	3000 2600 3600 2600
Connection ~ 3600 2600
NoConn ~ 3400 2250
NoConn ~ 3400 2150
$Comp
L CONN_01X02 P1
U 1 1 58BEB1DB
P 2950 3850
F 0 "P1" H 2950 4000 50  0000 C CNN
F 1 "Button" V 3050 3850 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_1x02_Pitch2.54mm" H 2950 3850 50  0001 C CNN
F 3 "" H 2950 3850 50  0000 C CNN
	1    2950 3850
	-1   0    0    1   
$EndComp
$Comp
L CONN_01X03 P2
U 1 1 58BEB35C
P 2600 3200
F 0 "P2" H 2600 3400 50  0000 C CNN
F 1 "WS2812B" V 2700 3200 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_1x03_Pitch2.54mm" H 2600 3200 50  0001 C CNN
F 3 "" H 2600 3200 50  0000 C CNN
	1    2600 3200
	-1   0    0    1   
$EndComp
Wire Wire Line
	2800 3300 2900 3300
Wire Wire Line
	2900 3300 2900 3400
$Comp
L +5V #PWR06
U 1 1 58BDE554
P 2900 3000
F 0 "#PWR06" H 2900 2850 50  0001 C CNN
F 1 "+5V" H 2900 3140 50  0000 C CNN
F 2 "" H 2900 3000 50  0000 C CNN
F 3 "" H 2900 3000 50  0000 C CNN
	1    2900 3000
	1    0    0    -1  
$EndComp
Wire Wire Line
	2800 3100 2900 3100
Wire Wire Line
	2900 3100 2900 3000
Wire Wire Line
	3850 3100 3950 3100
NoConn ~ 3950 3400
NoConn ~ 4450 3100
Wire Wire Line
	5350 3400 4450 3400
Wire Wire Line
	3250 3300 3950 3300
Wire Wire Line
	3950 3200 2800 3200
Wire Wire Line
	3250 3300 3250 3800
Wire Wire Line
	3250 3800 3150 3800
Wire Wire Line
	3150 3900 3250 3900
Wire Wire Line
	3250 3900 3250 4000
$EndSCHEMATC
