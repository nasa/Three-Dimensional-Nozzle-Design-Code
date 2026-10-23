"""Smoke tests: run with `python -m pytest viewer` from the repository root."""
from pathlib import Path

import pytest

import plt_viewer as pv

ROOT = Path(__file__).resolve().parent.parent
SAMPLES = [p for p in ROOT.rglob("*.plt")
           if p.stat().st_size > 0 and "viewer" not in p.parts]


def test_zone_title_containing_dimensions_is_not_parsed_as_dimensions(tmp_path):
    f = tmp_path / "a.plt"
    f.write_text('TITLE = "t"\nVARIABLES = "x","y"\nzone t="J = 7" I = 3 J = 1 K = 1\n1 2\n3 4\n5 6\n')
    z = pv.read_plt(f).zones[0]
    assert z.dims == (3, 1, 1) and z.n == 3


def test_malformed_variable_line_and_nan_tokens(tmp_path):
    f = tmp_path / "b.plt"
    f.write_text('VARIABLES = "a""b"\nzone t="z"\n1 2 3\n4 -nan(ind) 6\n')
    p = pv.read_plt(f)
    assert p.variables == ["a", "b", "col3"]
    assert p.zones[0].data.shape == (2, 3)


def test_binary_file_is_rejected(tmp_path):
    f = tmp_path / "c.plt"
    f.write_bytes(b"#!TDV75\x00\x01\x02")
    with pytest.raises(pv.PltFormatError):
        pv.read_plt(f)


@pytest.mark.parametrize("path", SAMPLES, ids=lambda p: str(p.relative_to(ROOT)))
def test_every_sample_file_reads_and_plots(path):
    try:
        f = pv.read_plt(path)
    except pv.PltFormatError:
        pytest.skip("binary or empty Tecplot file")
    pv._load_pyplot(headless=True)
    fig = pv.make_figure(f)
    assert fig.axes
    pv.plt.close(fig)


# ---------------------------------------------------------------------------------------
#  Binary files and the run-folder -> images behaviour
# ---------------------------------------------------------------------------------------
import struct

import numpy as np


def _s(text, e="<"):
    return b"".join(struct.pack(e + "i", ord(c)) for c in text) + struct.pack(e + "i", 0)


def write_binary_plt(path, version, variables, zones, endian="<", double=False):
    """Writes ordered zones in the Tecplot binary layout (versions 75 and 112)."""
    e = endian
    out = b"#!TDV75 " if version == 75 else b"#!TDV%d" % version
    out += struct.pack(e + "i", 1)
    if version >= 108:
        out += struct.pack(e + "i", 0)                          # file type
    out += _s("test title", e) + struct.pack(e + "i", len(variables))
    for v in variables:
        out += _s(v, e)
    for name, dims, _ in zones:
        out += struct.pack(e + "f", 299.0) + _s(name, e)
        if version == 75:
            out += struct.pack(e + "ii", 0, -1) + struct.pack(e + "iii", *dims)
        else:
            out += struct.pack(e + "ii", -1, -1) + struct.pack(e + "d", 0.0)
            out += struct.pack(e + "ii", -1, 0)                  # colour, zone type (ordered)
            out += struct.pack(e + "iii", 0, 0, 0)               # var location, raw faces, user faces
            out += struct.pack(e + "iii", *dims) + struct.pack(e + "i", 0)   # dims, no aux data
    out += struct.pack(e + "f", 357.0)
    for _, dims, cols in zones:
        out += struct.pack(e + "f", 299.0)
        code = 2 if double else 1
        if version == 75:
            out += struct.pack(e + "i", 0) + struct.pack(e + "i", code) * len(cols)
        else:
            out += struct.pack(e + "i", code) * len(cols) + struct.pack(e + "iii", 0, 0, -1)
            for c in cols:
                out += struct.pack(e + "dd", float(np.min(c)), float(np.max(c)))
        for c in cols:
            out += np.asarray(c, dtype=e + ("f8" if double else "f4")).tobytes()
    Path(path).write_bytes(out)


@pytest.mark.parametrize("version,endian,double", [(75, "<", False), (112, "<", False),
                                                   (112, ">", True), (108, "<", False)])
def test_binary_round_trip(tmp_path, version, endian, double):
    x = np.linspace(0, 1, 12)
    zones = [("surf", (4, 3, 1), [x, x**2, x + 5]), ("line", (5, 1, 1), [x[:5], x[:5] * 2, x[:5] * 0])]
    f = tmp_path / "b.plt"
    write_binary_plt(f, version, ["X", "Y", "P"], zones, endian, double)
    p = pv.read_plt(f)
    assert p.variables == ["X", "Y", "P"] and p.title == "test title"
    assert [z.dims for z in p.zones] == [(4, 3, 1), (5, 1, 1)]
    assert np.allclose(p.zones[0].data[:, 1], x**2, atol=1e-6)
    assert np.allclose(p.zones[1].data[:, 0], x[:5], atol=1e-6)


def test_unsupported_binary_version_is_reported(tmp_path):
    f = tmp_path / "old.plt"
    f.write_bytes(b"#!TDV60 " + struct.pack("<i", 1) + b"\0" * 64)
    with pytest.raises(pv.PltFormatError, match="unsupported"):
        pv.read_plt(f)


def test_run_folder_writes_pngs_to_images_subfolder(tmp_path, monkeypatch):
    x = np.linspace(0, 3, 30)
    (tmp_path / "Summary.plt").write_text(
        'TITLE = "Primary MOC Contour Data file"\n'
        'VARIABLES = "X/R","R/R","Mach","Pres","Temp","Rho","Theta","Gamma"\n'
        'zone t="Wall contour"\n'
        + "".join(f"{v} {1 + v / 4} {1 + v / 3} {50 - v} 300 0.01 {v} 1.4\n" for v in x))
    write_binary_plt(tmp_path / "grid.plt", 112, ["X(in)", "Y(in)", "Z(in)", "Mach"],
                     [("s", (6, 5, 1), [np.random.rand(30)] * 4)])
    assert pv.main([str(tmp_path)]) == 0
    images = tmp_path / "images"
    assert (images / "Summary.png").stat().st_size > 1000
    assert (images / "grid.png").stat().st_size > 1000
