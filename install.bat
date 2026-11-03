@echo off
setlocal EnableDelayedExpansion

echo ==================================================
echo    JTag - Automatic Plugin Builder and Installer
echo ==================================================

cd /d "%~dp0"

if not exist "build\JTag_artefacts\Release\VST3\JTag.vst3" (
    echo [1/2] Building JTag VST3 plugin...
    cmake -B build -DCMAKE_BUILD_TYPE=Release
    cmake --build build --config Release
)

set "VST3_SRC=build\JTag_artefacts\Release\VST3\JTag.vst3"
set "VST3_DEST=%CommonProgramFiles%\VST3\JTag.vst3"

echo [2/2] Installing JTag automatically to "%VST3_DEST%"...
if not exist "%CommonProgramFiles%\VST3" mkdir "%CommonProgramFiles%\VST3"
xcopy /E /I /Y "%VST3_SRC%" "%VST3_DEST%" >nul

if %ERRORLEVEL% EQU 0 (
    echo Done! JTag has been automatically installed to: %VST3_DEST%
    echo Open your DAW and rescan plugins.
) else (
    echo Please right-click install.bat and select "Run as Administrator".
)
pause