#!/usr/bin/env bash
# Build ngspice as a shared library (libngspice.dylib) into $KICAD_DEPS/ngspice, as kicad-mac-builder does.
set -euo pipefail
VER=45.2
PREFIX="$KICAD_DEPS/ngspice"
SRC="$KICAD_DEPS/src/ngspice-$VER"
if [ -f "$PREFIX/lib/libngspice.dylib" ]; then echo "ngspice already built at $PREFIX"; exit 0; fi
mkdir -p "$KICAD_DEPS/src"
cd "$KICAD_DEPS/src"
if [ ! -d "$SRC" ]; then
  git clone --depth 1 --branch "ngspice-$VER" https://git.code.sf.net/p/ngspice/ngspice "$SRC"
fi
cd "$SRC"
./autogen.sh
# the bundled cppduals specializes std::is_compound, which libc++ 21+ rejects by default
CC="$CC" CXXFLAGS="${CXXFLAGS:-} -Wno-invalid-specialization" ./configure --prefix="$PREFIX" --with-ngshared --enable-xspice --enable-cider --disable-debug --disable-openmp
make -j"$(sysctl -n hw.ncpu)"
make install
echo "ngspice installed to $PREFIX"
