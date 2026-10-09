"""Exercise actual CMake generation in concurrent builds of a shared checkout."""

from concurrent.futures import ThreadPoolExecutor
from pathlib import Path
import shutil
import subprocess
import sys

import pytest


@pytest.mark.skipif(
    shutil.which("cmake") is None or shutil.which("ninja") is None,
    reason="CMake and Ninja required",
)
def test_codegen_is_build_local_and_profile_changes_regenerate(tmp_path):
    root = Path(__file__).parents[2]
    source = tmp_path / "source"
    shutil.copytree(
        root,
        source,
        ignore=shutil.ignore_patterns(
            ".git", "build", ".venv", "__pycache__", ".pytest_cache", "apis"
        ),
    )

    def profile(name):
        path = tmp_path / f"{name}.yaml"
        path.write_text(f"commands:\n  - name: {name}\n    handler: handle_{name}\n    args: []\n")
        return path

    alpha, beta = profile("alpha"), profile("beta")

    def build(name, yaml):
        directory = tmp_path / name
        subprocess.run(
            [
                "cmake",
                "-G",
                "Ninja",
                "-S",
                str(source),
                "-B",
                str(directory),
                f"-DINPUT_YAML={yaml}",
                f"-DPython_EXECUTABLE={sys.executable}",
            ],
            check=True,
            capture_output=True,
            text=True,
            timeout=30,
        )
        subprocess.run(
            ["cmake", "--build", str(directory), "--target", "generate_apis"],
            check=True,
            capture_output=True,
            text=True,
            timeout=30,
        )
        return directory / "tools/apis/ce_command_ids.h"

    with ThreadPoolExecutor(max_workers=2) as pool:
        a, b = pool.submit(build, "a", alpha), pool.submit(build, "b", beta)
        header_a, header_b = a.result(), b.result()
    assert "CE_CMD_ALPHA_e" in header_a.read_text()
    assert "CE_CMD_BETA_e" in header_b.read_text()
    assert not (source / "apis").exists()
    old_b = header_b.read_bytes()
    build("a", beta)
    assert "CE_CMD_BETA_e" in header_a.read_text()
    assert "CE_CMD_ALPHA_e" not in header_a.read_text()
    assert header_b.read_bytes() == old_b
