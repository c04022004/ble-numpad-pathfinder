EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 3 4
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
L Device:R_Small R9
U 1 1 60AE90F8
P 5475 3725
F 0 "R9" H 5534 3771 50  0000 L CNN
F 1 "2k" H 5534 3680 50  0000 L CNN
F 2 "Resistor_SMD:R_0805_2012Metric_Pad1.15x1.40mm_HandSolder" H 5475 3725 50  0001 C CNN
F 3 "~" H 5475 3725 50  0001 C CNN
	1    5475 3725
	1    0    0    -1  
$EndComp
Text GLabel 5400 3950 0    50   Input ~ 0
GND
Wire Wire Line
	5475 3625 5475 3575
Wire Wire Line
	5425 3375 5475 3375
$Comp
L Device:LED_Small D3
U 1 1 60AE90EE
P 5475 3475
F 0 "D3" V 5521 3405 50  0000 R CNN
F 1 "RED" V 5430 3405 50  0000 R CNN
F 2 "Diode_SMD:D_0805_2012Metric_Pad1.15x1.40mm_HandSolder" V 5475 3475 50  0001 C CNN
F 3 "~" V 5475 3475 50  0001 C CNN
	1    5475 3475
	0    -1   -1   0   
$EndComp
$Comp
L Device:C_Small C3
U 1 1 5FF989A1
P 6450 4175
F 0 "C3" H 6542 4221 50  0000 L CNN
F 1 "0.1u" H 6542 4130 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric_Pad1.15x1.40mm_HandSolder" H 6450 4175 50  0001 C CNN
F 3 "~" H 6450 4175 50  0001 C CNN
	1    6450 4175
	1    0    0    -1  
$EndComp
Wire Wire Line
	6450 3975 6450 4075
Connection ~ 6450 3975
Wire Wire Line
	6450 4275 6450 4350
Wire Wire Line
	6400 4350 6450 4350
Connection ~ 6450 4350
Wire Wire Line
	6450 4350 6800 4350
$Comp
L Device:C_Small C4
U 1 1 5FF9A00F
P 6800 4175
F 0 "C4" H 6892 4221 50  0000 L CNN
F 1 "1u" H 6892 4130 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric_Pad1.15x1.40mm_HandSolder" H 6800 4175 50  0001 C CNN
F 3 "~" H 6800 4175 50  0001 C CNN
	1    6800 4175
	1    0    0    -1  
$EndComp
Wire Wire Line
	6800 4350 6800 4275
Wire Wire Line
	6450 3975 6800 3975
Wire Wire Line
	6850 3975 6800 3975
Connection ~ 6800 3975
Wire Wire Line
	6800 3975 6800 4075
Text GLabel 6400 4350 0    50   Input ~ 0
GND
Wire Wire Line
	7625 3475 7675 3475
Wire Wire Line
	8475 3375 8525 3375
Wire Wire Line
	5825 3050 5925 3050
Wire Wire Line
	5525 3050 5450 3050
$Comp
L Device:Polyfuse F1
U 1 1 6017BF1A
P 5675 3050
F 0 "F1" V 5450 3050 50  0000 C CNN
F 1 "Polyfuse" V 5541 3050 50  0000 C CNN
F 2 "Fuse:Fuse_0805_2012Metric_Pad1.15x1.40mm_HandSolder" H 5725 2850 50  0001 L CNN
F 3 "~" H 5675 3050 50  0001 C CNN
	1    5675 3050
	0    1    1    0   
$EndComp
Wire Wire Line
	8475 3175 8525 3175
Wire Wire Line
	7625 3375 7675 3375
Wire Wire Line
	8475 3475 8525 3475
NoConn ~ 7675 2975
NoConn ~ 7675 2875
NoConn ~ 7675 3575
NoConn ~ 7675 3675
NoConn ~ 8475 3275
$Comp
L power:PWR_FLAG #FLG0103
U 1 1 60641809
P 4600 2775
F 0 "#FLG0103" H 4600 2850 50  0001 C CNN
F 1 "PWR_FLAG" H 4600 2948 50  0000 C CNN
F 2 "" H 4600 2775 50  0001 C CNN
F 3 "~" H 4600 2775 50  0001 C CNN
	1    4600 2775
	1    0    0    -1  
$EndComp
NoConn ~ 8475 3075
$Comp
L Device:C_Small C5
U 1 1 600177AD
P 6500 3600
F 0 "C5" H 6592 3646 50  0000 L CNN
F 1 "0.1u" H 6592 3555 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric_Pad1.15x1.40mm_HandSolder" H 6500 3600 50  0001 C CNN
F 3 "~" H 6500 3600 50  0001 C CNN
	1    6500 3600
	1    0    0    -1  
$EndComp
Text GLabel 6375 3775 0    50   Input ~ 0
GND
Wire Wire Line
	6375 3775 6500 3775
Wire Wire Line
	6500 3775 6500 3700
Wire Wire Line
	6500 3500 6500 3400
NoConn ~ 8175 2675
Wire Wire Line
	7675 3175 7625 3175
Wire Wire Line
	7625 3075 7675 3075
Wire Wire Line
	7625 3275 7675 3275
$Comp
L Device:C_Small C2
U 1 1 5FDBB12A
P 3525 4450
F 0 "C2" V 3650 4325 50  0000 L CNN
F 1 "0.1uF" V 3725 4325 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric_Pad1.15x1.40mm_HandSolder" H 3525 4450 50  0001 C CNN
F 3 "~" H 3525 4450 50  0001 C CNN
	1    3525 4450
	0    1    1    0   
