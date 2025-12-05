@echo off

cmake --build build --config Debug
if errorlevel 1 (
    echo.
    echo [31m[ INSANE.tf2 ] Failed build. [Config : Debug][0m
    pause
    exit /b 1 
)

echo.
echo [32m[ INSANE.tf2 ] Build successful. [Config : Debug][0m
pause
