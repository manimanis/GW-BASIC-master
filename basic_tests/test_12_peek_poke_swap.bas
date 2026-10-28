10 REM ========================================================
20 REM  SUITE : TEST 12 - PEEK, POKE ET SWAP
30 REM ========================================================
40 T_SUITE$ = "TEST 12 : PEEK / POKE / SWAP"
50 T_TOTAL = 0 : T_PASS = 0 : T_FAIL = 0
60 PRINT "=== "; T_SUITE$; " ==="
70 PRINT

100 REM --------------------------------------------------------
110 REM 1. POKE et PEEK sur differentes adresses et valeurs
120 REM --------------------------------------------------------
130 POKE 100, 255
140 T_NAME$ = "POKE 100, 255 -> PEEK(100)" : T_ACT = PEEK(100) : T_EXP = 255 : GOSUB 9000

150 POKE 100, 0
160 T_NAME$ = "POKE 100, 0 -> PEEK(100)"   : T_ACT = PEEK(100) : T_EXP = 0   : GOSUB 9000

170 POKE 100, 42
180 T_NAME$ = "POKE 100, 42 -> PEEK(100)"  : T_ACT = PEEK(100) : T_EXP = 42  : GOSUB 9000

190 REM Ecriture d'une sequence d'octets
200 FOR ADDR = 200 TO 205
210   POKE ADDR, (ADDR - 190)
220 NEXT ADDR

230 SEQ_OK = 1
240 FOR ADDR = 200 TO 205
250   IF PEEK(ADDR) <> (ADDR - 190) THEN SEQ_OK = 0
260 NEXT ADDR
270 T_NAME$ = "Sequence of 6 POKE/PEEK bytes (10..15)" : T_ACT = SEQ_OK : T_EXP = 1 : GOSUB 9000

300 REM --------------------------------------------------------
310 REM 2. SWAP sur scalaires (entiers, chaines)
320 REM --------------------------------------------------------
330 V1 = 1234 : V2 = 5678
340 SWAP V1, V2
350 T_NAME$ = "SWAP scalar V1" : T_ACT = V1 : T_EXP = 5678 : GOSUB 9000
360 T_NAME$ = "SWAP scalar V2" : T_ACT = V2 : T_EXP = 1234 : GOSUB 9000

370 S1$ = "FIRST" : S2$ = "SECOND"
380 SWAP S1$, S2$
390 T_NAME$ = "SWAP string S1$" : T_ACT$ = S1$ : T_EXP$ = "SECOND" : GOSUB 9100
400 T_NAME$ = "SWAP string S2$" : T_ACT$ = S2$ : T_EXP$ = "FIRST"  : GOSUB 9100

500 REM --------------------------------------------------------
510 REM 3. SWAP sur elements de tableaux
520 REM --------------------------------------------------------
530 DIM ARR(5)
540 ARR(1) = 111 : ARR(2) = 222 : ARR(3) = 333
550 SWAP ARR(1), ARR(3)
560 T_NAME$ = "SWAP ARR(1) with ARR(3)" : T_ACT = ARR(1) : T_EXP = 333 : GOSUB 9000
570 T_NAME$ = "SWAP ARR(3) with ARR(1)" : T_ACT = ARR(3) : T_EXP = 111 : GOSUB 9000
580 T_NAME$ = "Unchanged ARR(2)"        : T_ACT = ARR(2) : T_EXP = 222 : GOSUB 9000

8000 REM ========================================================
8010 REM  SYNTHESE ET RESULTAT
8020 REM ========================================================
8030 PRINT "--------------------------------------------------------"
8040 PRINT "Suite   : "; T_SUITE$
8050 PRINT "Total   : "; T_TOTAL; " assertions"
8060 PRINT "Reussis : "; T_PASS
8070 PRINT "Echecs  : "; T_FAIL
8080 IF T_FAIL = 0 THEN GOTO 8100 ELSE GOTO 8110
8100 PRINT "TEST 12 PASSED" : END
8110 PRINT "TEST 12 FAILED ("; T_FAIL; " echecs)" : ERROR 1
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
