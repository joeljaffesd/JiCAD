#!/usr/bin/env bash
# Runs on every pixi activation. The conda-forge compilers need the macOS SDK path.
if [ -z "${SDKROOT:-}" ]; then
  export SDKROOT="$(xcrun --sdk macosx --show-sdk-path 2>/dev/null)"
fi
export CONDA_BUILD_SYSROOT="$SDKROOT"
