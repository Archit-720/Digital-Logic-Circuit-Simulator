@echo off
setlocal

set "PATH=C:\Qt\6.12.0\mingw_64\bin;C:\Qt\Tools\mingw1310_64\bin;%PATH%"
set "QT_PLUGIN_PATH=C:\Qt\6.12.0\mingw_64\plugins"

cd /d "%~dp0"

if not exist "build\DigitalLogicWorkbench.exe" (
    echo Application not found. Building project...
    cmake -S . -B build -G Ninja -DCMAKE_MAKE_PROGRAM="C:/Qt/Tools/Ninja/ninja.exe" -DCMAKE_CXX_COMPILER="C:/Qt/Tools/mingw1310_64/bin/g++.exe" -DCMAKE_PREFIX_PATH="C:/Qt/6.12.0/mingw_64"

    if errorlevel 1 (
        echo Configuration failed.
        pause
        exit /b 1
    )
)

cmake --build build

if errorlevel 1 (
    echo Build failed.
    pause
    exit /b 1
)

start "" "%~dp0build\DigitalLogicWorkbench.exe"

endlocal