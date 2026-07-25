#!/usr/bin/env python3
"""Analyze an overlay on the EOD archive without building or altering either tree."""
from __future__ import annotations

import argparse
import csv
import hashlib
import json
import re
import xml.etree.ElementTree as ET
from collections import defaultdict
from pathlib import Path

TEXT_SUFFIXES = {".c", ".cpp", ".cxx", ".h", ".hpp", ".inl", ".rc", ".dsp", ".vcproj", ".vcxproj"}
GM_RE = re.compile(r"(?:^|/)(?:XWindow_GM|.*GMCommand)|_XGMCLIENT|_XADMINISTRATORMODE", re.I)


def files(root: Path) -> dict[str, Path]:
    return {p.relative_to(root).as_posix(): p for p in root.rglob("*") if p.is_file()}


def digest(path: Path) -> str:
    h = hashlib.sha256()
    with path.open("rb") as f:
        for block in iter(lambda: f.read(1024 * 1024), b""):
            h.update(block)
    return h.hexdigest()


def text(path: Path) -> str:
    data = path.read_bytes()
    for enc in ("utf-8-sig", "cp949", "cp1252", "latin1"):
        try:
            return data.decode(enc)
        except UnicodeDecodeError:
            pass
    return data.decode("latin1", errors="replace")


def resolve_include(source_rel: str, inc: str, merged: dict[str, Path]) -> str | None:
    lookup = {k.lower(): k for k in merged}
    clean = inc.replace("\\", "/").lstrip("./")
    candidates = [
        (Path(source_rel).parent / clean).as_posix(),
        clean,
        f"XProcess/{clean}",
        f"Library/{clean}",
        f"Library/QHTM/{clean}",
        f"Library/FMod/{clean}",
        f"Library/BinkSDK/{clean}",
        f"Library/radsdk/{clean}",
    ]
    for candidate in candidates:
        if candidate.lower() in lookup:
            return lookup[candidate.lower()]
    by_name = [k for k in merged if Path(k).name.lower() == Path(clean).name.lower()]
    return by_name[0] if len(by_name) == 1 else None


def project_items(project: Path) -> list[str]:
    root = ET.parse(project).getroot()
    result = []
    for elem in root.iter():
        include = elem.attrib.get("Include")
        if include and elem.tag.rsplit("}", 1)[-1] in {"ClCompile", "ClInclude", "ResourceCompile", "Manifest"}:
            result.append(include.replace("\\", "/").lstrip("./"))
    return result


