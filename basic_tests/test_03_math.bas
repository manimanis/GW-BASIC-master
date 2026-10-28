10 REM ========================================================
20 REM  SUITE : TEST 03 - FONCTIONS ET ARITHMETIQUE MATHEMATIQUE
30 REM ========================================================
40 T_SUITE$ = "TEST 03 : MATHEMATIQUES"
50 T_TOTAL = 0 : T_PASS = 0 : T_FAIL = 0
60 PRINT "=== "; T_SUITE$; " ==="
70 PRINT

100 REM --------------------------------------------------------
110 REM 1. Fonctions de base : ABS, SGN, INT, FIX
120 REM --------------------------------------------------------
130 T_NAME$ = "ABS(42)"    : T_ACT = ABS(42)    : T_EXP = 42 : GOSUB 9000
140 T_NAME$ = "ABS(-42)"   : T_ACT = ABS(-42)   : T_EXP = 42 : GOSUB 9000
150 T_NAME$ = "ABS(0)"     : T_ACT = ABS(0)     : T_EXP = 0  : GOSUB 9000

160 T_NAME$ = "SGN(99)"    : T_ACT = SGN(99)    : T_EXP = 1  : GOSUB 9000
170 T_NAME$ = "SGN(-99)"   : T_ACT = SGN(-99)   : T_EXP = -1 : GOSUB 9000
180 T_NAME$ = "SGN(0)"     : T_ACT = SGN(0)     : T_EXP = 0  : GOSUB 9000

190 REM INT arrondit vers -l'infini (floor)
200 T_NAME$ = "INT(3.7)"   : T_ACT = INT(3.7)   : T_EXP = 3  : GOSUB 9000
210 T_NAME$ = "INT(3.0)"   : T_ACT = INT(3.0)   : T_EXP = 3  : GOSUB 9000
220 T_NAME$ = "INT(-3.2)"  : T_ACT = INT(-3.2)  : T_EXP = -4 : GOSUB 9000
230 T_NAME$ = "INT(-3.0)"  : T_ACT = INT(-3.0)  : T_EXP = -3 : GOSUB 9000
240 T_NAME$ = "INT(0.0)"   : T_ACT = INT(0.0)   : T_EXP = 0  : GOSUB 9000

250 REM FIX tronque vers 0 (truncation)
260 T_NAME$ = "FIX(3.7)"   : T_ACT = FIX(3.7)   : T_EXP = 3  : GOSUB 9000
270 T_NAME$ = "FIX(-3.7)"  : T_ACT = FIX(-3.7)  : T_EXP = -3 : GOSUB 9000
280 T_NAME$ = "FIX(-0.8)"  : T_ACT = FIX(-0.8)  : T_EXP = 0  : GOSUB 9000

300 REM --------------------------------------------------------
310 REM 2. Racines et Puissances : SQR et ^
320 REM --------------------------------------------------------
330 T_NAME$ = "SQR(0)"     : T_ACT = SQR(0)     : T_EXP = 0  : GOSUB 9000
340 T_NAME$ = "SQR(1)"     : T_ACT = SQR(1)     : T_EXP = 1  : GOSUB 9000
350 T_NAME$ = "SQR(4)"     : T_ACT = SQR(4)     : T_EXP = 2  : GOSUB 9000
360 T_NAME$ = "SQR(9)"     : T_ACT = SQR(9)     : T_EXP = 3  : GOSUB 9000
370 T_NAME$ = "SQR(144)"   : T_ACT = SQR(144)   : T_EXP = 12 : GOSUB 9000
380 T_NAME$ = "SQR(2)"     : T_ACT = SQR(2)     : T_EXP = 1.41421356 : T_EPS = 0.0001 : GOSUB 9200

390 T_NAME$ = "Pow: 2^0"   : T_ACT = 2 ^ 0      : T_EXP = 1  : GOSUB 9000
400 T_NAME$ = "Pow: 2^3"   : T_ACT = 2 ^ 3      : T_EXP = 8  : GOSUB 9000
410 T_NAME$ = "Pow: 3^4"   : T_ACT = 3 ^ 4      : T_EXP = 81 : GOSUB 9000
420 T_NAME$ = "Pow: 4^0.5" : T_ACT = 4 ^ 0.5    : T_EXP = 2  : GOSUB 9000
430 T_NAME$ = "Pow: 2^-1"  : T_ACT = 2 ^ -1     : T_EXP = 0.5 : GOSUB 9000

500 REM --------------------------------------------------------
510 REM 3. Exponentielle et Logarithme naturel : EXP et LOG
520 REM --------------------------------------------------------
530 T_NAME$ = "EXP(0)"     : T_ACT = EXP(0)     : T_EXP = 1  : GOSUB 9000
540 T_NAME$ = "LOG(1)"     : T_ACT = LOG(1)     : T_EXP = 0  : GOSUB 9000
550 E_VAL = EXP(1)
560 T_NAME$ = "EXP(1) = e" : T_ACT = E_VAL      : T_EXP = 2.7182818 : T_EPS = 0.0001 : GOSUB 9200
570 T_NAME$ = "LOG(e) = 1" : T_ACT = LOG(E_VAL) : T_EXP = 1  : T_EPS = 0.0001 : GOSUB 9200
580 T_NAME$ = "EXP(LOG(7)) = 7" : T_ACT = EXP(LOG(7)) : T_EXP = 7 : T_EPS = 0.0001 : GOSUB 9200

