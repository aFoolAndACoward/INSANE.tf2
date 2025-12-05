@echo off

cmake --build build --config Release
if errorlevel 1 (
    echo.
    echo [31m[ INSANE.tf2 ] Failed release build. [Config : Release][0m
    pause
    exit /b 1 
)

echo.
echo [32m[ INSANE.tf2 ] Build successful. [Config : Release] [0m
pause
