10 REM ========================================================
20 REM  SUITE : TEST 08 - GRAPHISME AVANCE (CIRCLE / PAINT / DRAW / POINT)
30 REM ========================================================
40 T_SUITE$ = "TEST 08 : GRAPHISME AVANCE"
50 T_TOTAL = 0 : T_PASS = 0 : T_FAIL = 0
60 PRINT "=== "; T_SUITE$; " ==="
70 PRINT

100 SCREEN 1
110 CLS

120 REM --------------------------------------------------------
130 REM 1. CIRCLE et verification de contour
140 REM --------------------------------------------------------
150 CIRCLE (60, 60), 40, 2
160 T_NAME$ = "Before PAINT: Center (60,60) is empty" : T_ACT = POINT(60, 60) : T_EXP = 0 : GOSUB 9000
170 T_NAME$ = "Circle top border pixel (60,20)"       : T_ACT = POINT(60, 20) : T_EXP = 2 : GOSUB 9000
180 T_NAME$ = "Circle bottom border pixel (60,100)"    : T_ACT = POINT(60, 100): T_EXP = 2 : GOSUB 9000

200 REM --------------------------------------------------------
210 REM 2. PAINT (Flood fill hermetique a l'interieur du cercle)
220 REM --------------------------------------------------------
230 PAINT (60, 60), 3, 2
240 T_NAME$ = "After PAINT: Center (60,60) filled with 3" : T_ACT = POINT(60, 60) : T_EXP = 3 : GOSUB 9000
250 T_NAME$ = "After PAINT: Point inside (70,70) filled"   : T_ACT = POINT(70, 70) : T_EXP = 3 : GOSUB 9000
260 T_NAME$ = "Hermetic check: Outside pixel (5,5) is 0"   : T_ACT = POINT(5, 5)   : T_EXP = 0 : GOSUB 9000

300 REM --------------------------------------------------------
310 REM 3. Commandes DRAW macro
320 REM --------------------------------------------------------
330 PSET (150, 150), 0
340 DRAW "C2 M150,150 U20 R20 D20 L20"
350 T_NAME$ = "DRAW path pixel at start (150,150)"    : T_ACT = POINT(150, 150) : T_EXP = 2 : GOSUB 9000
360 T_NAME$ = "DRAW path pixel at top (150,130)"      : T_ACT = POINT(150, 130) : T_EXP = 2 : GOSUB 9000
370 T_NAME$ = "DRAW path pixel at top-right (170,130)": T_ACT = POINT(170, 130) : T_EXP = 2 : GOSUB 9000

400 REM --------------------------------------------------------
410 REM 4. POINT hors ecran (out-of-bounds protection)
420 REM --------------------------------------------------------
430 T_NAME$ = "POINT(500,500) out of bounds returns 0" : T_ACT = POINT(500, 500) : T_EXP = 0 : GOSUB 9000

500 SAVE_SCREEN "basic_tests\out_test08.bmp"
510 GOTO 8000

8000 REM ========================================================
8010 REM  SYNTHESE ET RESULTAT
8020 REM ========================================================
8030 PRINT "--------------------------------------------------------"
8040 PRINT "Suite   : "; T_SUITE$
8050 PRINT "Total   : "; T_TOTAL; " assertions"
8060 PRINT "Reussis : "; T_PASS
8070 PRINT "Echecs  : "; T_FAIL
8080 IF T_FAIL = 0 THEN GOTO 8100 ELSE GOTO 8110
8100 PRINT "TEST 08 PASSED" : END
8110 PRINT "TEST 08 FAILED ("; T_FAIL; " echecs)" : ERROR 1
8120 END

9000 REM === ASSERT NUM EQUAL ===
9010 REM Entrees: T_NAME$, T_ACT, T_EXP
9020 T_TOTAL = T_TOTAL + 1
9030 IF T_ACT = T_EXP THEN T_PASS = T_PASS + 1 : RETURN
9040 T_FAIL = T_FAIL + 1
9050 PRINT "  [FAIL] "; T_NAME$; " -> Attendu: "; T_EXP; " | Obtenu: "; T_ACT
9060 RETURN
