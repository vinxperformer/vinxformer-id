#!/usr/bin/env python3
"""Check the small VINX|FORMER ID site and its local links."""

from html.parser import HTMLParser
from pathlib import Path
from urllib.parse import unquote, urlsplit
import re
import sys


ROOT = Path(__file__).resolve().parents[1]
DOCS = ROOT / "docs"
INDEX = DOCS / "index.html"


class SiteParser(HTMLParser):
    def __init__(self):
        super().__init__(convert_charrefs=True)
        self.ids = set()
        self.refs = []

    def handle_starttag(self, tag, attrs):
        attrs = dict(attrs)
        if attrs.get("id"):
            self.ids.add(attrs["id"])
        attribute = "href" if tag in ("a", "link") else "src" if tag in ("img", "script") else None
        if attribute and attrs.get(attribute):
            self.refs.append((attrs[attribute], self.getpos()[0]))


def main() -> int:
    errors = []
    html_files = sorted(DOCS.rglob("*.html"))
    if html_files != [INDEX]:
        errors.append("ID site must contain only docs/index.html")
    if not INDEX.is_file():
        print("[id-site] FAILED: docs/index.html is missing")
        return 1

    content = INDEX.read_text(encoding="utf-8")
    site_text = re.sub(r"<[^>]+>", "", content)
    if "VINX|FORMER ID" not in site_text:
        errors.append("VINX|FORMER ID identity is missing")

    for required in (
        "PERSONAL PERFORMANCE FIRMWARE",
        "BUILT FOR ONE.",
        "SHARED WITH EVERYONE.",
        "NO ROADMAP. JUST EXPLORATION.",
        "MY WORKFLOW. MY RULES.",
    ):
        if required not in content:
            errors.append(f"missing identity line: {required}")

    parser = SiteParser()
    parser.feed(content)
    for ref, line in parser.refs:
        if ref.startswith(("http://", "https://", "mailto:", "tel:", "data:", "//")):
            continue
        parts = urlsplit(ref)
        if parts.path:
            target = (DOCS / unquote(parts.path).lstrip("/")).resolve() if parts.path.startswith("/") else (INDEX.parent / unquote(parts.path)).resolve()
            if target.is_dir():
                target = target / "index.html"
            if not target.is_file():
                errors.append(f"index.html:{line} missing local target: {ref}")
        elif parts.fragment and parts.fragment not in parser.ids:
            errors.append(f"index.html:{line} missing anchor: {ref}")

    if list(DOCS.rglob("*.beatsteppro")) or list(DOCS.rglob("*.pdf")):
        errors.append("old controller templates or manuals remain in the ID site")

    if errors:
        print("[id-site] FAILED")
        for error in errors:
            print(f"  - {error}")
        return 1
    print("[id-site] OK: identity, content, and local links")
    return 0


if __name__ == "__main__":
    sys.exit(main())
