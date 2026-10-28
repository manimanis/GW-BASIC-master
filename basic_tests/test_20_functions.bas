10 REM ========================================================
20 REM  SUITE : TEST 20 - FONCTIONS INTEGREES AVANCEES
30 REM ========================================================
40 T_SUITE$ = "TEST 20 : FONCTIONS INTEGREES AVANCEES"
50 T_TOTAL = 0 : T_PASS = 0 : T_FAIL = 0
60 PRINT "=== "; T_SUITE$; " ==="
70 PRINT

100 REM --------------------------------------------------------
110 REM 1. FIX() - Troncature vers zero (different de INT)
120 REM --------------------------------------------------------
130 REM FIX truncates toward zero (unlike INT which floors)
140 T_NAME$ = "FIX(3.9) = 3"   : T_ACT = FIX(3.9)   : T_EXP = 3  : GOSUB 9000
150 T_NAME$ = "FIX(-3.9) = -3" : T_ACT = FIX(-3.9)  : T_EXP = -3 : GOSUB 9000
160 T_NAME$ = "FIX(0.99) = 0"  : T_ACT = FIX(0.99)  : T_EXP = 0  : GOSUB 9000
170 T_NAME$ = "FIX(-0.99) = 0" : T_ACT = FIX(-0.99) : T_EXP = 0  : GOSUB 9000
180 T_NAME$ = "FIX(7.0) = 7"   : T_ACT = FIX(7.0)   : T_EXP = 7  : GOSUB 9000

200 REM --------------------------------------------------------
210 REM 2. INT() - Plancher (floor vers moins infini)
220 REM --------------------------------------------------------
230 T_NAME$ = "INT(3.9) = 3"    : T_ACT = INT(3.9)    : T_EXP = 3  : GOSUB 9000
240 T_NAME$ = "INT(-3.1) = -4"  : T_ACT = INT(-3.1)   : T_EXP = -4 : GOSUB 9000
250 T_NAME$ = "INT(-3.9) = -4"  : T_ACT = INT(-3.9)   : T_EXP = -4 : GOSUB 9000
260 T_NAME$ = "INT(0.0) = 0"    : T_ACT = INT(0.0)    : T_EXP = 0  : GOSUB 9000

300 REM --------------------------------------------------------
310 REM 3. CINT() - Arrondi bancaire (half-even) vers entier
320 REM --------------------------------------------------------
330 T_NAME$ = "CINT(3.4) = 3"  : T_ACT = CINT(3.4)  : T_EXP = 3 : GOSUB 9000
340 T_NAME$ = "CINT(3.6) = 4"  : T_ACT = CINT(3.6)  : T_EXP = 4 : GOSUB 9000
350 T_NAME$ = "CINT(-3.4) = -3": T_ACT = CINT(-3.4) : T_EXP = -3 : GOSUB 9000
360 T_NAME$ = "CINT(-3.6) = -4": T_ACT = CINT(-3.6) : T_EXP = -4 : GOSUB 9000

400 REM --------------------------------------------------------
410 REM 4. SGN() - Signe d'un nombre
420 REM --------------------------------------------------------
430 T_NAME$ = "SGN(100) = 1"   : T_ACT = SGN(100)   : T_EXP = 1  : GOSUB 9000
440 T_NAME$ = "SGN(-5.5) = -1" : T_ACT = SGN(-5.5)  : T_EXP = -1 : GOSUB 9000
450 T_NAME$ = "SGN(0) = 0"     : T_ACT = SGN(0)     : T_EXP = 0  : GOSUB 9000
460 T_NAME$ = "SGN(0.001) = 1" : T_ACT = SGN(0.001) : T_EXP = 1  : GOSUB 9000

500 REM --------------------------------------------------------
510 REM 5. MOD - Modulo entier
520 REM --------------------------------------------------------
530 T_NAME$ = "10 MOD 3 = 1"   : T_ACT = 10 MOD 3  : T_EXP = 1 : GOSUB 9000
540 T_NAME$ = "9 MOD 3 = 0"    : T_ACT = 9 MOD 3   : T_EXP = 0 : GOSUB 9000
550 T_NAME$ = "7 MOD 4 = 3"    : T_ACT = 7 MOD 4   : T_EXP = 3 : GOSUB 9000
560 T_NAME$ = "100 MOD 7 = 2"  : T_ACT = 100 MOD 7 : T_EXP = 2 : GOSUB 9000
570 T_NAME$ = "15 MOD 5 = 0"   : T_ACT = 15 MOD 5  : T_EXP = 0 : GOSUB 9000

