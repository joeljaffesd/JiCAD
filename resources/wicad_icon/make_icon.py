#!/usr/bin/env python3
"""WiCAD app icon: KiCad's icon construction (rounded tile, flared-serif two-letter mark, dotted i)
in Wolff Audio's palette.  Letter geometry is in KiCad's 100-unit tile space: cap height y 25.75-83.84.
The 'i' stem is KiCad's own outline; the W is drawn to the same rules (flared serifs, thick/thin contrast).
Usage: make_icon.py  -> writes wicad_<variant>.svg files next to this script."""
import os

ORANGE, BLACK, CREAM, SLATE = '#FC4C01', '#101820', '#DFD1A7', '#6B8197'
Y0, Y1 = 25.75, 83.84          # cap height and baseline (KiCad's K)
H = Y1 - Y0

def w_outline(a0, s=15.2, T=12.6, t=5.9, f=4.2, g=4.2):
    """Mitred W: strokes 1 and 3 thick, 2 and 4 thin; flat cuts of width f at the two feet and g at the apex."""
    L1 = lambda u: a0 + s*u;               R1 = lambda u: L1(u) + T
    R2 = lambda u: a0 + s + f + s*(1-u);   L2 = lambda u: R2(u) - t
    r3 = L2(0) + g
    R3 = lambda u: r3 + s*u;               L3 = lambda u: R3(u) - T
    r4 = L3(1) + f
    R4 = lambda u: r4 + s*(1-u);           L4 = lambda u: R4(u) - t
    def cross(A, B):                       # u where edge A meets edge B (both linear in u)
        a0_, a1_ = A(0), A(1); b0_, b1_ = B(0), B(1)
        return (b0_-a0_) / ((a1_-a0_) - (b1_-b0_))
    y = lambda u: Y0 + H*u
    q1 = cross(R1, L2); q2 = cross(L3, R2); q3 = cross(R3, L4)
    pts = [(L1(0),Y0),(R1(0),Y0),(R1(q1),y(q1)),(L2(0),Y0),(R3(0),Y0),(R3(q3),y(q3)),(L4(0),Y0),(R4(0),Y0),
           (R4(1),Y1),(L3(1),Y1),(L3(q2),y(q2)),(R2(1),Y1),(L1(1),Y1)]
    d = 'M ' + ' L '.join(f'{x:.3f},{yy:.3f}' for x,yy in pts) + ' Z'
    ends = dict(left=(L1, R1, +1), right=(L4, R4, -1))
    return d, ends, R4(0)

def serif(L, R, lean, over=3.4, depth=5.2):
    """Bracketed flat serif on a top terminal, like the head serifs of KiCad's K."""
    u = depth / H
    xl0, xr0 = L(0), R(0); xl1, xr1 = L(u), R(u)
    return (f'M {xl0-over:.3f},{Y0:.3f} H {xr0+over:.3f} '
            f'Q {xr0+0.6:.3f},{Y0+0.9:.3f} {xr1:.3f},{Y0+depth:.3f} H {xl1:.3f} '
            f'Q {xl0-0.6:.3f},{Y0+0.9:.3f} {xl0-over:.3f},{Y0:.3f} Z')

# KiCad's 'i' stem outline (path5477, first sub-path), used unchanged apart from a horizontal shift
I_STEM = ('m 83.222955,41.16207 v 35.814285 a 25.157009,25.195098 0 0 0 0.155357,3.667322 '
          '6.2958722,6.3054043 0 0 0 1.636115,3.199036 H 67.874383 a 6.3068468,6.3163955 0 0 0 1.636115,-3.199036 '
          '25.895022,25.934228 0 0 0 0.155355,-3.667322 V 47.950279 A 24.950506,24.988281 0 0 0 69.510498,44.322385 '
          '6.4313929,6.4411302 0 0 0 67.913045,41.16207 Z')
I_DOT = (76.170616, 28.789175, 8.15)