$EndComp
Text GLabel 3800 4450 2    50   Input ~ 0
GND
Wire Wire Line
	8175 4475 8175 4500
Connection ~ 8175 4500
Wire Wire Line
	8075 4475 8075 4500
Text GLabel 8250 4500 2    50   Input ~ 0
GND
$Comp
L dk_Interface-Controllers:CP2102-GMR_NRND U?
U 1 1 611D89CC
P 7975 3475
F 0 "U?" H 8075 2378 60  0000 C CNN
F 1 "CP2102-GMR_NRND" H 8075 2272 60  0000 C CNN
F 2 "digikey-footprints:VFQFN-28-1EP_5x5mm" H 8175 3675 60  0001 L CNN
F 3 "https://www.silabs.com/documents/public/data-sheets/CP2102-9.pdf" H 8175 3775 60  0001 L CNN
F 4 "336-1160-1-ND" H 8175 3875 60  0001 L CNN "Digi-Key_PN"
F 5 "CP2102-GMR" H 8175 3975 60  0001 L CNN "MPN"
F 6 "Integrated Circuits (ICs)" H 8175 4075 60  0001 L CNN "Category"
F 7 "Interface - Controllers" H 8175 4175 60  0001 L CNN "Family"
F 8 "https://www.silabs.com/documents/public/data-sheets/CP2102-9.pdf" H 8175 4275 60  0001 L CNN "DK_Datasheet_Link"
F 9 "/product-detail/en/silicon-labs/CP2102-GMR/336-1160-1-ND/3672615" H 8175 4375 60  0001 L CNN "DK_Detail_Page"
F 10 "IC USB-TO-UART BRIDGE 28VQFN" H 8175 4475 60  0001 L CNN "Description"
F 11 "Silicon Labs" H 8175 4575 60  0001 L CNN "Manufacturer"
F 12 "Not For New Designs" H 8175 4675 60  0001 L CNN "Status"
	1    7975 3475
	1    0    0    -1  
$EndComp
Wire Wire Line
	8175 4500 8250 4500
Wire Wire Line
	8075 4500 8175 4500
$Comp
L Connector:USB_C_Receptacle_USB2.0 J?
U 1 1 610998D6
P 3675 3500
F 0 "J?" H 3782 4367 50  0000 C CNN
F 1 "USB_C_Receptacle_USB2.0" H 3782 4276 50  0000 C CNN
F 2 "" H 3825 3500 50  0001 C CNN
F 3 "https://www.usb.org/sites/default/files/documents/usb_type-c.zip" H 3825 3500 50  0001 C CNN
	1    3675 3500
	1    0    0    -1  
$EndComp
Text HLabel 4525 2900 2    50   Input ~ 0
VBUS
Text HLabel 4375 3400 2    50   Input ~ 0
TYPEC_D-
Text HLabel 4375 3600 2    50   Input ~ 0
TYPEC_D+
Wire Wire Line
	3375 4400 3375 4450
Wire Wire Line
	3375 4450 3425 4450
Wire Wire Line
	3625 4450 3675 4450
Wire Wire Line
	3675 4450 3675 4400
Wire Wire Line
	3800 4450 3675 4450
Connection ~ 3675 4450
Wire Wire Line
	4275 3400 4325 3400
Wire Wire Line
	4325 3400 4325 3500
Wire Wire Line
	4325 3500 4275 3500
Connection ~ 4325 3400
Wire Wire Line
	4325 3400 4375 3400
Wire Wire Line
	4375 3600 4325 3600
Wire Wire Line
	4325 3600 4325 3700
Wire Wire Line
	4325 3700 4275 3700
Connection ~ 4325 3600
Wire Wire Line
	4325 3600 4275 3600
NoConn ~ 4275 4000
NoConn ~ 4275 4100
Wire Wire Line
	4350 2900 4350 2775
Wire Wire Line
	4350 2775 4600 2775
Wire Wire Line
	4275 2900 4350 2900
Connection ~ 4350 2900
Wire Wire Line
	4350 2900 4525 2900
Text HLabel 7625 3075 0    50   Input ~ 0
TYPEC_D+
Text HLabel 7625 3375 0    50   Input ~ 0
RST_2102
Text HLabel 7625 3175 0    50   Input ~ 0
TYPEC_D-
Text HLabel 7625 3475 0    50   Input ~ 0
TX_2102
Text HLabel 7625 3275 0    50   Input ~ 0
VBUS
Text HLabel 8525 3175 2    50   Input ~ 0
SUS_2102
Text HLabel 8525 3375 2    50   Input ~ 0
RX_2102
Text HLabel 8525 3475 2    50   Input ~ 0
DTR_2102
Text HLabel 5450 3050 0    50   Input ~ 0
VBUS
Text HLabel 5925 3050 2    50   Input ~ 0
VBUS_FUSED
Text HLabel 5425 3375 0    50   Input ~ 0
VBUS
Wire Wire Line
	6200 3400 6500 3400
Text Label 6225 3400 0    50   ~ 0
VDD
Wire Wire Line
	8075 2550 8075 2675
Text Label 7825 2550 0    50   ~ 0
VDD
Text Label 8325 2575 0    50   ~ 0
REGIN
Wire Wire Line
	7825 2550 8075 2550
Wire Wire Line
	8275 2575 8550 2575
Wire Wire Line
	8275 2575 8275 2675
Text Label 6100 3975 0    50   ~ 0
REGIN
Text HLabel 6850 3975 2    50   Input ~ 0
VBUS
Wire Wire Line
	6100 3975 6450 3975
Wire Wire Line
	5475 3825 5475 3950
Wire Wire Line
	5475 3950 5400 3950
$EndSCHEMATC
