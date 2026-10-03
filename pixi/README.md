# Building KiCad on macOS with Pixi

A project-local, hash-locked development environment using [Pixi](https://pixi.sh) and conda-forge.
Nothing is installed system-wide: packages go to `.pixi/`, the two dependencies conda-forge cannot
supply go to `.deps/`, and the build goes to `build/`. Remove the three folders to undo everything.

```
pixi install          # conda-forge deps into .pixi/ (pinned by pixi.lock)
pixi run deps         # build KiCad's patched wxWidgets 3.2 and ngspice into .deps/  (one time, ~30 min)
pixi run configure    # cmake -G Ninja into build/  (KICAD_BUILD_TYPE=Release|Debug|RelWithDebInfo, default RelWithDebInfo)
pixi run build        # ninja
pixi run install      # KiCad.app into build/install/
pixi run app          # launch it
```

Extra CMake flags can be appended: `pixi run configure -- -DKICAD_USE_OCC=OFF`.

## What comes from where

| Source | Packages |
|---|---|
| conda-forge (pinned in `pixi.lock`) | the clang/clang++ toolchain and libc++, cmake 3.31, ninja, ccache, swig, bison, flex, autotools, gettext, python 3.13, boost, cairo, pixman, glew, glm, harfbuzz, freetype, fontconfig, occt 7.9, protobuf, nng, libgit2, zstd, zlib, curl, llvm-openmp, unixodbc, libjpeg-turbo, libpng, expat |
| built from source by `pixi run deps` | **wxWidgets** from KiCad's fork (`gitlab.com/kicad/code/wxWidgets`, branch `kicad/macos-wx-3.2`, with the configure line kicad-mac-builder uses) and **ngspice 45.2** (`--with-ngshared`), because conda-forge has only unpatched wx 3.3 and no Apple Silicon ngspice |
| the system | only the macOS SDK from Xcode or the command line tools (Cocoa, OpenGL, ... frameworks); `pixi/activate.sh` exports `SDKROOT` |

`MACOSX_DEPLOYMENT_TARGET` is set to 12.0 by the environment activation; conda-forge osx-arm64 packages target 11.0, so they are compatible.

## Why the conda-forge compilers
conda-forge libraries link conda-forge's libc++. Building KiCad with Apple clang links Apple's libc++ into some
binaries, and two C++ runtimes in one process abort at runtime. Using the conda-forge toolchain for KiCad and for
the wxWidgets build keeps everything on one libc++. `configure.sh` also passes `-D_LIBCPP_DISABLE_AVAILABILITY`,
conda-forge's documented remedy for newer libc++ marking `<chrono>` time zone support as macOS 15.4+.

## Notes
- `pixi run install` stages a self-contained `KiCad.app` in `build/install/` (nothing is installed system-wide); it relies
  on `KICAD_APPLE_RUNTIME_DEP_DIRS` so the @rpath-named conda-forge dylibs are copied into the bundle.
- `pixi run app` stages the bundle first (the build-tree bundle has no icons/resources, only `build/install/` is runnable) and
  runs the binary directly instead of `open`. `pixi run install` also unregisters the dev bundles from LaunchServices, because
  macOS registers every .app it sees and the dev build shares KiCad's bundle id
  under KiCad's bundle id (which could make Finder open projects with the dev build).
- wxPython scripting is off (`KICAD_SCRIPTING_WXPYTHON=OFF`), as are i18n, QA tests and Sentry, to keep the first build small. Turn them on via extra flags.
- The `git describe` warnings during configure are harmless on a shallow or fork checkout.
- Only `osx-arm64` is declared in `pixi.toml` so far. Adding `linux-64` should be a matter of listing the platform and letting conda-forge supply wx/ngspice there directly.
