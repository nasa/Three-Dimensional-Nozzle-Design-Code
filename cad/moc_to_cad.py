#!/usr/bin/env python3
"""
moc_to_cad - turn the nozzle contour from a MOC_Grid run into files a CAD system can use.

The MOC_Grid contour is nondimensional (X/R*, R/R*). This script scales it to a real throat
radius and writes, into <run folder>/cad:

  <name>_flow.dxf     closed half-section of the flow path: ONE cubic spline for the wall, plus the
                      inlet, axis and exit lines. In NX: import it and Revolve about the axis.
  <name>_wall.dxf     (only with --thickness) closed half-section of the nozzle wall (inner and outer
                      spline joined by flat inlet and exit caps). Revolve it to get a wall solid.
  <name>_profile.dat  the wall points as "x y z" lines: NX Insert > Curve > Spline > Through Points
                      > Points from File (SolidWorks "Curve Through XYZ Points" reads it too).
  <name>_profile.csv  the same points with a header, for spreadsheets.
  <name>_flow.step    the flow path already revolved into a solid (always in millimetres).
  <name>_wall.step    the wall already revolved into a solid (only with --thickness).

Examples
--------
    python moc_to_cad.py C:\\nozzles\\mach2_perfect                       # 1 in throat radius, inches
    python moc_to_cad.py C:\\nozzles\\mach2_perfect --units mm --throat-radius 12.7
    python moc_to_cad.py C:\\nozzles\\mach2_perfect --units mm --mass-flow 0.5 --thickness 3
    python moc_to_cad.py C:\\nozzles\\mach2_perfect\\mach2_perfect.geo --units mm

The input can be a run folder (it uses Summary.plt, else wall.out, else a *.geo), a Summary.plt or
wall.out file, or a 3D_MOC .geo file (Z R0 X0 Y0 per station; X0 and Y0 must be 0 to be revolved).

Requirements: Python 3.9+, numpy, ezdxf (DXF files) and cadquery (STEP files, optional).
"""
from __future__ import annotations

import argparse
import math
import re
import sys
from pathlib import Path
from typing import Optional, Sequence

import numpy as np

UNITS_PER_INCH = {"in": 1.0, "mm": 25.4, "cm": 2.54, "m": 0.0254}
MM_PER_UNIT = {u: 25.4 / f for u, f in UNITS_PER_INCH.items()}


class CadExportError(Exception):
    """A problem the user can fix (bad input, missing file...)."""


# --------------------------------------------------------------------------------------
#  Reading the nozzle contour
# --------------------------------------------------------------------------------------
def _numbers(line: str) -> list[float]:
    out = []
    for tok in line.replace(",", " ").split():
        try:
            out.append(float(tok))
        except ValueError:
            return []
    return out


def _read_summary_plt(path: Path) -> tuple[list[float], list[float]]:
    x, r, in_wall = [], [], False
    for line in path.read_text(errors="replace").splitlines():
        if line.lower().startswith("zone"):
            if in_wall:
                break
            in_wall = "wall contour" in line.lower()
            continue
        if in_wall:
            v = _numbers(line)
            if len(v) >= 2:
                x.append(v[0])
                r.append(v[1])
    return x, r


def _read_wall_out(path: Path) -> tuple[list[float], list[float]]:
    x, r = [], []
    for line in path.read_text(errors="replace").splitlines():
        v = _numbers(line)
        if len(v) >= 4:                          # i  j  X/R*  R/R*  ...
            x.append(v[2])
            r.append(v[3])
    return x, r


def _read_geo(path: Path) -> tuple[list[float], list[float]]:
    x, r, offsets = [], [], 0.0
    lines = path.read_text(errors="replace").splitlines()
    for line in lines[2:]:                       # station count, then a title line
        v = _numbers(line)
        if len(v) >= 4:
            x.append(v[0])
            r.append(v[1])
            offsets = max(offsets, abs(v[2]), abs(v[3]))
    if offsets > 1e-9:
        raise CadExportError(f"{path.name}: X0/Y0 are not zero, so the sections are not "
                             "concentric and the nozzle cannot be made by revolving one profile.")
    return x, r


