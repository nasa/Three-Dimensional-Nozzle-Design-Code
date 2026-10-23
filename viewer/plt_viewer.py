#!/usr/bin/env python3
"""
plt_viewer - view the Tecplot (.plt) files, ASCII or binary, written by MOC_Grid, STT2001 and 3D_MOC.

No Tecplot licence is needed. The only requirements are numpy and matplotlib.

Examples
--------
    python plt_viewer.py path\\to\\run_folder                 # every .plt -> PNGs in run_folder\\images
    python plt_viewer.py Summary.plt                          # wall contour + Mach/P/theta along the wall
    python plt_viewer.py MOC_Grid.plt --mirror                # characteristic net coloured by Mach
    python plt_viewer.py MOC_SL.plt --var Pres                # meridional streamlines coloured by pressure
    python plt_viewer.py mynozzle.plt --mode 3d --var Pressure   # STT2001 trimmed nozzle surface
    python plt_viewer.py Wall.plt --mode 3d --var Mach        # 3D_MOC wall surface
    python plt_viewer.py full_mesh.plt                        # 3D_MOC flowfield (axial/radial + exit plane)
    python plt_viewer.py Summary.plt --list                   # list the zones and variables only
    python plt_viewer.py run_folder --show                    # same, and also open the windows
    python plt_viewer.py run_folder --save pngs               # PNGs somewhere else

Both Tecplot ASCII files and Tecplot binary files (they start with ``#!TDV``) are read.
ASCII: a ``VARIABLES = "a","b",...`` header, then one or more ``zone t="name" I=.. J=.. K=..``
blocks of whitespace separated numbers (I runs fastest). Binary: ordered (structured) zones of
format versions 7.5 (``#!TDV75``) and 10.8-11.2 (``#!TDV108`` .. ``#!TDV112``, written by current
Tecplot 360). Finite-element zones in binary files are skipped, with a note.
"""
from __future__ import annotations

import argparse
import math
import re
import struct
import sys
from collections import Counter
from dataclasses import dataclass, field
from pathlib import Path
from typing import Iterable, Optional, Sequence

import numpy as np

plt = None  # matplotlib.pyplot, imported lazily by _load_pyplot() so the parser is usable headless


# --------------------------------------------------------------------------------------
#  Reading Tecplot ASCII files
# --------------------------------------------------------------------------------------
class PltFormatError(Exception):
    """The file is not a Tecplot ASCII file that this viewer understands."""


@dataclass
class Zone:
    title: str
    dims: tuple[int, int, int]          # (I, J, K); I varies fastest in the data
    data: np.ndarray                    # shape (n_points, n_variables)

    @property
    def n(self) -> int:
        return self.data.shape[0]

    def grid(self, column: int) -> np.ndarray:
        """One variable reshaped to (K, J, I)."""
        i, j, k = self.dims
        return self.data[:, column].reshape(k, j, i)


@dataclass
class PltFile:
    path: Path
    title: str
    variables: list[str]
    zones: list[Zone] = field(default_factory=list)
    notes: list[str] = field(default_factory=list)       # things the reader skipped or assumed

    def find(self, *candidates: str) -> Optional[int]:
        """Index of the first variable whose lower-case name equals/starts with a candidate."""
        names = [v.lower().replace(" ", "") for v in self.variables]
        for cand in candidates:
            cand = cand.lower()
            for idx, name in enumerate(names):
                if name == cand:
                    return idx
        for cand in candidates:
            cand = cand.lower()
            for idx, name in enumerate(names):
                if name.startswith(cand):
                    return idx
        return None

    def require(self, *candidates: str) -> int:
        idx = self.find(*candidates)
        if idx is None:
            raise PltFormatError(f"{self.path.name}: no variable like {candidates[0]!r} "
                                 f"(has: {', '.join(self.variables)})")
        return idx


_QUOTED = re.compile(r'"([^"]*)"')
_NUMBER_START = re.compile(r"^[0-9+\-.]|^(nan|inf)", re.IGNORECASE)


def _to_float(token: str) -> float:
    try:
        return float(token)
    except ValueError:
        return math.nan                 # e.g. "-nan(ind)" written by the Windows runtime


def _parse_zone_header(line: str) -> tuple[str, tuple[int, int, int]]:
    m = re.search(r't\s*=\s*"([^"]*)"', line, re.IGNORECASE)
    title = m.group(1) if m else ""
    rest = re.sub(r't\s*=\s*"[^"]*"', "", line, flags=re.IGNORECASE)   # the title may contain "J = 0"
    dims = []
    for axis in "IJK":
        m = re.search(rf"\b{axis}\s*=\s*(\d+)", rest, re.IGNORECASE)
        dims.append(int(m.group(1)) if m else 1)
    return title, (dims[0], dims[1], dims[2])


