10 REM ========================================================
20 REM  SUITE : TEST 14 - LIGNES MULTI-INSTRUCTIONS ET COMMENTAIRES
30 REM ========================================================
40 T_SUITE$ = "TEST 14 : MULTI-INSTRUCTIONS & COMMENTAIRES"
50 T_TOTAL = 0 : T_PASS = 0 : T_FAIL = 0
60 PRINT "=== "; T_SUITE$; " ==="
70 PRINT

100 REM --------------------------------------------------------
110 REM 1. Enchainement simple avec ':'
120 REM --------------------------------------------------------
130 A = 10 : B = 20 : C = A + B
140 T_NAME$ = "Multi-statement linear sequence (A)" : T_ACT = A : T_EXP = 10 : GOSUB 9000
150 T_NAME$ = "Multi-statement linear sequence (B)" : T_ACT = B : T_EXP = 20 : GOSUB 9000
160 T_NAME$ = "Multi-statement linear sequence (C)" : T_ACT = C : T_EXP = 30 : GOSUB 9000

200 REM --------------------------------------------------------
210 REM 2. Boucle FOR ... NEXT intra-ligne
220 REM --------------------------------------------------------
230 FOR_ACC = 0 : FOR I = 1 TO 5 : FOR_ACC = FOR_ACC + I : NEXT I
240 T_NAME$ = "Intra-line FOR NEXT loop" : T_ACT = FOR_ACC : T_EXP = 15 : GOSUB 9000

250 REM Double boucle FOR imbriquee intra-ligne
260 GRID_CNT = 0 : FOR R = 1 TO 3 : FOR CL = 1 TO 3 : GRID_CNT = GRID_CNT + 1 : NEXT CL : NEXT R
270 T_NAME$ = "Intra-line nested FOR loops (3*3)" : T_ACT = GRID_CNT : T_EXP = 9 : GOSUB 9000

300 REM --------------------------------------------------------
310 REM 3. Boucle WHILE ... WEND intra-ligne
320 REM --------------------------------------------------------
330 W_VAL = 1 : WHILE W_VAL < 16 : W_VAL = W_VAL * 2 : WEND
340 T_NAME$ = "Intra-line WHILE WEND loop" : T_ACT = W_VAL : T_EXP = 16 : GOSUB 9000

400 REM --------------------------------------------------------
410 REM 4. Commentaires en ligne avec apostrophe (')
420 REM --------------------------------------------------------
430 COMM_X = 42 ' Ceci est un commentaire : COMM_Y = 99
440 T_NAME$ = "Assignment before inline comment executed" : T_ACT = COMM_X : T_EXP = 42 : GOSUB 9000
450 T_NAME$ = "Statement inside comment NOT executed"     : T_ACT = COMM_Y : T_EXP = 0  : GOSUB 9000

500 REM --------------------------------------------------------
510 REM 5. IF ... THEN avec instructions multiples apres ':'
520 REM --------------------------------------------------------
530 T1 = 0 : T2 = 0
540 IF 10 > 5 THEN T1 = 100 : T2 = 200
550 T_NAME$ = "IF True executes all subsequent statements (1)" : T_ACT = T1 : T_EXP = 100 : GOSUB 9000
560 T_NAME$ = "IF True executes all subsequent statements (2)" : T_ACT = T2 : T_EXP = 200 : GOSUB 9000

570 F1 = 0 : F2 = 0
580 IF 5 > 10 THEN F1 = 100 : F2 = 200
590 T_NAME$ = "IF False skips all subsequent statements (1)" : T_ACT = F1 : T_EXP = 0 : GOSUB 9000
600 T_NAME$ = "IF False skips all subsequent statements (2)" : T_ACT = F2 : T_EXP = 0 : GOSUB 9000

8000 REM ========================================================
8010 REM  SYNTHESE ET RESULTAT
8020 REM ========================================================
8030 PRINT "--------------------------------------------------------"
8040 PRINT "Suite   : "; T_SUITE$
8050 PRINT "Total   : "; T_TOTAL; " assertions"
8060 PRINT "Reussis : "; T_PASS
8070 PRINT "Echecs  : "; T_FAIL
8080 IF T_FAIL = 0 THEN GOTO 8100 ELSE GOTO 8110
8100 PRINT "TEST 14 PASSED" : END
8110 PRINT "TEST 14 FAILED ("; T_FAIL; " echecs)" : ERROR 1
8120 END

9000 REM === ASSERT NUM EQUAL ===
9010 REM Entrees: T_NAME$, T_ACT, T_EXP
9020 T_TOTAL = T_TOTAL + 1
9030 IF T_ACT = T_EXP THEN T_PASS = T_PASS + 1 : RETURN
9040 T_FAIL = T_FAIL + 1
9050 PRINT "  [FAIL] "; T_NAME$; " -> Attendu: "; T_EXP; " | Obtenu: "; T_ACT
9060 RETURN
