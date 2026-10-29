@echo off
echo ============================================================
echo   Compiling Smart Canteen Billing System with GCC...
echo ============================================================
gcc -Wall -Wextra -std=c99 src/main.c src/file_manager.c src/ui_utils.c src/customer.c src/admin.c src/web_exporter.c -Iinclude -o canteen.exe

if %ERRORLEVEL% EQU 0 (
    echo.
    echo [SUCCESS] Compilation successful! Created canteen.exe
    echo Run canteen.exe to start the system.
) else (
    echo.
    echo [ERROR] Compilation failed. Please inspect the errors above.
)
pause