def _finish_zone(title: str, dims: tuple[int, int, int], rows: list[list[float]],
                 n_vars: int) -> Optional[Zone]:
    if not rows:
        return None
    width = Counter(len(r) for r in rows).most_common(1)[0][0]
    rows = [r for r in rows if len(r) == width]
    data = np.array(rows, dtype=float).reshape(len(rows), width)
    if int(np.prod(dims)) != data.shape[0]:
        dims = (data.shape[0], 1, 1)    # header missing or wrong: treat as one line of points
    return Zone(title, dims, data)


def read_plt(path: Path | str) -> PltFile:
    path = Path(path)
    raw = path.read_bytes()
    if raw[:5] == b"#!TDV":
        return _read_binary(path, raw)
    if b"\x00" in raw[:4096]:
        raise PltFormatError("not a Tecplot ASCII or binary file")
    text = raw.decode("latin-1")

    title = path.stem
    variables: list[str] = []
    zones: list[Zone] = []
    cur_title, cur_dims, cur_rows = "", (1, 1, 1), []
    have_zone = False

    for line in text.splitlines():
        s = line.strip()
        if not s:
            continue
        low = s.lower()
        if low.startswith("title"):
            q = _QUOTED.findall(s)
            if q:
                title = q[0]
        elif low.startswith("variables"):
            variables = _QUOTED.findall(s) or [v.strip() for v in s.split("=", 1)[1].split(",")]
        elif low.startswith("zone"):
            if have_zone:
                z = _finish_zone(cur_title, cur_dims, cur_rows, len(variables))
                if z is not None:
                    zones.append(z)
            cur_title, cur_dims = _parse_zone_header(s)
            cur_rows, have_zone = [], True
        elif _NUMBER_START.match(s):
            if not have_zone:           # data without a ZONE line
                cur_title, cur_dims, cur_rows, have_zone = "data", (1, 1, 1), [], True
            cur_rows.append([_to_float(t) for t in s.replace(",", " ").split()])
        # anything else (TEXT, GEOMETRY, DATAPACKING ...) is annotation and is ignored
    if have_zone:
        z = _finish_zone(cur_title, cur_dims, cur_rows, len(variables))
        if z is not None:
            zones.append(z)

    if not zones:
        raise PltFormatError("no numeric data found")
    n_cols = zones[0].data.shape[1]
    # tolerate a VARIABLES line that does not match the number of columns
    variables = (variables + [f"col{k + 1}" for k in range(len(variables), n_cols)])[:n_cols]
    return PltFile(path, title, variables, zones)


# --------------------------------------------------------------------------------------
#  Reading Tecplot binary files
# --------------------------------------------------------------------------------------
_ZONE, _GEOM, _TEXT, _LABELS, _USERREC, _DSAUX, _EOH = 299.0, 399.0, 499.0, 599.0, 799.0, 899.0, 357.0
_DTYPES = {1: "f4", 2: "f8", 3: "i4", 4: "i2", 5: "u1"}      # Tecplot data-format codes
_NODES_PER_ELEMENT = {1: 2, 2: 3, 3: 4, 4: 4, 5: 8}          # FE line, triangle, quad, tet, brick


class _Cursor:
    """Sequential reader of little/big endian values."""

    def __init__(self, raw: bytes, endian: str, pos: int = 0):
        self.raw, self.endian, self.pos = raw, endian, pos

    def _take(self, fmt: str, size: int):
        if self.pos + size > len(self.raw):
            raise PltFormatError("binary file ends unexpectedly")
        value = struct.unpack_from(self.endian + fmt, self.raw, self.pos)[0]
        self.pos += size
        return value

    def i32(self) -> int:
        return self._take("i", 4)

    def f32(self) -> float:
        return self._take("f", 4)

    def f64(self) -> float:
        return self._take("d", 8)

    def string(self) -> str:                   # Tecplot stores one character per 32-bit integer
        chars = []
        while True:
            c = self.i32()
            if c == 0:
                return "".join(chars)
            if not 0 < c < 0x110000 or len(chars) > 4096:
                raise PltFormatError("unreadable string in binary file")
            chars.append(chr(c))


def _next_marker(raw: bytes, start: int, endian: str, markers: Sequence[float]) -> int:
    """Offset of the first 4-byte aligned record marker at or after start (-1 if none)."""
    best = -1
    for m in markers:
        pat = struct.pack(endian + "f", m)
        pos = raw.find(pat, start)
        while pos != -1 and pos % 4:
            pos = raw.find(pat, pos + 1)
        if pos != -1 and (best == -1 or pos < best):
            best = pos
    return best


