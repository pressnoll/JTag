#!/usr/bin/env bash
set -e

echo "=== JTag Automatic Builder & Installer ==="
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR"

if [ ! -d "build/JTag_artefacts" ]; then
    echo "[1/2] Building JTag..."
    cmake -B build -DCMAKE_BUILD_TYPE=Release
    cmake --build build --config Release -j4
fi

echo "[2/2] Installing JTag into system plugin folders..."
OS_NAME="$(uname -s)"

if [ "$OS_NAME" = "Darwin" ]; then
    VST3_SRC="build/JTag_artefacts/Release/VST3/JTag.vst3"
    AU_SRC="build/JTag_artefacts/Release/AU/JTag.component"
    
    mkdir -p "/Library/Audio/Plug-Ins/VST3" 2>/dev/null || sudo mkdir -p "/Library/Audio/Plug-Ins/VST3"
    cp -R "$VST3_SRC" "/Library/Audio/Plug-Ins/VST3/" 2>/dev/null || sudo cp -R "$VST3_SRC" "/Library/Audio/Plug-Ins/VST3/"
    
    mkdir -p "/Library/Audio/Plug-Ins/Components" 2>/dev/null || sudo mkdir -p "/Library/Audio/Plug-Ins/Components"
    cp -R "$AU_SRC" "/Library/Audio/Plug-Ins/Components/" 2>/dev/null || sudo cp -R "$AU_SRC" "/Library/Audio/Plug-Ins/Components/"
    
    killall -9 AudioComponentRegistrar 2>/dev/null || true
    echo "Done! JTag VST3 & AU installed."
elif [ "$OS_NAME" = "Linux" ]; then
    mkdir -p "$HOME/.vst3"
    cp -R "build/JTag_artefacts/Release/VST3/JTag.vst3" "$HOME/.vst3/"
    echo "Done! Installed to $HOME/.vst3/JTag.vst3"
fi