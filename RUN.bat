@echo off
title Smart Canteen Billing System
color 0B
cd /d "%~dp0"

:: 1. If canteen.exe does not exist, compile it automatically
if not exist "canteen.exe" (
    echo ========================================================
    echo   First-time setup: Compiling Canteen POS System...
    echo ========================================================
    gcc -Wall -Wextra -std=c99 src/main.c src/file_manager.c src/ui_utils.c src/customer.c src/admin.c src/web_exporter.c -Iinclude -o canteen.exe
    if %ERRORLEVEL% NEQ 0 (
        color 0C
        echo.
        echo [ERROR] Could not compile canteen.exe!
        echo Please make sure MinGW GCC is installed on your computer.
        pause
        exit /b
    )
    echo [SUCCESS] Compilation completed successfully!
    echo.
)

:: 2. Launch the billing system
cls
canteen.exe