def main() -> None:
    ap = argparse.ArgumentParser()
    ap.add_argument("--archive", type=Path, required=True)
    ap.add_argument("--overlay", type=Path, required=True)
    ap.add_argument("--out", type=Path, required=True)
    args = ap.parse_args()
    args.out.mkdir(parents=True, exist_ok=True)

    archive = files(args.archive)
    overlay = files(args.overlay)
    collisions = sorted(set(archive) & set(overlay))
    merged = dict(archive)
    merged.update(overlay)

    include_rows = []
    include_re = re.compile(r'^\s*#\s*include\s*"([^"]+)"', re.M)
    for rel, path in sorted(overlay.items()):
        if path.suffix.lower() not in TEXT_SUFFIXES:
            continue
        for inc in include_re.findall(text(path)):
            resolved = resolve_include(rel, inc, merged)
            include_rows.append({
                "source": rel,
                "include": inc,
                "resolved_path": resolved or "",
                "status": "resolved" if resolved else "unresolved",
                "gm_named": "yes" if GM_RE.search(inc) else "no",
            })

    project = args.archive / "EmperorOfDragons.vcxproj"
    listed = project_items(project)
    lookup = {k.lower(): k for k in merged}
    missing_project = [p for p in listed if p.lower() not in lookup]

    # Extract member-definition names as a structural cross-check.
    definition_re = re.compile(r"\bXProc_MainGame::([A-Za-z_]\w*)\s*\(")
    definitions = defaultdict(set)
    for rel, path in merged.items():
        if path.suffix.lower() in {".cpp", ".cxx", ".c"}:
            try:
                for name in definition_re.findall(text(path)):
                    definitions[name].add(rel)
            except OSError:
                pass
    header = text(overlay["XProcess/XProc_MainGame.h"])
    handler = text(archive["XProcess/XProc_MainGameMessageHandler.cpp"])
    handler_methods = sorted(set(definition_re.findall(handler)))
    handler_missing_from_header = [
        name for name in handler_methods
        if not re.search(rf"\b{re.escape(name)}\s*\(", header)
    ]

    # Read US Release linker inputs directly from the converted project.
    ns = {"m": "http://schemas.microsoft.com/developer/msbuild/2003"}
    xml_root = ET.parse(project).getroot()
    us_deps = []
    for group in xml_root.findall("m:ItemDefinitionGroup", ns):
        if "US_Release|Win32" not in group.attrib.get("Condition", ""):
            continue
        node = group.find("m:Link/m:AdditionalDependencies", ns)
        if node is not None and node.text:
            us_deps = [x for x in node.text.split(";") if x and "%(" not in x]
    local_deps = [x for x in us_deps if "/" in x or "\\" in x or x.lower() == "speedtreert.lib"]
    local_dep_rows = []
    for dep in local_deps:
        clean = dep.replace("\\", "/").lstrip("./")
        matches = [p for p in merged if p.lower() == clean.lower()]
        # SpeedTree is supplied by a library directory, not a rooted path.
        if not matches and "/" not in clean:
            matches = [p for p in merged if Path(p).name.lower() == clean.lower()]
        local_dep_rows.append({
            "linker_input": dep,
            "resolved_path": matches[0] if len(matches) == 1 else "",
            "status": "resolved" if len(matches) == 1 else ("ambiguous" if matches else "unresolved"),
        })

    with (args.out / "overlay-files.csv").open("w", newline="", encoding="utf-8") as f:
        fields = ["path", "size", "sha256"]
        w = csv.DictWriter(f, fieldnames=fields)
        w.writeheader()
        for rel, path in sorted(overlay.items()):
            w.writerow({"path": rel, "size": path.stat().st_size, "sha256": digest(path)})
    with (args.out / "overlay-quoted-includes.csv").open("w", newline="", encoding="utf-8") as f:
        w = csv.DictWriter(f, fieldnames=include_rows[0].keys())
        w.writeheader()
        w.writerows(include_rows)
    with (args.out / "us-release-local-linker-inputs.csv").open("w", newline="", encoding="utf-8") as f:
        w = csv.DictWriter(f, fieldnames=local_dep_rows[0].keys())
        w.writeheader()
        w.writerows(local_dep_rows)

    result = {
        "overlay_files": len(overlay),
        "overlay_archive_path_collisions": collisions,
        "quoted_includes": len(include_rows),
        "quoted_includes_resolved": sum(r["status"] == "resolved" for r in include_rows),
        "quoted_includes_unresolved": sorted({
            r["include"] for r in include_rows if r["status"] == "unresolved"
        }),
        "unresolved_gm_named_includes": sorted({
            r["include"] for r in include_rows
            if r["status"] == "unresolved" and r["gm_named"] == "yes"
        }),
        "project_items": len(listed),
        "project_items_missing_after_overlay": sorted(missing_project),
        "xproc_maingame_member_names_defined_across_merged_tree": len(definitions),
        "message_handler_member_definitions": len(handler_methods),
        "message_handler_methods_absent_from_recovered_header": handler_missing_from_header,
        "us_release_local_linker_inputs": len(local_dep_rows),
        "us_release_local_linker_inputs_resolved": sum(r["status"] == "resolved" for r in local_dep_rows),
        "us_release_local_linker_inputs_unresolved": [
            r["linker_input"] for r in local_dep_rows if r["status"] == "unresolved"
        ],
    }
    (args.out / "virtual-overlay-summary.json").write_text(
        json.dumps(result, indent=2, sort_keys=True) + "\n", encoding="utf-8"
    )


if __name__ == "__main__":
    main()
