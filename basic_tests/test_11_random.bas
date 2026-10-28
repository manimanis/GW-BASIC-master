10 REM ========================================================
20 REM  SUITE : TEST 11 - GENERATEUR ALEATOIRE (RND / RANDOMIZE)
30 REM ========================================================
40 T_SUITE$ = "TEST 11 : GENERATEUR ALEATOIRE"
50 T_TOTAL = 0 : T_PASS = 0 : T_FAIL = 0
60 PRINT "=== "; T_SUITE$; " ==="
70 PRINT

100 REM --------------------------------------------------------
110 REM 1. Bornes du generateur RND: toujours dans [0, 1[
120 REM --------------------------------------------------------
130 IN_BOUNDS = 1
140 FOR I = 1 TO 50
150   R = RND(1)
160   IF R < 0 OR R >= 1 THEN IN_BOUNDS = 0
170 NEXT I
180 T_NAME$ = "50 consecutive RND calls are in [0, 1[" : T_ACT = IN_BOUNDS : T_EXP = 1 : GOSUB 9000

200 REM --------------------------------------------------------
210 REM 2. Propriete de RND(0) : repete la derniere valeur
220 REM --------------------------------------------------------
230 R_LAST = RND(1)
240 R_REPEAT1 = RND(0)
250 R_REPEAT2 = RND(0)
260 T_NAME$ = "RND(0) returns identical value (1)" : T_ACT = R_REPEAT1 : T_EXP = R_LAST : GOSUB 9000
270 T_NAME$ = "RND(0) returns identical value (2)" : T_ACT = R_REPEAT2 : T_EXP = R_LAST : GOSUB 9000

300 REM --------------------------------------------------------
310 REM 3. Reproductibilite avec RANDOMIZE seed
320 REM --------------------------------------------------------
330 RANDOMIZE 1234
340 S1 = RND(1) : S2 = RND(1) : S3 = RND(1)
350 RANDOMIZE 1234
360 S1_BIS = RND(1) : S2_BIS = RND(1) : S3_BIS = RND(1)
370 T_NAME$ = "Deterministic seed reproduces value 1" : T_ACT = S1_BIS : T_EXP = S1 : GOSUB 9000
380 T_NAME$ = "Deterministic seed reproduces value 2" : T_ACT = S2_BIS : T_EXP = S2 : GOSUB 9000
390 T_NAME$ = "Deterministic seed reproduces value 3" : T_ACT = S3_BIS : T_EXP = S3 : GOSUB 9000

400 REM --------------------------------------------------------
410 REM 4. Simulation de tirage de des : [1, 6]
420 REM --------------------------------------------------------
430 ALL_IN_RANGE = 1
440 FOR I = 1 TO 200
450   DICE = INT(RND(1) * 6) + 1
460   IF DICE < 1 OR DICE > 6 THEN ALL_IN_RANGE = 0
470 NEXT I
480 T_NAME$ = "200 dice rolls strictly in [1, 6]" : T_ACT = ALL_IN_RANGE : T_EXP = 1 : GOSUB 9000

8000 REM ========================================================
8010 REM  SYNTHESE ET RESULTAT
8020 REM ========================================================
8030 PRINT "--------------------------------------------------------"
8040 PRINT "Suite   : "; T_SUITE$
8050 PRINT "Total   : "; T_TOTAL; " assertions"
8060 PRINT "Reussis : "; T_PASS
8070 PRINT "Echecs  : "; T_FAIL
8080 IF T_FAIL = 0 THEN GOTO 8100 ELSE GOTO 8110
8100 PRINT "TEST 11 PASSED" : END
8110 PRINT "TEST 11 FAILED ("; T_FAIL; " echecs)" : ERROR 1
8120 END

9000 REM === ASSERT NUM EQUAL ===
9010 REM Entrees: T_NAME$, T_ACT, T_EXP
9020 T_TOTAL = T_TOTAL + 1
9030 IF T_ACT = T_EXP THEN T_PASS = T_PASS + 1 : RETURN
9040 T_FAIL = T_FAIL + 1
9050 PRINT "  [FAIL] "; T_NAME$; " -> Attendu: "; T_EXP; " | Obtenu: "; T_ACT
9060 RETURN
