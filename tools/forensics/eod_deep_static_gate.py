#!/usr/bin/env python3
"""Deeper non-building compatibility checks for the EOD virtual overlay."""
from __future__ import annotations

import argparse
import csv
import hashlib
import json
import re
import subprocess
from collections import Counter, defaultdict
from pathlib import Path

SOURCE_SUFFIXES = {".c", ".cpp", ".cxx", ".h", ".hpp", ".inl"}
TARGET_RE = re.compile(
    r"XWindow_GM|_XWindow_GM|_XGMCLIENT|_XADMINISTRATORMODE|"
    r"MASTER_AND_DISCIPLE|MasternDisciple|MnD(?:Current|Total|Struct|Group|Info)|NPCMnD",
    re.I,
)
MACRO_RE = re.compile(r"\b_[A-Z][A-Z0-9_]{3,}\b")


def read(path: Path) -> str:
    data = path.read_bytes()
    for enc in ("utf-8-sig", "cp949", "cp1252", "latin1"):
        try:
            return data.decode(enc)
        except UnicodeDecodeError:
            pass
    return data.decode("latin1", errors="replace")


def inventory(root: Path) -> dict[str, Path]:
    return {p.relative_to(root).as_posix(): p for p in root.rglob("*") if p.is_file()}


def digest(path: Path) -> str:
    h = hashlib.sha256()
    with path.open("rb") as f:
        for block in iter(lambda: f.read(1024 * 1024), b""):
            h.update(block)
    return h.hexdigest()


def split_params(value: str) -> list[str]:
    value = re.sub(r"/\*.*?\*/|//[^\n]*", "", value, flags=re.S).strip()
    if not value or value == "void":
        return []
    result, start, depth = [], 0, 0
    for i, ch in enumerate(value):
        if ch in "(<[":
            depth += 1
        elif ch in ")>]":
            depth = max(0, depth - 1)
        elif ch == "," and depth == 0:
            result.append(value[start:i].strip())
            start = i + 1
    result.append(value[start:].strip())
    return result


def definitions(path: Path) -> list[dict[str, object]]:
    content = read(path)
    rx = re.compile(
        r"XProc_MainGame::([A-Za-z_]\w*)\s*\((.*?)\)\s*"
        r"(?://[^\n]*)?\s*(const\s*)?\{",
        re.S,
    )
    rows = []
    for match in rx.finditer(content):
        line = content.count("\n", 0, match.start()) + 1
        params = split_params(match.group(2))
        rows.append({
            "definition_file": path.name,
            "definition_line": line,
            "method": match.group(1),
            "definition_parameter_count": len(params),
            "definition_parameters": " ".join(match.group(2).split()),
            "definition_const": "yes" if match.group(3) else "no",
        })
    return rows


def header_declarations(path: Path) -> dict[str, list[dict[str, object]]]:
    content = read(path)
    rx = re.compile(
        r"(?:^|[;{}])\s*(?:virtual\s+)?(?:[~\w:<>,*&]+\s+)+"
        r"([A-Za-z_]\w*)\s*\(([^;{}()]*)\)\s*(const\s*)?;",
        re.M,
    )
    result: dict[str, list[dict[str, object]]] = defaultdict(list)
    for match in rx.finditer(content):
        result[match.group(1)].append({
            "count": len(split_params(match.group(2))),
            "parameters": " ".join(match.group(2).split()),
            "const": "yes" if match.group(3) else "no",
        })
    return result


def reference_rows(rel: str, path: Path) -> list[dict[str, object]]:
    rows = []
    guards: list[str] = []
    for lineno, line in enumerate(read(path).splitlines(), 1):
        stripped = line.strip()
        if re.match(r"#\s*(if|ifdef|ifndef)\b", stripped):
            guards.append(stripped)
        elif re.match(r"#\s*(elif|else)\b", stripped):
            if guards:
                guards[-1] = stripped
        elif re.match(r"#\s*endif\b", stripped):
            if guards:
                guards.pop()
        if TARGET_RE.search(line):
            kind = "GM" if re.search(r"GM|ADMINISTRATOR", line, re.I) else "master/disciple"
            rows.append({
                "path": rel,
                "line": lineno,
                "kind": kind,
                "lexical_guards": " && ".join(guards),
                "text": stripped[:500],
            })
    return rows


