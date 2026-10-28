10 REM ========================================================
20 REM  SUITE : TEST 02 - VARIABLES, TYPAGE, TABLEAUX ET SWAP
30 REM ========================================================
40 T_SUITE$ = "TEST 02 : VARIABLES & TABLEAUX"
50 T_TOTAL = 0 : T_PASS = 0 : T_FAIL = 0
60 PRINT "=== "; T_SUITE$; " ==="
70 PRINT

100 REM --------------------------------------------------------
110 REM 1. Scalaires numeriques et typages
120 REM --------------------------------------------------------
130 INT_POS% = 32767
140 INT_NEG% = -32768
150 INT_ZERO% = 0
160 T_NAME$ = "Integer max: 32767" : T_ACT = INT_POS% : T_EXP = 32767 : GOSUB 9000
170 T_NAME$ = "Integer min: -32768" : T_ACT = INT_NEG% : T_EXP = -32768 : GOSUB 9000
180 T_NAME$ = "Integer zero: 0" : T_ACT = INT_ZERO% : T_EXP = 0 : GOSUB 9000

190 SNG_VAL! = 123.456
200 T_NAME$ = "Single precision scalar" : T_ACT = SNG_VAL! : T_EXP = 123.456 : T_EPS = 0.001 : GOSUB 9200

210 DBL_VAL# = 98765.4321
220 T_NAME$ = "Double precision scalar" : T_ACT = DBL_VAL# : T_EXP = 98765.4321 : T_EPS = 0.0001 : GOSUB 9200

230 DEF_VAL = 42
240 T_NAME$ = "Default numeric scalar" : T_ACT = DEF_VAL : T_EXP = 42 : GOSUB 9000

250 REM Variables non initialisees
260 T_NAME$ = "Uninitialized numeric is 0" : T_ACT = UNINIT_NUM : T_EXP = 0 : GOSUB 9000
270 T_NAME$ = "Uninitialized string is empty" : T_ACT$ = UNINIT_STR$ : T_EXP$ = "" : GOSUB 9100

300 REM --------------------------------------------------------
310 REM 2. Scalaires chaines
320 REM --------------------------------------------------------
330 STR1$ = "GW" : STR2$ = "-BASIC"
340 STR_RES$ = STR1$ + STR2$
350 T_NAME$ = "String concatenation" : T_ACT$ = STR_RES$ : T_EXP$ = "GW-BASIC" : GOSUB 9100

360 STR_EMPTY$ = ""
370 T_NAME$ = "Empty string length" : T_ACT = LEN(STR_EMPTY$) : T_EXP = 0 : GOSUB 9000

380 STR_REASSIGN$ = "First"
390 STR_REASSIGN$ = "Second"
400 T_NAME$ = "String reassignment" : T_ACT$ = STR_REASSIGN$ : T_EXP$ = "Second" : GOSUB 9100

410 REM Arithmetique combinee
420 X = 10 : Y = 20 : Z = 5
430 CALC = (X * Y) + (Y / Z) - (X + Z)
440 T_NAME$ = "Compound expression (10*20)+(20/5)-(10+5)" : T_ACT = CALC : T_EXP = 189 : GOSUB 9000

500 REM --------------------------------------------------------
510 REM 3. Tableaux 1D (numeriques et chaines)
520 REM --------------------------------------------------------
530 DIM A1D(10)
540 FOR I = 0 TO 10
550   A1D(I) = I * 10
560 NEXT I
570 T_NAME$ = "1D Array: Base index 0" : T_ACT = A1D(0) : T_EXP = 0 : GOSUB 9000
580 T_NAME$ = "1D Array: Middle index 5" : T_ACT = A1D(5) : T_EXP = 50 : GOSUB 9000
590 T_NAME$ = "1D Array: Max index 10" : T_ACT = A1D(10) : T_EXP = 100 : GOSUB 9000

600 DIM S1D$(4)
610 S1D$(0) = "Zero" : S1D$(1) = "One" : S1D$(2) = "Two" : S1D$(3) = "Three" : S1D$(4) = "Four"
620 T_NAME$ = "String array index 0" : T_ACT$ = S1D$(0) : T_EXP$ = "Zero" : GOSUB 9100
630 T_NAME$ = "String array index 3" : T_ACT$ = S1D$(3) : T_EXP$ = "Three" : GOSUB 9100
640 T_NAME$ = "String array index 4" : T_ACT$ = S1D$(4) : T_EXP$ = "Four" : GOSUB 9100