def _read_binary(path: Path, raw: bytes) -> PltFile:
    m = re.match(rb"#!TDV(\d+)", raw)
    if not m or len(raw) < 16:
        raise PltFormatError("not a Tecplot binary file")
    version = int(m.group(1))
    if version != 75 and not 108 <= version <= 199:
        raise PltFormatError(f"unsupported Tecplot binary version {version} "
                             "(supported: 75 and 108-112)")
    order = struct.unpack_from("<i", raw, 8)[0]
    if order == 1:
        endian = "<"
    elif struct.unpack_from(">i", raw, 8)[0] == 1:
        endian = ">"
    else:
        raise PltFormatError("bad byte-order mark in binary file")
    cur = _Cursor(raw, endian, 12)
    if version >= 108:
        cur.i32()                              # file type: full / grid / solution
    title = cur.string() or path.stem
    n_vars = cur.i32()
    if not 1 <= n_vars <= 500:
        raise PltFormatError("implausible number of variables in binary file")
    variables = [cur.string() for _ in range(n_vars)]

    # ---- header records, up to the end-of-header marker
    zone_heads = []
    notes: list[str] = []
    every = (_ZONE, _GEOM, _TEXT, _LABELS, _USERREC, _DSAUX, _EOH)
    while True:
        marker = cur.f32()
        if marker == _EOH:
            break
        if marker == _ZONE:
            zone_heads.append(_read_zone_header(cur, version, n_vars))
        elif marker in (_GEOM, _TEXT, _LABELS, _USERREC, _DSAUX):   # annotations: skip
            nxt = _next_marker(raw, cur.pos, endian, every)
            if nxt == -1:
                raise PltFormatError("could not find the end of the header")
            cur.pos = nxt
        else:
            raise PltFormatError(f"unexpected record marker {marker} in header")

    # ---- data sections, one per zone
    zones: list[Zone] = []
    skipped = 0
    for head in zone_heads:
        marker = cur.f32()
        if marker != _ZONE:
            raise PltFormatError("zone data not found where expected")
        arrays = _read_zone_data(cur, version, head, n_vars)
        if head["type"] != 0:                  # finite-element zone: not drawn
            skipped += 1
            continue
        i, j, k = head["dims"]
        data = np.column_stack([a.astype(float) for a in arrays])
        zones.append(Zone(head["name"], (i, j, k), data))
    if skipped:
        notes.append(f"{skipped} finite-element zone(s) were skipped (only structured zones are drawn)")
    if not zones:
        raise PltFormatError("no structured zones found in binary file")
    return PltFile(path, title, variables, zones, notes)


def _read_zone_header(cur: _Cursor, version: int, n_vars: int) -> dict:
    head: dict = {"name": cur.string(), "var_loc": [0] * n_vars}
    if version == 75:
        head["type"] = cur.i32()               # 0 ordered, 1 line, 2 triangle, 3 quad, 4 tet, 5 brick
        cur.i32()                              # -1 (zone colour)
        if head["type"] == 0:
            head["dims"] = (cur.i32(), cur.i32(), cur.i32())
        else:
            head["numpts"], head["numelems"] = cur.i32(), cur.i32()
            cur.i32()
            head["dims"] = (head["numpts"], 1, 1)
        return head
    cur.i32()                                  # parent zone
    cur.i32()                                  # strand id
    cur.f64()                                  # solution time
    cur.i32()                                  # -1 (zone colour)
    head["type"] = cur.i32()
    if cur.i32():                              # variable locations are specified
        head["var_loc"] = [cur.i32() for _ in range(n_vars)]
    cur.i32()                                  # raw local 1-to-1 face neighbours supplied
    if cur.i32():                              # user-defined face neighbour connections
        raise PltFormatError("zones with user-defined face neighbours are not supported")
    if head["type"] == 0:
        head["dims"] = (cur.i32(), cur.i32(), cur.i32())
    elif head["type"] <= 5:
        head["numpts"], head["numelems"] = cur.i32(), cur.i32()
        cur.i32(), cur.i32(), cur.i32()        # cell dimensions
        head["dims"] = (head["numpts"], 1, 1)
    else:
        raise PltFormatError("polygonal/polyhedral zones are not supported")
    while cur.i32() == 1:                      # auxiliary name/value pairs
        cur.string()
        cur.i32()
        cur.string()
    return head


