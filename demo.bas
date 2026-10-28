10 REM ===================================================
20 REM GW-BASIC Complete C++ Port Demonstration (Modules 1-5)
30 REM ===================================================
40 PRINT "GW-BASIC C++ ENGINE ACTIVE"
50 PRINT "---------------------------------------------------"
60 REM [Module 1 & 2] Math, Random Knuth LCG, and Strings
70 MSG$ = "GW-BASIC 1983"
80 MID$(MSG$, 10, 4) = "2026"
90 PRINT "String modification test: "; MSG$
100 FOR I = 1 TO 3
110   PRINT "  Index "; I; " | Sqr: "; SQR(I); " | Rnd: "; INT(RND(1) * 100)
120 NEXT I
130 REM [Module 4] File System I/O (GIODSK.ASM)
140 PRINT "Writing data to disk file 'sample.txt'..."
150 OPEN "sample.txt" FOR OUTPUT AS #1
160 PRINT #1, "RECORD 1: Microsoft GW-BASIC C++ Port"
170 PRINT #1, 3.14159
180 CLOSE #1
190 PRINT "Reading back from disk file..."
200 OPEN "sample.txt" FOR INPUT AS #1
210 LINE INPUT #1, L1$
220 INPUT #1, PI_VAL
230 CLOSE #1
240 PRINT "File read success: ["; L1$; "] Pi = "; PI_VAL
250 REM [Module 3 & 4] Graphics and BMP Image Export (ADVGRP.ASM & SCNDRV.ASM)
260 PRINT "Drawing Graphics (Line, Circle, Paint) & Exporting BMP..."
270 CLS
280 LINE (10, 10)-(150, 150), 1, B
290 PAINT (50, 50), 14, 1
300 CIRCLE (220, 100), 50, 2
310 SAVE_SCREEN "gwbasic_drawing.bmp"
320 PRINT "Graphics exported successfully to 'gwbasic_drawing.bmp'!"
330 PRINT "---------------------------------------------------"
340 PRINT "GW-BASIC demonstration completed with 100% success."
