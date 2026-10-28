10 REM ========================================================
20 REM  SUITE : TEST 07 - PRIMITIVES GRAPHIQUES (PSET / LINE / BOX / POINT)
30 REM ========================================================
40 T_SUITE$ = "TEST 07 : PRIMITIVES GRAPHIQUES"
50 T_TOTAL = 0 : T_PASS = 0 : T_FAIL = 0
60 PRINT "=== "; T_SUITE$; " ==="
70 PRINT

100 SCREEN 1
110 CLS

120 REM --------------------------------------------------------
130 REM 1. PSET et lecture POINT
140 REM --------------------------------------------------------
150 PSET (5, 5), 3
160 T_NAME$ = "PSET(5,5), 3 read-back" : T_ACT = POINT(5, 5) : T_EXP = 3 : GOSUB 9000

170 REM PRESET efface le pixel (couleur 0)
180 PRESET (5, 5)
190 T_NAME$ = "PRESET(5,5) resets pixel to 0" : T_ACT = POINT(5, 5) : T_EXP = 0 : GOSUB 9000

200 REM --------------------------------------------------------
210 REM 2. LINE simple et verifications POINT
220 REM --------------------------------------------------------
230 LINE (0, 50)-(319, 50), 2
240 T_NAME$ = "LINE horizontal: start pixel (0,50)"  : T_ACT = POINT(0, 50)   : T_EXP = 2 : GOSUB 9000
250 T_NAME$ = "LINE horizontal: middle pixel (160,50)": T_ACT = POINT(160, 50) : T_EXP = 2 : GOSUB 9000
260 T_NAME$ = "LINE horizontal: end pixel (319,50)"   : T_ACT = POINT(319, 50) : T_EXP = 2 : GOSUB 9000
270 T_NAME$ = "Pixel outside line is 0 (160,49)"       : T_ACT = POINT(160, 49) : T_EXP = 0 : GOSUB 9000

300 REM --------------------------------------------------------
310 REM 3. Rectangle plein (LINE BF)
320 REM --------------------------------------------------------
330 LINE (100, 70)-(150, 120), 1, BF
340 T_NAME$ = "Box BF: top-left (100,70)"     : T_ACT = POINT(100, 70)  : T_EXP = 1 : GOSUB 9000
350 T_NAME$ = "Box BF: center (125,95)"       : T_ACT = POINT(125, 95)  : T_EXP = 1 : GOSUB 9000
360 T_NAME$ = "Box BF: bottom-right (150,120)": T_ACT = POINT(150, 120) : T_EXP = 1 : GOSUB 9000
370 T_NAME$ = "Outside Box BF (99,70)"        : T_ACT = POINT(99, 70)   : T_EXP = 0 : GOSUB 9000

400 SAVE_SCREEN "basic_tests\out_test07.bmp"
410 GOTO 8000

8000 REM ========================================================
8010 REM  SYNTHESE ET RESULTAT
8020 REM ========================================================
8030 PRINT "--------------------------------------------------------"
8040 PRINT "Suite   : "; T_SUITE$
8050 PRINT "Total   : "; T_TOTAL; " assertions"
8060 PRINT "Reussis : "; T_PASS
8070 PRINT "Echecs  : "; T_FAIL
8080 IF T_FAIL = 0 THEN GOTO 8100 ELSE GOTO 8110
8100 PRINT "TEST 07 PASSED" : END
8110 PRINT "TEST 07 FAILED ("; T_FAIL; " echecs)" : ERROR 1
8120 END

9000 REM === ASSERT NUM EQUAL ===
9010 REM Entrees: T_NAME$, T_ACT, T_EXP
9020 T_TOTAL = T_TOTAL + 1
9030 IF T_ACT = T_EXP THEN T_PASS = T_PASS + 1 : RETURN
9040 T_FAIL = T_FAIL + 1
9050 PRINT "  [FAIL] "; T_NAME$; " -> Attendu: "; T_EXP; " | Obtenu: "; T_ACT
9060 RETURN