def read_contour(source: Path) -> tuple[np.ndarray, np.ndarray, str, Optional[Path]]:
    """(x, r, description, summary.out path or None). x, r are in the file's own units."""
    folder = source if source.is_dir() else source.parent
    summary = folder / "summary.out"
    summary_out = summary if summary.is_file() else None
    candidates: list[Path]
    if source.is_dir():
        candidates = [folder / "Summary.plt", folder / "wall.out"] + sorted(folder.glob("*.geo"))
    else:
        candidates = [source]
    for path in candidates:
        if not path.is_file():
            continue
        name = path.name.lower()
        if name.endswith(".geo"):
            x, r = _read_geo(path)
        elif name == "wall.out":
            x, r = _read_wall_out(path)
        else:
            x, r = _read_summary_plt(path)
        if len(x) >= 10:
            return _clean(np.array(x), np.array(r)) + (path.name, summary_out)
    raise CadExportError(f"no nozzle contour found in {source} (looked for Summary.plt, wall.out, *.geo)")


def _clean(x: np.ndarray, r: np.ndarray) -> tuple[np.ndarray, np.ndarray]:
    keep = [0]
    for i in range(1, len(x)):                   # drop repeated points
        if abs(x[i] - x[keep[-1]]) > 1e-9 or abs(r[i] - r[keep[-1]]) > 1e-9:
            keep.append(i)
    x, r = x[keep], r[keep]
    if len(x) < 10 or np.any(np.diff(x) <= 0):
        raise CadExportError("the contour is not a function of the axial position "
                             "(x must increase along the wall)")
    return x, r


def read_mass_flow(summary: Path) -> float:
    """2-D mass flow (lbm/s) of the MOC throat for R* = 1 in, from summary.out."""
    m = re.search(r"2-D Mass Flow \(lbm/s\):\s*([-+0-9.eE]+)", summary.read_text(errors="replace"))
    if not m or float(m.group(1)) <= 0:
        raise CadExportError(f"{summary} has no 2-D mass flow")
    return float(m.group(1))


# --------------------------------------------------------------------------------------
#  Geometry
# --------------------------------------------------------------------------------------
def thin_points(x: np.ndarray, r: np.ndarray, min_spacing: float) -> np.ndarray:
    """Indices to keep: drops points closer than min_spacing to the last kept one."""
    keep = [0]
    for i in range(1, len(x) - 1):
        if math.hypot(x[i] - x[keep[-1]], r[i] - r[keep[-1]]) >= min_spacing:
            keep.append(i)
    keep.append(len(x) - 1)
    return np.array(keep)


def fit_spline(x: np.ndarray, r: np.ndarray):
    """Cubic B-spline through the points (the same kind of curve a CAD spline-through-points is)."""
    from ezdxf.math import fit_points_to_cad_cv
    return fit_points_to_cad_cv([(float(a), float(b), 0.0) for a, b in zip(x, r)])


def sample_spline(bs, n: int = 3000) -> np.ndarray:
    pts = np.array([tuple(p) for p in bs.points(bs.params(n))])
    return pts[:, :2]


def offset_outward(curve: np.ndarray, t: float) -> np.ndarray:
    """Curve moved by t along its normal, away from the axis."""
    d = np.gradient(curve, axis=0)
    d /= np.hypot(d[:, 0], d[:, 1])[:, None]
    normal = np.column_stack([-d[:, 1], d[:, 0]])          # +r side when x increases
    return curve + t * normal


def clip_to_x_range(curve: np.ndarray, x0: float, x1: float) -> np.ndarray:
    """Trim (or extend along the end tangent) a left-to-right curve so it spans exactly x0..x1."""
    pts = curve.copy()
    # start
    if pts[0, 0] > x0:                                     # too short: extend backwards
        d = pts[1] - pts[0]
        pts = np.vstack([pts[0] - d * ((pts[0, 0] - x0) / d[0]), pts])
    else:
        i = int(np.searchsorted(pts[:, 0], x0))
        a, b = pts[i - 1], pts[i]
        pts = np.vstack([a + (b - a) * ((x0 - a[0]) / (b[0] - a[0])), pts[i:]])
    # end
    if pts[-1, 0] < x1:
        d = pts[-1] - pts[-2]
        pts = np.vstack([pts, pts[-1] + d * ((x1 - pts[-1, 0]) / d[0])])
    else:
        i = int(np.searchsorted(pts[:, 0], x1))
        a, b = pts[i - 1], pts[i]
        pts = np.vstack([pts[:i], a + (b - a) * ((x1 - a[0]) / (b[0] - a[0]))])
    pts[0, 0], pts[-1, 0] = x0, x1
    return pts


