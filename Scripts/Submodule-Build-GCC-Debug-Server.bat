@echo off
setlocal
REM =========================
REM Build using GCC
REM =========================

cd /d "%~dp0"
cd ..

cmake -B Build/GCC -S . -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Debug -DUSE_ENGINE_SUBMODULE="ON" 
cmake --build Build/GCC  --target PongTerminalServer

echo.
echo Build with GCC completed.
pause