600 REM --------------------------------------------------------
610 REM 6. STRING$() - Repetition d'un caractere
620 REM --------------------------------------------------------
630 T_NAME$ = "STRING$(5,42) = *****"   : T_ACT$ = STRING$(5, 42)   : T_EXP$ = "*****"   : GOSUB 9100
640 T_NAME$ = "STRING$(3,65) = AAA"     : T_ACT$ = STRING$(3, 65)   : T_EXP$ = "AAA"     : GOSUB 9100
650 T_NAME$ = "STRING$(4,'X') = XXXX"   : T_ACT$ = STRING$(4, "X")  : T_EXP$ = "XXXX"    : GOSUB 9100
660 T_NAME$ = "STRING$(0,42) = ''"      : T_ACT$ = STRING$(0, 42)   : T_EXP$ = ""        : GOSUB 9100

700 REM --------------------------------------------------------
710 REM 7. SPACE$() - Generation d'espaces
720 REM --------------------------------------------------------
730 T_NAME$ = "SPACE$(5) length = 5" : T_ACT = LEN(SPACE$(5)) : T_EXP = 5 : GOSUB 9000
740 T_NAME$ = "SPACE$(0) = ''"      : T_ACT$ = SPACE$(0)     : T_EXP$ = "" : GOSUB 9100
750 T_NAME$ = "SPACE$(3) = '   '"   : T_ACT$ = SPACE$(3)     : T_EXP$ = "   " : GOSUB 9100

800 REM --------------------------------------------------------
810 REM 8. HEX$() - Conversion en hexadecimal
820 REM --------------------------------------------------------
830 T_NAME$ = "HEX$(0) = '0'"     : T_ACT$ = HEX$(0)     : T_EXP$ = "0"    : GOSUB 9100
840 T_NAME$ = "HEX$(10) = 'A'"    : T_ACT$ = HEX$(10)    : T_EXP$ = "A"    : GOSUB 9100
850 T_NAME$ = "HEX$(15) = 'F'"    : T_ACT$ = HEX$(15)    : T_EXP$ = "F"    : GOSUB 9100
860 T_NAME$ = "HEX$(16) = '10'"   : T_ACT$ = HEX$(16)    : T_EXP$ = "10"   : GOSUB 9100
870 T_NAME$ = "HEX$(255) = 'FF'"  : T_ACT$ = HEX$(255)   : T_EXP$ = "FF"   : GOSUB 9100
880 T_NAME$ = "HEX$(256) = '100'" : T_ACT$ = HEX$(256)   : T_EXP$ = "100"  : GOSUB 9100
890 T_NAME$ = "HEX$(65535)='FFFF'": T_ACT$ = HEX$(65535) : T_EXP$ = "FFFF" : GOSUB 9100

900 REM --------------------------------------------------------
910 REM 9. OCT$() - Conversion en octal
920 REM --------------------------------------------------------
930 T_NAME$ = "OCT$(0) = '0'"     : T_ACT$ = OCT$(0)    : T_EXP$ = "0"    : GOSUB 9100
940 T_NAME$ = "OCT$(7) = '7'"     : T_ACT$ = OCT$(7)    : T_EXP$ = "7"    : GOSUB 9100
950 T_NAME$ = "OCT$(8) = '10'"    : T_ACT$ = OCT$(8)    : T_EXP$ = "10"   : GOSUB 9100
960 T_NAME$ = "OCT$(64) = '100'"  : T_ACT$ = OCT$(64)   : T_EXP$ = "100"  : GOSUB 9100
970 T_NAME$ = "OCT$(511) = '777'" : T_ACT$ = OCT$(511)  : T_EXP$ = "777"  : GOSUB 9100