def resample(curve: np.ndarray, n: int) -> np.ndarray:
    s = np.concatenate([[0], np.cumsum(np.hypot(*np.diff(curve, axis=0).T))])
    t = np.linspace(0, s[-1], n)
    return np.column_stack([np.interp(t, s, curve[:, 0]), np.interp(t, s, curve[:, 1])])


def revolved_volume(curve: np.ndarray, inner: Optional[np.ndarray] = None) -> float:
    """Volume swept by revolving the region under `curve` (and above `inner`) about the axis."""
    def pappus(c):
        return math.pi * float(np.sum(0.5 * (c[1:, 1] ** 2 + c[:-1, 1] ** 2) * np.diff(c[:, 0])))
    return pappus(curve) - (pappus(inner) if inner is not None else 0.0)


class Axes:
    """Maps (axial, radial) to 3-D coordinates: nozzle axis along +X (profile in XY) or +Z (XZ)."""

    def __init__(self, axial: str):
        self.axial = axial

    def point(self, x: float, r: float) -> tuple[float, float, float]:
        return (x, r, 0.0) if self.axial == "x" else (r, 0.0, x)


# --------------------------------------------------------------------------------------
#  Writers
# --------------------------------------------------------------------------------------
def write_points(path: Path, x, r, ax: Axes) -> None:
    with path.open("w", newline="\n") as f:
        for a, b in zip(x, r):
            p = ax.point(a, b)
            f.write(f"{p[0]:.6f} {p[1]:.6f} {p[2]:.6f}\n")


def write_csv(path: Path, x, r, units: str) -> None:
    with path.open("w", newline="\n") as f:
        f.write(f"axial_position_{units},radius_{units}\n")
        for a, b in zip(x, r):
            f.write(f"{a:.6f},{b:.6f}\n")


def _add_spline(msp, bs, ax: Axes, layer: str, reverse: bool = False):
    cps = [ax.point(p[0], p[1]) for p in bs.control_points]
    if reverse:
        cps.reverse()
        knots = [bs.max_t - k for k in reversed(list(bs.knots()))]
    else:
        knots = list(bs.knots())
    return msp.add_open_spline(control_points=cps, degree=bs.degree, knots=knots,
                               dxfattribs={"layer": layer})


def write_dxf(path: Path, units: str, ax: Axes, inner_bs, x0: float, xe: float,
              outer_bs=None) -> None:
    import ezdxf
    doc = ezdxf.new("R2000")
    doc.units = {"in": ezdxf.units.IN, "mm": ezdxf.units.MM, "cm": ezdxf.units.CM,
                 "m": ezdxf.units.M}[units]
    doc.layers.add("NOZZLE_WALL", color=5)
    doc.layers.add("CLOSURE", color=1)
    msp = doc.modelspace()
    r_in0 = tuple(inner_bs.control_points[0])[1]
    r_ine = tuple(inner_bs.control_points[-1])[1]
    _add_spline(msp, inner_bs, ax, "NOZZLE_WALL")
    if outer_bs is None:                                    # flow path: down to the axis
        msp.add_line(ax.point(x0, r_in0), ax.point(x0, 0.0), dxfattribs={"layer": "CLOSURE"})
        msp.add_line(ax.point(x0, 0.0), ax.point(xe, 0.0), dxfattribs={"layer": "CLOSURE"})
        msp.add_line(ax.point(xe, 0.0), ax.point(xe, r_ine), dxfattribs={"layer": "CLOSURE"})
    else:                                                   # wall: outer spline and flat caps
        _add_spline(msp, outer_bs, ax, "NOZZLE_WALL")
        r_out0 = tuple(outer_bs.control_points[0])[1]
        r_oute = tuple(outer_bs.control_points[-1])[1]
        msp.add_line(ax.point(x0, r_in0), ax.point(x0, r_out0), dxfattribs={"layer": "CLOSURE"})
        msp.add_line(ax.point(xe, r_ine), ax.point(xe, r_oute), dxfattribs={"layer": "CLOSURE"})
    doc.saveas(path)


