10 REM ========================================================
20 REM  SUITE : TEST 06 - DONNEES EN LIGNE (DATA / READ / RESTORE)
30 REM ========================================================
40 T_SUITE$ = "TEST 06 : DATA / READ / RESTORE"
50 T_TOTAL = 0 : T_PASS = 0 : T_FAIL = 0
60 PRINT "=== "; T_SUITE$; " ==="
70 PRINT

100 REM --------------------------------------------------------
110 REM 1. Lecture sequentielle de types basiques (entiers, flottants)
120 REM --------------------------------------------------------
130 READ D_INT1, D_INT2, D_FLT1, D_FLT2
140 T_NAME$ = "READ Int 1"   : T_ACT = D_INT1 : T_EXP = 100   : GOSUB 9000
150 T_NAME$ = "READ Int 2"   : T_ACT = D_INT2 : T_EXP = -250  : GOSUB 9000
160 T_NAME$ = "READ Float 1" : T_ACT = D_FLT1 : T_EXP = 3.14159 : T_EPS = 0.0001 : GOSUB 9200
170 T_NAME$ = "READ Float 2" : T_ACT = D_FLT2 : T_EXP = -0.005   : T_EPS = 0.0001 : GOSUB 9200
180 DATA 100, -250, 3.14159, -0.005

200 REM --------------------------------------------------------
210 REM 2. Chaines entre guillemets avec virgules et espaces
220 REM --------------------------------------------------------
230 READ STR_Q1$, STR_Q2$, STR_PLAIN$
240 T_NAME$ = "READ quoted string with comma" : T_ACT$ = STR_Q1$ : T_EXP$ = "Paris, France" : GOSUB 9100
250 T_NAME$ = "READ quoted string with space" : T_ACT$ = STR_Q2$ : T_EXP$ = "Tokyo, Japan"   : GOSUB 9100
260 T_NAME$ = "READ plain string"             : T_ACT$ = STR_PLAIN$ : T_EXP$ = "London"      : GOSUB 9100
270 DATA "Paris, France", "Tokyo, Japan", London

300 REM --------------------------------------------------------
310 REM 3. Lecture dans un tableau et somme
320 REM --------------------------------------------------------
330 DIM TAB_VAL(6)
340 SUM_READ = 0
350 FOR I = 1 TO 6
360   READ TAB_VAL(I)
370   SUM_READ = SUM_READ + TAB_VAL(I)
380 NEXT I
390 T_NAME$ = "READ into array element TAB_VAL(1)" : T_ACT = TAB_VAL(1) : T_EXP = 10 : GOSUB 9000
400 T_NAME$ = "READ into array element TAB_VAL(6)" : T_ACT = TAB_VAL(6) : T_EXP = 60 : GOSUB 9000
410 T_NAME$ = "READ array total sum (10..60)"      : T_ACT = SUM_READ   : T_EXP = 210 : GOSUB 9000
420 DATA 10, 20, 30, 40, 50, 60

500 REM --------------------------------------------------------
510 REM 4. RESTORE global (retour au premier DATA du programme)
520 REM --------------------------------------------------------
530 RESTORE
540 READ REREAD_1, REREAD_2
550 T_NAME$ = "RESTORE rewinds to first DATA (val 1)" : T_ACT = REREAD_1 : T_EXP = 100  : GOSUB 9000
560 T_NAME$ = "RESTORE rewinds to first DATA (val 2)" : T_ACT = REREAD_2 : T_EXP = -250 : GOSUB 9000

600 REM --------------------------------------------------------
610 REM 5. RESTORE cible vers un numero de ligne specifique
620 REM --------------------------------------------------------
630 RESTORE 750
640 READ REC_ID%, REC_NAME$, REC_VAL
650 T_NAME$ = "RESTORE line 750 (ID)"   : T_ACT = REC_ID%     : T_EXP = 42      : GOSUB 9000
660 T_NAME$ = "RESTORE line 750 (Name)" : T_ACT$ = REC_NAME$  : T_EXP$ = "Alpha": GOSUB 9100
670 T_NAME$ = "RESTORE line 750 (Val)"  : T_ACT = REC_VAL     : T_EXP = 999.5   : T_EPS = 0.01 : GOSUB 9200

680 READ REC2_ID%, REC2_NAME$, REC2_VAL
690 T_NAME$ = "Next record (ID)"   : T_ACT = REC2_ID%    : T_EXP = 43     : GOSUB 9000
700 T_NAME$ = "Next record (Name)" : T_ACT$ = REC2_NAME$ : T_EXP$ = "Beta": GOSUB 9100
710 T_NAME$ = "Next record (Val)"  : T_ACT = REC2_VAL    : T_EXP = 888.5  : T_EPS = 0.01 : GOSUB 9200

740 GOTO 8000
750 DATA 42, "Alpha", 999.5
760 DATA 43, "Beta", 888.5

8000 REM ========================================================
8010 REM  SYNTHESE ET RESULTAT
8020 REM ========================================================
8030 PRINT "--------------------------------------------------------"
8040 PRINT "Suite   : "; T_SUITE$
8050 PRINT "Total   : "; T_TOTAL; " assertions"
8060 PRINT "Reussis : "; T_PASS
8070 PRINT "Echecs  : "; T_FAIL
8080 IF T_FAIL = 0 THEN GOTO 8100 ELSE GOTO 8110
8100 PRINT "TEST 06 PASSED" : END
8110 PRINT "TEST 06 FAILED ("; T_FAIL; " echecs)" : ERROR 1
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