def _read_zone_data(cur: _Cursor, version: int, head: dict, n_vars: int) -> list[np.ndarray]:
    npts = int(np.prod(head["dims"]))
    if version == 75:
        cur.i32()                              # 0
        formats = [cur.i32() for _ in range(n_vars)]
        passive = [0] * n_vars
        shared = [-1] * n_vars
        share_connectivity = -1
    else:
        formats = [cur.i32() for _ in range(n_vars)]
        passive = [cur.i32() for _ in range(n_vars)] if cur.i32() else [0] * n_vars
        shared = [cur.i32() for _ in range(n_vars)] if cur.i32() else [-1] * n_vars
        share_connectivity = cur.i32()
        for v in range(n_vars):                # minimum and maximum of each stored variable
            if not passive[v] and shared[v] == -1:
                cur.f64()
                cur.f64()
    if any(loc == 1 for loc in head["var_loc"]):
        raise PltFormatError("cell-centred variables are not supported")
    arrays = []
    for v in range(n_vars):
        if passive[v] or shared[v] != -1:
            arrays.append(np.zeros(npts))     # not stored in this zone
            continue
        code = formats[v]
        if code not in _DTYPES:
            raise PltFormatError(f"unsupported variable format {code} in binary file")
        dt = np.dtype(cur.endian + _DTYPES[code])
        nbytes = npts * dt.itemsize
        if cur.pos + nbytes > len(cur.raw):
            raise PltFormatError("binary file ends in the middle of the data")
        arrays.append(np.frombuffer(cur.raw, dtype=dt, count=npts, offset=cur.pos))
        cur.pos += nbytes
    if head["type"] != 0 and share_connectivity == -1:      # connectivity list follows the data
        nodes = _NODES_PER_ELEMENT.get(head["type"], 0)
        cur.pos += 4 * nodes * head["numelems"]
        if version == 75:
            cur.pos += 4                       # one extra word follows in the 7.5 files seen
    return arrays


# --------------------------------------------------------------------------------------
#  Recognising which program wrote the file
# --------------------------------------------------------------------------------------
def detect_kind(f: PltFile) -> str:
    title = f.title.lower()
    names = [v.lower() for v in f.variables]
    if "rrc contours" in title or (names[:2] == ["x/r", "r/r"] and "massflow" in names and len(f.zones) > 20):
        return "moc_grid"
    if "primary moc contour" in title:
        return "summary"
    if "streamline data" in title or (names[:4] == ["x/r", "y/r", "z/r", "r/r"]):
        return "moc_sl"
    if all(f.find(c) is not None for c in ("x(in)", "y(in)", "z(in)")):
        return "3d"
    return "generic"


def default_mode(f: PltFile) -> str:
    kind = detect_kind(f)
    if kind == "moc_grid":
        return "grid"
    if kind == "summary":
        return "summary"
    if kind == "moc_sl":
        return "streamlines"
    if kind == "3d":
        return "field" if "mesh" in f.path.stem.lower() else "3d"
    return "xy"


# --------------------------------------------------------------------------------------
#  Plotting helpers
# --------------------------------------------------------------------------------------
def _load_pyplot(headless: bool) -> None:
    global plt
    import matplotlib
    if headless:
        matplotlib.use("Agg")
    import matplotlib.pyplot as _plt
    _plt.rcParams.update({
        "figure.facecolor": "white", "axes.facecolor": "#fbfbfc",
        "axes.grid": True, "grid.alpha": 0.25, "grid.linewidth": 0.6,
        "axes.spines.top": False, "axes.spines.right": False,
        "axes.titleweight": "semibold", "axes.titlesize": 11, "axes.labelsize": 10,
        "font.size": 9.5, "figure.dpi": 110, "savefig.dpi": 200,
        "legend.frameon": False,
    })
    plt = _plt


def _pick_color_var(f: PltFile, requested: Optional[str], *defaults: str) -> int:
    if requested:
        idx = f.find(requested)
        if idx is None:
            raise PltFormatError(f"variable {requested!r} not found. Available: {', '.join(f.variables)}")
        return idx
    for cand in defaults:
        idx = f.find(cand)
        if idx is not None:
            return idx
    return len(f.variables) - 1


def _segments(x: np.ndarray, y: np.ndarray, v: np.ndarray):
    pts = np.column_stack([x, y])
    seg = np.stack([pts[:-1], pts[1:]], axis=1)
    return seg, 0.5 * (v[:-1] + v[1:])


def _line_collection(ax, segs, vals, cmap, lw=1.2, vmin=None, vmax=None, alpha=1.0):
    from matplotlib.collections import LineCollection
    lc = LineCollection(segs, cmap=cmap, linewidths=lw, alpha=alpha)
    lc.set_array(np.asarray(vals))
    if vmin is not None:
        lc.set_clim(vmin, vmax)
    ax.add_collection(lc)
    return lc