def write_step(path: Path, units: str, ax: Axes, inner: np.ndarray, outer: Optional[np.ndarray]) -> float:
    """Revolves the section into a solid and writes STEP (millimetres). Returns the volume (units^3)."""
    import cadquery as cq
    k = MM_PER_UNIT[units]
    plane = "XY" if ax.axial == "x" else "XZ"
    # local 2-D coordinates of the work plane: XY -> (x, y); XZ -> (x_global, z_global)
    def loc(x, r):
        return (x * k, r * k) if ax.axial == "x" else (r * k, x * k)
    if outer is None:
        pts = [loc(a, b) for a, b in inner]
        wp = (cq.Workplane(plane).moveTo(*loc(inner[0, 0], 0.0)).lineTo(*pts[0])
              .spline(pts[1:], includeCurrent=True)
              .lineTo(*loc(inner[-1, 0], 0.0)).close())
    else:
        pin = [loc(a, b) for a, b in inner]
        pout = [loc(a, b) for a, b in outer[::-1]]
        wp = (cq.Workplane(plane).moveTo(*pin[0]).spline(pin[1:], includeCurrent=True)
              .lineTo(*pout[0]).spline(pout[1:], includeCurrent=True).close())
    axis_end = (1, 0, 0) if ax.axial == "x" else (0, 1, 0)
    solid = wp.revolve(360, (0, 0, 0), axis_end)
    cq.exporters.export(solid, str(path))
    return solid.val().Volume() / k ** 3


# --------------------------------------------------------------------------------------
#  Command line
# --------------------------------------------------------------------------------------
def build_parser() -> argparse.ArgumentParser:
    ap = argparse.ArgumentParser(
        prog="moc_to_cad", description="Export a MOC_Grid nozzle contour for CAD (DXF, points, STEP).",
        epilog="See the top of this file (or the repository README) for examples.")
    ap.add_argument("source", help="run folder, Summary.plt, wall.out or a 3D_MOC .geo file")
    ap.add_argument("--units", choices=sorted(UNITS_PER_INCH), default="in",
                    help="units of the output files (default: in)")
    size = ap.add_mutually_exclusive_group()
    size.add_argument("--throat-radius", type=float, metavar="R",
                      help="throat radius in the output units (default: 1 inch, the size MOC_Grid ran)")
    size.add_argument("--mass-flow", type=float, metavar="LBM_S",
                      help="size the throat so the nozzle passes this mass flow (lbm/s); uses summary.out")
    ap.add_argument("--thickness", type=float, metavar="T",
                    help="also make a wall section/solid with this wall thickness (output units, "
                         "measured normal to the flow surface)")
    ap.add_argument("--axial", choices=["x", "z"], default="x",
                    help="nozzle axis along X (profile in the XY plane, default) or Z (XZ plane)")
    ap.add_argument("--min-spacing", type=float, default=0.002, metavar="F",
                    help="drop contour points closer than F x throat radius to the last kept one "
                         "(default 0.002; 0 keeps every point)")
    ap.add_argument("--out", help="output folder (default: <run folder>/cad)")
    ap.add_argument("--name", help="file name prefix (default: the run folder name)")
    ap.add_argument("--no-step", action="store_true", help="do not write STEP solids")
    return ap


def main(argv: Optional[Sequence[str]] = None) -> int:
    args = build_parser().parse_args(argv)
    try:
        return run(args)
    except CadExportError as exc:
        print(f"error: {exc}", file=sys.stderr)
        return 2