def packet_rows(
    scope: dict[str, Path],
    packet_definition_paths: dict[str, set[str]],
) -> list[dict[str, object]]:
    rows = []
    function = ""
    guards: list[str] = []
    function_re = re.compile(
        r"(?:(?:[A-Za-z_]\w*)::)?([A-Za-z_]\w*)\s*\([^;]*\)\s*(?:const\s*)?\{"
    )
    for rel, path in scope.items():
        for lineno, line in enumerate(read(path).splitlines(), 1):
            stripped = line.strip()
            if re.match(r"#\s*(if|ifdef|ifndef)\b", stripped):
                guards.append(stripped)
            elif re.match(r"#\s*(elif|else)\b", stripped):
                if guards:
                    guards[-1] = stripped
            elif re.match(r"#\s*endif\b", stripped):
                if guards:
                    guards.pop()
            match = function_re.search(line)
            if match:
                function = match.group(1)
            packets = sorted(set(re.findall(r"\bMSG_[A-Za-z0-9_]+\b", line)))
            gm_guard = any(
                "_XGMCLIENT" in guard or "_XADMINISTRATORMODE" in guard
                for guard in guards
            )
            if not packets and "SendPacket" not in line:
                continue
            if not (
                rel.startswith("reference-only/gm/")
                or gm_guard
                or re.search(r"\bGM\b|GMCommand|GM_", line, re.I)
            ):
                continue
            rows.append({
                "path": rel,
                "line": lineno,
                "enclosing_function_guess": function,
                "lexical_guards": " && ".join(guards),
                "gm_feature_guarded": "yes" if gm_guard else "no",
                "packet_tokens": ";".join(packets),
                "packet_definition_paths": ";".join(sorted({
                    p for packet in packets for p in packet_definition_paths.get(packet, ())
                })),
                "text": stripped[:500],
            })
    return rows


def nm_summary(path: Path) -> dict[str, object]:
    proc = subprocess.run(
        ["nm", "-A", str(path)],
        check=False,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        text=True,
        errors="replace",
    )
    defined = undefined = imports = stdcall = 0
    dll_names = set()
    for line in proc.stdout.splitlines():
        if re.search(r"\sU\s", line):
            undefined += 1
        elif re.search(r"\s[TDBIR]\s", line):
            defined += 1
        if "__IMPORT_DESCRIPTOR_" in line or "__imp_" in line:
            imports += 1
        if re.search(r"@[0-9]+$", line):
            stdcall += 1
        dll_names.update(re.findall(r"([A-Za-z0-9_.-]+\.dll)", line, re.I))
    return {
        "path": path.as_posix(),
        "nm_exit": proc.returncode,
        "defined_or_import_symbols": defined,
        "undefined_symbols": undefined,
        "import_markers": imports,
        "stdcall_decorated_symbols": stdcall,
        "referenced_dll_names": ";".join(sorted(dll_names, key=str.lower)),
    }


