10 REM ========================================================
20 REM  SUITE : TEST 18 - TIMER SYSTEME ET CURSEUR (CSRLIN)
30 REM ========================================================
40 T_SUITE$ = "TEST 18 : TIMER SYSTEME"
50 T_TOTAL = 0 : T_PASS = 0 : T_FAIL = 0
60 PRINT "=== "; T_SUITE$; " ==="
70 PRINT

100 REM --------------------------------------------------------
110 REM 1. Validite de TIMER (secondes depuis minuit >= 0)
120 REM --------------------------------------------------------
130 T1 = TIMER
140 T_NAME$ = "TIMER returns non-negative value (>= 0)" : T_ACT = (T1 >= 0) : T_EXP = -1 : GOSUB 9000

200 REM --------------------------------------------------------
210 REM 2. Monotonicite de TIMER
220 REM --------------------------------------------------------
230 FOR I = 1 TO 20000 : NEXT I
240 T2 = TIMER
250 T_NAME$ = "TIMER is monotonically non-decreasing" : T_ACT = (T2 >= T1) : T_EXP = -1 : GOSUB 9000

300 REM --------------------------------------------------------
310 REM 3. Mesure de duree et calcul dans boucle
320 REM --------------------------------------------------------
330 T_START = TIMER
340 SUM = 0
350 FOR I = 1 TO 5000 : SUM = SUM + I : NEXT I
360 T_END = TIMER
370 ELAPSED = T_END - T_START
380 T_NAME$ = "Sum 1..5000 is 12502500" : T_ACT = SUM : T_EXP = 12502500 : GOSUB 9000
390 T_NAME$ = "Elapsed time is non-negative" : T_ACT = (ELAPSED >= 0) : T_EXP = -1 : GOSUB 9000

400 REM --------------------------------------------------------
410 REM 4. Position du curseur avec CSRLIN
420 REM --------------------------------------------------------
430 ROW = CSRLIN
440 T_NAME$ = "CSRLIN returns valid positive row (>= 1)" : T_ACT = (ROW >= 1) : T_EXP = -1 : GOSUB 9000

8000 REM ========================================================
8010 REM  SYNTHESE ET RESULTAT
8020 REM ========================================================
8030 PRINT "--------------------------------------------------------"
8040 PRINT "Suite   : "; T_SUITE$
8050 PRINT "Total   : "; T_TOTAL; " assertions"
8060 PRINT "Reussis : "; T_PASS
8070 PRINT "Echecs  : "; T_FAIL
8080 IF T_FAIL = 0 THEN GOTO 8100 ELSE GOTO 8110
8100 PRINT "TEST 18 PASSED" : END
8110 PRINT "TEST 18 FAILED ("; T_FAIL; " echecs)" : ERROR 1
8120 END

9000 REM === ASSERT NUM EQUAL ===
9010 REM Entrees: T_NAME$, T_ACT, T_EXP
9020 T_TOTAL = T_TOTAL + 1
9030 IF T_ACT = T_EXP THEN T_PASS = T_PASS + 1 : RETURN
9040 T_FAIL = T_FAIL + 1
9050 PRINT "  [FAIL] "; T_NAME$; " -> Attendu: "; T_EXP; " | Obtenu: "; T_ACT
9060 RETURN
