#!/usr/bin/env bash
# Build KiCad's patched wxWidgets 3.2 (gitlab.com/kicad/code/wxWidgets, branch kicad/macos-wx-3.2)
# with the same configure line kicad-mac-builder uses, installing into $KICAD_DEPS/wx.
set -euo pipefail
PREFIX="$KICAD_DEPS/wx"
SRC="$KICAD_DEPS/src/wxWidgets"
if [ -x "$PREFIX/bin/wx-config" ]; then echo "wxWidgets already built at $PREFIX"; exit 0; fi
mkdir -p "$KICAD_DEPS/src"
if [ ! -d "$SRC" ]; then
  git clone --depth 1 --branch kicad/macos-wx-3.2 https://gitlab.com/kicad/code/wxWidgets.git "$SRC"
fi
cd "$SRC"
git submodule update --init --depth 1
# bundled third-party libs (pcre, tiff, ...) ship config.sub/config.guess too old to know the
# arm64-apple-darwin triplet used by the conda-forge compilers; replace them with wxWidgets' own copies
find . -mindepth 2 -name config.sub -exec cp -f config.sub {} \;
find . -mindepth 2 -name config.guess -exec cp -f config.guess {} \;
# -include algorithm: the bundled Scintilla relies on transitive <algorithm> includes that newer libc++ headers dropped
CPPFLAGS=-D__ASSERT_MACROS_DEFINE_VERSIONS_WITHOUT_UNDERSCORES=1 CXXFLAGS="${CXXFLAGS:-} -include algorithm" CC="$CC" CXX="$CXX" \
./configure --prefix="$PREFIX" --with-macosx-version-min="$MACOSX_DEPLOYMENT_TARGET" \
  --enable-unicode --with-osx_cocoa --enable-sound --enable-graphics_ctx --enable-display --enable-geometry \
  --enable-debug_flag --enable-debug --enable-optimise --disable-debugreport --enable-uiactionsim --enable-autoidman \
  --enable-monolithic --enable-aui --enable-html --disable-stl --enable-richtext --disable-mediactrl \
  --with-libjpeg=builtin --with-libpng=builtin --with-regex=builtin --with-libtiff=builtin --with-zlib=builtin \
  --with-expat=builtin --without-liblzma --with-opengl
make -j"$(sysctl -n hw.ncpu)" BUILD=debug
make install
echo "wxWidgets installed to $PREFIX"
