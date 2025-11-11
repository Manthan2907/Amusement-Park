@echo off
echo Building Amusement Park System with Web UI...
echo.

REM Create build directory if it doesn't exist
if not exist "build" mkdir build

REM Compile all source files
echo Compiling source files...

gcc -Wall -Wextra -Iinclude -std=c99 -DMG_ENABLE_PACKED_FS=0 -c src/main.c -o build/main.o
gcc -Wall -Wextra -Iinclude -std=c99 -DMG_ENABLE_PACKED_FS=0 -c src/ride_manager.c -o build/ride_manager.o
gcc -Wall -Wextra -Iinclude -std=c99 -DMG_ENABLE_PACKED_FS=0 -c src/visitor.c -o build/visitor.o
gcc -Wall -Wextra -Iinclude -std=c99 -DMG_ENABLE_PACKED_FS=0 -c src/visitor_history.c -o build/visitor_history.o
gcc -Wall -Wextra -Iinclude -std=c99 -DMG_ENABLE_PACKED_FS=0 -c src/queue_manager.c -o build/queue_manager.o
gcc -Wall -Wextra -Iinclude -std=c99 -DMG_ENABLE_PACKED_FS=0 -c src/dual_queue.c -o build/dual_queue.o
gcc -Wall -Wextra -Iinclude -std=c99 -DMG_ENABLE_PACKED_FS=0 -c src/priority_queue.c -o build/priority_queue.o
gcc -Wall -Wextra -Iinclude -std=c99 -DMG_ENABLE_PACKED_FS=0 -c src/graph.c -o build/graph.o
gcc -Wall -Wextra -Iinclude -std=c99 -DMG_ENABLE_PACKED_FS=0 -c src/stack.c -o build/stack.o
gcc -Wall -Wextra -Iinclude -std=c99 -DMG_ENABLE_PACKED_FS=0 -c src/bst.c -o build/bst.o
gcc -Wall -Wextra -Iinclude -std=c99 -DMG_ENABLE_PACKED_FS=0 -c src/file_io.c -o build/file_io.o
gcc -Wall -Wextra -Iinclude -std=c99 -DMG_ENABLE_PACKED_FS=0 -c src/utils.c -o build/utils.o
gcc -Wall -Wextra -Iinclude -std=c99 -DMG_ENABLE_PACKED_FS=0 -c src/web_server.c -o build/web_server.o
gcc -Wall -Wextra -Iinclude -std=c99 -DMG_ENABLE_PACKED_FS=0 -c src/web_server_handlers.c -o build/web_server_handlers.o
gcc -Wall -Wextra -Iinclude -std=c99 -DMG_ENABLE_PACKED_FS=0 -c src/mongoose.c -o build/mongoose.o

echo.
echo Linking...
gcc build/main.o build/ride_manager.o build/visitor.o build/visitor_history.o build/queue_manager.o build/dual_queue.o build/priority_queue.o build/graph.o build/stack.o build/bst.o build/file_io.o build/utils.o build/web_server.o build/web_server_handlers.o build/mongoose.o -o park_system.exe -lm -lws2_32

if %errorlevel% == 0 (
    echo.
    echo ========================================
    echo Build successful!
    echo ========================================
    echo.
    echo Run with: park_system.exe
    echo Web UI will be at: http://localhost:8000
    echo.
) else (
    echo.
    echo Build failed!
    echo.
)
