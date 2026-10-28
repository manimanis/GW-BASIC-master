10 REM ========================================================
20 REM  SUITE : TEST 19 - DEMO GRAPHIQUE COMBINEE ET EXPORT BMP
30 REM ========================================================
40 T_SUITE$ = "TEST 19 : DEMO GRAPHIQUE"
50 T_TOTAL = 0 : T_PASS = 0 : T_FAIL = 0
60 PRINT "=== "; T_SUITE$; " ==="
70 PRINT

100 SCREEN 1
110 CLS
120 PI = 3.14159265

130 REM 1. Fond degrade par bandes
140 FOR Y = 0 TO 199 STEP 8
150   C = (Y \ 8) MOD 4
160   LINE (0, Y)-(319, Y+7), C, BF
170 NEXT Y

200 REM 2. Cercles concentriques
210 FOR R = 30 TO 10 STEP -10
220   C = (R \ 10) MOD 3 + 1
230   CIRCLE (80, 80), R, C
240   PAINT (80, 80), C, C
250 NEXT R

300 REM 3. Grille de rectangles
310 FOR X = 150 TO 270 STEP 30
320   FOR Y = 20 TO 80 STEP 30
330     LINE (X, Y)-(X+20, Y+20), 2, BF
340   NEXT Y
350 NEXT X

400 REM 4. Courbe sinusoidale
410 FOR X = 0 TO 319
420   Y_VAL = 150 + INT(20 * SIN(X * PI / 40))
430   PSET (X, Y_VAL), 3
440 NEXT X

500 REM 5. Assertions programmatiques sur les pixels du rendu
510 T_NAME$ = "Concentric circle center is filled (not 0)" : T_ACT = (POINT(80, 80) <> 0) : T_EXP = -1 : GOSUB 9000
520 T_NAME$ = "Grid rectangle interior pixel (160,30) is 2" : T_ACT = POINT(160, 30) : T_EXP = 2 : GOSUB 9000
530 T_NAME$ = "Sine wave pixel at X=0, Y=150 is 3" : T_ACT = POINT(0, 150) : T_EXP = 3 : GOSUB 9000
540 T_NAME$ = "Sine wave pixel at X=20, Y=170 is 3" : T_ACT = POINT(20, 170) : T_EXP = 3 : GOSUB 9000

600 SAVE_SCREEN "basic_tests\out_test19_demo.bmp"
610 GOTO 8000

8000 REM ========================================================
8010 REM  SYNTHESE ET RESULTAT
8020 REM ========================================================
8030 PRINT "--------------------------------------------------------"
8040 PRINT "Suite   : "; T_SUITE$
8050 PRINT "Total   : "; T_TOTAL; " assertions"
8060 PRINT "Reussis : "; T_PASS
8070 PRINT "Echecs  : "; T_FAIL
8080 IF T_FAIL = 0 THEN GOTO 8100 ELSE GOTO 8110
8100 PRINT "TEST 19 PASSED" : END
8110 PRINT "TEST 19 FAILED ("; T_FAIL; " echecs)" : ERROR 1
8120 END

9000 REM === ASSERT NUM EQUAL ===
9010 REM Entrees: T_NAME$, T_ACT, T_EXP
9020 T_TOTAL = T_TOTAL + 1
9030 IF T_ACT = T_EXP THEN T_PASS = T_PASS + 1 : RETURN
9040 T_FAIL = T_FAIL + 1
9050 PRINT "  [FAIL] "; T_NAME$; " -> Attendu: "; T_EXP; " | Obtenu: "; T_ACT
9060 RETURN
