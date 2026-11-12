#pragma once
#include <cstdint>
#include <cstring>
#include <list>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

// No medical data is read or parsed: load() accepts only the four-byte TEST
// token and otherwise throws, standing in for the documented
// CodecWrongFormatError of CodecFactory::load().
namespace imebra {
struct ReadMemory {
    const char* p; int n;
    ReadMemory(const char* p, int n) : p(p), n(n) {}
};
struct MemoryStreamInput { ReadMemory& m; explicit MemoryStreamInput(ReadMemory& m) : m(m) {} };
struct StreamReader { MemoryStreamInput& i; explicit StreamReader(MemoryStreamInput& i) : i(i) {} };
enum class tagId_t { VOILUTSequence_0028_3010 };
struct TagId {
    explicit TagId(tagId_t = tagId_t::VOILUTSequence_0028_3010) {}
    std::uint16_t getTagId() const { return 1; }
    std::uint16_t getGroupId() const { return 2; }
    std::uint32_t getGroupOrder() const { return 0; }
};
using tagsIds_t = std::vector<TagId>;
enum class tagVR_t { text = ('L' << 8) | 'O' };
struct Tag { tagVR_t getDataType() const { return tagVR_t::text; } };
struct ReadingDataHandlerNumeric {};
struct Image {
    std::string getColorSpace() const { return "GRAY"; }
    std::uint32_t getWidth() const { return 1; }
    std::uint32_t getHeight() const { return 1; }
    ReadingDataHandlerNumeric* getReadingDataHandler() { return new ReadingDataHandlerNumeric; }
};
struct LUT {};
struct VOI { double center = 1, width = 2; };
using vois_t = std::vector<VOI>;
struct DataSet {
    tagsIds_t getTags() { return {TagId()}; }
    Tag* getTag(TagId) { static Tag t; return &t; }
    bool bufferExists(TagId, std::size_t n) { return n == 0; }
    std::wstring getUnicodeString(TagId, std::size_t, const wchar_t*) { return L"synthetic \u03a9\U0001f600"; }
    Image* getImageApplyModalityTransform(std::size_t n) {
        if (n) throw std::out_of_range("fixture has one image");
        static Image i;
        return &i;
    }
    vois_t getVOIs() { return {}; }
    LUT* getLUT(TagId, std::size_t) { throw std::out_of_range("no fixture LUT"); }
};
struct CodecFactory {
    static DataSet* load(StreamReader& r) {
        if (r.i.m.n != 4 || std::memcmp(r.i.m.p, "TEST", 4))
            throw std::runtime_error("synthetic CodecWrongFormatError");
        return new DataSet;
    }
};
struct Transform {};
struct ColorTransformsFactory {
    static std::string normalizeColorSpace(std::string s) { return s; }
    static bool isMonochrome(std::string) { return true; }
    static Transform* getTransform(std::string, std::string) { static Transform t; return &t; }
};
struct VOILUT : Transform {
    void setCenterWidth(double, double) {}
    void setLUT(LUT&) {}
    void applyOptimalVOI(Image&, int, int, unsigned, unsigned) {}
};
struct TransformsChain { void addTransform(Transform&) {} };
enum class drawBitmapType_t { drawBitmapRGBA };
struct DrawBitmap {
    explicit DrawBitmap(TransformsChain&) {}
    std::size_t getBitmap(Image&, drawBitmapType_t, int, char* p, std::size_t n) {
        if (p) {
            if (n != 4) throw std::runtime_error("unexpected synthetic bitmap capacity");
            std::memset(p, 0, 4);
        }
        return 4;
    }
};
}
