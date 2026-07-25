#!/usr/bin/env python3
"""Static comparison of the EOD archive tree and spree/project2.

This never loads or executes any file from either client tree.
"""
from __future__ import annotations

import argparse
import csv
import difflib
import hashlib
import json
import re
from collections import Counter, defaultdict, deque
from pathlib import Path

TEXT_EXTS = {
    ".c", ".cc", ".cpp", ".cxx", ".h", ".hh", ".hpp", ".inl", ".rc", ".rc2",
    ".dsp", ".dsw", ".mak", ".sln", ".vcproj", ".vcxproj", ".filters", ".txt",
    ".ini", ".xml", ".html", ".htm", ".js", ".css", ".def", ".bat", ".cmd",
}
SOURCE_EXTS = {".c", ".cc", ".cpp", ".cxx", ".h", ".hh", ".hpp", ".inl", ".rc", ".rc2"}
GENERATED_DIRS = {
    "debug", "release", ".vs", "backup", "_upgradereport_files", "ipch",
}
GM_MARKERS = (
    "gmclient", "gm_", "gmcommand", "gamemaster", "administrator", "adminmode",
)
THIRD_PARTY_MARKERS = (
    "library/", "fmod", "bink", "radsdk", "qhtm", "speedtree", "flash",
    "gameguard", "hshield", "xtrap",
)


def sha256(path: Path) -> str:
    h = hashlib.sha256()
    with path.open("rb") as f:
        for block in iter(lambda: f.read(1024 * 1024), b""):
            h.update(block)
    return h.hexdigest()


def is_text(path: Path) -> bool:
    return path.suffix.lower() in TEXT_EXTS


def read_text(path: Path) -> str:
    data = path.read_bytes()
    for enc in ("utf-8-sig", "cp949", "cp1252", "latin1"):
        try:
            return data.decode(enc)
        except UnicodeDecodeError:
            pass
    return data.decode("latin1", errors="replace")


def normalized_text_hash(path: Path) -> str:
    text = read_text(path).replace("\r\n", "\n").replace("\r", "\n")
    return hashlib.sha256(text.encode("utf-8")).hexdigest()


def inventory(root: Path) -> dict[str, Path]:
    return {
        p.relative_to(root).as_posix(): p
        for p in root.rglob("*")
        if p.is_file() and ".git" not in p.parts
    }


def category(rel: str) -> str:
    low = rel.lower()
    parts = set(low.split("/"))
    if any(marker in low for marker in GM_MARKERS):
        return "GM/admin"
    if any(part in GENERATED_DIRS for part in parts) or Path(low).suffix in {
        ".obj", ".pdb", ".ilk", ".idb", ".sbr", ".bsc", ".ncb", ".suo", ".user", ".log"
    }:
        return "generated/build artifact"
    if any(marker in low for marker in THIRD_PARTY_MARKERS):
        return "third-party/middleware"
    if low.startswith("xlauncher"):
        return "launcher/patcher"
    if low.startswith("xprocess/"):
        return "normal player/game process"
    if low.startswith(("xkernel/", "xfcontrol/", "xsound/", "audiolib/", "filtering/", "nmclass/")):
        return "shared engine/internal library"
    if Path(low).suffix in SOURCE_EXTS:
        return "normal/shared source"
    return "resource/config/other"


def include_targets(files: dict[str, Path]) -> tuple[dict[str, set[str]], dict[str, set[str]]]:
    """Return forward and reverse include edges, resolving case-insensitively."""
    lower = {k.lower(): k for k in files}
    basename = defaultdict(list)
    for rel in files:
        basename[Path(rel).name.lower()].append(rel)
    forward: dict[str, set[str]] = defaultdict(set)
    reverse: dict[str, set[str]] = defaultdict(set)
    rx = re.compile(r'^\s*#\s*include\s*"([^"]+)"', re.M)
    for rel, path in files.items():
        if not is_text(path):
            continue
        try:
            includes = rx.findall(read_text(path))
        except OSError:
            continue
        parent = Path(rel).parent
        for inc in includes:
            clean = inc.replace("\\", "/").lstrip("./")
            candidates = [
                (parent / clean).as_posix().lower(),
                clean.lower(),
            ]
            target = next((lower[c] for c in candidates if c in lower), None)
            if target is None:
                matches = basename.get(Path(clean).name.lower(), [])
                if len(matches) == 1:
                    target = matches[0]
            if target:
                forward[rel].add(target)
                reverse[target].add(rel)
    return forward, reverse