700 REM --------------------------------------------------------
710 REM 4. Tableaux 2D (Matrices) et 3D
720 REM --------------------------------------------------------
730 DIM MAT(3, 3)
740 FOR R = 1 TO 3
750   FOR C = 1 TO 3
760     MAT(R, C) = R * 10 + C
770   NEXT C
780 NEXT R
790 T_NAME$ = "Matrix MAT(1,1)" : T_ACT = MAT(1,1) : T_EXP = 11 : GOSUB 9000
800 T_NAME$ = "Matrix MAT(2,3)" : T_ACT = MAT(2,3) : T_EXP = 23 : GOSUB 9000
810 T_NAME$ = "Matrix MAT(3,2)" : T_ACT = MAT(3,2) : T_EXP = 32 : GOSUB 9000
820 T_NAME$ = "Matrix MAT(3,3)" : T_ACT = MAT(3,3) : T_EXP = 33 : GOSUB 9000

830 REM Somme diagonale
840 DIAG_SUM = MAT(1,1) + MAT(2,2) + MAT(3,3)
850 T_NAME$ = "Matrix Diagonal Sum (11+22+33)" : T_ACT = DIAG_SUM : T_EXP = 66 : GOSUB 9000

860 DIM CUBE(2, 2, 2)
870 CUBE(0, 0, 0) = 100
880 CUBE(1, 1, 1) = 200
890 CUBE(2, 2, 2) = 300
900 T_NAME$ = "3D Array CUBE(0,0,0)" : T_ACT = CUBE(0,0,0) : T_EXP = 100 : GOSUB 9000
910 T_NAME$ = "3D Array CUBE(1,1,1)" : T_ACT = CUBE(1,1,1) : T_EXP = 200 : GOSUB 9000
920 T_NAME$ = "3D Array CUBE(2,2,2)" : T_ACT = CUBE(2,2,2) : T_EXP = 300 : GOSUB 9000

1000 REM --------------------------------------------------------
1010 REM 5. Instruction SWAP
1020 REM --------------------------------------------------------
1030 SW1 = 111 : SW2 = 999
1040 SWAP SW1, SW2
1050 T_NAME$ = "SWAP scalar num (SW1)" : T_ACT = SW1 : T_EXP = 999 : GOSUB 9000
1060 T_NAME$ = "SWAP scalar num (SW2)" : T_ACT = SW2 : T_EXP = 111 : GOSUB 9000

1070 TXT1$ = "ALPHA" : TXT2$ = "OMEGA"
1080 SWAP TXT1$, TXT2$
1090 T_NAME$ = "SWAP scalar str (TXT1)" : T_ACT$ = TXT1$ : T_EXP$ = "OMEGA" : GOSUB 9100
1100 T_NAME$ = "SWAP scalar str (TXT2)" : T_ACT$ = TXT2$ : T_EXP$ = "ALPHA" : GOSUB 9100

1110 DIM AR_SW(5)
1120 AR_SW(1) = 10 : AR_SW(2) = 20
1130 SWAP AR_SW(1), AR_SW(2)
1140 T_NAME$ = "SWAP array elem AR(1)" : T_ACT = AR_SW(1) : T_EXP = 20 : GOSUB 9000
1150 T_NAME$ = "SWAP array elem AR(2)" : T_ACT = AR_SW(2) : T_EXP = 10 : GOSUB 9000

1160 REM Double SWAP redonne l'etat initial
1170 SWAP SW1, SW2
1180 T_NAME$ = "Double SWAP restores SW1" : T_ACT = SW1 : T_EXP = 111 : GOSUB 9000
1190 T_NAME$ = "Double SWAP restores SW2" : T_ACT = SW2 : T_EXP = 999 : GOSUB 9000

8000 REM ========================================================
8010 REM  SYNTHESE ET RESULTAT
8020 REM ========================================================
8030 PRINT "--------------------------------------------------------"
8040 PRINT "Suite   : "; T_SUITE$
8050 PRINT "Total   : "; T_TOTAL; " assertions"
8060 PRINT "Reussis : "; T_PASS
8070 PRINT "Echecs  : "; T_FAIL
8080 IF T_FAIL = 0 THEN GOTO 8100 ELSE GOTO 8110
8100 PRINT "TEST 02 PASSED" : END
8110 PRINT "TEST 02 FAILED ("; T_FAIL; " echecs)" : ERROR 1
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
