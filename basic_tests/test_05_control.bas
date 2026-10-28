10 REM ========================================================
20 REM  SUITE : TEST 05 - STRUCTURES DE CONTROLE
30 REM ========================================================
40 T_SUITE$ = "TEST 05 : STRUCTURES DE CONTROLE"
50 T_TOTAL = 0 : T_PASS = 0 : T_FAIL = 0
60 PRINT "=== "; T_SUITE$; " ==="
70 PRINT

100 REM --------------------------------------------------------
110 REM 1. IF ... THEN ... ELSE
120 REM --------------------------------------------------------
130 BRANCH1 = 0 : BRANCH2 = 0
140 IF 10 > 5 THEN BRANCH1 = 1 ELSE BRANCH1 = 2
150 T_NAME$ = "IF True takes THEN" : T_ACT = BRANCH1 : T_EXP = 1 : GOSUB 9000

160 IF 5 > 10 THEN BRANCH2 = 1 ELSE BRANCH2 = 2
170 T_NAME$ = "IF False takes ELSE" : T_ACT = BRANCH2 : T_EXP = 2 : GOSUB 9000

180 REM IF imbrique
190 NEST_RES = 0 : V = 15
200 IF V > 0 THEN IF V < 20 THEN NEST_RES = 1 ELSE NEST_RES = 2 ELSE NEST_RES = 3
210 T_NAME$ = "Nested IF taking inner THEN" : T_ACT = NEST_RES : T_EXP = 1 : GOSUB 9000

220 V = 25
230 IF V > 0 THEN IF V < 20 THEN NEST_RES = 1 ELSE NEST_RES = 2 ELSE NEST_RES = 3
240 T_NAME$ = "Nested IF taking inner ELSE" : T_ACT = NEST_RES : T_EXP = 2 : GOSUB 9000

300 REM --------------------------------------------------------
310 REM 2. FOR ... NEXT (Step par defaut, custom, negatif, 0-iter)
320 REM --------------------------------------------------------
330 FOR_SUM = 0
340 FOR I = 1 TO 5
350   FOR_SUM = FOR_SUM + I
360 NEXT I
370 T_NAME$ = "FOR 1 TO 5 sum (1+2+3+4+5)" : T_ACT = FOR_SUM : T_EXP = 15 : GOSUB 9000
380 T_NAME$ = "FOR loop variable after exit" : T_ACT = I : T_EXP = 6 : GOSUB 9000

390 STEP_SUM = 0
400 FOR K = 0 TO 10 STEP 2
410   STEP_SUM = STEP_SUM + K
420 NEXT K
430 T_NAME$ = "FOR STEP 2 sum (0+2+4+6+8+10)" : T_ACT = STEP_SUM : T_EXP = 30 : GOSUB 9000

440 NEG_SUM = 0
450 FOR M = 10 TO 2 STEP -2
460   NEG_SUM = NEG_SUM + M
470 NEXT M
480 T_NAME$ = "FOR STEP -2 sum (10+8+6+4+2)" : T_ACT = NEG_SUM : T_EXP = 30 : GOSUB 9000

490 REM Boucle qui ne doit pas s'executer (start > end avec step > 0)
500 ZERO_EXEC = 0
510 FOR Z = 5 TO 1
520   ZERO_EXEC = ZERO_EXEC + 1
530 NEXT Z
540 T_NAME$ = "FOR start>end executes 0 times" : T_ACT = ZERO_EXEC : T_EXP = 0 : GOSUB 9000

550 REM Boucles FOR imbriquees (3 niveaux)
560 TRIPLE_CNT = 0
570 FOR A = 1 TO 2
580   FOR B = 1 TO 3
590     FOR C = 1 TO 4
600       TRIPLE_CNT = TRIPLE_CNT + 1
610     NEXT C
620   NEXT B
630 NEXT A
640 T_NAME$ = "3-level nested FOR (2*3*4 = 24)" : T_ACT = TRIPLE_CNT : T_EXP = 24 : GOSUB 9000