def _finite_range(values: Iterable[np.ndarray]) -> tuple[float, float]:
    arr = np.concatenate([np.asarray(v).ravel() for v in values])
    arr = arr[np.isfinite(arr)]
    if arr.size == 0:
        return 0.0, 1.0
    lo, hi = float(arr.min()), float(arr.max())
    return (lo, hi) if hi > lo else (lo - 0.5, hi + 0.5)


# --------------------------------------------------------------------------------------
#  Views
# --------------------------------------------------------------------------------------
def view_grid(f: PltFile, var: Optional[str], mirror: bool, cmap: str):
    """MOC_Grid.plt: the right-running characteristic net, coloured by Mach (or any variable)."""
    ix, ir = f.require("x/r", "x"), f.require("r/r", "r")
    ic = _pick_color_var(f, var, "mach")
    segs, vals = [], []
    for z in f.zones:
        if z.n < 2:
            continue
        s, v = _segments(z.data[:, ix], z.data[:, ir], z.data[:, ic])
        segs.append(s)
        vals.append(v)
    segs, vals = np.concatenate(segs), np.concatenate(vals)
    vmin, vmax = _finite_range([vals])

    fig, ax = plt.subplots(figsize=(11, 5.2 if mirror else 4.2), layout="constrained")
    lc = _line_collection(ax, segs, vals, cmap, lw=0.7, vmin=vmin, vmax=vmax)
    if mirror:
        _line_collection(ax, segs * np.array([1, -1]), vals, cmap, lw=0.7, vmin=vmin, vmax=vmax)
    wall = np.array([[z.data[0, ix], z.data[0, ir]] for z in f.zones])
    ax.plot(wall[:, 0], wall[:, 1], color="black", lw=1.8, label="wall")
    if mirror:
        ax.plot(wall[:, 0], -wall[:, 1], color="black", lw=1.8)
    ax.autoscale()
    ax.set_aspect("equal", adjustable="datalim")
    ax.set_xlabel(f.variables[ix])
    ax.set_ylabel(f.variables[ir])
    ax.set_title(f"MOC characteristic net  -  {len(f.zones)} right-running characteristics")
    fig.colorbar(lc, ax=ax, pad=0.01, shrink=0.9, label=f.variables[ic])
    return fig


def view_summary(f: PltFile, var: Optional[str], mirror: bool, cmap: str):
    """Summary.plt: nozzle contour plus properties along the wall."""
    ix, ir = f.require("x/r", "x"), f.require("r/r", "r")
    wall = next((z for z in f.zones if "wall" in z.title.lower()), f.zones[0])
    fig = plt.figure(figsize=(12, 7), layout="constrained")
    gs = fig.add_gridspec(2, 3, height_ratios=[1.15, 1])

    ax = fig.add_subplot(gs[0, :])
    colors = {"wall": "#1f2937", "rrc": "#2563eb", "lrc": "#dc2626"}
    for z in f.zones:
        t = z.title.lower()
        key = "wall" if "wall" in t else "rrc" if "rrc" in t else "lrc" if "lrc" in t else "wall"
        lw = 2.2 if key == "wall" else 1.3
        ax.plot(z.data[:, ix], z.data[:, ir], color=colors[key], lw=lw, label=z.title)
        if mirror:
            ax.plot(z.data[:, ix], -z.data[:, ir], color=colors[key], lw=lw)
    ax.axhline(0, color="#9ca3af", lw=0.8, ls="--")
    ax.set_aspect("equal", adjustable="datalim")
    ax.set_xlabel(f.variables[ix])
    ax.set_ylabel(f.variables[ir])
    ax.set_title("Nozzle contour and the characteristics that define the exit (BD, DE)")
    ax.legend(loc="upper left", ncols=3)

    panels = [("mach", "Mach number"), ("pres", "Static pressure"), ("theta", "Wall angle")]
    for k, (cand, title) in enumerate(panels):
        idx = f.find(cand)
        axp = fig.add_subplot(gs[1, k])
        if idx is not None:
            axp.plot(wall.data[:, ix], wall.data[:, idx], color="#0f766e", lw=1.8)
            axp.set_ylabel(f.variables[idx])
        axp.set_xlabel(f.variables[ix])
        axp.set_title(f"{title} along the wall")
    return fig


