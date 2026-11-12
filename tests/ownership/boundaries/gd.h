#pragma once
#include <cstdlib>
#include <cstring>
#include <vector>

// These are memory-only stand-ins, not libgd or image decoders.
struct gdImage { int marker = 17; };
using gdImagePtr = gdImage*;
// Accepts only the TEST token or the plugin's own generated 1x1 32-bit BMP
// (14 + 40 header bytes + 4 pixel bytes); FAIL models a NULL decode result.
inline gdImagePtr syntheticDecode(int size, void* bytes) {
    auto b = static_cast<const unsigned char*>(bytes);
    if (size == 4 && !std::memcmp(b, "FAIL", 4)) return nullptr;
    if (size == 4 && !std::memcmp(b, "TEST", 4)) return new gdImage;
    if (size == 58 && b[0] == 'B' && b[1] == 'M' && b[2] == 58 && b[10] == 54 &&
        b[14] == 40 && b[18] == 1 && b[22] == 1 && b[28] == 32) return new gdImage;
    std::abort();
}
#define DECODE(name) inline gdImagePtr name(int n, void* p) { return syntheticDecode(n, p); }
DECODE(gdImageCreateFromBmpPtr)
DECODE(gdImageCreateFromPngPtr)
DECODE(gdImageCreateFromJpegPtr)
DECODE(gdImageCreateFromGifPtr)
DECODE(gdImageCreateFromTiffPtr)
DECODE(gdImageCreateFromWBMPPtr)
DECODE(gdImageCreateFromWebpPtr)
#undef DECODE
inline void gdImageDestroy(gdImagePtr p) { delete p; }
inline void* syntheticEncode(gdImagePtr p, int* n) {
    if (!p || p->marker != 17) std::abort();
    *n = 4;
    void* bytes = std::malloc(4);
    std::memcpy(bytes, "TEST", 4);
    return bytes;
}
#define ENCODE2(name) inline void* name(gdImagePtr p, int* n) { return syntheticEncode(p, n); }
#define ENCODE3(name) inline void* name(gdImagePtr p, int* n, int) { return syntheticEncode(p, n); }
ENCODE2(gdImageGifPtr)
ENCODE2(gdImageTiffPtr)
ENCODE3(gdImageBmpPtr)
ENCODE3(gdImagePngPtrEx)
ENCODE3(gdImageJpegPtr)
ENCODE3(gdImageWBMPPtr)
ENCODE3(gdImageWebpPtrEx)
#undef ENCODE2
#undef ENCODE3
inline void gdFree(void* p) { std::free(p); }
#define FILTER(name) inline int name(gdImagePtr p) { return p && p->marker == 17; }
FILTER(gdImageSelectiveBlur)
FILTER(gdImageEdgeDetectQuick)
FILTER(gdImageEmboss)
FILTER(gdImageMeanRemoval)
FILTER(gdImageGrayScale)
FILTER(gdImageNegate)
#undef FILTER
inline int gdImageSmooth(gdImagePtr p, double) { return p != nullptr; }
inline int gdImageBrightness(gdImagePtr p, int) { return p != nullptr; }
inline int gdImageContrast(gdImagePtr p, double) { return p != nullptr; }
inline int gdImageScatter(gdImagePtr p, int, int) { return p != nullptr; }
inline int gdImagePixelate(gdImagePtr p, int, int) { return p != nullptr; }
inline int gdImageColor(gdImagePtr p, int, int, int, int) { return p != nullptr; }
// Records the values the plugin passes across the codec boundary.
inline std::vector<int>& syntheticScatterColors() { static std::vector<int> v; return v; }
inline std::vector<float>& syntheticConvolution() { static std::vector<float> v; return v; }
inline int gdImageScatterColor(gdImagePtr p, int, int, int* c, unsigned int n) {
    syntheticScatterColors().assign(c, c + n);
    return p != nullptr;
}
inline int gdImageConvolution(gdImagePtr p, float m[3][3], double, double) {
    syntheticConvolution().assign(&m[0][0], &m[0][0] + 9);
    return p != nullptr;
}
inline gdImagePtr gdImageCreateTrueColor(int, int) { return new gdImage; }
inline gdImagePtr gdImageCopyGaussianBlurred(gdImagePtr, int, double) { return new gdImage; }
inline void gdImageAlphaBlending(gdImagePtr, int) {}
inline void gdImageSaveAlpha(gdImagePtr, int) {}
inline int gdImageColorAllocateAlpha(gdImagePtr, int, int, int, int) { return 0; }
inline void gdImageFill(gdImagePtr, int, int, int) {}
inline void gdImageCopyRotated(gdImagePtr, gdImagePtr, double, double, int, int, int, int, int) {}
