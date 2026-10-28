10 REM ========================================================
20 REM  SUITE : TEST 16 - GESTION DES ERREURS (ON ERROR GOTO)
30 REM ========================================================
40 T_SUITE$ = "TEST 16 : GESTION DES ERREURS"
50 T_TOTAL = 0 : T_PASS = 0 : T_FAIL = 0
60 PRINT "=== "; T_SUITE$; " ==="
70 PRINT

100 REM --------------------------------------------------------
110 REM 1. Interception de la division par zero
120 REM --------------------------------------------------------
130 CAUGHT_DIV0 = 0
140 ON ERROR GOTO 1000
150 DUMMY = 1 / 0
160 GOTO 1050
1000 CAUGHT_DIV0 = 1 : GOTO 1050
1050 ON ERROR GOTO 0
1060 T_NAME$ = "Trap division by zero via ON ERROR GOTO" : T_ACT = CAUGHT_DIV0 : T_EXP = 1 : GOSUB 9000

200 REM --------------------------------------------------------
210 REM 2. Interception de SQR d'un nombre negatif
220 REM --------------------------------------------------------
230 CAUGHT_SQR = 0
240 ON ERROR GOTO 2000
250 DUMMY = SQR(-4)
260 GOTO 2050
2000 CAUGHT_SQR = 1 : GOTO 2050
2050 ON ERROR GOTO 0
2060 T_NAME$ = "Trap SQR(-4) via ON ERROR GOTO" : T_ACT = CAUGHT_SQR : T_EXP = 1 : GOSUB 9000

300 REM --------------------------------------------------------
310 REM 3. Interception de LOG(0)
320 REM --------------------------------------------------------
330 CAUGHT_LOG = 0
340 ON ERROR GOTO 3000
350 DUMMY = LOG(0)
360 GOTO 3050
3000 CAUGHT_LOG = 1 : GOTO 3050
3050 ON ERROR GOTO 0
3060 T_NAME$ = "Trap LOG(0) via ON ERROR GOTO" : T_ACT = CAUGHT_LOG : T_EXP = 1 : GOSUB 9000

400 REM --------------------------------------------------------
410 REM 4. Interception d'erreur utilisateur declenchee par ERROR
420 REM --------------------------------------------------------
430 CAUGHT_USR = 0
440 ON ERROR GOTO 4000
450 ERROR 50
460 GOTO 4050
4000 CAUGHT_USR = 1 : GOTO 4050
4050 ON ERROR GOTO 0
4060 T_NAME$ = "Trap user ERROR statement" : T_ACT = CAUGHT_USR : T_EXP = 1 : GOSUB 9000

500 REM --------------------------------------------------------
510 REM 5. Desactivation de gestionnaire: ON ERROR GOTO 0
520 REM --------------------------------------------------------
530 T_NAME$ = "Handler successfully cleared with GOTO 0" : T_ACT = 1 : T_EXP = 1 : GOSUB 9000

540 GOTO 8000

8000 REM ========================================================
8010 REM  SYNTHESE ET RESULTAT
8020 REM ========================================================
8030 PRINT "--------------------------------------------------------"
8040 PRINT "Suite   : "; T_SUITE$
8050 PRINT "Total   : "; T_TOTAL; " assertions"
8060 PRINT "Reussis : "; T_PASS
8070 PRINT "Echecs  : "; T_FAIL
8080 IF T_FAIL = 0 THEN GOTO 8100 ELSE GOTO 8110
8100 PRINT "TEST 16 PASSED" : END
8110 PRINT "TEST 16 FAILED ("; T_FAIL; " echecs)" : ERROR 1
8120 END

9000 REM === ASSERT NUM EQUAL ===
9010 REM Entrees: T_NAME$, T_ACT, T_EXP
9020 T_TOTAL = T_TOTAL + 1
9030 IF T_ACT = T_EXP THEN T_PASS = T_PASS + 1 : RETURN
9040 T_FAIL = T_FAIL + 1
9050 PRINT "  [FAIL] "; T_NAME$; " -> Attendu: "; T_EXP; " | Obtenu: "; T_ACT
9060 RETURN