1000 REM --------------------------------------------------------
1010 REM 10. INSTR() - Recherche de sous-chaine
1020 REM --------------------------------------------------------
1030 T_NAME$ = "INSTR('HELLO','LL') = 3"       : T_ACT = INSTR("HELLO","LL")       : T_EXP = 3 : GOSUB 9000
1040 T_NAME$ = "INSTR('HELLO','HE') = 1"       : T_ACT = INSTR("HELLO","HE")       : T_EXP = 1 : GOSUB 9000
1050 T_NAME$ = "INSTR('HELLO','O') = 5"        : T_ACT = INSTR("HELLO","O")        : T_EXP = 5 : GOSUB 9000
1060 T_NAME$ = "INSTR('HELLO','XY') = 0"       : T_ACT = INSTR("HELLO","XY")       : T_EXP = 0 : GOSUB 9000
1070 T_NAME$ = "INSTR('HELLO','') = 1"         : T_ACT = INSTR("HELLO","")         : T_EXP = 1 : GOSUB 9000
1080 REM INSTR with starting position
1090 T_NAME$ = "INSTR(2,'HELLO','L') = 3"      : T_ACT = INSTR(2,"HELLO","L")      : T_EXP = 3 : GOSUB 9000
1100 T_NAME$ = "INSTR(4,'HELLO','L') = 4"      : T_ACT = INSTR(4,"HELLO","L")      : T_EXP = 4 : GOSUB 9000
1110 T_NAME$ = "INSTR(5,'HELLO','L') = 0"      : T_ACT = INSTR(5,"HELLO","L")      : T_EXP = 0 : GOSUB 9000
1120 T_NAME$ = "INSTR(1,'ABCABC','BC') = 2"    : T_ACT = INSTR(1,"ABCABC","BC")    : T_EXP = 2 : GOSUB 9000
1130 T_NAME$ = "INSTR(3,'ABCABC','BC') = 5"    : T_ACT = INSTR(3,"ABCABC","BC")    : T_EXP = 5 : GOSUB 9000

1200 REM --------------------------------------------------------
1210 REM 11. Affectation MID$() (LHS assignment)
1220 REM --------------------------------------------------------
1230 S$ = "HELLO WORLD"
1240 MID$(S$, 7, 5) = "BASIC"
1250 T_NAME$ = "MID$ assignment (HELLO BASIC)" : T_ACT$ = S$ : T_EXP$ = "HELLO BASIC" : GOSUB 9100

1260 S2$ = "ABCDEFGH"
1270 MID$(S2$, 3, 3) = "XYZ"
1280 T_NAME$ = "MID$ assign middle 3 chars"    : T_ACT$ = S2$ : T_EXP$ = "ABXYZFGH" : GOSUB 9100

1290 S3$ = "ABCDEF"
1300 MID$(S3$, 4) = "12"
1310 T_NAME$ = "MID$ assign at pos 4 no len"   : T_ACT$ = S3$ : T_EXP$ = "ABC12F" : GOSUB 9100

1320 REM Remplacement partiel : la source est plus courte que la destination
1330 S4$ = "ABCDEF"
1340 MID$(S4$, 2, 4) = "XY"
1350 T_NAME$ = "MID$ assign short src (AXYCDEF?"): T_ACT$ = S4$ : T_EXP$ = "AXYDEF" : GOSUB 9100

1400 REM --------------------------------------------------------
1410 REM 12. FRE() - Memoire disponible (valeur > 0 attendue)
1420 REM --------------------------------------------------------
1430 FREE_NUM = FRE(0)
1440 T_NAME$ = "FRE(0) numeric mem > 0"   : T_ACT = (FREE_NUM > 0) : T_EXP = -1 : GOSUB 9000
1450 FREE_STR = FRE("")
1460 T_NAME$ = "FRE('') string mem > 0"   : T_ACT = (FREE_STR > 0) : T_EXP = -1 : GOSUB 9000

1500 REM --------------------------------------------------------
1510 REM 13. SPC() et TAB() comme chaines de remplissage
1520 REM --------------------------------------------------------
1530 T_NAME$ = "SPC(4) = 4 espaces"     : T_ACT = LEN(SPC(4)) : T_EXP = 4 : GOSUB 9000
1540 T_NAME$ = "SPC(0) = 0 espaces"     : T_ACT = LEN(SPC(0)) : T_EXP = 0 : GOSUB 9000

