10 REM ========================================================
20 REM  SUITE : TEST 17 - LOGIQUE BOOLEENNE ET OPERATEURS BITWISE
30 REM ========================================================
40 T_SUITE$ = "TEST 17 : LOGIQUE & BITWISE"
50 T_TOTAL = 0 : T_PASS = 0 : T_FAIL = 0
60 PRINT "=== "; T_SUITE$; " ==="
70 PRINT

100 REM --------------------------------------------------------
110 REM 1. Tables de verite booleennes completes (True=-1, False=0)
120 REM --------------------------------------------------------
130 T_NAME$ = "AND: True AND True"   : T_ACT = (-1 AND -1) : T_EXP = -1 : GOSUB 9000
140 T_NAME$ = "AND: True AND False"  : T_ACT = (-1 AND 0)  : T_EXP = 0  : GOSUB 9000
150 T_NAME$ = "AND: False AND True"  : T_ACT = (0 AND -1)  : T_EXP = 0  : GOSUB 9000
160 T_NAME$ = "AND: False AND False" : T_ACT = (0 AND 0)   : T_EXP = 0  : GOSUB 9000

170 T_NAME$ = "OR: True OR True"     : T_ACT = (-1 OR -1)  : T_EXP = -1 : GOSUB 9000
180 T_NAME$ = "OR: True OR False"    : T_ACT = (-1 OR 0)   : T_EXP = -1 : GOSUB 9000
190 T_NAME$ = "OR: False OR True"    : T_ACT = (0 OR -1)   : T_EXP = -1 : GOSUB 9000
200 T_NAME$ = "OR: False OR False"   : T_ACT = (0 OR 0)    : T_EXP = 0  : GOSUB 9000

210 T_NAME$ = "XOR: True XOR True"   : T_ACT = (-1 XOR -1) : T_EXP = 0  : GOSUB 9000
220 T_NAME$ = "XOR: True XOR False"  : T_ACT = (-1 XOR 0)  : T_EXP = -1 : GOSUB 9000
230 T_NAME$ = "XOR: False XOR True"  : T_ACT = (0 XOR -1)  : T_EXP = -1 : GOSUB 9000
240 T_NAME$ = "XOR: False XOR False" : T_ACT = (0 XOR 0)   : T_EXP = 0  : GOSUB 9000

250 T_NAME$ = "NOT: NOT False"       : T_ACT = (NOT 0)     : T_EXP = -1 : GOSUB 9000
260 T_NAME$ = "NOT: NOT True"        : T_ACT = (NOT -1)    : T_EXP = 0  : GOSUB 9000

300 REM --------------------------------------------------------
310 REM 2. Operations bitwise sur entiers
320 REM --------------------------------------------------------
330 A% = 12 : B% = 10
340 T_NAME$ = "Bitwise: 12 AND 10"   : T_ACT = (A% AND B%) : T_EXP = 8  : GOSUB 9000
350 T_NAME$ = "Bitwise: 12 OR 10"    : T_ACT = (A% OR B%)  : T_EXP = 14 : GOSUB 9000
360 T_NAME$ = "Bitwise: 12 XOR 10"   : T_ACT = (A% XOR B%) : T_EXP = 6  : GOSUB 9000
370 T_NAME$ = "Bitwise: NOT 12"      : T_ACT = (NOT A%)    : T_EXP = -13 : GOSUB 9000
380 T_NAME$ = "Bitwise: NOT 0"       : T_ACT = (NOT 0%)    : T_EXP = -1 : GOSUB 9000
390 T_NAME$ = "Bitwise: 255 AND 15"  : T_ACT = (255 AND 15) : T_EXP = 15 : GOSUB 9000
400 T_NAME$ = "Bitwise: 240 OR 15"   : T_ACT = (240 OR 15) : T_EXP = 255 : GOSUB 9000
410 T_NAME$ = "Bitwise: 255 XOR 255" : T_ACT = (255 XOR 255) : T_EXP = 0 : GOSUB 9000
420 T_NAME$ = "Bitwise: 255 XOR 0"   : T_ACT = (255 XOR 0) : T_EXP = 255 : GOSUB 9000

500 REM --------------------------------------------------------
510 REM 3. Proprietes algebriques (De Morgan, involution, neutre)
520 REM --------------------------------------------------------
530 X% = 42 : Y% = 99
540 T_NAME$ = "Double NOT: NOT(NOT X) = X"
550 T_ACT = (NOT (NOT X%)) : T_EXP = X% : GOSUB 9000

560 T_NAME$ = "De Morgan: NOT(X AND Y) = (NOT X) OR (NOT Y)"
570 T_ACT = (NOT (X% AND Y%)) : T_EXP = ((NOT X%) OR (NOT Y%)) : GOSUB 9000

580 T_NAME$ = "De Morgan: NOT(X OR Y) = (NOT X) AND (NOT Y)"
590 T_ACT = (NOT (X% OR Y%)) : T_EXP = ((NOT X%) AND (NOT Y%)) : GOSUB 9000

600 T_NAME$ = "XOR Reversibilite: (X XOR Y) XOR Y = X"
610 T_ACT = ((X% XOR Y%) XOR Y%) : T_EXP = X% : GOSUB 9000

620 T_NAME$ = "Idempotence: X AND X = X"
630 T_ACT = (X% AND X%) : T_EXP = X% : GOSUB 9000