def dependency_partition(files: dict[str, Path], forward: dict[str, set[str]]) -> dict[str, str]:
    """Conservative static partition: explicit GM roots and their private closure."""
    gm_roots = {r for r in files if category(r) == "GM/admin"}
    gm_closure = set(gm_roots)
    q = deque(gm_roots)
    while q:
        src = q.popleft()
        for dep in forward.get(src, ()):
            if dep not in gm_closure:
                gm_closure.add(dep)
                q.append(dep)
    normal_roots = {
        r for r in files
        if (r.lower().startswith("xprocess/") or Path(r).suffix.lower() in SOURCE_EXTS)
        and r not in gm_roots
    }
    normal_closure = set(normal_roots)
    q = deque(normal_roots)
    while q:
        src = q.popleft()
        for dep in forward.get(src, ()):
            if dep not in normal_closure:
                normal_closure.add(dep)
                q.append(dep)
    result = {}
    for rel in files:
        in_gm = rel in gm_closure
        in_normal = rel in normal_closure
        if rel in gm_roots:
            result[rel] = "GM-only root"
        elif in_gm and in_normal:
            result[rel] = "shared by GM and normal"
        elif in_gm:
            result[rel] = "GM dependency only"
        elif in_normal:
            result[rel] = "normal player dependency"
        else:
            result[rel] = "unclassified/non-code"
    return result


