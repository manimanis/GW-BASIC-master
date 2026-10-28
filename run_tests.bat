@echo off
setlocal enabledelayedexpansion

REM ============================================================
REM  GW-BASIC C++ Port - Automated Test Runner
REM  Runs all tests in basic_tests\ and reports PASS/FAIL
REM ============================================================

set GWBASIC=.\build\gwbasic.exe
set TEST_DIR=.\basic_tests
set PASS=0
set FAIL=0
set TOTAL=0

REM -- Check SDL2 DLL presence (needed by gwbasic.exe) --
if not exist ".\build\SDL2.dll" (
    if exist "C:\msys64\mingw64\bin\SDL2.dll" (
        copy "C:\msys64\mingw64\bin\SDL2.dll" ".\build\" >nul 2>&1
    )
)

echo ============================================================
echo   GW-BASIC Test Suite Runner
echo ============================================================
echo.

REM -- Text-mode tests (no SDL window) --
set TESTS=test_01_print test_02_variables test_03_math test_04_strings ^
          test_05_control test_06_data test_09_file_io test_10_on_goto ^
          test_11_random test_12_peek_poke_swap test_13_print_using ^
          test_14_multistatement test_15_integration test_16_errors ^
          test_17_logic test_18_timer ^
          test_20_functions test_21_file_io_advanced test_22_get_put_sound_play

for %%T in (%TESTS%) do (
    set /a TOTAL+=1
    set OUTFILE=%TEST_DIR%\%%T.out
    %GWBASIC% %TEST_DIR%\%%T.bas > "!OUTFILE!" 2>&1
    if !ERRORLEVEL! EQU 0 (
        findstr /C:"FAILED" "!OUTFILE!" >nul 2>&1
        if !ERRORLEVEL! EQU 0 (
            echo   [FAIL] %%T  ^(assertion failure reported^)
            set /a FAIL+=1
            type "!OUTFILE!"
        ) else (
            findstr /C:"PASSED" "!OUTFILE!" >nul 2>&1
            if !ERRORLEVEL! EQU 0 (
                echo   [PASS] %%T
                set /a PASS+=1
            ) else (
                echo   [FAIL] %%T  ^(no PASSED marker found^)
                set /a FAIL+=1
                type "!OUTFILE!"
            )
        )
    ) else (
        echo   [FAIL] %%T  ^(exit code !ERRORLEVEL!^)
        set /a FAIL+=1
        echo          --- Output:
        type "!OUTFILE!"
        echo          ---
    )
)

REM -- Graphics tests (BMP output only, no SDL window) --
set GFX_TESTS=test_07_graphics_basic test_08_graphics_advanced test_19_graphics_demo

echo.
echo Graphics tests (BMP export):
for %%T in (%GFX_TESTS%) do (
    set /a TOTAL+=1
    set OUTFILE=%TEST_DIR%\%%T.out
    %GWBASIC% %TEST_DIR%\%%T.bas > "!OUTFILE!" 2>&1
    if !ERRORLEVEL! EQU 0 (
        findstr /C:"FAILED" "!OUTFILE!" >nul 2>&1
        if !ERRORLEVEL! EQU 0 (
            echo   [FAIL] %%T  ^(assertion failure reported^)
            set /a FAIL+=1
            type "!OUTFILE!"
        ) else (
            findstr /C:"PASSED" "!OUTFILE!" >nul 2>&1
            if !ERRORLEVEL! EQU 0 (
                echo   [PASS] %%T
                set /a PASS+=1
            ) else (
                echo   [FAIL] %%T  ^(no PASSED marker found^)
                set /a FAIL+=1
                type "!OUTFILE!"
            )
        )
    ) else (
        echo   [FAIL] %%T  ^(exit code !ERRORLEVEL!^)
        set /a FAIL+=1
        echo          --- Output:
        type "!OUTFILE!"
        echo          ---
    )
)

echo.
echo ============================================================
echo   Results: %PASS% / %TOTAL% passed   %FAIL% failed
echo ============================================================

if %FAIL% EQU 0 (
    echo   ALL TESTS PASSED
    exit /b 0
) else (
    echo   SOME TESTS FAILED - see .out files in basic_tests\
    exit /b 1
)