def view_streamlines(f: PltFile, var: Optional[str], mirror: bool, cmap: str):
    """MOC_SL.plt: the meridional streamlines (one per mass-flow fraction)."""
    ix = f.require("x/r", "x")
    ir = f.find("r/r", "r")
    ic = _pick_color_var(f, var, "mach")
    all_vals = [z.data[: z.dims[0], ic] for z in f.zones]
    vmin, vmax = _finite_range(all_vals)

    fig, ax = plt.subplots(figsize=(11, 5.4 if mirror else 4.6), layout="constrained")
    lc = None
    for z in f.zones:
        n = z.dims[0] if z.dims[0] > 1 else z.n          # first ray of the zone = meridional line
        d = z.data[:n]
        d = d[np.argsort(d[:, ix], kind="stable")]         # the first point of a ray can be out of order
        r = d[:, ir] if ir is not None else np.hypot(d[:, 1], d[:, 2])
        s, v = _segments(d[:, ix], r, d[:, ic])
        lc = _line_collection(ax, s, v, cmap, lw=1.8, vmin=vmin, vmax=vmax)
        if mirror:
            _line_collection(ax, s * np.array([1, -1]), v, cmap, lw=1.8, vmin=vmin, vmax=vmax)
        label = re.search(r"=\s*([\d.]+)", z.title)
        if label and np.isfinite(d[-1, ix]):
            ax.annotate(f"{label.group(1)}%", (d[-1, ix], r[-1]), xytext=(4, 0),
                        textcoords="offset points", fontsize=8, color="#374151", va="center")
    ax.autoscale()
    ax.set_aspect("equal", adjustable="datalim")
    ax.set_xlabel(f.variables[ix])
    ax.set_ylabel(f.variables[ir] if ir is not None else "R")
    ax.set_title("Streamlines at fixed fractions of the mass flow (outermost = wall)")
    if lc is not None:
        fig.colorbar(lc, ax=ax, pad=0.01, shrink=0.9, label=f.variables[ic])
    return fig


def view_3d(f: PltFile, var: Optional[str], mirror: bool, cmap: str,
            elev: float = 24, azim: float = -58):
    """Any file with three coordinates: structured zones become surfaces, 1-D zones become lines."""
    from matplotlib import cm
    from matplotlib.colors import Normalize
    from mpl_toolkits.mplot3d.art3d import Line3DCollection

    ix = f.find("x(in)", "x/r", "x")
    iy = f.find("y(in)", "y/r", "y")
    iz = f.find("z(in)", "z/r", "z")
    if None in (ix, iy, iz):
        raise PltFormatError("3d mode needs X, Y and Z variables in the file")
    ic = _pick_color_var(f, var, "mach", "pressure", "pres", "p(psia)")
    vmin, vmax = _finite_range([z.data[:, ic] for z in f.zones])
    norm, cmap_obj = Normalize(vmin, vmax), plt.get_cmap(cmap)

    fig = plt.figure(figsize=(10, 7.2), layout="constrained")
    ax = fig.add_subplot(111, projection="3d")
    all_pts = []
    line_segs, line_vals = [], []
    for z in f.zones:
        i, j, k = z.dims
        big = [n for n in (i, j, k) if n > 1]
        xyz = z.data[:, [ix, iy, iz]]
        all_pts.append(xyz)
        if len(big) >= 2:                                   # a surface
            shape = [n for n in (k, j, i) if n > 1]        # (slow, fast)
            if len(shape) == 3:                             # a real volume: draw its outer k-planes only
                shape = [j if j > 1 else k, i]
            X, Y, Z = (xyz[: shape[0] * shape[1], c].reshape(shape) for c in range(3))
            V = z.data[: shape[0] * shape[1], ic].reshape(shape)
            rs, cs = max(1, shape[0] // 160), max(1, shape[1] // 160)
            X, Y, Z, V = X[::rs, ::cs], Y[::rs, ::cs], Z[::rs, ::cs], V[::rs, ::cs]
            ax.plot_surface(X, Y, Z, facecolors=cmap_obj(norm(V)), shade=False,
                            rstride=1, cstride=1, linewidth=0, antialiased=False)
        elif len(big) == 1 and z.n > 1:                     # a polyline
            pts = xyz
            line_segs.append(np.stack([pts[:-1], pts[1:]], axis=1))
            line_vals.append(0.5 * (z.data[:-1, ic] + z.data[1:, ic]))
    if line_segs:
        lc = Line3DCollection(np.concatenate(line_segs), cmap=cmap_obj, norm=norm, linewidths=1.3)
        lc.set_array(np.concatenate(line_vals))
        ax.add_collection3d(lc)

    pts = np.concatenate(all_pts)
    pts = pts[np.isfinite(pts).all(axis=1)]
    lo, hi = pts.min(axis=0), pts.max(axis=0)
    span = np.maximum(hi - lo, 0.04 * max(float((hi - lo).max()), 1e-9))
    ax.set_xlim(lo[0], lo[0] + span[0])
    ax.set_ylim(lo[1], lo[1] + span[1])
    ax.set_zlim(lo[2], lo[2] + span[2])
    ax.set_box_aspect(span)
    ax.set_proj_type("ortho")                               # no perspective distortion on long nozzles
    ax.view_init(elev=elev, azim=azim)
    ax.set_xlabel(f.variables[ix])
    ax.set_ylabel(f.variables[iy])
    ax.set_zlabel(f.variables[iz])
    ax.set_title(f"{f.title}  -  coloured by {f.variables[ic]}")
    fig.colorbar(cm.ScalarMappable(norm=norm, cmap=cmap_obj), ax=ax, shrink=0.6, pad=0.08,
                 label=f.variables[ic])
    return fig


def view_field(f: PltFile, var: Optional[str], mirror: bool, cmap: str):
    """3D_MOC full_mesh-style files: axial/radial view of the flowfield plus the exit plane."""
    ix, iy, iz = f.require("x(in)", "x"), f.require("y(in)", "y"), f.require("z(in)", "z")
    ir = f.find("r(in)", "radius")
    ic = _pick_color_var(f, var, "mach", "pressure", "p(psia)")
    data = np.concatenate([z.data for z in f.zones])
    data = data[np.isfinite(data[:, [ix, iy, iz, ic]]).all(axis=1)]
    rad = data[:, ir] if ir is not None else np.hypot(data[:, ix], data[:, iy])
    vmin, vmax = _finite_range([data[:, ic]])

    fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(13, 5), layout="constrained",
                                   gridspec_kw={"width_ratios": [2.2, 1]})
    order = np.argsort(data[:, ic])                           # draw high values last
    sc = ax1.scatter(data[order, iz], rad[order], c=data[order, ic], s=6, cmap=cmap,
                     vmin=vmin, vmax=vmax, linewidths=0, rasterized=True)
    ax1.set_xlabel(f.variables[iz])
    ax1.set_ylabel(f.variables[ir] if ir is not None else "radius")
    ax1.set_title("Flowfield, axial station vs radius (all points)")
    ax1.set_aspect("equal", adjustable="datalim")

    zmax = data[:, iz].max()
    tol = 0.003 * max(data[:, iz].max() - data[:, iz].min(), 1e-9)
    sel = data[:, iz] >= zmax - tol
    if sel.sum() >= 3:
        ax2.scatter(data[sel, ix], data[sel, iy], c=data[sel, ic], s=14, cmap=cmap, vmin=vmin,
                    vmax=vmax, linewidths=0)
    ax2.set_aspect("equal", adjustable="datalim")
    ax2.set_xlabel(f.variables[ix])
    ax2.set_ylabel(f.variables[iy])
    ax2.set_title(f"Exit plane ({f.variables[iz]} = {zmax:.3g})")
    fig.colorbar(sc, ax=[ax1, ax2], pad=0.01, shrink=0.9, label=f.variables[ic])
    return fig