def mark(letter_fill, w_fill, dot_fill):
    a0 = 0.0
    d, ends, wright = w_outline(a0)
    lw = wright + 3.4                       # W right edge incl. serif overhang
    gap = 3.2
    ishift = (lw + gap) - 67.874383         # move the i so its serif starts after the W
    total_r = 85.014427 + ishift
    left = -3.4
    width = total_r - left
    scale = min(1.0, 74.0 / width)          # keep KiCad's side margins (mark about 70-74 wide)
    cx = (left + total_r)/2; cy = (Y0 + Y1)/2 - 2.0   # optical centre sits slightly high because of the dot
    g = [f'<g transform="translate(50 {50+1.5:.2f}) scale({scale:.4f}) translate({-cx:.3f} {-cy:.3f})">',
         f'<path d="{d}" fill="{w_fill}"/>',
         f'<path d="{serif(*ends["left"][:2], 1)}" fill="{w_fill}"/>',
         f'<path d="{serif(*ends["right"][:2], -1)}" fill="{w_fill}"/>',
         f'<g transform="translate({ishift:.3f} 0)"><path d="{I_STEM}" fill="{letter_fill}"/>'
         f'<circle cx="{I_DOT[0]}" cy="{I_DOT[1]}" r="{I_DOT[2]}" fill="{dot_fill}"/></g>', '</g>']
    return '\n    '.join(g)

DEFS = f'''<defs>
    <linearGradient id="wolffOrange" x1="0" y1="0.35" x2="1" y2="0.65">
      <stop offset="0" stop-color="#F0301A"/><stop offset="0.45" stop-color="{ORANGE}"/><stop offset="1" stop-color="#FFB400"/>
    </linearGradient>
    <linearGradient id="wolffOrangeW" gradientUnits="userSpaceOnUse" x1="-4" y1="40" x2="62" y2="72">
      <stop offset="0" stop-color="#F0301A"/><stop offset="0.45" stop-color="{ORANGE}"/><stop offset="1" stop-color="#FFB400"/>
    </linearGradient>
    <radialGradient id="creamW" gradientUnits="userSpaceOnUse" cx="29" cy="46" r="52">
      <stop offset="0" stop-color="#F3EACB"/><stop offset="1" stop-color="{CREAM}"/>
    </radialGradient>
    <linearGradient id="tileBlack" x1="0" y1="0" x2="0" y2="1">
      <stop offset="0" stop-color="#1C2937"/><stop offset="1" stop-color="{BLACK}"/>
    </linearGradient>
    <linearGradient id="tileOrange" x1="0" y1="0" x2="0" y2="1">
      <stop offset="0" stop-color="#FF6A1F"/><stop offset="1" stop-color="#E23E00"/>
    </linearGradient>
    <radialGradient id="cream" cx="0.5" cy="0.35" r="0.8">
      <stop offset="0" stop-color="#F3EACB"/><stop offset="1" stop-color="{CREAM}"/>
    </radialGradient>
    __MARKDEFS__
    <filter id="shadow" x="-20%" y="-20%" width="140%" height="140%">
      <feGaussianBlur in="SourceAlpha" stdDeviation="1.6"/><feOffset dy="1.4"/>
      <feComponentTransfer><feFuncA type="linear" slope="0.45"/></feComponentTransfer>
      <feMerge><feMergeNode/><feMergeNode in="SourceGraphic"/></feMerge>
    </filter>
  </defs>'''


# ---------------------------------------------------------------------------------------------------
# The Wolff Audio "W" mark, rebuilt geometrically from the logo artwork: a zigzag of two parallelograms
# with verticals at five x positions, four parallel diagonals and rounded corners.  Coordinates are the
# logo's own pixels (mark is about 293 x 157); use wolff_mark(x, y, width) to place it.
def wolff_mark_path(radius=5.6, box=None):
    m = -0.4935                                     # common slope of the four diagonals
    xL, x1, x2, x3, xR = 15.75, 42.75, 149.0, 176.2, 308.4
    top = 11.7
    line = lambda px, py: (lambda x: py + m*(x - px))      # diagonal through a measured midpoint
    A = line(84.3, 138.45); B = line(235.15, 129.1); C = line(240.7, 45.1); D = line(108.55, 44.65)
    V = [(xL, top), (x1, top), (x1, D(x1)), (x3, D(x3)), (x3, C(x3)), (xR, C(xR)),
         (xR, B(xR)), (x2, B(x2)), (x2, A(x2)), (xL, A(xL))]
    if box:                                         # fit the vertices to box=(x, y, w, h) before rounding
        ys_ = [p[1] for p in V]; y0_, h_ = min(ys_), max(ys_) - min(ys_)
        V = [(box[0] + (px - xL)/(xR - xL)*box[2], box[1] + (py - y0_)/h_*box[3]) for px, py in V]
    import math
    def toward(p, q, dist):
        dx, dy = q[0]-p[0], q[1]-p[1]; L = math.hypot(dx, dy); f = min(dist, L/2)/L
        return (p[0]+dx*f, p[1]+dy*f)
    n = len(V); d = ''
    for i, v in enumerate(V):
        a = toward(v, V[i-1], radius); b = toward(v, V[(i+1) % n], radius)
        d += (f'M {a[0]:.2f},{a[1]:.2f} ' if i == 0 else f'L {a[0]:.2f},{a[1]:.2f} ') + f'Q {v[0]:.2f},{v[1]:.2f} {b[0]:.2f},{b[1]:.2f} '
    ys = [p[1] for p in V]
    return d + 'Z', (xL, min(ys), xR - xL, max(ys) - min(ys))

