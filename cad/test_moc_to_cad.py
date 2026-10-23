"""Tests for moc_to_cad.py. Run with `python -m pytest cad` from the repository root."""
import math
from pathlib import Path

import numpy as np
import pytest

import moc_to_cad as m

ezdxf = pytest.importorskip("ezdxf")


def contour(n=120):
    x = np.linspace(0.0, 3.0, n) ** 1.0
    r = 1.0 + 0.3 * (1.0 - np.exp(-1.5 * x))
    return x, r


def make_run(folder: Path, mass_flow=9.6):
    folder.mkdir(parents=True, exist_ok=True)
    x, r = contour()
    rows = "".join(f"{a:.6f}\t{b:.6f}\t1.5\t50\t300\t0.01\t0\t1.4\n" for a, b in zip(x, r))
    (folder / "Summary.plt").write_text(
        'TITLE = "Primary MOC Contour Data file"\n'
        'VARIABLES = "X/R","R/R","Mach","Pres","Temp","Rho","Theta","Gamma"\n'
        'zone t="Wall contour"\n' + rows + 'zone t="RRC BD"\n0 0 1 1 1 1 1 1\n')
    (folder / "summary.out").write_text(f"x\n2-D Mass Flow (lbm/s):\t{mass_flow}\n")
    return x, r


def analytic_volume(x, r, scale=1.0):
    return math.pi * float(np.sum(0.5 * (r[1:] ** 2 + r[:-1] ** 2) * np.diff(x))) * scale ** 3


def test_default_run_writes_all_files(tmp_path):
    make_run(tmp_path / "run")
    assert m.main([str(tmp_path / "run"), "--no-step"]) == 0
    out = tmp_path / "run" / "cad"
    assert sorted(p.name for p in out.iterdir()) == ["run_flow.dxf", "run_profile.csv", "run_profile.dat"]
    pts = np.loadtxt(out / "run_profile.dat")
    assert pts.shape[1] == 3 and np.allclose(pts[:, 2], 0) and pts[0, 0] == 0 and abs(pts[0, 1] - 1.0) < 1e-9


def test_dxf_is_a_closed_loop_with_one_cubic_spline(tmp_path):
    make_run(tmp_path / "run")
    m.main([str(tmp_path / "run"), "--no-step", "--units", "mm", "--throat-radius", "12.7"])
    doc = ezdxf.readfile(tmp_path / "run" / "cad" / "run_flow.dxf")
    assert doc.units == ezdxf.units.MM
    ents = list(doc.modelspace())
    assert [e.dxftype() for e in ents] == ["SPLINE", "LINE", "LINE", "LINE"]
    spl = ents[0]
    assert spl.dxf.degree == 3
    bs = spl.construction_tool()
    start, end = np.array(bs.point(0)), np.array(bs.point(bs.max_t))
    assert np.allclose(start, [0, 12.7, 0], atol=1e-6)
    loop = [start, end]
    # inlet line goes from the spline start to the axis, exit line comes back up to the spline end
    assert np.allclose(ents[1].dxf.start, start) and np.allclose(ents[3].dxf.end, end)
    assert np.allclose(ents[1].dxf.end, ents[2].dxf.start) and np.allclose(ents[2].dxf.end, ents[3].dxf.start)
    # the spline passes through the (scaled) contour
    x, r = contour()
    pts = np.array([tuple(p)[:2] for p in bs.points(bs.params(4000))])
    dist = np.min(np.hypot(x[:, None] * 12.7 - pts[None, :, 0], r[:, None] * 12.7 - pts[None, :, 1]), axis=1)
    assert dist.max() < 0.02


def test_mass_flow_sizes_the_throat(tmp_path, capsys):
    make_run(tmp_path / "run", mass_flow=9.6)
    m.main([str(tmp_path / "run"), "--no-step", "--units", "mm", "--mass-flow", "0.6"])
    pts = np.loadtxt(tmp_path / "run" / "cad" / "run_profile.dat")
    expected = math.sqrt(0.6 / 9.6) * 25.4               # R* scales with sqrt(mass flow)
    assert abs(pts[0, 1] - expected) < 1e-4


