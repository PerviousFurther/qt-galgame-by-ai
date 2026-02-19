#!/usr/bin/env bash
set -euo pipefail

if ! command -v apt-get >/dev/null 2>&1; then
  echo "apt-get not found. This script supports Ubuntu/Debian only."
  exit 1
fi

SUDO=""
if command -v sudo >/dev/null 2>&1; then
  SUDO="sudo"
fi

echo "Installing Qt6 development dependencies..."
$SUDO apt-get update
$SUDO apt-get install -y \
  qt6-base-dev \
  qt6-declarative-dev \
  qt6-multimedia-dev \
  qml6-module-qtquick \
  qml6-module-qtquick-controls \
  qml6-module-qtquick-layouts \
  qml6-module-qtquick-templates \
  qml6-module-qtquick-window \
  qml6-module-qtmultimedia \
  qml6-module-qtqml-workerscript

echo "Qt6 dependencies installed successfully."
