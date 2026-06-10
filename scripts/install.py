#!/usr/bin/env python3

from __future__ import annotations

import argparse
import os
import shutil
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path


BINARIES = ("hyperion-cli", "hyperiond")


@dataclass(frozen=True)
class InstallContext:
    hyperion_home: Path
    mode: str
    build_type: str
    binaries: tuple[Path, ...]


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="Install Hyperion into HYPERION_HOME")
    group = parser.add_mutually_exclusive_group()
    group.add_argument("--debug", action="store_true",
                       help="Use debug build artifacts")
    group.add_argument("--release", "--relase",
                       action="store_true", help="Use release build artifacts")
    args = parser.parse_args()
    if not args.debug and not args.release:
        parser.error("choose one build mode: --debug or --release")
    return args


def require_hyperion_home() -> Path:
    value = os.environ.get("HYPERION_HOME", "").strip()
    if not value:
        print("[install] ERROR: HYPERION_HOME is not set.", file=sys.stderr)
        print("[install] Set it before running install, for example:",
              file=sys.stderr)
        print("[install] export HYPERION_HOME=/opt/hyperion", file=sys.stderr)
        raise SystemExit(1)
    return Path(value).expanduser()


def ask_existing_install_mode(path: Path) -> str:
    print(f"[install] Target already exists: {path}")
    print("[install] Choose action:")
    print("[install]   q) quit")
    print("[install]   s) install from scratch (delete existing directory)")
    print("[install]   u) update existing installation")

    while True:
        choice = input("[install] Enter choice [q/s/u]: ").strip().lower()
        if choice in {"q", "quit"}:
            return "quit"
        if choice in {"s", "scratch", "install-from-scratch"}:
            return "scratch"
        if choice in {"u", "update"}:
            return "update"
        print("[install] Invalid choice. Please enter q, s, or u.")


def resolve_binaries(build_type: str) -> tuple[Path, ...]:
    project_root = Path(__file__).resolve().parents[1]
    build_root = project_root / "build"
    config_dir = "Debug" if build_type == "debug" else "Release"
    base = build_root / config_dir

    return (
        base / "src" / "cli" / BINARIES[0],
        base / "src" / "daemon" / BINARIES[1],
    )


def build_context(args: argparse.Namespace) -> InstallContext:
    hyperion_home = require_hyperion_home()
    mode = "fresh"
    build_type = "debug" if args.debug else "release"
    binaries = resolve_binaries(build_type)

    if hyperion_home.exists():
        if not hyperion_home.is_dir():
            print(
                f"[install] ERROR: HYPERION_HOME exists but is not a directory: {hyperion_home}", file=sys.stderr)
            raise SystemExit(1)
        mode = ask_existing_install_mode(hyperion_home)

    return InstallContext(hyperion_home=hyperion_home, mode=mode, build_type=build_type, binaries=binaries)


def ensure_layout_dirs(home: Path) -> None:
    required_dirs = [
        home / "etc",
        home / "bin",
        home / "var" / "log",
        home / "var" / "run",
        home / "var" / "lib",
        home / "var" / "lib" / "workspaces",
    ]

    for directory in required_dirs:
        directory.mkdir(parents=True, exist_ok=True)
        print(f"[install] Ensured directory: {directory}")


def install_binaries(ctx: InstallContext) -> None:
    dest_bin = ctx.hyperion_home / "bin"

    missing = [path for path in ctx.binaries if not path.exists()]
    if missing:
        print("[install] ERROR: missing build artifacts:", file=sys.stderr)
        for path in missing:
            print(f"[install]   - {path}", file=sys.stderr)
        print(
            "[install] Build the selected profile first, then rerun install.", file=sys.stderr)
        raise SystemExit(1)

    for source in ctx.binaries:
        destination = dest_bin / source.name
        shutil.copy2(source, destination)
        destination.chmod(0o755)
        print(f"[install] Installed binary: {destination}")


def resolve_git_version() -> str:
    project_root = Path(__file__).resolve().parents[1]
    try:
        result = subprocess.run(
            ["git", "describe", "--tags", "--abbrev=0"],
            cwd=project_root,
            check=True,
            capture_output=True,
            text=True,
        )
        version = result.stdout.strip()
        return version or "unknown"
    except (subprocess.CalledProcessError, FileNotFoundError):
        return "unknown"


def write_version_file(ctx: InstallContext) -> None:
    version = resolve_git_version()
    version_file = ctx.hyperion_home / "etc" / "version"
    version_file.write_text(f"{version}\n", encoding="utf-8")
    print(f"[install] Wrote version file: {version_file} ({version})")


def write_default_config(ctx: InstallContext) -> None:
    config_file = ctx.hyperion_home / "etc" / "hyperion.toml"

    if ctx.mode == "update":
        print(f"[install] Update mode: not touching config: {config_file}")
        return

    project_root = Path(__file__).resolve().parents[1]
    source_config = project_root / "cfg" / "hyperion.toml"

    if not source_config.exists():
        print(
            f"[install] ERROR: default config not found: {source_config}", file=sys.stderr)
        raise SystemExit(1)

    shutil.copy2(source_config, config_file)
    print(f"[install] Copied default config: {config_file}")


def run_install(ctx: InstallContext) -> int:
    print(f"[install] HYPERION_HOME={ctx.hyperion_home}")
    print(f"[install] Build type={ctx.build_type}")

    if ctx.mode == "quit":
        print("[install] Aborted by user.")
        return 0

    if ctx.mode == "scratch":
        print(f"[install] Removing existing directory: {ctx.hyperion_home}")
        shutil.rmtree(ctx.hyperion_home)
        ctx.hyperion_home.mkdir(parents=True, exist_ok=True)
        print("[install] Clean install directory prepared.")
    elif ctx.mode == "update":
        print("[install] Update mode selected.")
    else:
        ctx.hyperion_home.mkdir(parents=True, exist_ok=True)
        print("[install] Fresh install directory prepared.")

    ensure_layout_dirs(ctx.hyperion_home)

    print("[install] Binaries to install:")
    for binary_path in ctx.binaries:
        if not binary_path.exists():
            print(
                f"[install] ERROR: build artifact not found: {binary_path}", file=sys.stderr)
            print(
                "[install] Build the selected profile first, then rerun install.", file=sys.stderr)
            raise SystemExit(1)

        print(f"[install]   - {binary_path} (exists)")

    print("[install] Pre-checks passed.")

    install_binaries(ctx)
    write_version_file(ctx)
    write_default_config(ctx)

    print("[install] Installation completed successfully.")
    return 0


def main() -> int:
    args = parse_args()
    ctx = build_context(args)
    return run_install(ctx)


if __name__ == "__main__":
    raise SystemExit(main())