def main() -> None:
    ap = argparse.ArgumentParser()
    ap.add_argument("--archive", type=Path, required=True)
    ap.add_argument("--github", type=Path, required=True)
    ap.add_argument("--out", type=Path, required=True)
    args = ap.parse_args()
    args.out.mkdir(parents=True, exist_ok=True)

    arc = inventory(args.archive)
    gh = inventory(args.github)
    arc_ci = defaultdict(list)
    gh_ci = defaultdict(list)
    for rel in arc:
        arc_ci[rel.lower()].append(rel)
    for rel in gh:
        gh_ci[rel.lower()].append(rel)

    shared_keys = sorted(set(arc_ci) & set(gh_ci))
    archive_only_keys = sorted(set(arc_ci) - set(gh_ci))
    github_only_keys = sorted(set(gh_ci) - set(arc_ci))

    shared_rows = []
    status_count = Counter()
    for key in shared_keys:
        ar = arc_ci[key][0]
        gr = gh_ci[key][0]
        apath, gpath = arc[ar], gh[gr]
        ah, ghh = sha256(apath), sha256(gpath)
        if ah == ghh:
            status = "byte-identical"
        elif is_text(apath) and is_text(gpath) and normalized_text_hash(apath) == normalized_text_hash(gpath):
            status = "text-identical (encoding/EOL differs)"
        else:
            status = "content-different"
        archive_added_lines = ""
        archive_removed_lines = ""
        similarity = ""
        if status == "content-different" and is_text(apath) and is_text(gpath):
            alines = read_text(apath).replace("\r\n", "\n").replace("\r", "\n").splitlines()
            glines = read_text(gpath).replace("\r\n", "\n").replace("\r", "\n").splitlines()
            matcher = difflib.SequenceMatcher(None, glines, alines, autojunk=False)
            added = removed = equal = 0
            for tag, i1, i2, j1, j2 in matcher.get_opcodes():
                if tag in ("insert", "replace"):
                    added += j2 - j1
                if tag in ("delete", "replace"):
                    removed += i2 - i1
                if tag == "equal":
                    equal += i2 - i1
            archive_added_lines = added
            archive_removed_lines = removed
            similarity = round((2 * equal / (len(alines) + len(glines))) if alines or glines else 1.0, 6)
        status_count[status] += 1
        shared_rows.append({
            "archive_path": ar,
            "github_path": gr,
            "category": category(ar),
            "status": status,
            "archive_size": apath.stat().st_size,
            "github_size": gpath.stat().st_size,
            "archive_added_lines_vs_github": archive_added_lines,
            "archive_removed_lines_vs_github": archive_removed_lines,
            "line_similarity": similarity,
            "archive_mtime": int(apath.stat().st_mtime),
            "archive_sha256": ah,
            "github_sha256": ghh,
        })

    forward, reverse = include_targets(gh)
    partition = dependency_partition(gh, forward)
    archive_hashes = defaultdict(list)
    archive_basenames = defaultdict(list)
    for ar, path in arc.items():
        archive_hashes[sha256(path)].append(ar)
        archive_basenames[Path(ar).name.lower()].append(ar)
    github_only_rows = []
    for key in github_only_keys:
        for gr in gh_ci[key]:
            users = sorted(reverse.get(gr, ()))
            file_hash = sha256(gh[gr])
            github_only_rows.append({
                "github_path": gr,
                "category": category(gr),
                "dependency_partition": partition.get(gr, "unclassified/non-code"),
                "size": gh[gr].stat().st_size,
                "sha256": file_hash,
                "archive_identical_elsewhere": ";".join(sorted(archive_hashes.get(file_hash, ()))),
                "archive_same_basename_elsewhere": ";".join(
                    sorted(archive_basenames.get(Path(gr).name.lower(), ()))
                ),
                "included_by_count": len(users),
                "included_by": ";".join(users),
            })

    with (args.out / "shared-files.csv").open("w", newline="", encoding="utf-8") as f:
        w = csv.DictWriter(f, fieldnames=shared_rows[0].keys())
        w.writeheader()
        w.writerows(shared_rows)
    with (args.out / "shared-content-differences.patch").open("w", encoding="utf-8") as f:
        for row in shared_rows:
            if row["status"] != "content-different":
                continue
            ar, gr = row["archive_path"], row["github_path"]
            apath, gpath = arc[ar], gh[gr]
            if is_text(apath) and is_text(gpath):
                glines = read_text(gpath).replace("\r\n", "\n").replace("\r", "\n").splitlines(True)
                alines = read_text(apath).replace("\r\n", "\n").replace("\r", "\n").splitlines(True)
                f.writelines(difflib.unified_diff(
                    glines,
                    alines,
                    fromfile=f"github/project2/{gr}",
                    tofile=f"archive/EmperorOfDragons/{ar}",
                    n=3,
                ))
            else:
                f.write(
                    f"Binary files differ: github/project2/{gr} "
                    f"[{row['github_sha256']}] and archive/EmperorOfDragons/{ar} "
                    f"[{row['archive_sha256']}]\n"
                )
    with (args.out / "github-only-files.csv").open("w", newline="", encoding="utf-8") as f:
        w = csv.DictWriter(f, fieldnames=github_only_rows[0].keys())
        w.writeheader()
        w.writerows(github_only_rows)
    for filename, predicate in (
        ("github-only-gm.csv", lambda r: r["dependency_partition"] in {"GM-only root", "GM dependency only"}),
        ("github-only-normal-player.csv", lambda r: r["dependency_partition"] in {
            "normal player dependency", "shared by GM and normal"
        }),
        ("github-only-unclassified.csv", lambda r: r["dependency_partition"] == "unclassified/non-code"),
    ):
        selected = [r for r in github_only_rows if predicate(r)]
        with (args.out / filename).open("w", newline="", encoding="utf-8") as f:
            w = csv.DictWriter(f, fieldnames=github_only_rows[0].keys())
            w.writeheader()
            w.writerows(selected)
    with (args.out / "archive-only-files.txt").open("w", encoding="utf-8") as f:
        for key in archive_only_keys:
            for rel in arc_ci[key]:
                f.write(rel + "\n")

    summary = {
        "archive_files": len(arc),
        "github_files": len(gh),
        "shared_case_insensitive_paths": len(shared_keys),
        "archive_only_case_insensitive_paths": len(archive_only_keys),
        "github_only_case_insensitive_paths": len(github_only_keys),
        "shared_status": dict(status_count),
        "github_only_categories": dict(Counter(r["category"] for r in github_only_rows)),
        "github_only_dependency_partition": dict(Counter(r["dependency_partition"] for r in github_only_rows)),
        "case_path_differences": sum(arc_ci[k][0] != gh_ci[k][0] for k in shared_keys),
    }
    (args.out / "comparison-summary.json").write_text(
        json.dumps(summary, indent=2, sort_keys=True) + "\n", encoding="utf-8"
    )


if __name__ == "__main__":
    main()
