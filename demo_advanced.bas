10 REM ====================================================
20 REM  GW-BASIC C++ ADVANCED FEATURES DEMONSTRATION
30 REM ====================================================
40 CLS : PRINT "=== GW-BASIC ADVANCED FEATURES DEMO ==="
50 COLOR 14, 1, 0 : PRINT "Color and Screen configured."
60 REM --- Test 1: Multi-statement lines and SWAP ---
70 A = 100 : B = 999 : SWAP A, B
80 PRINT USING "After SWAP A=$$###, B=$$###"; A, B
90 REM --- Test 2: DATA, READ, and RESTORE ---
100 DATA 12.5, 45.75, 99.0
110 READ D1, D2, D3
120 PRINT USING "Read DATA values: ###.##, ###.##, ###.##"; D1, D2, D3
130 RESTORE 100 : READ D1 : PRINT "Restored first DATA item: "; D1
140 REM --- Test 3: WHILE ... WEND Loop ---
150 CNT = 1 : TOTAL = 0
160 WHILE CNT <= 4
170   TOTAL = TOTAL + (CNT * CNT)
180   CNT = CNT + 1
190 WEND
200 PRINT "Sum of squares (1^2 + 2^2 + 3^2 + 4^2) = "; TOTAL
210 REM --- Test 4: Low-level PEEK and POKE (GWRAM emulation) ---
220 POKE 500, 42
230 VAL_PEEK = PEEK(500)
240 PRINT "Memory POKE/PEEK check at addr 500: "; VAL_PEEK
250 REM --- Test 5: Advanced Vector Graphics with DRAW Macro Language ---
260 PRINT "Drawing complex vector shape using DRAW macro..."
270 PSET (160, 100), 1
280 DRAW "C2 U30 R30 D30 L30"
290 DRAW "C3 E20 F20 G20 H20"
300 DRAW "BM +10, +10 C1 U10 R10 D10 L10"
310 PAINT (165, 95), 1, 2
320 SAVE_SCREEN "gwbasic_advanced.bmp"
330 PRINT "Advanced graphics saved to 'gwbasic_advanced.bmp'."
340 REM --- Test 6: ON ... GOTO Branching ---
350 SEL = 2
360 ON SEL GOTO 400, 500, 600
370 PRINT "Branching error!" : END
400 PRINT "Option 1" : END
500 PRINT "ON GOTO successfully branched to Option 2!" : GOTO 700
600 PRINT "Option 3" : END
700 PRINT "=== ADVANCED DEMO COMPLETE WITH 100% SUCCESS ==="
800 END
