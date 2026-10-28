10 REM ========================================================
20 REM  SUITE : TEST 04 - MANIPULATION COMPLETE DES CHAINES
30 REM ========================================================
40 T_SUITE$ = "TEST 04 : CHAINES DE CARACTERES"
50 T_TOTAL = 0 : T_PASS = 0 : T_FAIL = 0
60 PRINT "=== "; T_SUITE$; " ==="
70 PRINT

100 REM --------------------------------------------------------
110 REM 1. LEN, LEFT$, RIGHT$
120 REM --------------------------------------------------------
130 S$ = "MICROSOFT GW-BASIC"
140 T_NAME$ = "LEN(chaine normale)" : T_ACT = LEN(S$) : T_EXP = 18 : GOSUB 9000
150 T_NAME$ = "LEN(chaine vide)"    : T_ACT = LEN("")  : T_EXP = 0  : GOSUB 9000
160 T_NAME$ = "LEN(1 caractere)"    : T_ACT = LEN("X") : T_EXP = 1  : GOSUB 9000

170 T_NAME$ = "LEFT$(S$, 9)"   : T_ACT$ = LEFT$(S$, 9)  : T_EXP$ = "MICROSOFT" : GOSUB 9100
180 T_NAME$ = "LEFT$(S$, 0)"   : T_ACT$ = LEFT$(S$, 0)  : T_EXP$ = ""          : GOSUB 9100
190 T_NAME$ = "LEFT$(S$, 100)" : T_ACT$ = LEFT$(S$, 100): T_EXP$ = S$         : GOSUB 9100

200 T_NAME$ = "RIGHT$(S$, 8)"   : T_ACT$ = RIGHT$(S$, 8)  : T_EXP$ = "GW-BASIC" : GOSUB 9100
210 T_NAME$ = "RIGHT$(S$, 0)"   : T_ACT$ = RIGHT$(S$, 0)  : T_EXP$ = ""         : GOSUB 9100
220 T_NAME$ = "RIGHT$(S$, 100)" : T_ACT$ = RIGHT$(S$, 100): T_EXP$ = S$        : GOSUB 9100

300 REM --------------------------------------------------------
310 REM 2. MID$ en lecture et assignation (LHS)
320 REM --------------------------------------------------------
330 T_NAME$ = "MID$(S$, 11, 2)" : T_ACT$ = MID$(S$, 11, 2) : T_EXP$ = "GW" : GOSUB 9100
340 T_NAME$ = "MID$(S$, 11)"    : T_ACT$ = MID$(S$, 11)    : T_EXP$ = "GW-BASIC" : GOSUB 9100
350 T_NAME$ = "MID$(S$, 1, 1)"  : T_ACT$ = MID$(S$, 1, 1)  : T_EXP$ = "M" : GOSUB 9100
360 T_NAME$ = "MID$(S$, 100, 2)": T_ACT$ = MID$(S$, 100, 2): T_EXP$ = ""  : GOSUB 9100

370 REM Assignation MID$ (remplacement en place)
380 TARGET$ = "HELLO WORLD"
390 MID$(TARGET$, 7, 5) = "BASIC"
400 T_NAME$ = "LHS MID$: replace WORLD with BASIC" : T_ACT$ = TARGET$ : T_EXP$ = "HELLO BASIC" : GOSUB 9100

410 MID$(TARGET$, 1, 1) = "J"
420 T_NAME$ = "LHS MID$: replace first char" : T_ACT$ = TARGET$ : T_EXP$ = "JELLO BASIC" : GOSUB 9100

500 REM --------------------------------------------------------
510 REM 3. Recherche de sous-chaine avec INSTR
520 REM --------------------------------------------------------
530 HAY$ = "ABRACADABRA"
540 T_NAME$ = "INSTR sans offset: 'RA'"   : T_ACT = INSTR(HAY$, "RA")       : T_EXP = 3  : GOSUB 9000
550 T_NAME$ = "INSTR avec offset: (4,'RA')": T_ACT = INSTR(4, HAY$, "RA")    : T_EXP = 10 : GOSUB 9000
560 T_NAME$ = "INSTR non trouve: 'XYZ'"   : T_ACT = INSTR(HAY$, "XYZ")      : T_EXP = 0  : GOSUB 9000
570 T_NAME$ = "INSTR premier caractere 'A'": T_ACT = INSTR(HAY$, "A")       : T_EXP = 1  : GOSUB 9000
580 T_NAME$ = "INSTR 2e occurrence: (2,'A')": T_ACT = INSTR(2, HAY$, "A")    : T_EXP = 4  : GOSUB 9000
590 T_NAME$ = "INSTR 3e occurrence: (5,'A')": T_ACT = INSTR(5, HAY$, "A")    : T_EXP = 6  : GOSUB 9000