600 REM --------------------------------------------------------
610 REM 4. Trigonometrie : SIN, COS, TAN, ATN
620 REM --------------------------------------------------------
630 PI = 3.14159265
640 T_NAME$ = "SIN(0)"     : T_ACT = SIN(0)     : T_EXP = 0  : T_EPS = 0.0001 : GOSUB 9200
650 T_NAME$ = "COS(0)"     : T_ACT = COS(0)     : T_EXP = 1  : T_EPS = 0.0001 : GOSUB 9200
660 T_NAME$ = "TAN(0)"     : T_ACT = TAN(0)     : T_EXP = 0  : T_EPS = 0.0001 : GOSUB 9200
670 T_NAME$ = "ATN(0)"     : T_ACT = ATN(0)     : T_EXP = 0  : T_EPS = 0.0001 : GOSUB 9200
680 T_NAME$ = "ATN(1) = PI/4" : T_ACT = ATN(1)  : T_EXP = PI / 4 : T_EPS = 0.0001 : GOSUB 9200
690 T_NAME$ = "SIN(PI/2) = 1" : T_ACT = SIN(PI / 2) : T_EXP = 1 : T_EPS = 0.0001 : GOSUB 9200
700 T_NAME$ = "COS(PI) = -1"  : T_ACT = COS(PI)     : T_EXP = -1 : T_EPS = 0.0001 : GOSUB 9200
710 REM Identite trigonometrique : sin^2(x) + cos^2(x) = 1
720 TRIG_ID = (SIN(1.23) ^ 2) + (COS(1.23) ^ 2)
730 T_NAME$ = "sin^2(x) + cos^2(x) = 1" : T_ACT = TRIG_ID : T_EXP = 1 : T_EPS = 0.0001 : GOSUB 9200

800 REM --------------------------------------------------------
810 REM 5. Arithmetique entiere : Division entiere (\) et Modulo (MOD)
820 REM --------------------------------------------------------
830 T_NAME$ = "Idiv: 7 \ 2"     : T_ACT = 7 \ 2      : T_EXP = 3  : GOSUB 9000
840 T_NAME$ = "Idiv: 10 \ 3"    : T_ACT = 10 \ 3     : T_EXP = 3  : GOSUB 9000
850 T_NAME$ = "Idiv: 100 \ 25"  : T_ACT = 100 \ 25   : T_EXP = 4  : GOSUB 9000
860 T_NAME$ = "Idiv: 0 \ 5"     : T_ACT = 0 \ 5      : T_EXP = 0  : GOSUB 9000
870 T_NAME$ = "MOD: 10 MOD 3"   : T_ACT = 10 MOD 3   : T_EXP = 1  : GOSUB 9000
880 T_NAME$ = "MOD: 15 MOD 4"   : T_ACT = 15 MOD 4   : T_EXP = 3  : GOSUB 9000
890 T_NAME$ = "MOD: 8 MOD 2"    : T_ACT = 8 MOD 2    : T_EXP = 0  : GOSUB 9000

900 REM --------------------------------------------------------
910 REM 6. Precedence et arithmetique complexe
920 REM --------------------------------------------------------
930 REM 2 + 3 * 4 ^ 2 \ 8 - 1  ->  4^2=16, 3*16=48, 48\8=6, 2+6-1=7
940 P_ACT = 2 + 3 * 4 ^ 2 \ 8 - 1
950 T_NAME$ = "Precedence: 2 + 3 * 4^2 \ 8 - 1" : T_ACT = P_ACT : T_EXP = 7 : GOSUB 9000

960 REM Parentheses prioritaires
970 P_PAREN = ((2 + 3) * 4) ^ 2 \ (8 - 1)
980 REM (5*4)^2 \ 7 = 400 \ 7 = 57
990 T_NAME$ = "Precedence with parens: ((2+3)*4)^2 \\ (8-1)" : T_ACT = P_PAREN : T_EXP = 57 : GOSUB 9000

8000 REM ========================================================
8010 REM  SYNTHESE ET RESULTAT
8020 REM ========================================================
8030 PRINT "--------------------------------------------------------"
8040 PRINT "Suite   : "; T_SUITE$
8050 PRINT "Total   : "; T_TOTAL; " assertions"
8060 PRINT "Reussis : "; T_PASS
8070 PRINT "Echecs  : "; T_FAIL
8080 IF T_FAIL = 0 THEN GOTO 8100 ELSE GOTO 8110
8100 PRINT "TEST 03 PASSED" : END
8110 PRINT "TEST 03 FAILED ("; T_FAIL; " echecs)" : ERROR 1
8120 END

9000 REM === ASSERT NUM EQUAL ===
9010 REM Entrees: T_NAME$, T_ACT, T_EXP
9020 T_TOTAL = T_TOTAL + 1
9030 IF T_ACT = T_EXP THEN T_PASS = T_PASS + 1 : RETURN
9040 T_FAIL = T_FAIL + 1
9050 PRINT "  [FAIL] "; T_NAME$; " -> Attendu: "; T_EXP; " | Obtenu: "; T_ACT
9060 RETURN

9200 REM === ASSERT NUM NEAR (float) ===
9210 REM Entrees: T_NAME$, T_ACT, T_EXP, T_EPS
9220 T_TOTAL = T_TOTAL + 1
9230 IF ABS(T_ACT - T_EXP) <= T_EPS THEN T_PASS = T_PASS + 1 : RETURN
9240 T_FAIL = T_FAIL + 1
9250 PRINT "  [FAIL] "; T_NAME$; " -> Attendu: "; T_EXP; " (+/-"; T_EPS; ") | Obtenu: "; T_ACT
9260 RETURN
