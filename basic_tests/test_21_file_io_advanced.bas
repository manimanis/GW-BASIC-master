10 REM ========================================================
20 REM  SUITE : TEST 21 - FICHIERS AVANCES (LOF, LOC, WRITE #, APPEND)
30 REM ========================================================
40 T_SUITE$ = "TEST 21 : FICHIERS AVANCES"
50 T_TOTAL = 0 : T_PASS = 0 : T_FAIL = 0
60 PRINT "=== "; T_SUITE$; " ==="
70 PRINT

100 REM --------------------------------------------------------
110 REM 1. Ecriture et relecture avec WRITE # (format CSV interne)
120 REM --------------------------------------------------------
130 FNAME$ = "basic_tests\tmp_test21.txt"
140 OPEN FNAME$ FOR OUTPUT AS #1
150 REM WRITE # ajoute des guillemets autour des chaines et separe par virgule
160 PRINT #1, 42
170 PRINT #1, "HELLO WORLD"
180 PRINT #1, 3.14
190 CLOSE #1

200 REM Relecture ligne par ligne
210 OPEN FNAME$ FOR INPUT AS #1
220 LINE INPUT #1, R1$
230 LINE INPUT #1, R2$
240 LINE INPUT #1, R3$
250 CLOSE #1

260 T_NAME$ = "PRINT to file: integer line" : T_ACT$ = R1$ : T_EXP$ = " 42" : GOSUB 9100
270 T_NAME$ = "PRINT to file: string line"  : T_ACT$ = R2$ : T_EXP$ = "HELLO WORLD" : GOSUB 9100
280 T_NAME$ = "PRINT to file: float line"   : T_ACT = VAL(R3$) : T_EXP = 3.14 : T_EPS = 0.01 : GOSUB 9200

300 REM --------------------------------------------------------
310 REM 2. LOF() - Taille du fichier en octets (doit etre > 0)
320 REM --------------------------------------------------------
330 OPEN FNAME$ FOR INPUT AS #2
340 FILE_SIZE = LOF(2)
350 CLOSE #2
360 T_NAME$ = "LOF() > 0 after writing 3 lines" : T_ACT = (FILE_SIZE > 0) : T_EXP = -1 : GOSUB 9000

400 REM --------------------------------------------------------
410 REM 3. EOF() pendant lecture sequentielle
420 REM --------------------------------------------------------
430 OPEN FNAME$ FOR INPUT AS #1
440 T_NAME$ = "EOF false at start of file" : T_ACT = EOF(1) : T_EXP = 0 : GOSUB 9000

450 LINE INPUT #1, DUMMY$
460 T_NAME$ = "EOF false after 1st line"   : T_ACT = EOF(1) : T_EXP = 0 : GOSUB 9000

470 LINE INPUT #1, DUMMY$
480 T_NAME$ = "EOF false after 2nd line"   : T_ACT = EOF(1) : T_EXP = 0 : GOSUB 9000

490 LINE INPUT #1, DUMMY$
500 T_NAME$ = "EOF true after last line"   : T_ACT = EOF(1) : T_EXP = -1 : GOSUB 9000
510 CLOSE #1

600 REM --------------------------------------------------------
610 REM 4. APPEND - Ajout en fin de fichier sans ecraser
620 REM --------------------------------------------------------
630 REM D'abord ecrire un fichier neuf
640 FNAME2$ = "basic_tests\tmp_test21b.txt"
650 OPEN FNAME2$ FOR OUTPUT AS #1
660 PRINT #1, "LINE1"
670 CLOSE #1

680 REM Ajouter une ligne
690 OPEN FNAME2$ FOR APPEND AS #1
700 PRINT #1, "LINE2"
710 CLOSE #1

720 REM Verifier les deux lignes
730 OPEN FNAME2$ FOR INPUT AS #1
740 LINE INPUT #1, AL1$
750 LINE INPUT #1, AL2$
760 CLOSE #1

770 T_NAME$ = "APPEND keeps first line"  : T_ACT$ = AL1$ : T_EXP$ = "LINE1" : GOSUB 9100
780 T_NAME$ = "APPEND adds second line"  : T_ACT$ = AL2$ : T_EXP$ = "LINE2" : GOSUB 9100

800 REM --------------------------------------------------------
810 REM 5. INPUT # multi-types depuis fichier
820 REM --------------------------------------------------------
830 FNAME3$ = "basic_tests\tmp_test21c.txt"
840 OPEN FNAME3$ FOR OUTPUT AS #1
850 PRINT #1, "999,-3.14,GWBASIC"
860 CLOSE #1

870 OPEN FNAME3$ FOR INPUT AS #1
880 INPUT #1, INUM, IFLT, ISTR$
890 CLOSE #1

900 T_NAME$ = "INPUT # integer"     : T_ACT = INUM  : T_EXP = 999   : GOSUB 9000
910 T_NAME$ = "INPUT # float"       : T_ACT = IFLT  : T_EXP = -3.14 : T_EPS = 0.001 : GOSUB 9200
920 T_NAME$ = "INPUT # string"      : T_ACT$ = ISTR$ : T_EXP$ = "GWBASIC" : GOSUB 9100

1000 REM --------------------------------------------------------
1010 REM 6. Plusieurs fichiers simultanes (#1 et #2)
1020 REM --------------------------------------------------------
1030 FOUT1$ = "basic_tests\tmp_test21d1.txt"
1040 FOUT2$ = "basic_tests\tmp_test21d2.txt"
1050 OPEN FOUT1$ FOR OUTPUT AS #1
1060 OPEN FOUT2$ FOR OUTPUT AS #2
1070 PRINT #1, "FILE_ONE"
1080 PRINT #2, "FILE_TWO"
1090 CLOSE #1
1100 CLOSE #2

1110 OPEN FOUT1$ FOR INPUT AS #1
1120 OPEN FOUT2$ FOR INPUT AS #2
1130 LINE INPUT #1, MF1$
1140 LINE INPUT #2, MF2$
1150 CLOSE #1
1160 CLOSE #2

1170 T_NAME$ = "Multi-file #1 reads correctly" : T_ACT$ = MF1$ : T_EXP$ = "FILE_ONE" : GOSUB 9100
1180 T_NAME$ = "Multi-file #2 reads correctly" : T_ACT$ = MF2$ : T_EXP$ = "FILE_TWO" : GOSUB 9100

8000 REM ========================================================
8010 REM  SYNTHESE ET RESULTAT
8020 REM ========================================================
8030 PRINT "--------------------------------------------------------"
8040 PRINT "Suite   : "; T_SUITE$
8050 PRINT "Total   : "; T_TOTAL; " assertions"
8060 PRINT "Reussis : "; T_PASS
8070 PRINT "Echecs  : "; T_FAIL
8080 IF T_FAIL = 0 THEN GOTO 8100 ELSE GOTO 8110
8100 PRINT "TEST 21 PASSED" : END
8110 PRINT "TEST 21 FAILED ("; T_FAIL; " echecs)" : ERROR 1
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
