#!/usr/bin/env bash
# macOS registers every .app it sees with LaunchServices. The dev bundles share KiCad's bundle id and have a
# higher version than a released KiCad, so Finder may start opening project files with the dev build.
# Unregister them after every build/install; the installed KiCad in /Applications is unaffected.
LS=/System/Library/Frameworks/CoreServices.framework/Frameworks/LaunchServices.framework/Support/lsregister
[ -x "$LS" ] || exit 0
find "$KICAD_BUILD_DIR" -maxdepth 6 -name '*.app' -type d 2>/dev/null | while read -r app; do "$LS" -u "$app" >/dev/null 2>&1; done
echo "unregistered dev app bundles from LaunchServices"