def main() -> None:
    ap = argparse.ArgumentParser()
    ap.add_argument("--archive", type=Path, required=True)
    ap.add_argument("--overlay", type=Path, required=True)
    ap.add_argument("--reference", type=Path)
    ap.add_argument("--out", type=Path, required=True)
    args = ap.parse_args()
    args.out.mkdir(parents=True, exist_ok=True)
    arc, over = inventory(args.archive), inventory(args.overlay)
    reference = inventory(args.reference) if args.reference else {}
    merged = dict(arc)
    merged.update(over)

    header = header_declarations(over["XProcess/XProc_MainGame.h"])
    signature_rows = []
    definition_paths = [
        over["XProcess/XProc_MainGame.cpp"],
        arc["XProcess/XProc_MainGameMessageHandler.cpp"],
    ]
    for path in definition_paths:
        for row in definitions(path):
            candidates = header.get(str(row["method"]), [])
            matching_arity = [
                c for c in candidates
                if c["count"] == row["definition_parameter_count"]
                and c["const"] == row["definition_const"]
            ]
            row["header_declaration_count"] = len(candidates)
            row["matching_arity_and_const"] = "yes" if matching_arity else "no"
            row["header_candidate_parameters"] = " | ".join(
                str(c["parameters"]) for c in candidates
            )
            signature_rows.append(row)

    reference_scope = {
        "overlay/XProcess/XProc_MainGame.cpp": over["XProcess/XProc_MainGame.cpp"],
        "overlay/XProcess/XProc_MainGameCallBackFunctions.cpp":
            over["XProcess/XProc_MainGameCallBackFunctions.cpp"],
        "archive/XProcess/XProc_FirstLoad.cpp": arc["XProcess/XProc_FirstLoad.cpp"],
        "archive/XProcess/XProc_MainGameMessageHandler.cpp":
            arc["XProcess/XProc_MainGameMessageHandler.cpp"],
        "archive/XProcess/XWindow_WorldMinimap.cpp": arc["XProcess/XWindow_WorldMinimap.cpp"],
        "archive/Network/XNetwork.cpp": arc["Network/XNetwork.cpp"],
    }
    refs = []
    for rel, path in reference_scope.items():
        refs.extend(reference_rows(rel, path))

    packet_definition_paths: dict[str, set[str]] = defaultdict(set)
    for rel, path in {**merged, **{f"reference-only/{k}": v for k, v in reference.items()}}.items():
        if path.suffix.lower() not in SOURCE_SUFFIXES:
            continue
        for token in set(re.findall(r"\bMSG_[A-Za-z0-9_]+\b", read(path))):
            packet_definition_paths[token].add(rel)
    packet_scope = dict(reference_scope)
    packet_scope.update({f"reference-only/{k}": v for k, v in reference.items() if k.startswith("gm/")})
    gm_packet_rows = packet_rows(packet_scope, packet_definition_paths)

    defined_macros: dict[str, set[str]] = defaultdict(set)
    used_macros = Counter()
    for rel, path in merged.items():
        if path.suffix.lower() not in SOURCE_SUFFIXES:
            continue
        try:
            content = read(path)
        except OSError:
            continue
        for macro in re.findall(r"^\s*#\s*define\s+([A-Za-z_]\w*)", content, re.M):
            defined_macros[macro].add(rel)
    for rel, path in over.items():
        if path.suffix.lower() not in SOURCE_SUFFIXES:
            continue
        for directive in re.findall(
            r"^\s*#\s*(?:if|ifdef|ifndef|elif)\b([^\n]*)",
            read(path),
            re.M,
        ):
            used_macros.update(MACRO_RE.findall(directive))
    config_macros = {
        "NDEBUG", "WIN32", "_WINDOWS", "NOUSE_VORBIS", "NOUSE_WMA", "_XUSEFMOD",
        "_XNOCHECKMEMORYUSAGE", "_XESTABLISHEDSERVER", "_NEW_TYPE", "_ACCLAIM_VERSION",
        "_XENGLISH", "_ACCLAIM_RUBICONADSYSTEM", "_X_US_EXPANDSERVERLIST",
    }
    macro_rows = []
    for macro, count in sorted(used_macros.items()):
        macro_rows.append({
            "macro": macro,
            "overlay_occurrences": count,
            "defined_in_merged_source": "yes" if macro in defined_macros else "no",
            "defined_by_us_release": "yes" if macro in config_macros else "no",
            "definition_paths": ";".join(sorted(defined_macros.get(macro, ()))),
        })

    libs = [
        p for p in over.values()
        if p.suffix.lower() == ".lib"
    ]
    symbol_rows = [nm_summary(p) for p in sorted(libs)]
    reference_rows_out = []
    if args.reference:
        for rel, path in sorted(reference.items()):
            reference_rows_out.append({
                "path": rel,
                "size": path.stat().st_size,
                "sha256": digest(path),
                "classification": "GM structural reference"
                if rel.startswith("gm/") else "deliberately removed feature reference",
            })

    def write_csv(name: str, rows: list[dict[str, object]]) -> None:
        with (args.out / name).open("w", newline="", encoding="utf-8") as f:
            if not rows:
                return
            w = csv.DictWriter(f, fieldnames=rows[0].keys())
            w.writeheader()
            w.writerows(rows)

    write_csv("xproc-method-signatures.csv", signature_rows)
    write_csv("gm-mnd-reference-map.csv", refs)
    write_csv("overlay-macro-usage.csv", macro_rows)
    write_csv("candidate-library-symbol-summary.csv", symbol_rows)
    write_csv("reference-only-files.csv", reference_rows_out)
    write_csv("gm-packet-reachability.csv", gm_packet_rows)

    summary = {
        "method_definitions_checked": len(signature_rows),
        "method_definitions_with_header_name": sum(
            int(r["header_declaration_count"]) > 0 for r in signature_rows
        ),
        "method_definitions_matching_header_arity_and_const": sum(
            r["matching_arity_and_const"] == "yes" for r in signature_rows
        ),
        "gm_reference_lines_in_selected_core_paths": sum(r["kind"] == "GM" for r in refs),
        "mnd_reference_lines_in_selected_core_paths": sum(
            r["kind"] == "master/disciple" for r in refs
        ),
        "overlay_macros_observed": len(macro_rows),
        "overlay_macros_not_defined_in_merged_source_or_us_config": [
            r["macro"] for r in macro_rows
            if r["defined_in_merged_source"] == "no"
            and r["defined_by_us_release"] == "no"
        ],
        "libraries_inspected_with_nm": len(symbol_rows),
        "reference_only_files": len(reference_rows_out),
        "gm_related_packet_lines": len(gm_packet_rows),
        "gm_related_packet_lines_without_gm_feature_guard": sum(
            r["gm_feature_guarded"] == "no" for r in gm_packet_rows
        ),
        "gm_packet_tokens_observed": sorted({
            token
            for r in gm_packet_rows
            for token in str(r["packet_tokens"]).split(";")
            if token
        }),
        "libraries_with_x86_stdcall_decoration": [
            r["path"] for r in symbol_rows if int(r["stdcall_decorated_symbols"]) > 0
        ],
        "imported_runtime_dll_names": sorted({
            dll
            for r in symbol_rows
            for dll in str(r["referenced_dll_names"]).split(";")
            if dll
        }, key=str.lower),
    }
    (args.out / "deep-static-gate-summary.json").write_text(
        json.dumps(summary, indent=2, sort_keys=True) + "\n", encoding="utf-8"
    )


if __name__ == "__main__":
    main()
