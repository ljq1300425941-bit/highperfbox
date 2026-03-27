#!/usr/bin/env python3
import json
import argparse
from pathlib import Path
from collections import OrderedDict


def load_benchmarks(json_path: Path):
    with json_path.open("r", encoding="utf-8") as f:
        data = json.load(f)

    benches = data.get("benchmarks", [])
    result = []

    for b in benches:
        if "aggregate_name" in b:
            continue

        name = b.get("name", "")
        cpu_time = b.get("cpu_time", None)
        cpu_unit = b.get("time_unit", "ns")

        hit_rate = None
        ops_per_sec = None

        if "HitRate" in b:
            hit_rate = b["HitRate"]
        elif "counters" in b and isinstance(b["counters"], dict):
            hit_rate = b["counters"].get("HitRate")

        if "OpsPerSec" in b:
            ops_per_sec = b["OpsPerSec"]
        elif "counters" in b and isinstance(b["counters"], dict):
            ops_per_sec = b["counters"].get("OpsPerSec")

        result.append({
            "name": name,
            "cpu_time": cpu_time,
            "cpu_unit": cpu_unit,
            "hit_rate": hit_rate,
            "ops_per_sec": ops_per_sec,
        })

    return result


def format_hit_rate(x):
    if x is None:
        return ""
    if 0.0 <= x <= 1.0:
        return f"{x * 100:.2f}%"
    return f"{x:.4f}"


def format_ops(x):
    if x is None:
        return ""
    if x >= 1_000_000:
        return f"{x / 1_000_000:.2f}M/s"
    if x >= 1_000:
        return f"{x / 1_000:.2f}K/s"
    return f"{x:.2f}/s"


def format_cpu(x, unit):
    if x is None:
        return ""
    return f"{x:.0f} {unit}"


def group_name(prefix: str) -> str:
    mapping = {
        "LRU": "Default Cases",
        "LRU_KeySpaceSweep": "Key Space Sweep",
        "LRU_HotspotSweep": "Hotspot Intensity Sweep",
        "LRU_ZipfSweep": "Zipf Alpha Sweep",
    }
    return mapping.get(prefix, prefix)


def extract_prefix(name: str) -> str:
    if "/" not in name:
        return name
    return name.split("/", 1)[0]


def strip_prefix(name: str) -> str:
    if "/" not in name:
        return name
    return name.split("/", 1)[1]


def render_table(rows, simplify_name=False):
    lines = []
    lines.append("| Benchmark | CPU Time | HitRate | OpsPerSec |")
    lines.append("|-----------|----------|---------|-----------|")

    for r in rows:
        display_name = strip_prefix(r["name"]) if simplify_name else r["name"]
        lines.append(
            f"| {display_name} | "
            f"{format_cpu(r['cpu_time'], r['cpu_unit'])} | "
            f"{format_hit_rate(r['hit_rate'])} | "
            f"{format_ops(r['ops_per_sec'])} |"
        )

    return "\n".join(lines)


def to_markdown(rows, title=None, group_by_prefix=False, simplify_name=False):
    lines = []

    if title:
        lines.append(f"# {title}")
        lines.append("")

    if not group_by_prefix:
        lines.append(render_table(rows, simplify_name=simplify_name))
        return "\n".join(lines)

    grouped = OrderedDict()
    for r in rows:
        prefix = extract_prefix(r["name"])
        section = group_name(prefix)
        grouped.setdefault(section, []).append(r)

    for section, section_rows in grouped.items():
        lines.append(f"## {section}")
        lines.append("")
        lines.append(render_table(section_rows, simplify_name=simplify_name))
        lines.append("")

    return "\n".join(lines).rstrip()


def main():
    parser = argparse.ArgumentParser(description="Convert Google Benchmark JSON to Markdown table")
    parser.add_argument("input", help="path to benchmark json")
    parser.add_argument("-o", "--output", help="path to markdown output file")
    parser.add_argument("--title", help="optional markdown title")
    parser.add_argument("--group-by-prefix", action="store_true",
                        help="group benchmarks by prefix such as LRU / LRU_KeySpaceSweep / LRU_HotspotSweep / LRU_ZipfSweep")
    parser.add_argument("--simplify-name", action="store_true",
                        help="remove prefix before rendering benchmark name")
    args = parser.parse_args()

    input_path = Path(args.input)
    rows = load_benchmarks(input_path)
    md = to_markdown(
        rows,
        title=args.title,
        group_by_prefix=args.group_by_prefix,
        simplify_name=args.simplify_name,
    )

    if args.output:
        output_path = Path(args.output)
        output_path.parent.mkdir(parents=True, exist_ok=True)
        output_path.write_text(md, encoding="utf-8")
        print(f"[OK] wrote markdown to {output_path}")
    else:
        print(md)


if __name__ == "__main__":
    main()