def test_axis_along_z_puts_profile_in_xz_plane(tmp_path):
    make_run(tmp_path / "run")
    m.main([str(tmp_path / "run"), "--no-step", "--axial", "z"])
    pts = np.loadtxt(tmp_path / "run" / "cad" / "run_profile.dat")
    assert np.allclose(pts[:, 1], 0) and pts[-1, 2] > 2.9 and abs(pts[0, 0] - 1.0) < 1e-9


def test_geo_file_input_and_nonconcentric_geo_is_rejected(tmp_path):
    x, r = contour(60)
    geo = tmp_path / "n.geo"
    geo.write_text(f"{len(x)}\t\t\nZ\tR0\tX0\tY0\n" + "".join(f"{a}\t{b}\t0\t0\n" for a, b in zip(x, r)))
    assert m.main([str(geo), "--no-step", "--out", str(tmp_path / "o")]) == 0
    assert (tmp_path / "o" / "tmp_flow.dxf").exists() or any((tmp_path / "o").glob("*_flow.dxf"))
    bad = tmp_path / "bad.geo"
    bad.write_text(f"{len(x)}\t\t\nZ\tR0\tX0\tY0\n" + "".join(f"{a}\t{b}\t0.5\t0\n" for a, b in zip(x, r)))
    assert m.main([str(bad), "--no-step"]) == 2


def test_wall_section_has_two_splines_and_flat_caps(tmp_path):
    make_run(tmp_path / "run")
    m.main([str(tmp_path / "run"), "--no-step", "--thickness", "0.1"])
    ents = list(ezdxf.readfile(tmp_path / "run" / "cad" / "run_wall.dxf").modelspace())
    assert [e.dxftype() for e in ents] == ["SPLINE", "SPLINE", "LINE", "LINE"]
    for cap in ents[2:]:
        assert abs(cap.dxf.start[0] - cap.dxf.end[0]) < 1e-9        # perpendicular to the axis
    inner = np.array([tuple(p)[:2] for p in ents[0].construction_tool().points(np.linspace(0, ents[0].construction_tool().max_t, 1500))])
    outer = np.array([tuple(p)[:2] for p in ents[1].construction_tool().points(np.linspace(0, ents[1].construction_tool().max_t, 1500))])
    gap = np.min(np.hypot(outer[:, None, 0] - inner[None, :, 0], outer[:, None, 1] - inner[None, :, 1]), axis=1)
    assert 0.098 < gap[10:-10].min() and gap[10:-10].max() < 0.102   # wall thickness measured normal to the flow surface


def test_step_solids_have_the_right_volume(tmp_path):
    cq = pytest.importorskip("cadquery")
    x, r = make_run(tmp_path / "run")
    m.main([str(tmp_path / "run"), "--units", "mm", "--throat-radius", "10", "--thickness", "2"])
    out = tmp_path / "run" / "cad"
    flow = cq.importers.importStep(str(out / "run_flow.step")).val()
    assert flow.isValid() and len(flow.Solids()) == 1
    assert flow.Volume() == pytest.approx(analytic_volume(x, r, 10.0), rel=2e-3)
    bb = flow.BoundingBox()
    assert bb.xmax - bb.xmin == pytest.approx(3.0 * 10.0, rel=1e-3)
    assert bb.ymax == pytest.approx(10.0 * r[-1], rel=1e-3)
    wall = cq.importers.importStep(str(out / "run_wall.step")).val()
    assert wall.isValid() and wall.Volume() > 0


def test_step_in_inches_is_written_in_millimetres(tmp_path):
    cq = pytest.importorskip("cadquery")
    x, r = make_run(tmp_path / "run")
    m.main([str(tmp_path / "run")])                                 # inches, throat radius 1 in
    bb = cq.importers.importStep(str(tmp_path / "run" / "cad" / "run_flow.step")).val().BoundingBox()
    assert bb.xmax - bb.xmin == pytest.approx(3.0 * 25.4, rel=1e-3)


def test_bad_inputs_give_clean_errors(tmp_path, capsys):
    assert m.main([str(tmp_path / "missing")]) == 2
    (tmp_path / "empty").mkdir()
    assert m.main([str(tmp_path / "empty")]) == 2
    make_run(tmp_path / "run")
    (tmp_path / "run" / "summary.out").unlink()
    assert m.main([str(tmp_path / "run"), "--no-step", "--mass-flow", "1"]) == 2
