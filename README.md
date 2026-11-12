![version](https://img.shields.io/badge/version-18%2B-EB8E5F)
![platform](https://img.shields.io/static/v1?label=platform&message=mac-intel%20|%20mac-arm%20|%20win-64&color=blue)
[![license](https://img.shields.io/github/license/miyako/4d-plugin-imebra-v3)](LICENSE)
![downloads](https://img.shields.io/github/downloads/miyako/4d-plugin-imebra-v3/total)

# 4d-plugin-imebra-v3
Simple DICOM reader using [Imebra](https://imebra.com) with some [LibGD](https://libgd.github.io) filters.

### Syntax

[miyako.github.io](https://miyako.github.io/2019/06/11/4d-plugin-imebra-v3.html)

### Ownership tests

`tests/ownership/run.py` compiles the complete bundled SDK, the wrapper classes and the exported dispatcher with ASan/UBSan against a memory-only 4D engine and synthetic Imebra/gd stubs (no codecs, files or DICOM data), then runs every case in its own process:

```sh
python3 tests/ownership/run.py .
# reproduce the exact pre-fix failure set on an archive of a5feaa5
python3 tests/ownership/run.py --expect-fail tests/ownership/baseline-expected-failures.txt <baseline-tree>
```

Engine out-parameter pictures (commands 678/679) have no documented release contract and are reported as `DEFERRED`.
