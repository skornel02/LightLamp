EESchema Schematic File Version 4
EELAYER 26 0
EELAYER END
$Descr User 5512 5119
encoding utf-8
Sheet 1 1
Title "LightLamp"
Date "2019-05-27"
Rev "1"
Comp "Kornél Stefán"
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L MCU_Module:Adafruit_HUZZAH_ESP8266_breakout E1
U 1 1 5CEC2FFD
P 1850 2200
F 0 "E1" H 1350 3000 50  0000 C CNN
F 1 "ESP8266" H 1350 2900 50  0000 C CNB
F 2 "" H 2050 2800 50  0001 C CNN
F 3 "https://www.adafruit.com/product/2471" H 2150 2900 50  0001 C CNN
	1    1850 2200
	1    0    0    -1  
$EndComp
$Comp
L Device:Q_PMOS_DGS M1
U 1 1 5CEC3A97
P 3600 1100
F 0 "M1" H 3600 800 50  0000 C CNN
F 1 "AO3400" H 3550 900 50  0000 C CNB
F 2 "" H 3800 1200 50  0001 C CNN
F 3 "~" H 3600 1100 50  0001 C CNN
	1    3600 1100
	-1   0    0    1   
$EndComp
$Comp
L power:+12V #PWR?
U 1 1 5CEC3C72
P 3050 950
F 0 "#PWR?" H 3050 800 50  0001 C CNN
F 1 "+12V" V 3000 900 50  0000 C CNN
F 2 "" H 3050 950 50  0001 C CNN
F 3 "" H 3050 950 50  0001 C CNN
	1    3050 950 
	0    1    1    0   
$EndComp
$Comp
L Regulator_Switching:R-7812-0.5 S1
U 1 1 5CEC4300
P 2450 950
F 0 "S1" H 2450 1100 50  0000 C CNN
F 1 "Step-down 12v-3.3v" H 2450 1200 50  0000 C CNB
F 2 "Converter_DCDC:Converter_DCDC_RECOM_R-78E-0.5_THT" H 2500 700 50  0001 L CIN
F 3 "https://www.recom-power.com/pdf/Innoline/R-78xx-0.5.pdf" H 2450 950 50  0001 C CNN
	1    2450 950 
	-1   0    0    1   
$EndComp
$Comp
L power:GND #PWR?
U 1 1 5CEC449C
P 1750 3100
F 0 "#PWR?" H 1750 2850 50  0001 C CNN
F 1 "GND" H 1755 2927 50  0000 C CNN
F 2 "" H 1750 3100 50  0001 C CNN
F 3 "" H 1750 3100 50  0001 C CNN
	1    1750 3100
	1    0    0    -1  
$EndComp
$Comp
L Device:LED L1
U 1 1 5CEC458E
P 3100 1400
F 0 "L1" H 3050 1150 50  0000 L CNN
F 1 "LED 12V" H 2900 1250 50  0000 L CNB
F 2 "" H 3100 1400 50  0001 C CNN
F 3 "~" H 3100 1400 50  0001 C CNN
	1    3100 1400
	-1   0    0    1   
$EndComp
Wire Wire Line
	2900 1400 2900 950 
Wire Wire Line
	2150 950  2050 950 
Wire Wire Line
	2050 950  2050 1400
Wire Wire Line
	1750 3100 1750 3000
$Comp
L power:GND #PWR?
U 1 1 5CEC4B5A
P 3050 650
F 0 "#PWR?" H 3050 400 50  0001 C CNN
F 1 "GND" H 3000 600 50  0000 R CNN
F 2 "" H 3050 650 50  0001 C CNN
F 3 "" H 3050 650 50  0001 C CNN
	1    3050 650 
	-1   0    0    1   
$EndComp
Wire Wire Line
	3050 650  2450 650 
Wire Wire Line
	3500 1400 3500 1300
Wire Wire Line
	3500 800  3050 800 
Wire Wire Line
	3050 800  3050 650 
Connection ~ 3050 650 
Wire Wire Line
	2750 950  2900 950 
Connection ~ 2900 950 
Wire Wire Line
	2900 950  3050 950 
Wire Wire Line
	3500 800  3500 900 
Wire Wire Line
	3250 1400 3500 1400
Wire Wire Line
	2900 1400 2950 1400
Wire Wire Line
	3800 2000 3800 1100
Wire Wire Line
	2450 2000 3800 2000
$Comp
L Device:R R1
U 1 1 5CED5531
P 3550 650
F 0 "R1" V 3450 650 50  0000 C CNN
F 1 "100K" V 3550 650 50  0000 C CNN
F 2 "" V 3480 650 50  0001 C CNN
F 3 "~" H 3550 650 50  0001 C CNN
	1    3550 650 
	0    1    1    0   
$EndComp
Wire Wire Line
	3700 650  3800 650 
Wire Wire Line
	3800 650  3800 1100
Connection ~ 3800 1100
Wire Wire Line
	3400 650  3050 650 
$EndSCHEMATC
