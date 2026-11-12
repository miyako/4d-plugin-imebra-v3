#!/usr/bin/env python3
"""Compile complete native sources with memory-only codec and host boundaries."""
import hashlib
import os
from pathlib import Path
import subprocess
import sys
import tempfile

args = sys.argv[1:]
expect = None
if args[:1] == ["--expect-fail"]:
    expect = set(Path(args[1]).read_text().split())
    args = args[2:]
root = Path(args[0] if args else ".").resolve()
here = Path(__file__).resolve().parent
sdk = root / "imebra/4D Plugin API"
sources = [sdk / "4DPluginAPI.c", *sorted((sdk / "Classes").glob("*.cpp")),
           root / "imebra/4DPlugin.cpp", here / "main.mm"]
for path in sorted(set(sources + list(sdk.rglob("*.h")) +
                       [root / "imebra/4DPlugin.h", root / "imebra/manifest.json",
                        here / "run.py"] + list((here / "boundaries").rglob("*.h")))):
    print(hashlib.sha256(path.read_bytes()).hexdigest(), path, flush=True)

with tempfile.TemporaryDirectory(prefix="imebra-ownership-") as directory:
    out = Path(directory)
    flags = ["-g", "-O1", "-fsanitize=address,undefined", "-fno-sanitize=alignment",
             "-fno-omit-frame-pointer", "-Wno-deprecated-declarations", "-Wno-multichar",
             "-Wno-deprecated-builtins", "-Wno-return-type-c-linkage",
             "-include", "Cocoa/Cocoa.h", "-I" + str(here / "boundaries"), "-I" + str(sdk),
             "-I" + str(sdk / "Classes")]
    objects = []
    for i, source in enumerate(sources):
        obj = out / (str(i) + ".o")
        subprocess.run(["clang++", *flags, "-x", "objective-c++", "-std=c++14",
                        "-c", str(source), "-o", str(obj)], check=True)
        objects.append(str(obj))
    exe = out / "ownership"
    subprocess.run(["clang++", *flags, *objects, "-framework", "Cocoa",
                    "-framework", "CoreFoundation", "-o", str(exe)], check=True)
    imports = subprocess.check_output(["nm", "-u", str(exe)], text=True)
    exact = {
        "_abort", "_calloc", "_free", "_malloc", "_realloc", "_memcmp", "_memcpy",
        "_memmove", "_memset", "_strlen", "_wcslen", "_isspace", "_sprintf",
        "_snprintf", "_vsnprintf", "_printf", "_fprintf", "_fwrite", "_puts",
        "_exit", "_atexit", "_bzero", "dyld_stub_binder", "__Unwind_Resume",
        "__tlv_bootstrap", "_memchr", "_kCFAllocatorDefault", "_CFRelease",
        "_CFStringCreateWithBytes", "_CFStringCreateWithCharacters", "_CFStringGetLength",
        "_CFStringGetCharacters", "_CFStringGetMaximumSizeForEncoding", "_CFStringGetBytes",
        "_class_getClassMethod", "_class_getInstanceMethod", "_method_setImplementation",
        "_sel_registerName", "___stack_chk_fail", "___stack_chk_guard",
        "_strcmp", "_strcpy", "_strncmp", "__DefaultRuneLocale", "_NSZeroSize",
    }
    unexpected = []
    for symbol in (s.strip() for s in imports.splitlines() if s.strip()):
        if any(s in symbol for s in ("fstream", "filebuf", "filesystem", "random_device")):
            raise RuntimeError("Forbidden external-data import: " + symbol)
        if symbol.startswith("_OBJC_CLASS_$_") and symbol[14:] in {
            "NSString", "NSMutableString", "NSURL", "NSImage"
        }:
            continue
        if symbol not in exact and not symbol.startswith(("___", "__Z", "_objc_")):
            unexpected.append(symbol)
    if unexpected:
        raise RuntimeError("Unexpected imports before execution: " + " ".join(unexpected))
    linked = subprocess.check_output(["otool", "-L", str(exe)], text=True)
    if any(n in linked.lower() for n in ("libgd.", "libimebra", "libjpeg", "libpng", "libtiff")):
        raise RuntimeError("Real codec linked")
    print("PASS pre-execution synthetic boundary/import gate", flush=True)
    env = dict(os.environ, ASAN_OPTIONS="detect_leaks=0:abort_on_error=0", UBSAN_OPTIONS="halt_on_error=1:print_stacktrace=1")
    names = subprocess.check_output([str(exe), "--list"], env=env, text=True).split()
    failed = []
    for name in names:
        result = subprocess.run([str(exe), name], env=env, capture_output=True, text=True)
        sys.stdout.write(result.stdout)
        if result.returncode:
            sys.stdout.write(result.stderr[-6000:])
            failed.append(name)
        print("CASE %s exit=%d" % (name, result.returncode), flush=True)
    print("cases=%d passed=%d failed=%d" % (len(names), len(names) - len(failed), len(failed)))
    print("failed_cases=" + ",".join(failed), flush=True)
    if expect is None:
        sys.exit(1 if failed else 0)
    if set(failed) != expect:
        print("EXPECTATION MISMATCH unexpected=%s missing=%s" % (
            sorted(set(failed) - expect), sorted(expect - set(failed))))
        sys.exit(1)
    print("PASS exact expected baseline failure set (%d)" % len(expect))
