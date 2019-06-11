# 4d-plugin-imebra-v3

library version: ``4.5.4.1``

#### New

- thread safe
- native object and collection support
- faster data load (return direct ``.bmp`` image by default)

### Platform

| carbon | cocoa | win32 | win64 |
|:------:|:-----:|:---------:|:---------:|
|<img src="https://cloud.githubusercontent.com/assets/1725068/22371562/1b091f0a-e4db-11e6-8458-8653954a7cce.png" width="24" height="24" /> |<img src="https://cloud.githubusercontent.com/assets/1725068/22371562/1b091f0a-e4db-11e6-8458-8653954a7cce.png" width="24" height="24" /> |<img src="https://cloud.githubusercontent.com/assets/1725068/22371562/1b091f0a-e4db-11e6-8458-8653954a7cce.png" width="24" height="24" /> |<img src="https://cloud.githubusercontent.com/assets/1725068/22371562/1b091f0a-e4db-11e6-8458-8653954a7cce.png" width="24" height="24" /> 

### Version

<img src="https://user-images.githubusercontent.com/1725068/41266195-ddf767b2-6e30-11e8-9d6b-2adf6a9f57a5.png" width="32" height="32" />

### Syntax

```
images:=Imebra Get images (data{;options})
```

Parameter|Type|Description
------------|------|----
data|BLOB|DICOM file data
options|OBJECT|see below
images|OBJECT|see below

#### Options

Property|Type|Description
------------|------|----
format|TEXT|``.png`` ``.jpg`` ``.jpeg`` ``.gif`` ``.wbmp`` ``.webp`` ``.tif`` ``.tiff`` default:``.bmp``
quality|LONGINT|``.bmp``: ``0`` (default) or ``1`` RLE compression<br />``.jpg``: ``0`` (default) to ``95`` quality<br />``.png``: ``-1`` (default) ``0`` (none) ``1`` to ``9`` zip compression<br />``.webp``: ``-1`` (default) ``0`` to ``100`` quality
fg|LONGINT|foreground index
tags|BOOLEAN|``true`` to read DICOM tags
count|LONGINT|max number of images to read, ``-1`` read all images

#### Images

Property|Type|Description
------------|------|----
images|COLLECTION|``.png`` ``.jpeg`` ``.gif`` ``.wbmp`` ``.webp`` ``.tiff`` ``.bmp``
images\[\].colorspace|TEXT|
images\[\].format|TEXT|
images\[\].height|REAL|pixels
images\[\].width|REAL|pixels
images\[\].size|REAL|bytes
images\[\].image|PICTURE|

``fg`` and ``quality`` are optional

```
images:=Imebra Apply filters (data{;options})
```

Parameter|Type|Description
------------|------|----
data|BLOB|image data
options|OBJECT|see below
images|OBJECT|see below

#### Options

Property|Type|Description
------------|------|----
format|TEXT|``.png`` ``.jpg`` ``.jpeg`` ``.gif`` ``.wbmp`` ``.webp`` ``.tif`` ``.tiff`` default:``.bmp``
filter|``edgeDetectQuick``<br/>``emboss``<br/>``meanRemoval``<br/>``grayScale``<br/>``negate``<br/>``smooth``<br/>``brightness``<br/>``contrast``<br/>``scatter``<br/>``pixelate``<br/>``gaussianBlur``<br/>``color``<br/>``scatterColor``<br/>``convolution``
