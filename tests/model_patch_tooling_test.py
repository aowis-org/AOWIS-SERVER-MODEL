#!/usr/bin/env python3
"""Exercise MODEL's patch/apply and snapshot scripts in a disposable Git tree."""

import os
from pathlib import Path
import shutil
import subprocess
import sys
import tempfile
import time
import zipfile


def run(*args: str, cwd: Path, expected: int = 0) -> subprocess.CompletedProcess[str]:
    result = subprocess.run(args, cwd=cwd, text=True, capture_output=True, check=False)
    if (result.returncode == 0) != (expected == 0):
        raise AssertionError(
            f"{' '.join(args)} returned {result.returncode}, expected "
            f"{'success' if expected == 0 else 'failure'}:\n"
            f"stdout:\n{result.stdout}\nstderr:\n{result.stderr}"
        )
    return result


def archive(path: Path, entries: dict[str, str]) -> None:
    with zipfile.ZipFile(path, "w") as output:
        for name, contents in entries.items():
            info = zipfile.ZipInfo(name, date_time=(2020, 1, 1, 0, 0, 0))
            info.compress_type = zipfile.ZIP_DEFLATED
            output.writestr(info, contents)


def main() -> None:
    for tool in ("git", "zip", "unzip", "realpath"):
        if shutil.which(tool) is None:
            raise RuntimeError(f"required patch tool unavailable: {tool}")

    source_root = Path(sys.argv[1]).resolve()
    with tempfile.TemporaryDirectory(prefix="aowis-model-patch-test-") as temp:
        parent = Path(temp)
        root = parent / "AOWIS-SERVER-MODEL"
        tools = root / "tools"
        tools.mkdir(parents=True)
        for name in ("apply_patch.sh", "zip_model.sh"):
            shutil.copy2(source_root / "tools" / name, tools / name)
            (tools / name).chmod(0o755)
        (root / "README.md").write_text("original\n", encoding="utf-8")
        (root / "old.txt").write_text("obsolete\n", encoding="utf-8")
        run("git", "init", "-q", cwd=root)
        run("git", "add", "README.md", "old.txt", "tools", cwd=root)

        invalid_name = parent / "other-repository-patch.zip"
        archive(invalid_name, {"AOWIS-SERVER-MODEL/README.md": "modified\n"})
        run(str(tools / "apply_patch.sh"), str(invalid_name), cwd=root, expected=1)
        assert (root / "README.md").read_text() == "original\n"

        invalid_path = parent / "aowis-server-model-invalid-path.zip"
        archive(invalid_path, {"README.md": "bad\n"})
        run(str(tools / "apply_patch.sh"), str(invalid_path), cwd=root, expected=1)
        assert (root / "README.md").read_text() == "original\n"

        patch = parent / "aowis-server-model-test-tooling.zip"
        archive(patch, {
            "AOWIS-SERVER-MODEL/README.md": "updated\n",
            "AOWIS-SERVER-MODEL/new.txt": "new file\n",
            "AOWIS-SERVER-MODEL/.aowis-patch-delete.txt": "old.txt\n",
        })
        before = time.time() - 5
        run(str(tools / "apply_patch.sh"), str(patch), cwd=root)
        assert (root / "README.md").read_text() == "updated\n"
        assert (root / "new.txt").read_text() == "new file\n"
        assert not (root / "old.txt").exists()
        assert not (root / ".aowis-patch-delete.txt").exists()
        assert (root / "new.txt").stat().st_mtime >= before
        assert (root / "README.md").stat().st_mtime >= before

        source_zip = parent / "model.zip"
        assert source_zip.exists(), "apply must automatically refresh model.zip"
        with zipfile.ZipFile(source_zip) as output:
            names = set(output.namelist())
            assert "AOWIS-SERVER-MODEL/README.md" in names
            assert "AOWIS-SERVER-MODEL/new.txt" in names
            assert "AOWIS-SERVER-MODEL/tools/apply_patch.sh" in names
            assert "AOWIS-SERVER-MODEL/tools/zip_model.sh" in names
            assert "AOWIS-SERVER-MODEL/old.txt" not in names
            assert not any("/.git/" in name or "/build-linux-tests/" in name for name in names)
            assert output.read("AOWIS-SERVER-MODEL/README.md") == b"updated\n"

        custom_zip = parent / "custom-model-snapshot.zip"
        run(str(tools / "zip_model.sh"), str(custom_zip), cwd=root)
        assert custom_zip.is_file()

    print("MODEL patch apply, deletion, timestamps and source snapshot: passed")


if __name__ == "__main__":
    main()
