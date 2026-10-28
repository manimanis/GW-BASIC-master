10 REM ========================================================
20 REM  SUITE : TEST 09 - GESTION DES FICHIERS (FILE I/O)
30 REM ========================================================
40 T_SUITE$ = "TEST 09 : FICHIERS & ENTREES/SORTIES"
50 T_TOTAL = 0 : T_PASS = 0 : T_FAIL = 0
60 PRINT "=== "; T_SUITE$; " ==="
70 PRINT

100 REM --------------------------------------------------------
110 REM 1. Creation et ecriture sequentielle (OUTPUT)
120 REM --------------------------------------------------------
130 FNAME$ = "basic_tests\tmp_test09.txt"
140 OPEN FNAME$ FOR OUTPUT AS #1
150 PRINT #1, "123,HELLO,987.65"
160 CLOSE #1

200 REM --------------------------------------------------------
210 REM 2. Relecture avec INPUT #
220 REM --------------------------------------------------------
230 OPEN FNAME$ FOR INPUT AS #1
240 INPUT #1, IN_N1, IN_S$, IN_N2
250 CLOSE #1

260 T_NAME$ = "Read numeric 1 from file" : T_ACT = IN_N1 : T_EXP = 123 : GOSUB 9000
270 T_NAME$ = "Read string from file"    : T_ACT$ = IN_S$ : T_EXP$ = "HELLO" : GOSUB 9100
280 T_NAME$ = "Read numeric 2 from file" : T_ACT = IN_N2 : T_EXP = 987.65 : T_EPS = 0.01 : GOSUB 9200

300 REM --------------------------------------------------------
310 REM 3. Ajout en fin de fichier (APPEND)
320 REM --------------------------------------------------------
330 OPEN FNAME$ FOR APPEND AS #1
340 PRINT #1, "SECOND LINE CONTENT"
350 CLOSE #1

400 REM --------------------------------------------------------
410 REM 4. Relecture ligne par ligne avec LINE INPUT # et verification de EOF
420 REM --------------------------------------------------------
430 OPEN FNAME$ FOR INPUT AS #1
440 T_NAME$ = "EOF before reading is false" : T_ACT = EOF(1) : T_EXP = 0 : GOSUB 9000

450 LINE INPUT #1, LINE1$
460 T_NAME$ = "LINE INPUT line 1" : T_ACT$ = LINE1$ : T_EXP$ = "123,HELLO,987.65" : GOSUB 9100
470 T_NAME$ = "EOF after line 1 is false" : T_ACT = EOF(1) : T_EXP = 0 : GOSUB 9000

480 LINE INPUT #1, LINE2$
490 T_NAME$ = "LINE INPUT line 2" : T_ACT$ = LINE2$ : T_EXP$ = "SECOND LINE CONTENT" : GOSUB 9100
500 T_NAME$ = "EOF at end of file is true" : T_ACT = EOF(1) : T_EXP = -1 : GOSUB 9000

510 CLOSE #1

8000 REM ========================================================
8010 REM  SYNTHESE ET RESULTAT
8020 REM ========================================================
8030 PRINT "--------------------------------------------------------"
8040 PRINT "Suite   : "; T_SUITE$
8050 PRINT "Total   : "; T_TOTAL; " assertions"
8060 PRINT "Reussis : "; T_PASS
8070 PRINT "Echecs  : "; T_FAIL
8080 IF T_FAIL = 0 THEN GOTO 8100 ELSE GOTO 8110
8100 PRINT "TEST 09 PASSED" : END
8110 PRINT "TEST 09 FAILED ("; T_FAIL; " echecs)" : ERROR 1
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
