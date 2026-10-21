@echo off
rem ==========================================================
rem  Build script for main_sdcc.c (electric-toy project)
rem  Double-click this file to compile -> main_sdcc.hex
rem
rem  If SDCC is moved to another folder,
rem  edit only the SDCC_BIN line below.
rem ==========================================================
chcp 65001 >nul
cd /d "%~dp0"

set SDCC_BIN=C:\MY\Program Files\SDCC\bin
rem add SDCC to PATH so sdcc can find its backend (cc1.exe)
set PATH=%SDCC_BIN%;%PATH%

echo [1/2] Compiling main_sdcc.c ...
"%SDCC_BIN%\sdcc.exe" -mmcs51 --model-small main_sdcc.c
if errorlevel 1 (
  echo.
  echo [FAILED] Compile error. Send me the messages above.
  pause
  exit /b 1
)

echo [2/2] Converting to .hex ...
"%SDCC_BIN%\packihx.exe" main_sdcc.ihx > main_sdcc.hex
if errorlevel 1 (
  echo.
  echo [FAILED] packihx error. Send me the messages above.
  pause
  exit /b 1
)

rem clean intermediate files, keep only main_sdcc.hex
del /q main_sdcc.ihx main_sdcc.rel main_sdcc.lst main_sdcc.asm main_sdcc.sym main_sdcc.map main_sdcc.mem main_sdcc.lk main_sdcc.rst main_sdcc.noi 2>nul

echo.
echo [OK] main_sdcc.hex generated. Flash it with STC-ISP.
pause