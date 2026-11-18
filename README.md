![version](https://img.shields.io/badge/version-18%2B-EB8E5F)
![platform](https://img.shields.io/static/v1?label=platform&message=mac-intel%20|%20mac-arm%20|%20win-64&color=blue)
[![license](https://img.shields.io/github/license/miyako/4d-plugin-imebra-v3)](LICENSE)
![downloads](https://img.shields.io/github/downloads/miyako/4d-plugin-imebra-v3/total)

# 4d-plugin-imebra-v3
Simple DICOM reader using [Imebra](https://imebra.com) with some [LibGD](https://libgd.github.io) filters.

| Command | Returns | Purpose |
| --- | --- | --- |
| [Imebra Get images](#imebra-get-images) | Object | Decode a DICOM blob and return its images (and optionally its tags) |
| [Imebra Apply filters](#imebra-apply-filters) | Object | Run LibGD filters over an encoded image and return the results |

**Platforms:** macOS (Intel + Apple Silicon, universal binary), Windows 64-bit.

---

## Requirements & platform notes

- Both commands take **two parameters**: a `Blob` containing the file data, and an `Object` of options. The options object may be omitted or empty — every option has a default.
- Both commands **always return an Object**. There is no 4D error on failure: if the input can't be decoded, or no `filters` collection is given to `Imebra Apply filters`, the returned object's collections are simply empty. Check the collection counts, not a return code.
- `Imebra Apply filters` returns **two** images per call: the image before the last filter and the image after all filters. With zero or one filter, `images[1]` is the filtered (or unchanged) result and `images[0]` is the original.
- `Imebra Get images` applies the DICOM modality VOI/LUT and color-space transforms automatically, so output pixels are display-ready 8-bit-per-channel RGBA.
- An older write-up of the original syntax lives at [miyako.github.io](https://miyako.github.io/2019/06/11/4d-plugin-imebra-v3.html); the reference below is authoritative for the current build.

---

## Imebra Get images

### Syntax

```4d
Imebra Get images ( blob ; options ) → Object
```

| Parameter | Type | Description |
| --- | --- | --- |
| `blob` | Blob | The DICOM file contents (the whole file as a blob, not a path) |
| `options` | Object | Output options; may be omitted or empty |
| `Result` | Object | `{ images : Collection, tags : Collection }` — see below |

### Options object

| Property | Type | Description |
| --- | --- | --- |
| `format` | Text | Output image format: `".bmp"` (default), `".png"`, `".jpg"` or `".jpeg"`, `".gif"`, `".wbmp"`, `".webp"`, `".tiff"` or `".tif"` |
| `tags` | Boolean | When true, the result also contains a `tags` collection with the file's DICOM tags |
| `count` | Number | Maximum number of images to return; `-1` or omitted means all pages |
| `quality` | Number | JPEG quality 0–95 (`0` = encoder default); also used as WebP quality 0–100 |
| `level` | Number | PNG compression level 0–9 (`-1` = default) |
| `compression` | Number | BMP RLE compression: any non-zero value enables it |
| `fg` | Number | WBMP foreground color index |

### Result object

| Property | Type | Description |
| --- | --- | --- |
| `images` | Collection of Object | One entry per image/page |
| `tags` | Collection of Object | Present only when `options.tags` is true |

Each `images` element:

| Property | Type | Description |
| --- | --- | --- |
| `width` | Longint | Pixel width |
| `height` | Longint | Pixel height |
| `colorspace` | Text | The image's color space as reported by Imebra |
| `image` | Picture | The rendered image in the requested `format` |
| `format` | Text | The format the picture was encoded in (e.g. `".png"`) |
| `size` | Longint | Encoded picture size in bytes |
| `level` / `quality` / `compression` / `fg` | Longint | The encoder setting actually used, present only for the matching `format` |

Each `tags` element:

| Property | Type | Description |
| --- | --- | --- |
| `type` | Text | The DICOM value representation (VR), two characters (e.g. `"OB"`, `"UI"`) |
| `value` | Text | The tag's textual value |
| `id` | Longint | Tag element number |
| `group` | Longint | Tag group number |
| `order` | Longint | Tag order within the dataset |
| `index` | Longint | Index of this value when a tag holds several values |

### Description

The blob is decoded by Imebra (DICOM and the other formats its codec factory supports). Every image/page in the dataset is rendered through the modality transform and any needed color-space conversion, then encoded into the requested `format` by LibGD. Multi-frame datasets return one `images` element per frame, up to `count`.

If the blob cannot be decoded as an image dataset, the command returns an object whose `images` (and `tags`) collections are empty — it does not raise a 4D error.

### Example

```4d
C_BLOB($tDICOM)

// read your .dcm file into $tDICOM however you normally load files

$objOptions := New object
$objOptions.format := ".png"
$objOptions.count := 1

$objResult := Imebra Get images ($tDICOM ; $objOptions)

If ($objResult.images.Length > 0)
	$objImage := $objResult.images[1]
	// $objImage.image is a 4D Picture, $objImage.width/.height/.colorspace describe it
End if
```

To also dump the DICOM tags:

```4d
$objOptions.tags := True
$objResult := Imebra Get images ($tDICOM ; $objOptions)

For each ($objTag ; $objResult.tags)
	// $objTag.group / .id / .type / .value
End for each
```

---

## Imebra Apply filters

### Syntax

```4d
Imebra Apply filters ( blob ; options ) → Object
```

| Parameter | Type | Description |
| --- | --- | --- |
| `blob` | Blob | An encoded image file (BMP, PNG, JPEG, GIF, TIFF, WBMP or WebP) |
| `options` | Object | Filter chain and output options; may be omitted or empty |
| `Result` | Object | `{ images : Collection, filters : Collection }` — see below |

### Options object

| Property | Type | Description |
| --- | --- | --- |
| `format` | Text | **Input** image format of the blob: `".bmp"` (default), `".png"`, `".jpg"` or `".jpeg"`, `".gif"`, `".wbmp"`, `".webp"`, `".tiff"` or `".tif"`. The output uses the same format |
| `filters` | Collection of Object | The filters to apply, in order. If absent, the result collections are empty |
| `quality` | Number | JPEG quality 0–95 (`0` = encoder default); also used as WebP quality 0–100 |
| `level` | Number | PNG compression level 0–9 (`-1` = default) |
| `compression` | Number | BMP RLE compression: any non-zero value enables it |
| `fg` | Number | WBMP foreground color index |

Each `filters` element is an Object with a `filter` property naming the LibGD filter, plus the parameters listed below. Unknown names are silently skipped.

| `filter` value | Extra properties | Effect |
| --- | --- | --- |
| `"selectiveBlur"` | — | Selective blur |
| `"edgeDetectQuick"` | — | Quick edge detection |
| `"emboss"` | — | Emboss |
| `"meanRemoval"` | — | Mean removal |
| `"grayScale"` | — | Convert to grayscale |
| `"negate"` | — | Invert colors |
| `"smooth"` | `weight` (Number) | Smooth |
| `"brightness"` | `brightness` (Number) | Adjust brightness |
| `"contrast"` | `contrast` (Number) | Adjust contrast |
| `"rotate"` | `angle`, `dstW`, `dstH`, `dstX`, `dstY`, `srcW`, `srcH`, `srcX`, `srcY` (Number) | Arbitrary rotation/copy into a new canvas; `dst*` is the destination rectangle, `src*` the source rectangle |
| `"scatter"` | `sub`, `plus` (Number) | Scatter |
| `"pixelate"` | `size`, `mode` (Number) | Pixelate |
| `"gaussianBlur"` | `radius` (Number), `sigma` (Number) | Gaussian blur |
| `"color"` | `red`, `green`, `blue`, `alpha` (Number) | Colorize |
| `"scatterColor"` | `sub`, `plus` (Number), `colors` (Collection of Number) | Scatter using the given palette; ignored when `colors` is empty |
| `"convolution"` | `div`, `offset` (Number), `matrix` (Collection of 3 rows × 3 Numbers) | 3×3 convolution kernel |

### Result object

| Property | Type | Description |
| --- | --- | --- |
| `images` | Collection of Object | Exactly two entries when filters ran: `images[1]` = before the last filter (or the original when there is at most one filter), `images[2]` = after all filters. Each element has the same shape as `Imebra Get images` (`width`, `height`, `colorspace`, `image`, `format`, `size`, encoder setting) |
| `filters` | Collection of Object | Copies of the filter objects that were actually applied, in order |

### Description

The blob is decoded by LibGD into the format named by `options.format`, the `filters` collection is applied in order, and both intermediate and final bitmaps are re-encoded in that same format. Filters that fail or are unknown are not recorded in the result's `filters` collection.

If the blob can't be decoded, or `options.filters` is missing, the command returns an object with empty `images` and `filters` collections.

### Example

```4d
C_BLOB($tImage)

// $tImage holds a .bmp file

$colFilters := New collection

$f1 := New object
$f1.filter := "gaussianBlur"
$f1.radius := 1
$f1.sigma := 1.5
$colFilters.push($f1)

$f2 := New object
$f2.filter := "negate"
$colFilters.push($f2)

$objOptions := New object
$objOptions.format := ".png"
$objOptions.filters := $colFilters

$objResult := Imebra Apply filters ($tImage ; $objOptions)

If ($objResult.images.Length = 2)
	// $objResult.images[1].image = blurred original
	// $objResult.images[2].image = blurred + negated
End if
```

---

## Error handling & troubleshooting

- **Empty result, no error.** Both commands report failure by returning empty collections, not by raising a 4D error. Always check `images.Length` (and `tags.Length` / `filters.Length`) before reading elements.
- **`Imebra Get images` returns objects without pixels for undecodable frames.** Frames that Imebra can't render still produce an `images` element with `width`/`height`/`colorspace` but no `image` property.
- **RGB color images.** Color-space conversion to RGB is applied automatically; `DrawBitmap` handles the final RGB conversion, so `colorspace` may report the source space while pixels are RGB.
- **`count` is a maximum, not an index.** `count := 3` returns up to 3 images starting at the first frame; there is no way to request a frame range.
- **`filters` must be a collection of objects.** A missing or non-collection `filters` property yields empty results; a filter object without a `filter` name, or with an unknown name, is skipped silently.
- **`scatterColor` needs `colors`.** Without a non-empty `colors` collection the filter is not applied.
- **Parameter types are strict-ish.** Numbers are read as reals and truncated to integers; text properties are compared byte-for-byte, so `".JPG"` will not match `".jpg"`.

## Quick reference

```4d
// DICOM blob -> pictures (+ optional tags)
$objOptions := New object
$objOptions.format := ".png"
$objResult := Imebra Get images ($tDICOM ; $objOptions)

// encoded image blob -> filtered pictures
$colFilters := New collection
$f := New object
$f.filter := "negate"
$colFilters.push($f)

$objOptions2 := New object
$objOptions2.format := ".png"
$objOptions2.filters := $colFilters
$objResult2 := Imebra Apply filters ($tImage ; $objOptions2)

// check .images.Length before use — failures are empty, not errors
```

### Ownership tests

`tests/ownership/run.py` compiles the complete bundled SDK, the wrapper classes and the exported dispatcher with ASan/UBSan against a memory-only 4D engine and synthetic Imebra/gd stubs (no codecs, files or DICOM data), then runs every case in its own process:

```sh
python3 tests/ownership/run.py .
# reproduce the exact pre-fix failure set on an archive of a5feaa5
python3 tests/ownership/run.py --expect-fail tests/ownership/baseline-expected-failures.txt <baseline-tree>
```

Engine out-parameter pictures (commands 678/679) have no documented release contract and are reported as `DEFERRED`.

## Releases

Prebuilt universal binaries for macOS and Windows are attached to each [GitHub release](../../releases).

Build from source: open `Imebra.xcodeproj` (macOS) or `Imebra.sln` (Windows, requires the vendored libraries under `lib/`).

## License

[LICENSE](LICENSE)