def run(args) -> int:
    source = Path(args.source)
    if not source.exists():
        raise CadExportError(f"{source} does not exist")
    x, r, used, summary = read_contour(source)

    # ---- scale: the contour's throat radius r[0] becomes the requested throat radius
    per_inch = UNITS_PER_INCH[args.units]
    r_throat_src = float(r[0])
    if args.mass_flow is not None:
        if summary is None:
            raise CadExportError("--mass-flow needs summary.out in the same folder as the contour")
        rstar_in = math.sqrt(args.mass_flow / read_mass_flow(summary))
        target = rstar_in * per_inch
        how = f"sized for {args.mass_flow:g} lbm/s"
    elif args.throat_radius is not None:
        target = args.throat_radius
        how = "as requested"
    else:
        target = r_throat_src * per_inch           # the file is in inches already (R* = 1 in)
        how = "as run in MOC_Grid"
    if target <= 0:
        raise CadExportError("the throat radius must be positive")
    s = target / r_throat_src
    x, r = x * s, r * s

    keep = thin_points(x, r, args.min_spacing * target)
    xf, rf = x[keep], r[keep]
    inner_bs = fit_spline(xf, rf)
    inner = sample_spline(inner_bs)
    dev = float(np.max(np.min(np.hypot(x[:, None] - inner[None, :, 0],
                                       r[:, None] - inner[None, :, 1]), axis=1)))

    outer = outer_bs = None
    if args.thickness:
        if args.thickness <= 0:
            raise CadExportError("--thickness must be positive")
        outer = clip_to_x_range(offset_outward(inner, args.thickness), float(xf[0]), float(xf[-1]))
        outer = resample(outer, max(60, len(keep)))
        outer_bs = fit_spline(outer[:, 0], outer[:, 1])

    base = Path(args.out) if args.out else (source if source.is_dir() else source.parent) / "cad"
    base.mkdir(parents=True, exist_ok=True)
    name = args.name or (source.name if source.is_dir() else source.parent.name or source.stem)
    name = re.sub(r"[^A-Za-z0-9_\-]", "_", name) or "nozzle"
    ax = Axes(args.axial)
    written = []

    write_points(base / f"{name}_profile.dat", xf, rf, ax)
    write_csv(base / f"{name}_profile.csv", xf, rf, args.units)
    written += [f"{name}_profile.dat", f"{name}_profile.csv"]
    write_dxf(base / f"{name}_flow.dxf", args.units, ax, inner_bs, float(xf[0]), float(xf[-1]))
    written.append(f"{name}_flow.dxf")
    if outer_bs is not None:
        write_dxf(base / f"{name}_wall.dxf", args.units, ax, inner_bs, float(xf[0]), float(xf[-1]), outer_bs)
        written.append(f"{name}_wall.dxf")

    vol_flow = revolved_volume(inner)
    vol_wall = revolved_volume(outer, inner) if outer is not None else None
    step_note = ""
    if not args.no_step:
        try:
            vf = write_step(base / f"{name}_flow.step", args.units, ax, inner, None)
            written.append(f"{name}_flow.step")
            if abs(vf - vol_flow) > 2e-3 * vol_flow:
                step_note = f"warning: STEP flow volume {vf:.5g} differs from the section integral {vol_flow:.5g}"
            if outer is not None:
                write_step(base / f"{name}_wall.step", args.units, ax, inner, outer)
                written.append(f"{name}_wall.step")
        except ImportError:
            step_note = "STEP files skipped: install cadquery (pip install cadquery) to write them"

    u = args.units
    print(f"contour from {used}: {len(x)} points ({len(keep)} used), max fit deviation {dev:.2e} {u}")
    print(f"throat radius {target:.6g} {u} ({how}); exit radius {rf[-1]:.6g} {u}; "
          f"length {xf[-1] - xf[0]:.6g} {u}; area ratio {(rf[-1] / rf[0]) ** 2:.4f}")
    print(f"flow-path volume {vol_flow:.6g} {u}^3" + (f"; wall volume {vol_wall:.6g} {u}^3" if vol_wall else ""))
    print(f"axis: nozzle axis along {args.axial.upper()}, throat plane at {args.axial.upper()} = {xf[0]:.6g}")
    for w in written:
        print(f"wrote {base / w}")
    if step_note:
        print(step_note)
    return 0


if __name__ == "__main__":
    sys.exit(main())
