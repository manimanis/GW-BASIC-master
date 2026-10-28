10 REM ========================================================
20 REM  SUITE : TEST 10 - BRANCHEMENTS CALCULES (ON GOTO / ON GOSUB)
30 REM ========================================================
40 T_SUITE$ = "TEST 10 : ON GOTO / ON GOSUB"
50 T_TOTAL = 0 : T_PASS = 0 : T_FAIL = 0
60 PRINT "=== "; T_SUITE$; " ==="
70 PRINT

100 REM --------------------------------------------------------
110 REM 1. ON ... GOTO (indices valides 1, 2, 3)
120 REM --------------------------------------------------------
130 GOTO_RES = 0
140 IDX = 1 : ON IDX GOTO 1000, 1100, 1200
150 GOTO 1300
1000 GOTO_RES = 10 : GOTO 1300
1100 GOTO_RES = 20 : GOTO 1300
1200 GOTO_RES = 30 : GOTO 1300
1300 T_NAME$ = "ON GOTO: Index 1 branches to line 1000" : T_ACT = GOTO_RES : T_EXP = 10 : GOSUB 9000

1310 IDX = 2 : ON IDX GOTO 2000, 2100, 2200
1320 GOTO 2300
2000 GOTO_RES = 100 : GOTO 2300
2100 GOTO_RES = 200 : GOTO 2300
2200 GOTO_RES = 300 : GOTO 2300
2300 T_NAME$ = "ON GOTO: Index 2 branches to line 2100" : T_ACT = GOTO_RES : T_EXP = 200 : GOSUB 9000

2310 IDX = 3 : ON IDX GOTO 3000, 3100, 3200
2320 GOTO 3300
3000 GOTO_RES = 1000 : GOTO 3300
3100 GOTO_RES = 2000 : GOTO 3300
3200 GOTO_RES = 3000 : GOTO 3300
3300 T_NAME$ = "ON GOTO: Index 3 branches to line 3200" : T_ACT = GOTO_RES : T_EXP = 3000 : GOSUB 9000

3400 REM --------------------------------------------------------
3410 REM 2. ON ... GOTO (indices hors limites: fallthrough)
3420 REM --------------------------------------------------------
3430 FALLTHROUGH_0 = 0
3440 IDX = 0 : ON IDX GOTO 4000, 4100, 4200
3450 FALLTHROUGH_0 = 1
3460 GOTO 4300
4000 FALLTHROUGH_0 = 99 : GOTO 4300
4100 FALLTHROUGH_0 = 99 : GOTO 4300
4200 FALLTHROUGH_0 = 99 : GOTO 4300
4300 T_NAME$ = "ON GOTO: Index 0 falls through" : T_ACT = FALLTHROUGH_0 : T_EXP = 1 : GOSUB 9000

4310 FALLTHROUGH_4 = 0
4320 IDX = 4 : ON IDX GOTO 4400, 4500, 4600
4330 FALLTHROUGH_4 = 1
4340 GOTO 4700
4400 FALLTHROUGH_4 = 99 : GOTO 4700
4500 FALLTHROUGH_4 = 99 : GOTO 4700
4600 FALLTHROUGH_4 = 99 : GOTO 4700
4700 T_NAME$ = "ON GOTO: Index 4 (> targets) falls through" : T_ACT = FALLTHROUGH_4 : T_EXP = 1 : GOSUB 9000

4800 REM --------------------------------------------------------
4810 REM 3. ON ... GOSUB (indices valides 1, 2, 3)
4820 REM --------------------------------------------------------
4830 GOSUB_RES = 0
4840 IDX = 1 : ON IDX GOSUB 5000, 5100, 5200
4850 T_NAME$ = "ON GOSUB: Index 1 returns to caller" : T_ACT = GOSUB_RES : T_EXP = 11 : GOSUB 9000

4860 IDX = 2 : ON IDX GOSUB 5000, 5100, 5200
4870 T_NAME$ = "ON GOSUB: Index 2 returns to caller" : T_ACT = GOSUB_RES : T_EXP = 22 : GOSUB 9000

4880 IDX = 3 : ON IDX GOSUB 5000, 5100, 5200
4890 T_NAME$ = "ON GOSUB: Index 3 returns to caller" : T_ACT = GOSUB_RES : T_EXP = 33 : GOSUB 9000

4900 REM --------------------------------------------------------
4910 REM 4. ON ... GOSUB (indices hors limites: pas d'appel)
4920 REM --------------------------------------------------------
4930 GOSUB_OUT = 0
4940 IDX = 0 : ON IDX GOSUB 5000, 5100, 5200
4950 T_NAME$ = "ON GOSUB: Index 0 does not call subroutine" : T_ACT = GOSUB_OUT : T_EXP = 0 : GOSUB 9000

4960 IDX = 5 : ON IDX GOSUB 5000, 5100, 5200
4970 T_NAME$ = "ON GOSUB: Index 5 does not call subroutine" : T_ACT = GOSUB_OUT : T_EXP = 0 : GOSUB 9000

4980 GOTO 8000

5000 GOSUB_RES = 11 : RETURN
5100 GOSUB_RES = 22 : RETURN
5200 GOSUB_RES = 33 : RETURN

8000 REM ========================================================
8010 REM  SYNTHESE ET RESULTAT
8020 REM ========================================================
8030 PRINT "--------------------------------------------------------"
8040 PRINT "Suite   : "; T_SUITE$
8050 PRINT "Total   : "; T_TOTAL; " assertions"
8060 PRINT "Reussis : "; T_PASS
8070 PRINT "Echecs  : "; T_FAIL
8080 IF T_FAIL = 0 THEN GOTO 8100 ELSE GOTO 8110
8100 PRINT "TEST 10 PASSED" : END
8110 PRINT "TEST 10 FAILED ("; T_FAIL; " echecs)" : ERROR 1
8120 END

9000 REM === ASSERT NUM EQUAL ===
9010 REM Entrees: T_NAME$, T_ACT, T_EXP
9020 T_TOTAL = T_TOTAL + 1
9030 IF T_ACT = T_EXP THEN T_PASS = T_PASS + 1 : RETURN
9040 T_FAIL = T_FAIL + 1
9050 PRINT "  [FAIL] "; T_NAME$; " -> Attendu: "; T_EXP; " | Obtenu: "; T_ACT
9060 RETURN
