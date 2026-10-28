10 REM ========================================================
20 REM  SUITE : TEST 15 - ALGORITHMES D'INTEGRATION COMPLETS
30 REM ========================================================
40 T_SUITE$ = "TEST 15 : ALGORITHMES D'INTEGRATION"
50 T_TOTAL = 0 : T_PASS = 0 : T_FAIL = 0
60 PRINT "=== "; T_SUITE$; " ==="
70 PRINT

100 REM --------------------------------------------------------
110 REM 1. Suite de Fibonacci (15 premiers termes)
120 REM --------------------------------------------------------
130 A = 0 : B = 1 : FIB_SUM = A + B
140 FOR I = 1 TO 13
150   C = A + B
160   FIB_SUM = FIB_SUM + C
170   A = B : B = C
180 NEXT I
190 T_NAME$ = "Fibonacci 15th term" : T_ACT = C : T_EXP = 377 : GOSUB 9000
200 T_NAME$ = "Fibonacci sum of 15 terms" : T_ACT = FIB_SUM : T_EXP = 986 : GOSUB 9000

300 REM --------------------------------------------------------
310 REM 2. Tri a bulles (Bubble Sort) avec DATA
320 REM --------------------------------------------------------
330 N = 8
340 DIM DAT(8)
350 FOR I = 1 TO N : READ DAT(I) : NEXT I
360 DATA 64, 34, 25, 12, 22, 11, 90, 3

370 FOR I = 1 TO N - 1
380   FOR J = 1 TO N - I
390     IF DAT(J) > DAT(J + 1) THEN SWAP DAT(J), DAT(J + 1)
400   NEXT J
410 NEXT I

420 DIM EXP_DAT(8)
430 EXP_DAT(1)=3: EXP_DAT(2)=11: EXP_DAT(3)=12: EXP_DAT(4)=22
440 EXP_DAT(5)=25: EXP_DAT(6)=34: EXP_DAT(7)=64: EXP_DAT(8)=90

450 SORT_OK = 1
460 FOR I = 1 TO 8
470   IF DAT(I) <> EXP_DAT(I) THEN SORT_OK = 0
480 NEXT I
490 T_NAME$ = "Bubble Sort: all 8 elements sorted in ascending order" : T_ACT = SORT_OK : T_EXP = 1 : GOSUB 9000

500 REM --------------------------------------------------------
510 REM 3. Crible d'Eratosthene (nombres premiers <= 50)
520 REM --------------------------------------------------------
530 DIM PRIME(50)
540 FOR I = 2 TO 50 : PRIME(I) = 1 : NEXT I
550 FOR I = 2 TO 7
560   IF PRIME(I) = 0 THEN GOTO 600
570   FOR J = I * I TO 50 STEP I
580     PRIME(J) = 0
590   NEXT J
600 NEXT I

610 P_CNT = 0 : P_SUM = 0
620 FOR I = 2 TO 50
630   IF PRIME(I) = 1 THEN P_CNT = P_CNT + 1 : P_SUM = P_SUM + I
640 NEXT I
650 T_NAME$ = "Primes <= 50: count (15 primes)" : T_ACT = P_CNT : T_EXP = 15 : GOSUB 9000
660 T_NAME$ = "Primes <= 50: sum of primes"    : T_ACT = P_SUM : T_EXP = 328 : GOSUB 9000
670 T_NAME$ = "Prime 2 is prime"                : T_ACT = PRIME(2) : T_EXP = 1 : GOSUB 9000
680 T_NAME$ = "Prime 47 is prime"               : T_ACT = PRIME(47) : T_EXP = 1 : GOSUB 9000
690 T_NAME$ = "Composite 49 is not prime"       : T_ACT = PRIME(49) : T_EXP = 0 : GOSUB 9000

700 REM --------------------------------------------------------
710 REM 4. Factorielles recursives (GOSUB) 0! a 10!
720 REM --------------------------------------------------------
730 N = 0 : GOSUB 7000 : T_NAME$ = "Factorial 0! = 1" : T_ACT = F_OUT : T_EXP = 1 : GOSUB 9000
740 N = 1 : GOSUB 7000 : T_NAME$ = "Factorial 1! = 1" : T_ACT = F_OUT : T_EXP = 1 : GOSUB 9000
750 N = 2 : GOSUB 7000 : T_NAME$ = "Factorial 2! = 2" : T_ACT = F_OUT : T_EXP = 2 : GOSUB 9000
760 N = 5 : GOSUB 7000 : T_NAME$ = "Factorial 5! = 120" : T_ACT = F_OUT : T_EXP = 120 : GOSUB 9000
770 N = 10 : GOSUB 7000 : T_NAME$ = "Factorial 10! = 3628800" : T_ACT = F_OUT : T_EXP = 3628800 : GOSUB 9000

800 REM --------------------------------------------------------
810 REM 5. Recherche binaire (Binary Search)
820 REM --------------------------------------------------------
830 TARGET = 25 : GOSUB 7500
840 T_NAME$ = "Binary Search: find 25 -> index 5" : T_ACT = FOUND_IDX : T_EXP = 5 : GOSUB 9000

850 TARGET = 99 : GOSUB 7500
860 T_NAME$ = "Binary Search: 99 not found -> index 0" : T_ACT = FOUND_IDX : T_EXP = 0 : GOSUB 9000

900 REM --------------------------------------------------------
910 REM 6. Inversion de chaine (String Reversal)
920 REM --------------------------------------------------------
930 SRC$ = "GW-BASIC"
940 REV$ = ""
950 FOR I = LEN(SRC$) TO 1 STEP -1
960   REV$ = REV$ + MID$(SRC$, I, 1)
970 NEXT I
980 T_NAME$ = "String Reversal of 'GW-BASIC'" : T_ACT$ = REV$ : T_EXP$ = "CISAB-WG" : GOSUB 9100

990 GOTO 8000

7000 REM Calcul factorielle
7010 IF N <= 1 THEN F_OUT = 1 : RETURN
7020 F_OUT = 1
7030 FOR K = 2 TO N : F_OUT = F_OUT * K : NEXT K
7040 RETURN

7500 REM Recherche binaire dans DAT(1..8)
7510 LOW = 1 : HIGH = 8 : FOUND_IDX = 0
7520 WHILE LOW <= HIGH
7530   MID_PT = (LOW + HIGH) \ 2
7540   IF DAT(MID_PT) = TARGET THEN FOUND_IDX = MID_PT : LOW = HIGH + 1
7550   IF DAT(MID_PT) < TARGET THEN LOW = MID_PT + 1
7560   IF DAT(MID_PT) > TARGET THEN HIGH = MID_PT - 1
7570 WEND
7580 RETURN

8000 REM ========================================================
8010 REM  SYNTHESE ET RESULTAT
8020 REM ========================================================
8030 PRINT "--------------------------------------------------------"
8040 PRINT "Suite   : "; T_SUITE$
8050 PRINT "Total   : "; T_TOTAL; " assertions"
8060 PRINT "Reussis : "; T_PASS
8070 PRINT "Echecs  : "; T_FAIL
8080 IF T_FAIL = 0 THEN GOTO 8100 ELSE GOTO 8110
8100 PRINT "TEST 15 PASSED" : END
8110 PRINT "TEST 15 FAILED ("; T_FAIL; " echecs)" : ERROR 1
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
