10 REM ========================================================
20 REM  SUITE : TEST 13 - FORMATAGE PRINT USING
30 REM ========================================================
40 T_SUITE$ = "TEST 13 : PRINT USING"
50 T_TOTAL = 0 : T_PASS = 0 : T_FAIL = 0
60 PRINT "=== "; T_SUITE$; " ==="
70 PRINT

100 REM --------------------------------------------------------
110 REM 1. Formatage dans un fichier pour relecture et assertion
120 REM --------------------------------------------------------
130 FNAME$ = "basic_tests\tmp_test13.txt"
140 OPEN FNAME$ FOR OUTPUT AS #1

150 REM Nombres a decimales fixes
160 PRINT #1, USING "###.##"; 3.14159
170 PRINT #1, USING "###.##"; -99.9

180 REM Signe force
190 PRINT #1, USING "+###.##"; 42.5
200 PRINT #1, USING "+###.##"; -42.5

210 REM Separateur de milliers
220 PRINT #1, USING "#,###,###"; 1234567

230 REM Formatage de chaines avec backslash
240 PRINT #1, USING "\     \"; "HELLO"
250 PRINT #1, USING "\  \"; "ABCDEF"

260 REM Formatage de chaine avec ampersand (&)
270 PRINT #1, USING "Lang: &"; "GW-BASIC"

280 CLOSE #1

300 REM --------------------------------------------------------
310 REM 2. Relecture et assertions rigoureuses
320 REM --------------------------------------------------------
330 OPEN FNAME$ FOR INPUT AS #1

340 LINE INPUT #1, RES1$
350 T_NAME$ = "PRINT USING '###.##' on 3.14159" : T_ACT$ = RES1$ : T_EXP$ = "  3.14" : GOSUB 9100

360 LINE INPUT #1, RES2$
370 T_NAME$ = "PRINT USING '###.##' on -99.9" : T_ACT$ = RES2$ : T_EXP$ = " -99.90" : GOSUB 9100

380 LINE INPUT #1, RES3$
390 T_NAME$ = "PRINT USING '+###.##' on +42.5" : T_ACT$ = RES3$ : T_EXP$ = " +42.50" : GOSUB 9100

400 LINE INPUT #1, RES4$
410 T_NAME$ = "PRINT USING '+###.##' on -42.5" : T_ACT$ = RES4$ : T_EXP$ = " -42.50" : GOSUB 9100

420 LINE INPUT #1, RES5$
430 T_NAME$ = "PRINT USING '#,###,###' on 1234567" : T_ACT$ = RES5$ : T_EXP$ = "1,234,567" : GOSUB 9100

440 LINE INPUT #1, RES6$
450 T_NAME$ = "PRINT USING '\     \' on 'HELLO'" : T_ACT$ = RES6$ : T_EXP$ = "HELLO  " : GOSUB 9100

460 LINE INPUT #1, RES7$
470 T_NAME$ = "PRINT USING '\  \' truncated on 'ABCDEF'" : T_ACT$ = RES7$ : T_EXP$ = "ABCD" : GOSUB 9100

480 LINE INPUT #1, RES8$
490 T_NAME$ = "PRINT USING 'Lang: &' on 'GW-BASIC'" : T_ACT$ = RES8$ : T_EXP$ = "Lang: GW-BASIC" : GOSUB 9100

500 CLOSE #1

8000 REM ========================================================
8010 REM  SYNTHESE ET RESULTAT
8020 REM ========================================================
8030 PRINT "--------------------------------------------------------"
8040 PRINT "Suite   : "; T_SUITE$
8050 PRINT "Total   : "; T_TOTAL; " assertions"
8060 PRINT "Reussis : "; T_PASS
8070 PRINT "Echecs  : "; T_FAIL
8080 IF T_FAIL = 0 THEN GOTO 8100 ELSE GOTO 8110
8100 PRINT "TEST 13 PASSED" : END
8110 PRINT "TEST 13 FAILED ("; T_FAIL; " echecs)" : ERROR 1
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