1600 REM --------------------------------------------------------
1610 REM 14. ABS() sur entiers et flottants
1620 REM --------------------------------------------------------
1630 T_NAME$ = "ABS(-100) = 100"    : T_ACT = ABS(-100)   : T_EXP = 100  : GOSUB 9000
1640 T_NAME$ = "ABS(0) = 0"         : T_ACT = ABS(0)      : T_EXP = 0    : GOSUB 9000
1650 T_NAME$ = "ABS(-3.14) ~= 3.14" : T_ACT = ABS(-3.14)  : T_EXP = 3.14 : T_EPS = 0.0001 : GOSUB 9200

1700 REM --------------------------------------------------------
1710 REM 15. VAL() et STR$() - Conversions chaine <-> nombre
1720 REM --------------------------------------------------------
1730 T_NAME$ = "VAL('42') = 42"        : T_ACT = VAL("42")        : T_EXP = 42    : GOSUB 9000
1740 T_NAME$ = "VAL('-3.14') ~= -3.14" : T_ACT = VAL("-3.14")     : T_EXP = -3.14 : T_EPS = 0.001 : GOSUB 9200
1750 T_NAME$ = "VAL('  10  ') = 10"    : T_ACT = VAL("  10  ")    : T_EXP = 10    : GOSUB 9000
1760 T_NAME$ = "VAL('0ABC') = 0"       : T_ACT = VAL("0ABC")      : T_EXP = 0     : GOSUB 9000
1770 REM STR$ adds a leading space for positive numbers
1780 T_NAME$ = "LEN(STR$(42)) = 3"     : T_ACT = LEN(STR$(42))    : T_EXP = 3     : GOSUB 9000
1790 T_NAME$ = "STR$(-5) = '-5'"       : T_ACT$ = STR$(-5)        : T_EXP$ = "-5" : GOSUB 9100

8000 REM ========================================================
8010 REM  SYNTHESE ET RESULTAT
8020 REM ========================================================
8030 PRINT "--------------------------------------------------------"
8040 PRINT "Suite   : "; T_SUITE$
8050 PRINT "Total   : "; T_TOTAL; " assertions"
8060 PRINT "Reussis : "; T_PASS
8070 PRINT "Echecs  : "; T_FAIL
8080 IF T_FAIL = 0 THEN GOTO 8100 ELSE GOTO 8110
8100 PRINT "TEST 20 PASSED" : END
8110 PRINT "TEST 20 FAILED ("; T_FAIL; " echecs)" : ERROR 1
8120 END

9000 REM === ASSERT NUM EQUAL ===
9010 REM Entrees: T_NAME$, T_ACT, T_EXP
9020 T_TOTAL = T_TOTAL + 1
9030 IF T_ACT = T_EXP THEN T_PASS = T_PASS + 1 : RETURN
9040 T_FAIL = T_FAIL + 1
9050 PRINT "  [FAIL] "; T_NAME$; " -> Attendu: "; T_EXP; " | Obtenu: "; T_ACT
9060 RETURN

9100 REM === ASSERT STR EQUAL ===
9110 REM Entrees: T_NAME$, T_ACT$, T_EXP$
9120 T_TOTAL = T_TOTAL + 1
9130 IF T_ACT$ = T_EXP$ THEN T_PASS = T_PASS + 1 : RETURN
9140 T_FAIL = T_FAIL + 1
9150 PRINT "  [FAIL] "; T_NAME$; " -> Attendu: ["; T_EXP$; "] | Obtenu: ["; T_ACT$; "]"
9160 RETURN

9200 REM === ASSERT NUM NEAR (float) ===
9210 REM Entrees: T_NAME$, T_ACT, T_EXP, T_EPS
9220 T_TOTAL = T_TOTAL + 1
9230 IF ABS(T_ACT - T_EXP) <= T_EPS THEN T_PASS = T_PASS + 1 : RETURN
9240 T_FAIL = T_FAIL + 1
9250 PRINT "  [FAIL] "; T_NAME$; " -> Attendu: "; T_EXP; " (+/-"; T_EPS; ") | Obtenu: "; T_ACT
9260 RETURN
