@echo off
where cmake
if %errorlevel% neq 0 (
    echo [31m[ INSANE.tf2 ] CMake not found!. Please install cmake from https://cmake.org/download/ [0m 
    pause
    exit /b 1 
)

cmake -G "Visual Studio 17 2022" -A x64 -S . -B Build
echo.
echo [32m[ INSANE.tf2 ] CMake files generated successfully :^). [0m 
pause
