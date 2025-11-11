@echo off
echo ========================================
echo COMPREHENSIVE TESTING SCRIPT
echo ========================================
echo.
echo This script will test all features of the parking system.
echo.
echo Test Sequence:
echo 1. Add a visitor
echo 2. Suggest next best ride
echo 3. Display visitor ride history
echo 4. Show queue status
echo 5. Show rides by wait time
echo 6. Display park statistics
echo 7. Find shortest path
echo 8. Enjoy a ride
echo 9. Undo last ride
echo 10. Exit and save
echo.
echo Starting program...
echo.

REM Create test input
(
echo 1
echo TestUser
echo 25
echo 7
echo 1
echo 1
echo 2
echo 1000
echo 3
echo 1000
echo 10
echo 11
echo 12
echo 13
echo 1
echo 5
echo 14
echo 1000
echo 1
echo 4
echo 1000
echo 0
) | park_system.exe

echo.
echo ========================================
echo Testing completed!
echo ========================================
pause
