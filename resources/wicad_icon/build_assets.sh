#!/bin/zsh
# Builds WiCAD's icon assets from make_icon.py and writes them into the WiCAD source tree:
#   kicad/kicad.icns, resources/bitmaps_png/sources/{light,dark}/icon_kicad*.svg, resources/bitmaps_png/png/icon_kicad*.png
# Needs inkscape and iconutil.  Usage: build_assets.sh [path/to/WiCAD]
set -e
here=${0:A:h}; repo=${1:-$here/../..}; tmp=$(mktemp -d)
python3 $here/make_icon.py >/dev/null
render() { inkscape $1 --export-type=png --export-width=$2 --export-height=$2 --export-filename=$3 >/dev/null 2>&1; }
mkdir $tmp/wicad.iconset
for n in 16 32 128 256 512; do
  render $here/wicad_app.svg $n $tmp/wicad.iconset/icon_${n}x${n}.png
  render $here/wicad_app.svg $((n*2)) $tmp/wicad.iconset/icon_${n}x${n}@2x.png
done
iconutil -c icns $tmp/wicad.iconset -o $repo/kicad/kicad.icns
for f in $repo/resources/bitmaps_png/sources/{light,dark}/icon_kicad*.svg; do cp $here/wicad_inapp.svg $f; done
for f in $repo/resources/bitmaps_png/png/icon_kicad*.png; do
  n=${${f:t:r}##*_}; render $here/wicad_inapp.svg $n $f
done
rm -rf $tmp; echo done
