10 REM ========================================================
20 REM  SUITE : TEST 01 - AFFICHAGE ET FORMATAGE (PRINT / TAB / SPC)
30 REM ========================================================
40 T_SUITE$ = "TEST 01 : PRINT / TAB / SPC"
50 T_TOTAL = 0 : T_PASS = 0 : T_FAIL = 0
60 PRINT "=== "; T_SUITE$; " ==="
70 PRINT

100 REM --------------------------------------------------------
110 REM 1. Fonctions de positionnement : TAB et SPC
120 REM --------------------------------------------------------
130 TAB10$ = TAB(10)
140 T_NAME$ = "TAB(10) generates 9 leading spaces" : T_ACT = LEN(TAB10$) : T_EXP = 9 : GOSUB 9000

150 TAB20$ = TAB(20)
160 T_NAME$ = "TAB(20) generates 19 leading spaces" : T_ACT = LEN(TAB20$) : T_EXP = 19 : GOSUB 9000

170 SPC5$ = SPC(5)
180 T_NAME$ = "SPC(5) generates 5 spaces" : T_ACT = LEN(SPC5$) : T_EXP = 5 : GOSUB 9000
190 T_NAME$ = "SPC(5) content is all spaces" : T_ACT$ = SPC5$ : T_EXP$ = "     " : GOSUB 9100

200 SPC0$ = SPC(0)
210 T_NAME$ = "SPC(0) generates empty string" : T_ACT = LEN(SPC0$) : T_EXP = 0 : GOSUB 9000

300 REM --------------------------------------------------------
310 REM 2. Ecriture et verification des separateurs (point-virgule et virgule)
320 REM --------------------------------------------------------
330 FNAME$ = "basic_tests\tmp_test01.txt"
340 OPEN FNAME$ FOR OUTPUT AS #1

350 REM Point-virgule sans espace
360 PRINT #1, "A"; "B"; "C"

370 REM Concatenation et expression arithmetique
380 PRINT #1, "RES="; 10 + 20

390 CLOSE #1

400 OPEN FNAME$ FOR INPUT AS #1
410 LINE INPUT #1, L1$
420 T_NAME$ = "Semicolon concatenates strings tightly" : T_ACT$ = L1$ : T_EXP$ = "ABC" : GOSUB 9100

430 LINE INPUT #1, L2$
440 T_NAME$ = "Expression printed with semicolon" : T_ACT$ = L2$ : T_EXP$ = "RES= 30" : GOSUB 9100

450 CLOSE #1

8000 REM ========================================================
8010 REM  SYNTHESE ET RESULTAT
8020 REM ========================================================
8030 PRINT "--------------------------------------------------------"
8040 PRINT "Suite   : "; T_SUITE$
8050 PRINT "Total   : "; T_TOTAL; " assertions"
8060 PRINT "Reussis : "; T_PASS
8070 PRINT "Echecs  : "; T_FAIL
8080 IF T_FAIL = 0 THEN GOTO 8100 ELSE GOTO 8110
8100 PRINT "TEST 01 PASSED" : END
8110 PRINT "TEST 01 FAILED ("; T_FAIL; " echecs)" : ERROR 1
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