600 REM --------------------------------------------------------
610 REM 4. CHR$ et ASC
620 REM --------------------------------------------------------
630 T_NAME$ = "CHR$(65) = 'A'"   : T_ACT$ = CHR$(65)     : T_EXP$ = "A"  : GOSUB 9100
640 T_NAME$ = "CHR$(90) = 'Z'"   : T_ACT$ = CHR$(90)     : T_EXP$ = "Z"  : GOSUB 9100
650 T_NAME$ = "CHR$(48) = '0'"   : T_ACT$ = CHR$(48)     : T_EXP$ = "0"  : GOSUB 9100
660 T_NAME$ = "ASC('A') = 65"    : T_ACT = ASC("A")      : T_EXP = 65   : GOSUB 9000
670 T_NAME$ = "ASC('Z') = 90"    : T_ACT = ASC("Z")      : T_EXP = 90   : GOSUB 9000
680 T_NAME$ = "ASC('HELLO') = 72": T_ACT = ASC("HELLO")  : T_EXP = 72   : GOSUB 9000

690 REM Aller-retour ASC(CHR$(n))
700 ALLER_RETOUR = 1
710 FOR C = 65 TO 90
720   IF ASC(CHR$(C)) <> C THEN ALLER_RETOUR = 0
730 NEXT C
740 T_NAME$ = "Roundtrip ASC(CHR$(C)) pour A..Z" : T_ACT = ALLER_RETOUR : T_EXP = 1 : GOSUB 9000

800 REM --------------------------------------------------------
810 REM 5. STR$ et VAL
820 REM --------------------------------------------------------
830 T_NAME$ = "STR$(42) = ' 42'"   : T_ACT$ = STR$(42)    : T_EXP$ = " 42"  : GOSUB 9100
840 T_NAME$ = "STR$(-42) = '-42'"  : T_ACT$ = STR$(-42)   : T_EXP$ = "-42"  : GOSUB 9100
850 T_NAME$ = "STR$(0) = ' 0'"     : T_ACT$ = STR$(0)     : T_EXP$ = " 0"   : GOSUB 9100
860 T_NAME$ = "VAL('123.45')"      : T_ACT = VAL("123.45"): T_EXP = 123.45 : T_EPS = 0.001 : GOSUB 9200
870 T_NAME$ = "VAL('-999')"        : T_ACT = VAL("-999")  : T_EXP = -999   : GOSUB 9000
880 T_NAME$ = "VAL('&HFF')"        : T_ACT = VAL("&HFF")  : T_EXP = 255    : GOSUB 9000
890 T_NAME$ = "VAL('&O77')"        : T_ACT = VAL("&O77")  : T_EXP = 63     : GOSUB 9000
900 T_NAME$ = "VAL(STR$(54321))"   : T_ACT = VAL(STR$(54321)) : T_EXP = 54321 : GOSUB 9000

1000 REM --------------------------------------------------------
1010 REM 6. SPACE$ et STRING$
1020 REM --------------------------------------------------------
1030 T_NAME$ = "SPACE$(5) length"   : T_ACT = LEN(SPACE$(5)) : T_EXP = 5 : GOSUB 9000
1040 T_NAME$ = "SPACE$(5) content"  : T_ACT$ = SPACE$(5)     : T_EXP$ = "     " : GOSUB 9100
1050 T_NAME$ = "STRING$(4, '*')"    : T_ACT$ = STRING$(4, "*") : T_EXP$ = "****" : GOSUB 9100
1060 T_NAME$ = "STRING$(3, 65)"     : T_ACT$ = STRING$(3, 65)  : T_EXP$ = "AAA"  : GOSUB 9100

8000 REM ========================================================
8010 REM  SYNTHESE ET RESULTAT
8020 REM ========================================================
8030 PRINT "--------------------------------------------------------"
8040 PRINT "Suite   : "; T_SUITE$
8050 PRINT "Total   : "; T_TOTAL; " assertions"
8060 PRINT "Reussis : "; T_PASS
8070 PRINT "Echecs  : "; T_FAIL
8080 IF T_FAIL = 0 THEN GOTO 8100 ELSE GOTO 8110
8100 PRINT "TEST 04 PASSED" : END
8110 PRINT "TEST 04 FAILED ("; T_FAIL; " echecs)" : ERROR 1
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