def wolff_mark(x, y, width, fill='url(#wolffMark)'):
    d, (bx, by, bw, bh) = wolff_mark_path()
    k = width / bw
    return f'<path d="{d}" fill="{fill}" transform="translate({x:.3f} {y:.3f}) scale({k:.5f}) translate({-bx:.3f} {-by:.3f})"/>', bh*k

K_BOX = (14.779, Y0, 50.134, H)     # bounding box of KiCad's K in tile space (path5475 of icon_kicad.svg)

def wolff_mark_in_k_box(fill='url(#wolffMarkK)', radius=1.7):
    d, _ = wolff_mark_path(radius, K_BOX)
    return f'<path d="{d}" fill="{fill}"/>'

MARK_DEFS = """<linearGradient id="wolffMarkK" gradientUnits="userSpaceOnUse" x1="14.779" y1="50" x2="64.913" y2="50">
      <stop offset="0" stop-color="#F9330F"/><stop offset="0.25" stop-color="#F4550D"/><stop offset="0.5" stop-color="#FB8A14"/>
      <stop offset="0.75" stop-color="#FFA607"/><stop offset="1" stop-color="#FFBC0F"/>
    </linearGradient>
    <linearGradient id="wolffMark" gradientUnits="userSpaceOnUse" x1="15.75" y1="90" x2="308.4" y2="90">
      <stop offset="0" stop-color="#F9330F"/><stop offset="0.25" stop-color="#F4550D"/><stop offset="0.5" stop-color="#FB8A14"/>
      <stop offset="0.75" stop-color="#FFA607"/><stop offset="1" stop-color="#FFBC0F"/>
    </linearGradient>"""

DEFS = DEFS.replace('__MARKDEFS__', MARK_DEFS)

VARIANTS = {
    # name: (tile fill, frame colour or None, letter fill, W fill, dot fill)
    'A_black_cream':   ('url(#tileBlack)',  None,  'url(#cream)', 'url(#creamW)',       'url(#wolffOrange)'),
    'B_black_orangeW': ('url(#tileBlack)',  None,  'url(#cream)', 'url(#wolffOrangeW)', 'url(#wolffOrange)'),
    'C_black_framed':  ('url(#tileBlack)',  CREAM, 'url(#cream)', 'url(#wolffOrangeW)', 'url(#wolffOrange)'),
    'D_orange_black':  ('url(#tileOrange)', None,  BLACK,         BLACK,               CREAM),
}

def i_group(shift, letter_fill, dot_fill):
    return (f'<g transform="translate({shift:.3f} 0)"><path d="{I_STEM}" fill="{letter_fill}"/>'
            f'<circle cx="{I_DOT[0]}" cy="{I_DOT[1]}" r="{I_DOT[2]}" fill="{dot_fill}"/></g>')