def view_xy(f: PltFile, var: Optional[str], mirror: bool, cmap: str):
    """Fallback: first variable on the x axis, any other variable on y, one line per zone."""
    iy = _pick_color_var(f, var, f.variables[1] if len(f.variables) > 1 else f.variables[0])
    fig, ax = plt.subplots(figsize=(9, 5), layout="constrained")
    many = len(f.zones) > 12
    for z in f.zones:
        ax.plot(z.data[:, 0], z.data[:, iy], lw=1.0 if many else 1.6,
                label=None if many else z.title)
    ax.set_xlabel(f.variables[0])
    ax.set_ylabel(f.variables[iy])
    ax.set_title(f.title)
    if not many:
        ax.legend()
    return fig


VIEWS = {
    "grid": view_grid, "summary": view_summary, "streamlines": view_streamlines,
    "3d": view_3d, "field": view_field, "xy": view_xy,
}


def make_figure(f: PltFile, mode: str = "auto", var: Optional[str] = None, mirror: bool = False,
                cmap: str = "turbo", elev: float = 24, azim: float = -58):
    if mode == "auto":
        mode = default_mode(f)
    if mode not in VIEWS:
        raise PltFormatError(f"unknown mode {mode!r}; choose from auto, {', '.join(VIEWS)}")
    fig = (view_3d(f, var, mirror, cmap, elev, azim) if mode == "3d"
           else VIEWS[mode](f, var, mirror, cmap))
    fig.canvas.manager.set_window_title(f"{f.path.name} [{mode}]") if fig.canvas.manager else None
    return fig