650 REM Sortie prematuree de boucle par GOTO
660 BREAK_VAL = 0
670 FOR B_I = 1 TO 10
680   IF B_I = 4 THEN GOTO 700
690 NEXT B_I
700 BREAK_VAL = B_I
710 T_NAME$ = "FOR exit via GOTO at index 4" : T_ACT = BREAK_VAL : T_EXP = 4 : GOSUB 9000

800 REM --------------------------------------------------------
810 REM 3. WHILE ... WEND
820 REM --------------------------------------------------------
830 W_CNT = 0 : W_VAL = 1
840 WHILE W_VAL <= 16
850   W_VAL = W_VAL * 2
860   W_CNT = W_CNT + 1
870 WEND
880 T_NAME$ = "WHILE powers of 2 (1->32: 5 iters)" : T_ACT = W_CNT : T_EXP = 5 : GOSUB 9000
890 T_NAME$ = "WHILE final value" : T_ACT = W_VAL : T_EXP = 32 : GOSUB 9000

900 REM WHILE faux des le depart
910 W_ZERO = 0
920 WHILE 1 = 0
930   W_ZERO = W_ZERO + 1
940 WEND
950 T_NAME$ = "WHILE False on entry executes 0 times" : T_ACT = W_ZERO : T_EXP = 0 : GOSUB 9000

960 REM WHILE imbrique
970 W_OUTER = 0 : W_INNER_SUM = 0
980 WHILE W_OUTER < 3
990   W_INNER = 0
1000  WHILE W_INNER < 3
1010    W_INNER_SUM = W_INNER_SUM + 1
1020    W_INNER = W_INNER + 1
1030  WEND
1040  W_OUTER = W_OUTER + 1
1050 WEND
1060 T_NAME$ = "Nested WHILE (3*3 = 9)" : T_ACT = W_INNER_SUM : T_EXP = 9 : GOSUB 9000

1100 REM --------------------------------------------------------
1110 REM 4. GOSUB ... RETURN (imbriques, retours multiples)
1120 REM --------------------------------------------------------
1130 GOSUB_TRACK$ = ""
1140 GOSUB 3000
1150 T_NAME$ = "Nested GOSUB/RETURN execution trace" : T_ACT$ = GOSUB_TRACK$ : T_EXP$ = "A_START;B_RUN;A_END" : GOSUB 9100

1160 REM Multi-return GOSUB
1170 PARAM = 1 : GOSUB 4000
1180 T_NAME$ = "GOSUB branch 1 return" : T_ACT = RET_CODE : T_EXP = 100 : GOSUB 9000
1190 PARAM = 2 : GOSUB 4000
1200 T_NAME$ = "GOSUB branch 2 return" : T_ACT = RET_CODE : T_EXP = 200 : GOSUB 9000

1210 GOTO 8000

3000 REM Sous-programme A
3010 GOSUB_TRACK$ = GOSUB_TRACK$ + "A_START;"
3020 GOSUB 3100
3030 GOSUB_TRACK$ = GOSUB_TRACK$ + "A_END"
3040 RETURN

3100 REM Sous-programme B appele par A
3110 GOSUB_TRACK$ = GOSUB_TRACK$ + "B_RUN;"
3120 RETURN

4000 REM Sous-programme a retours conditionnels
4010 IF PARAM = 1 THEN RET_CODE = 100 : RETURN
4020 RET_CODE = 200 : RETURN

8000 REM ========================================================
8010 REM  SYNTHESE ET RESULTAT
8020 REM ========================================================
8030 PRINT "--------------------------------------------------------"
8040 PRINT "Suite   : "; T_SUITE$
8050 PRINT "Total   : "; T_TOTAL; " assertions"
8060 PRINT "Reussis : "; T_PASS
8070 PRINT "Echecs  : "; T_FAIL
8080 IF T_FAIL = 0 THEN GOTO 8100 ELSE GOTO 8110
8100 PRINT "TEST 05 PASSED" : END
8110 PRINT "TEST 05 FAILED ("; T_FAIL; " echecs)" : ERROR 1
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