640 T_NAME$ = "Idempotence: X OR X = X"
650 T_ACT = (X% OR X%) : T_EXP = X% : GOSUB 9000

700 REM --------------------------------------------------------
710 REM 4. Precedence des operateurs (Arith > Rel > NOT > AND > OR/XOR)
720 REM --------------------------------------------------------
730 REM Arithmetique vs Relationnel : 2 + 3 = 5 doit evaluer 2+3 d'abord
740 T_NAME$ = "Precedence: 2 + 3 = 5"
750 T_ACT = (2 + 3 = 5) : T_EXP = -1 : GOSUB 9000

760 REM Relationnel vs AND : 5 > 3 AND 2 < 4
770 T_NAME$ = "Precedence: 5 > 3 AND 2 < 4"
780 T_ACT = (5 > 3 AND 2 < 4) : T_EXP = -1 : GOSUB 9000

790 REM NOT avant AND : NOT 0 AND -1 = (-1) AND (-1) = -1
800 T_NAME$ = "Precedence: NOT 0 AND -1"
810 T_ACT = (NOT 0 AND -1) : T_EXP = -1 : GOSUB 9000

820 REM AND avant OR : -1 OR 0 AND 0 = -1 OR (0 AND 0) = -1
830 T_NAME$ = "Precedence: -1 OR 0 AND 0"
840 T_ACT = (-1 OR 0 AND 0) : T_EXP = -1 : GOSUB 9000

850 REM AND avant XOR : 0 XOR -1 AND -1 = 0 XOR (-1 AND -1) = -1
860 T_NAME$ = "Precedence: 0 XOR -1 AND -1"
870 T_ACT = (0 XOR -1 AND -1) : T_EXP = -1 : GOSUB 9000

880 REM Expressions complexes parenthesees
890 V1 = 10 : V2 = 20 : V3 = 30
900 T_NAME$ = "Complexe: (V1<V2 AND V2<V3) OR V1=0"
910 T_ACT = ((V1 < V2 AND V2 < V3) OR V1 = 0) : T_EXP = -1 : GOSUB 9000

920 T_NAME$ = "Complexe: NOT (V1 > V2) AND (V3 = 30)"
930 T_ACT = (NOT (V1 > V2) AND (V3 = 30)) : T_EXP = -1 : GOSUB 9000

1000 REM --------------------------------------------------------
1010 REM 5. Operateurs relationnels (Nombres et Chaines)
1020 REM --------------------------------------------------------
1030 T_NAME$ = "Relational Num: 10 = 10"  : T_ACT = (10 = 10)  : T_EXP = -1 : GOSUB 9000
1040 T_NAME$ = "Relational Num: 10 <> 20" : T_ACT = (10 <> 20) : T_EXP = -1 : GOSUB 9000
1050 T_NAME$ = "Relational Num: 10 < 20"  : T_ACT = (10 < 20)  : T_EXP = -1 : GOSUB 9000
1060 T_NAME$ = "Relational Num: 20 > 10"  : T_ACT = (20 > 10)  : T_EXP = -1 : GOSUB 9000
1070 T_NAME$ = "Relational Num: 10 <= 10" : T_ACT = (10 <= 10) : T_EXP = -1 : GOSUB 9000
1080 T_NAME$ = "Relational Num: 10 <= 20" : T_ACT = (10 <= 20) : T_EXP = -1 : GOSUB 9000
1090 T_NAME$ = "Relational Num: 20 >= 20" : T_ACT = (20 >= 20) : T_EXP = -1 : GOSUB 9000
1100 T_NAME$ = "Relational Num: 20 >= 10" : T_ACT = (20 >= 10) : T_EXP = -1 : GOSUB 9000
1110 T_NAME$ = "Relational Num Faux: 10 > 20" : T_ACT = (10 > 20) : T_EXP = 0 : GOSUB 9000

1120 T_NAME$ = "Relational Str: A = A"     : T_ACT = ("ABC" = "ABC") : T_EXP = -1 : GOSUB 9000
1130 T_NAME$ = "Relational Str: A <> B"    : T_ACT = ("ABC" <> "XYZ") : T_EXP = -1 : GOSUB 9000
1140 T_NAME$ = "Relational Str: A < B"     : T_ACT = ("APPLE" < "BANANA") : T_EXP = -1 : GOSUB 9000
1150 T_NAME$ = "Relational Str: B > A"     : T_ACT = ("ZEBRA" > "YAK") : T_EXP = -1 : GOSUB 9000
1160 T_NAME$ = "Relational Str: prefix < full" : T_ACT = ("CAT" < "CATERPILLAR") : T_EXP = -1 : GOSUB 9000

8000 REM ========================================================
8010 REM  SYNTHESE ET RESULTAT
8020 REM ========================================================
8030 PRINT "--------------------------------------------------------"
8040 PRINT "Suite   : "; T_SUITE$
8050 PRINT "Total   : "; T_TOTAL; " assertions"
8060 PRINT "Reussis : "; T_PASS
8070 PRINT "Echecs  : "; T_FAIL
8080 IF T_FAIL = 0 THEN GOTO 8100 ELSE GOTO 8110
8100 PRINT "TEST 17 PASSED" : END
8110 PRINT "TEST 17 FAILED ("; T_FAIL; " echecs)" : ERROR 1
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