# --------------------------------------------------------------------------------------
#  Command line
# --------------------------------------------------------------------------------------
def describe(f: PltFile) -> str:
    lines = [f"{f.path}", f"  title     : {f.title}", f"  kind      : {detect_kind(f)} "
             f"(default view: {default_mode(f)})",
             f"  variables : {', '.join(f'{i}:{v}' for i, v in enumerate(f.variables))}",
             f"  zones     : {len(f.zones)}"]
    for z in f.zones[:8]:
        lines.append(f"    - {z.title or '(unnamed)'}  I={z.dims[0]} J={z.dims[1]} K={z.dims[2]}  "
                     f"({z.n} points)")
    if len(f.zones) > 8:
        lines.append(f"    ... and {len(f.zones) - 8} more")
    for note in f.notes:
        lines.append(f"  note      : {note}")
    return "\n".join(lines)


def collect_inputs(paths: Sequence[str]) -> list[tuple[Path, Optional[Path]]]:
    """(file, folder it was found in). The folder is None for files named directly."""
    found: list[tuple[Path, Optional[Path]]] = []
    for p in map(Path, paths):
        if p.is_dir():
            found.extend((q, p) for q in sorted(p.iterdir()) if q.suffix.lower() == ".plt")
        elif p.is_file():
            found.append((p, None))
        else:
            print(f"warning: {p} does not exist", file=sys.stderr)
    return found


def build_parser() -> argparse.ArgumentParser:
    ap = argparse.ArgumentParser(
        prog="plt_viewer",
        description="View Tecplot (ASCII and binary) .plt files from MOC_Grid, STT2001 and 3D_MOC. "
                    "Give it a run folder and it writes one PNG per .plt file into <folder>/images.",
        epilog="See the top of this file (or the repository README) for examples.")
    ap.add_argument("paths", nargs="+", help=".plt files and/or run folders containing them")
    ap.add_argument("--mode", default="auto",
                    help="auto (default), grid, summary, streamlines, 3d, field or xy")
    ap.add_argument("--var", help="variable used for colour / y axis (name or prefix, e.g. Mach, Pres)")
    ap.add_argument("--mirror", action="store_true", help="also draw the lower half of axisymmetric views")
    ap.add_argument("--cmap", default="turbo", help="matplotlib colormap (default: turbo)")
    ap.add_argument("--elev", type=float, default=24, help="3d view elevation, degrees")
    ap.add_argument("--azim", type=float, default=-58, help="3d view azimuth, degrees")
    ap.add_argument("--list", action="store_true", help="only print the zones and variables of each file")
    ap.add_argument("--save", metavar="PATH",
                    help="save PNG(s) here instead: a file name for one input, otherwise a folder")
    ap.add_argument("--show", action="store_true",
                    help="also open interactive windows when a folder is given (they are skipped by default)")
    ap.add_argument("--no-show", action="store_true", help="never open windows")
    return ap


def main(argv: Optional[Sequence[str]] = None) -> int:
    args = build_parser().parse_args(argv)
    inputs = collect_inputs(args.paths)
    if not inputs:
        print("No .plt files found.", file=sys.stderr)
        return 2

    parsed: list[tuple[PltFile, Optional[Path]]] = []
    for p, src in inputs:
        try:
            parsed.append((read_plt(p), src))
        except PltFormatError as exc:
            print(f"skipping {p.name}: {exc}", file=sys.stderr)
    if not parsed:
        return 2
    if args.list:
        print("\n\n".join(describe(f) for f, _ in parsed))
        return 0

    # A run folder is plotted straight into <folder>/images; named files are shown in windows.
    all_from_folders = all(src is not None for _, src in parsed)
    headless = args.no_show or (all_from_folders and not args.show and not args.save)
    _load_pyplot(headless=headless)

    save_dir: Optional[Path] = None
    save_file: Optional[Path] = None
    if args.save:
        target = Path(args.save)
        if len(parsed) == 1 and target.suffix.lower() in {".png", ".pdf", ".svg", ".jpg"}:
            save_file = target
        else:
            save_dir = target

    made = 0
    written: dict[Path, int] = {}
    for f, src in parsed:
        try:
            fig = make_figure(f, args.mode, args.var, args.mirror, args.cmap, args.elev, args.azim)
        except PltFormatError as exc:
            print(f"cannot plot {f.path.name}: {exc}", file=sys.stderr)
            continue
        made += 1
        out: Optional[Path] = None
        if save_file is not None:
            out = save_file
        elif save_dir is not None:
            out = save_dir / f"{f.path.stem}.png"
        elif src is not None:
            out = src / "images" / f"{f.path.stem}.png"
        if out is not None:
            out.parent.mkdir(parents=True, exist_ok=True)
            fig.savefig(out, bbox_inches="tight")
            written[out.parent] = written.get(out.parent, 0) + 1
            print(f"saved {out}")
        if headless:
            plt.close(fig)
    for folder, n in written.items():
        print(f"{n} image(s) written to {folder}")
    if made and not headless:
        plt.show()
    return 0 if made else 1


if __name__ == "__main__":
    sys.exit(main())
