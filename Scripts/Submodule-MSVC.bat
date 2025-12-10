@echo off
setlocal
REM =========================
REM Build using MSVC
REM =========================

cd /d "%~dp0"
cd ..

cmake -B Build/MSVC -S . -G "Visual Studio 17 2022" -DUSE_ENGINE_SUBMODULE="ON" 


echo.
echo Build with MSVC completed.
pause
