#!/usr/bin/env python3
"""Decode PNGs written by miniz with Python PNG libraries and compare them to the source images.

Usage: png_roundtrip_check.py <png_roundtrip_gen executable> <work dir>

Uses Pillow and/or pypng, whichever are installed. Exits with 77 (CTest "skipped")
if neither is available.
"""

import os
import subprocess
import sys

SKIP = 77

try:
    from PIL import Image
except ImportError:
    Image = None

try:
    import png as pypng
except ImportError:
    pypng = None


class Case:
    def __init__(self, workdir, line):
        f = line.split()
        self.name = f[0]
        self.w, self.h, self.chans, self.flip, self.plte_entries, self.trns_entries = map(int, f[1:])

        def read(ext):
            with open(os.path.join(workdir, self.name + ext), "rb") as fh:
                return fh.read()

        self.png = read(".png")
        raw = read(".raw")
        self.plte = read(".plte") if self.plte_entries else b""
        self.trns = read(".trns") if self.trns_entries else b""
        bpl = self.w * self.chans
        assert len(raw) == bpl * self.h
        rows = [raw[y * bpl:(y + 1) * bpl] for y in range(self.h)]
        if self.flip:
            rows.reverse()
        self.expected = b"".join(rows)


def check_pillow(case, data_path):
    with Image.open(data_path) as im:
        im.verify()  # checks chunk CRCs
    with Image.open(data_path) as im:
        im.load()
        assert im.format == "PNG", im.format
        assert im.size == (case.w, case.h), im.size
        if case.plte_entries:
            assert im.mode == "P", im.mode
            pal = bytes(im.getpalette())
            assert pal[:len(case.plte)] == case.plte, "palette mismatch"
            if case.trns:
                t = im.info.get("transparency")
                if isinstance(t, int):
                    # Pillow reports a tRNS of the form ff..ff 00 ff..ff as a single index
                    exp = bytearray(b"\xff" * case.trns_entries)
                    exp[t] = 0
                    t = bytes(exp)
                assert t == case.trns, "tRNS mismatch: %r" % (t,)
            else:
                assert "transparency" not in im.info
        else:
            want = {1: "L", 2: "LA", 3: "RGB", 4: "RGBA"}[case.chans]
            assert im.mode == want, "mode %s, expected %s" % (im.mode, want)
        assert im.tobytes() == case.expected, "pixel data mismatch"


def check_pypng(case, _data_path):
    r = pypng.Reader(bytes=case.png)
    w, h, pixels, info = r.read_flat()
    assert (w, h) == (case.w, case.h), (w, h)
    assert info["bitdepth"] == 8, info["bitdepth"]
    assert not info.get("interlace"), "interlaced"
    if case.plte_entries:
        pal = info["palette"]
        assert len(pal) == case.plte_entries, len(pal)
        for i, entry in enumerate(pal):
            exp = tuple(case.plte[i * 3:i * 3 + 3])
            if case.trns:
                exp += (case.trns[i] if i < len(case.trns) else 255,)
            assert tuple(entry) == exp, "palette entry %d: %r != %r" % (i, tuple(entry), exp)
    else:
        assert info["greyscale"] == (case.chans <= 2)
        assert info["alpha"] == (case.chans in (2, 4))
        assert info["planes"] == case.chans
    assert bytes(pixels) == case.expected, "pixel data mismatch"


def main():
    if len(sys.argv) != 3:
        print(__doc__, file=sys.stderr)
        return 2
    gen, workdir = sys.argv[1], sys.argv[2]

    decoders = []
    if Image is not None:
        decoders.append(("Pillow", check_pillow))
    if pypng is not None:
        decoders.append(("pypng", check_pypng))
    if not decoders:
        print("Neither Pillow nor pypng is installed; skipping")
        return SKIP

    os.makedirs(workdir, exist_ok=True)
    subprocess.run([gen, workdir], check=True)

    with open(os.path.join(workdir, "manifest.txt")) as fh:
        cases = [Case(workdir, line) for line in fh if line.strip()]
    if not cases:
        print("no test cases generated", file=sys.stderr)
        return 1

    failures = 0
    for dec_name, check in decoders:
        for case in cases:
            try:
                check(case, os.path.join(workdir, case.name + ".png"))
            except Exception as e:  # report every failing case, not just the first
                failures += 1
                print("FAIL [%s] %s: %s: %s" % (dec_name, case.name, type(e).__name__, e))
        print("%s: checked %d images" % (dec_name, len(cases)))
    return 1 if failures else 0


if __name__ == "__main__":
    sys.exit(main())