def layout(name):
    cream, orange = 'url(#cream)', 'url(#wolffOrange)'
    if name == 'E_mark_is_W':            # the Wolff mark stands in for the W, KiCad's i beside it
        mw = 60.0; mk, mh = wolff_mark(0, 0, mw)
        mk, mh = wolff_mark(0, Y1 - mh, mw)                  # sits on the baseline
        ishift = (mw + 4.0) - 67.874383
        right = 85.014427 + ishift; cx = right/2; cy = (Y0 + Y1)/2 - 2
        sc = 76.0/right
        return (f'<g transform="translate(50 47.5) scale({sc:.4f}) translate({-cx:.3f} {-cy:.3f})">{mk}'
                f'{i_group(ishift, cream, orange)}</g>')
    if name == 'K_mark_in_K_box':        # KiCad's own layout: the Wolff mark fills the K's box, the i is untouched
        return wolff_mark_in_k_box() + i_group(0, cream, orange)
    if name == 'H_mark_centred':         # as E, larger mark centred on the i's stem
        mw = 72.0; mk, mh = wolff_mark(0, 0, mw)
        mk, mh = wolff_mark(0, (41.16 + Y1)/2 - mh/2, mw)
        ishift = (mw + 5.0) - 67.874383
        right = 85.014427 + ishift; cx = right/2; cy = (Y0 + Y1)/2 - 2
        sc = 78.0/right
        return (f'<g transform="translate(50 47.5) scale({sc:.4f}) translate({-cx:.3f} {-cy:.3f})">{mk}'
                f'{i_group(ishift, cream, orange)}</g>')
    if name == 'F_Wi_over_mark':          # serif Wi on top, the Wolff mark as a band beneath
        mk, mh = wolff_mark(50 - 27, 66.5, 54.0)
        return f'<g transform="translate(50 37) scale(0.74) translate(-50 -51.5)">{mark(cream, "url(#wolffOrangeW)", orange)}</g>{mk}'
    if name == 'G_Wi_with_stamp':         # serif Wi full size, small Wolff mark as a maker's stamp
        mk, mh = wolff_mark(50 - 13, 84.2, 26.0)
        return f'<g transform="translate(0 -5.5)">{mark(cream, "url(#wolffOrangeW)", orange)}</g>{mk}'
    raise KeyError(name)

LAYOUTS = ['K_mark_in_K_box', 'E_mark_is_W', 'H_mark_centred', 'F_Wi_over_mark', 'G_Wi_with_stamp']

def svg_layout(name):
    k = 8.24; off = 100
    return f'''<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 1024 1024" width="1024" height="1024">
  {DEFS}
  <g transform="translate({off} {off}) scale({k})">
    <rect width="100" height="100" rx="22.4" fill="{BLACK}" filter="url(#shadow)"/>
    {layout(name)}
  </g>
</svg>
'''

def svg(variant, macos=True):
    tile, frame, lf, wf, df = VARIANTS[variant]
    # macOS icon grid: 1024 canvas, 824 tile, corner radius ~185, soft shadow (as KiCad's own .icns)
    k = 8.24; off = 100
    fr = (f'<rect x="6.2" y="6.2" width="87.6" height="87.6" rx="17" fill="none" stroke="{frame}" stroke-width="1.5"/>'
          if frame else '')
    return f'''<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 1024 1024" width="1024" height="1024">
  {DEFS}
  <g transform="translate({off} {off}) scale({k})">
    <rect width="100" height="100" rx="22.4" fill="{tile}" filter="url(#shadow)"/>
    {fr}
    {mark(lf, wf, df)}
  </g>
</svg>
'''

def svg_flat(name):
    """In-app icon: tile fills the canvas, no shadow (like KiCad's icon_kicad.svg)."""
    return f'''<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 100 100" width="48" height="48">
  {DEFS}
  <rect width="100" height="100" rx="22.4" fill="{BLACK}"/>
  {layout(name)}
</svg>
'''

if __name__ == '__main__':
    here = os.path.dirname(os.path.abspath(__file__))
    for v in VARIANTS:
        open(os.path.join(here, f'wicad_{v}.svg'), 'w').write(svg(v))
        print('wrote', f'wicad_{v}.svg')
    for v in LAYOUTS:
        open(os.path.join(here, f'wicad_{v}.svg'), 'w').write(svg_layout(v))
        print('wrote', f'wicad_{v}.svg')
    open(os.path.join(here, 'wicad_app.svg'), 'w').write(svg_layout('K_mark_in_K_box'))
    open(os.path.join(here, 'wicad_inapp.svg'), 'w').write(svg_flat('K_mark_in_K_box'))
