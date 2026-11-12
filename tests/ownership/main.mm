// Ownership harness: complete SDK, wrappers and exported dispatcher against a
// memory-only 4D engine. Every case runs in its own process (see run.py).
#include "4DPluginAPI.h"
#include "PrivateTypes.h"
#include "EntryPoints.h"
#include "gd.h"
#import <objc/runtime.h>
#include <algorithm>
#include <cstring>
#include <functional>
#include <iostream>
#include <set>
#include <stdexcept>
#include <type_traits>
#include <vector>

extern void json_wconv(const wchar_t*, CUTF16String*);
extern void ob_set_s(PA_ObjectRef, const wchar_t*, const char*);
extern void ob_set_a(PA_ObjectRef, const wchar_t*, const wchar_t*);
extern void ob_set_p(PA_ObjectRef, const wchar_t*, PA_Picture);
extern void ob_set_c(PA_ObjectRef, const wchar_t*, PA_CollectionRef);
extern void ob_set_i(PA_ObjectRef, const wchar_t*, PA_long32);
extern void ob_set_b(PA_ObjectRef, const wchar_t*, bool);
extern bool ob_get_a(PA_ObjectRef, const wchar_t*, CUTF8String*);
extern bool ob_get_b(PA_ObjectRef, const wchar_t*);
extern double ob_get_n(PA_ObjectRef, const wchar_t*);
extern PA_CollectionRef ob_get_c(PA_ObjectRef, const wchar_t*);

extern "C" CFURLRef CFURLCreateWithFileSystemPath(CFAllocatorRef, CFStringRef,
                                                CFURLPathStyle, Boolean) {
    throw std::runtime_error("path boundary denied");
}
extern "C" CFStringRef CFURLCopyFileSystemPath(CFURLRef, CFURLPathStyle) {
    throw std::runtime_error("path boundary denied");
}
extern "C" void CGContextScaleCTM(CGContextRef, CGFloat, CGFloat) {
    throw std::runtime_error("graphics boundary denied");
}
extern "C" void CGContextTranslateCTM(CGContextRef, CGFloat, CGFloat) {
    throw std::runtime_error("graphics boundary denied");
}

namespace {
using Text = std::basic_string<PA_Unichar>;
struct Reference {
    int owners = 1;
    std::map<Text, PA_Variable> properties;
    std::vector<PA_Variable> elements;
};
std::map<PA_Unichar*, Text> strings;
std::map<PA_Handle, size_t> handles;
std::map<PA_Handle, int> locks;
std::map<PA_Picture, int> pictures;
std::set<Reference*> references;
int checks = 0, failures = 0, invalidInspections = 0;
bool failResize = false, failProperty = false, poisonActive = false;
int failElementAt = 0, elementGets = 0;
Text failKey, lastConversion;
PA_Variable poison = {};
char poisonCell[16];

void require(bool b) { if (!b) throw std::runtime_error("synthetic contract violation"); }
void check(bool b, const std::string& n) {
    ++checks;
    if (!b) { ++failures; std::cout << "FAIL " << n << std::endl; }
    else std::cout << "PASS " << n << std::endl;
}
const PA_Unichar* chars(const char16_t* p) { return reinterpret_cast<const PA_Unichar*>(p); }
Text key(const char16_t* p) { return Text(chars(p)); }
PA_Unistring make(const PA_Unichar* p) {
    Text t(p ? p : chars(u""));
    PA_Unistring s = {};
    s.fLength = t.size();
    s.fString = new PA_Unichar[t.size() + 1];
    std::copy(t.begin(), t.end(), s.fString);
    s.fString[t.size()] = 0;
    strings.emplace(s.fString, t);
    return s;
}
void dispose(PA_Unistring s) {
    if (s.fString) { require(strings.erase(s.fString) == 1); delete[] s.fString; }
}
Reference* ref(void* p) {
    auto r = static_cast<Reference*>(p);
    require(references.count(r));
    return r;
}
void* createRef() { auto p = new Reference; references.insert(p); return p; }
PA_Picture newPicture() { auto p = new int(17); pictures[p] = 1; return p; }
PA_Handle newHandle(size_t n) {
    auto h = new char*;
    *h = static_cast<char*>(calloc(1, std::max<size_t>(1, n)));
    handles[h] = n;
    return h;
}
// Failed engine outputs are unspecified; the harness fills them with a typed
// value that must be neither inspected, adopted nor cleared.
bool isPoison(const PA_Variable& v) {
    if (!poisonActive || v.fType != poison.fType) return false;
    switch (poison.fType) {
        case eVK_Unistring: return v.uValue.fString.fString == poison.uValue.fString.fString;
        case eVK_Real: return v.uValue.fReal == poison.uValue.fReal;
        case eVK_Boolean: return true;
        case eVK_Object: return v.uValue.fObject == poison.uValue.fObject;
        case eVK_Collection: return v.uValue.fCollection == poison.uValue.fCollection;
        default: return false;
    }
}
void arm(int kind) {
    poison = {};
    poison.fType = kind;
    if (kind == eVK_Unistring) {
        poison.uValue.fString.fString = reinterpret_cast<PA_Unichar*>(poisonCell);
        poison.uValue.fString.fLength = 3;
    }
    if (kind == eVK_Real) poison.uValue.fReal = 4242.5;
    if (kind == eVK_Boolean) poison.uValue.fBoolean = 1;
    if (kind == eVK_Object) poison.uValue.fObject = poisonCell;
    if (kind == eVK_Collection) poison.uValue.fCollection = poisonCell;
}
void clear(PA_Variable&);
void release(void* p) {
    if (!p) return;
    auto r = ref(p);
    if (--r->owners == 0) {
        for (auto& it : r->properties) clear(it.second);
        for (auto& v : r->elements) clear(v);
        references.erase(r);
        delete r;
    }
}
void pictureRelease(PA_Picture p) {
    if (!p) return;
    require(pictures.count(p));
    if (--pictures[p] == 0) { pictures.erase(p); delete static_cast<int*>(p); }
}
void handleDispose(PA_Handle h) {
    if (h) { require(handles.erase(h) == 1); locks.erase(h); free(*h); delete h; }
}
void clear(PA_Variable& v) {
    switch (PA_GetVariableKind(v)) {
        case eVK_Unistring: dispose(v.uValue.fString); break;
        case eVK_Object: release(v.uValue.fObject); break;
        case eVK_Collection: release(v.uValue.fCollection); break;
        case eVK_Picture: pictureRelease(v.uValue.fPicture); break;
        case eVK_ArrayUnicode: {
            auto h = v.uValue.fArray.fData;
            if (h) {
                require(handles.at(h) >= (v.uValue.fArray.fNbElements + 1) * sizeof(PA_Unistring));
                auto p = reinterpret_cast<PA_Unistring*>(*h);
                for (int i = 0; i <= v.uValue.fArray.fNbElements; ++i) dispose(p[i]);
                handleDispose(h);
            }
            break;
        }
        case eVK_ArrayReal: case eVK_ArrayBoolean: case eVK_ArrayInteger:
        case eVK_ArrayLongint: case eVK_ArrayTime: case eVK_ArrayDate:
            handleDispose(v.uValue.fArray.fData); break;
        case eVK_Blob: handleDispose(v.uValue.fBlob.fHandle); break;
        default: break;
    }
    v = {}; v.fType = eVK_Undefined;
}
PA_Variable copy(PA_Variable v) {
    switch (PA_GetVariableKind(v)) {
        case eVK_Unistring:
            require(!v.uValue.fString.fString || strings.count(v.uValue.fString.fString));
            v.uValue.fString = make(v.uValue.fString.fString); break;
        case eVK_Object: if (v.uValue.fObject) ++ref(v.uValue.fObject)->owners; break;
        case eVK_Collection: if (v.uValue.fCollection) ++ref(v.uValue.fCollection)->owners; break;
        case eVK_Picture: if (v.uValue.fPicture) ++pictures.at(v.uValue.fPicture); break;
        case eVK_Blob:
            if (auto h = v.uValue.fBlob.fHandle) {
                auto n = handles.at(h);
                auto d = newHandle(n);
                memcpy(*d, *h, n);
                v.uValue.fBlob.fHandle = d;
            }
            break;
        default: break;
    }
    return v;
}
void set(void* obj, const char16_t* k, PA_Variable v) {
    auto& dest = ref(obj)->properties[key(k)];
    if (dest.fType != 0 || dest.uValue.fString.fString) clear(dest);
    dest = copy(v);
}
void text(void* obj, const char16_t* k, const char16_t* t) {
    auto s = make(chars(t));
    PA_Variable v = {}; PA_SetStringVariable(&v, &s);
    set(obj, k, v); clear(v);
}
void number(void* obj, const char16_t* k, double d) {
    PA_Variable v = {}; PA_SetRealVariable(&v, d); set(obj, k, v);
}
// Consumes the caller's reference to c.
void collection(void* obj, const char16_t* k, void* c) {
    PA_Variable v = {}; PA_SetCollectionVariable(&v, c); set(obj, k, v); clear(v);
}
void pushReal(void* col, double d) {
    PA_Variable v = {}; PA_SetRealVariable(&v, d); ref(col)->elements.push_back(v);
}
void pushText(void* col, const char16_t* t) {
    auto s = make(chars(t));
    PA_Variable v = {}; PA_SetStringVariable(&v, &s);
    ref(col)->elements.push_back(copy(v)); clear(v);
}
// Consumes the caller's reference to p.
void pushRef(void* col, void* p, bool object) {
    PA_Variable v = {};
    if (object) PA_SetObjectVariable(&v, p); else PA_SetCollectionVariable(&v, p);
    ref(col)->elements.push_back(copy(v)); clear(v);
}
void reset() {
    for (auto& p : strings) delete[] p.first;
    strings.clear();
    for (auto p : pictures) delete static_cast<int*>(p.first);
    pictures.clear();
    for (auto p : references) delete p;
    references.clear();
    for (auto h : handles) { free(*h.first); delete h.first; }
    handles.clear(); locks.clear();
}
void balance(const std::string& name) {
    check(strings.empty() && pictures.empty() && references.empty() && handles.empty(),
          name + " final ownership balance");
    if (!strings.empty() || !pictures.empty() || !references.empty() || !handles.empty())
        std::cout << "  outstanding strings=" << strings.size() << " pictures=" << pictures.size()
                  << " references=" << references.size() << " handles=" << handles.size() << std::endl;
    reset();
}
PA_Variable get(void* obj, const char16_t* k) {
    return ref(obj)->properties.at(key(k)); // Test inspection is borrowed.
}
Text textOf(void* obj, const char16_t* k) {
    auto v = get(obj, k);
    require(v.fType == eVK_Unistring);
    return Text(v.uValue.fString.fString, v.uValue.fString.fLength);
}
void engine(short selector, EngineBlock* eb) {
    eb->fError = eER_NoErr;
    switch (selector) {
        case EX_CREATE_UNISTRING:
            eb->fUniString1 = make(reinterpret_cast<PA_Unichar*>(eb->fHandle)); break;
        case EX_DISPOSE_UNISTRING: dispose(eb->fUniString1); break;
        case EX_SET_UNISTRING: {
            auto s = make(reinterpret_cast<PA_Unichar*>(eb->fHandle));
            dispose(eb->fUniString1); eb->fUniString1 = s; break;
        }
        case EX_VARIABLE_TO_STRING: {
            auto v = reinterpret_cast<PA_Variable*>(eb->fHandle);
            if (isPoison(*v)) { ++invalidInspections; eb->fUniString1 = {}; break; }
            require(PA_GetVariableKind(*v) == eVK_Unistring);
            eb->fUniString1 = v->uValue.fString; break;
        }
        case EX_CLEAR_VARIABLE: {
            auto v = static_cast<PA_Variable*>(eb->fPtr1);
            if (isPoison(*v)) { ++invalidInspections; break; }
            clear(*v); break;
        }
        case EX_HANDLE_MANAGER: {
            auto h = eb->fHandle;
            switch (eb->fParam1) {
                case 1: eb->fHandle = newHandle(eb->fParam2); break;
                case 2: handleDispose(h); break;
                case 3: {
                    if (failResize) { eb->fError = -108; break; } // memFullErr
                    auto old = handles.at(h);
                    *h = static_cast<char*>(realloc(*h, std::max<sLONG_PTR>(1, eb->fParam2)));
                    if (static_cast<size_t>(eb->fParam2) > old)
                        memset(*h + old, 0, eb->fParam2 - old);
                    handles[h] = eb->fParam2; break;
                }
                case 4: eb->fParam2 = handles.at(h); break;
                case 5: require(handles.count(h)); ++locks[h]; eb->fParam3 = reinterpret_cast<sLONG_PTR>(*h); break;
                case 6: require(handles.count(h)); --locks[h]; break;
                default: throw std::runtime_error("unexpected handle operation");
            }
            break;
        }
        case EX_GET_INFORMATION: eb->fParam2 = 0x00001800; break;
        case EX_GET_OBJ_VALUE: {
            auto out = static_cast<PA_Variable*>(eb->fPtr2);
            bool element = eb->fShort == eVK_Collection && eb->fLongint >= 0;
            bool fail = element ? (failElementAt && ++elementGets == failElementAt)
                                : (eb->fShort != eVK_Collection && failProperty &&
                                   Text(eb->fUniString1.fString) == failKey);
            if (fail) { *out = poison; poisonActive = true; eb->fError = -1; break; }
            auto r = ref(eb->fPtr1);
            if (eb->fShort == eVK_Collection) {
                if (eb->fLongint == -1) PA_SetLongintVariable(out, r->elements.size());
                else *out = copy(r->elements.at(eb->fLongint));
            } else *out = copy(r->properties.at(Text(eb->fUniString1.fString)));
            break;
        }
        case EX_SET_OBJ_VALUE: {
            auto r = ref(eb->fPtr1);
            auto v = *static_cast<PA_Variable*>(eb->fPtr2);
            if (eb->fShort == eVK_Collection) {
                require(eb->fLongint >= 0);
                if (static_cast<size_t>(eb->fLongint) >= r->elements.size()) {
                    PA_Variable undefined = {}; undefined.fType = eVK_Undefined;
                    r->elements.resize(eb->fLongint + 1, undefined);
                }
                clear(r->elements[eb->fLongint]);
                r->elements[eb->fLongint] = copy(v);
            } else {
                Text k(eb->fUniString1.fString);
                if (r->properties.count(k)) clear(r->properties[k]);
                r->properties[k] = copy(v);
            }
            break;
        }
        case EX_CREATE_PICTURE:
            // Picture creation only records an opaque token for fixture bytes.
            if (eb->fParam1) require(eb->fParam1 == 4 || eb->fParam1 == 58);
            eb->fPicture = newPicture(); break;
        case EX_DISPOSE_PICTURE: pictureRelease(eb->fPicture); break;
        case EX_DUPLICATE_PICTURE:
            require(!eb->fPicture || pictures.count(eb->fPicture));
            if (eb->fPicture) {
                if (eb->fParam1 == 0) eb->fPicture = newPicture();
                else ++pictures.at(eb->fPicture);
            }
            break;
        case EX_GET_PICTURE_DATA:
            // The metadata allocator contract is undocumented: return a borrowed
            // static descriptor and make no claim about its real lifetime.
            if (eb->fParam1 != 1) { eb->fError = -1; break; }
            eb->fUniString1 = {};
            eb->fUniString1.fString = const_cast<PA_Unichar*>(chars(u"fixture"));
            eb->fUniString1.fLength = 7;
            if (eb->fHandle) {
                PA_SetHandleSize(eb->fHandle, 4);
                memcpy(*eb->fHandle, "TEST", 4);
            }
            break;
        case EX_CONVERT_CHARSET_TO_CHARSET: {
            require(eb->fParam2 == eVTC_UTF_8 || eb->fParam2 == eVTC_UTF_16);
            CFStringEncoding encoding = eb->fParam2 == eVTC_UTF_8 ?
                kCFStringEncodingUTF8 : kCFStringEncodingUTF16LE;
            auto s = CFStringCreateWithBytes(nullptr, static_cast<UInt8*>(eb->fPtr1),
                                           eb->fParam1, encoding, false);
            require(s != nullptr);
            CFIndex used = 0;
            encoding = eb->fParam4 == eVTC_UTF_8 ? kCFStringEncodingUTF8 : kCFStringEncodingUTF16LE;
            auto n = CFStringGetLength(s);
            require(CFStringGetBytes(s, CFRangeMake(0, n), encoding, 0, false,
                    static_cast<UInt8*>(eb->fPtr2), eb->fParam3, &used) == n);
            eb->fParam3 = used; // SDK contract is bytes, not UTF-16 units.
            CFRelease(s); break;
        }
        case EX_EXECUTE_COMMAND_BY_ID: {
            auto result = static_cast<PA_Variable*>(eb->fPtr2);
            auto args = static_cast<PA_Variable**>(eb->fPtr1);
            switch (eb->fParam1) {
                case 1471: PA_SetObjectVariable(result, createRef()); break;
                case 1472: PA_SetCollectionVariable(result, createRef()); break;
                case 1231:
                    PA_SetBooleanVariable(result, ref(args[0]->uValue.fObject)->properties.count(
                        Text(args[1]->uValue.fString.fString))); break;
                case 1225: {
                    auto r = ref(args[0]->uValue.fObject);
                    auto out = createRef();
                    for (auto p : r->properties) ref(out)->properties[p.first] = copy(p.second);
                    PA_SetObjectVariable(result, out); break;
                }
                case 1002:
                    require(args[0]->fType == eVK_Picture && pictures.count(args[0]->uValue.fPicture));
                    lastConversion = args[1]->uValue.fString.fString; break;
                case 988:
                    require(args[0]->fType == eVK_Picture && pictures.count(args[0]->uValue.fPicture));
                    break; // Synchronous in-memory transform keeps the fixture picture.
                case 457:
                    require(args[0]->fType == eVK_Picture && pictures.count(args[0]->uValue.fPicture));
                    PA_SetLongintVariable(args[1], 1); PA_SetLongintVariable(args[2], 1); break;
                case 678: case 679:
                    // Engine out-parameter: the release contract for the prior
                    // args[1] owner is unknown, so the stub neither releases it nor
                    // claims to; affected cases report it as DEFERRED.
                    args[1]->fType = eVK_Picture;
                    args[1]->uValue.fPicture = newPicture();
                    break;
                default: throw std::runtime_error("unexpected synthetic command " + std::to_string(eb->fParam1));
            }
            break;
        }
        case EX_YIELD_ABSOLUTE: break;
        default: throw std::runtime_error("unexpected engine selector " + std::to_string(selector));
    }
}
void denyAppleBoundary(id, SEL, ...) { throw std::runtime_error("native Apple boundary denied"); }
void deny(Class cls, const char* selector, bool classMethod = false) {
    auto m = classMethod ? class_getClassMethod(cls, sel_registerName(selector)) :
                          class_getInstanceMethod(cls, sel_registerName(selector));
    require(m != nullptr);
    method_setImplementation(m, reinterpret_cast<IMP>(denyAppleBoundary));
}

std::vector<std::pair<std::string, std::function<void()>>> cases;
void add(const std::string& name, std::function<void()> f) { cases.emplace_back(name, f); }

// ---- scalar property getters and setters
struct Kind { const char* name; int kind; };
const Kind kinds[] = {{"text", eVK_Unistring}, {"object", eVK_Object},
    {"collection", eVK_Collection}, {"picture", eVK_Picture}, {"blob", eVK_Blob},
    {"real", eVK_Real}, {"boolean", eVK_Boolean}};
PA_Variable sample(int kind) {
    PA_Variable v = {};
    if (kind == eVK_Unistring) { auto s = make(chars(u"sample")); PA_SetStringVariable(&v, &s); }
    if (kind == eVK_Object) PA_SetObjectVariable(&v, createRef());
    if (kind == eVK_Collection) PA_SetCollectionVariable(&v, createRef());
    if (kind == eVK_Picture) PA_SetPictureVariable(&v, newPicture());
    if (kind == eVK_Blob) { auto h = newHandle(4); memcpy(*h, "BLOB", 4); PA_SetBlobHandleVariable(&v, h); }
    if (kind == eVK_Real) PA_SetRealVariable(&v, 2.5);
    if (kind == eVK_Boolean) PA_SetBooleanVariable(&v, true);
    return v;
}
void registerGetters() {
    const std::pair<const char*, const char16_t*> texts[] = {{"ascii", u"ASCII"},
        {"bmp", u"\u03a9"}, {"supplementary", u"\U0001f600"}, {"empty", u""}};
    for (auto t : texts) {
        std::string name = std::string("getter-text-") + t.first;
        auto fixture = t.second;
        add(name, [=] {
            auto obj = createRef();
            text(obj, u"value", fixture);
            auto n = strings.size();
            CUTF8String out;
            check(ob_get_a(obj, L"value", &out), name + " property exists");
            {
                C_TEXT expected; expected.setUTF16String(chars(fixture), key(fixture).size());
                CUTF8String utf8; expected.copyUTF8String(&utf8);
                check(out == utf8, name + " exact UTF-8 content");
            }
            check(strings.size() == n, name + " getter releases owned result");
            release(obj); balance(name);
        });
    }
    for (auto k : kinds) {
        for (char g : {'a', 'b', 'n', 'c'}) {
            std::string name = std::string("getter-") + k.name + "-" + g;
            int kind = k.kind;
            add(name, [=] {
                auto obj = createRef();
                auto v = sample(kind); set(obj, u"value", v); clear(v);
                if (g == 'a') {
                    CUTF8String out(reinterpret_cast<const uint8_t*>("untouched"));
                    ob_get_a(obj, L"value", &out);
                    check(out == CUTF8String(reinterpret_cast<const uint8_t*>(
                        kind == eVK_Unistring ? "sample" : "untouched")), name + " value");
                }
                if (g == 'b') check(ob_get_b(obj, L"value") == (kind == eVK_Boolean), name + " value");
                if (g == 'n') check(ob_get_n(obj, L"value") == (kind == eVK_Real ? 2.5 : 0), name + " value");
                if (g == 'c') {
                    auto c = ob_get_c(obj, L"value");
                    check((c != nullptr) == (kind == eVK_Collection), name + " handoff only on match");
                    if (c) PA_DisposeCollection(c); // Caller releases the handed-off reference.
                }
                release(obj); balance(name);
            });
        }
    }
    const std::pair<char, int> failed[] = {{'a', eVK_Unistring}, {'b', eVK_Boolean},
        {'n', eVK_Real}, {'c', eVK_Collection}};
    for (auto f : failed) {
        std::string name = std::string("getter-failed-") + f.first;
        char g = f.first; int kind = f.second;
        add(name, [=] {
            auto obj = createRef();
            text(obj, u"value", u"fixture");
            arm(kind); failProperty = true; failKey = key(u"value");
            bool untouched = true;
            if (g == 'a') {
                CUTF8String out(reinterpret_cast<const uint8_t*>("untouched"));
                ob_get_a(obj, L"value", &out);
                untouched = out == CUTF8String(reinterpret_cast<const uint8_t*>("untouched"));
            }
            if (g == 'b') untouched = !ob_get_b(obj, L"value");
            if (g == 'n') untouched = ob_get_n(obj, L"value") == 0;
            if (g == 'c') untouched = ob_get_c(obj, L"value") == nullptr; // Never touch a poisoned ref.
            failProperty = false; poisonActive = false;
            check(untouched, name + " failed output not adopted");
            check(invalidInspections == 0, name + " failed output not inspected or cleared");
            release(obj); balance(name);
        });
    }
}
void registerSetters() {
    const std::pair<const char*, const wchar_t*> wide[] = {{"ascii", L"ASCII"},
        {"supplementary", L"\u03a9\U0001f600"}, {"empty", L""}};
    for (auto w : wide) {
        std::string name = std::string("setter-a-") + w.first;
        auto value = w.second;
        add(name, [=] {
            auto obj = createRef();
            ob_set_a(obj, L"value", value);
            CUTF16String expected; json_wconv(value, &expected);
            check(textOf(obj, u"value") == Text(expected.c_str()), name + " exact UTF-16 content");
            release(obj); balance(name);
        });
    }
    add("setter-s-utf8", [] {
        auto obj = createRef();
        ob_set_s(obj, L"value", "A\xce\xa9\xf0\x9f\x98\x80");
        check(textOf(obj, u"value") == key(u"A\u03a9\U0001f600"), "setter-s-utf8 exact UTF-16 content");
        release(obj); balance("setter-s-utf8");
    });
    add("setter-p", [] {
        auto obj = createRef();
        auto p = newPicture();
        ob_set_p(obj, L"value", p); // Consumes the caller's reference.
        check(get(obj, u"value").uValue.fPicture == p && pictures.at(p) == 1, "setter-p stored sole owner");
        release(obj); balance("setter-p");
    });
    add("setter-c", [] {
        auto obj = createRef();
        auto c = createRef();
        ob_set_c(obj, L"value", c); // Consumes the caller's reference.
        check(ref(c)->owners == 1, "setter-c stored sole owner");
        release(obj); balance("setter-c");
    });
    add("setter-i-b", [] {
        auto obj = createRef();
        ob_set_i(obj, L"i", 7); ob_set_b(obj, L"b", true);
        check(get(obj, u"i").uValue.fLongint == 7 && get(obj, u"b").uValue.fBoolean, "setter-i-b values");
        release(obj); balance("setter-i-b");
    });
    const std::pair<const char*, const char16_t*> texts[] = {{"ascii", u"ASCII"},
        {"bmp", u"\u03a9"}, {"supplementary", u"\U0001f600"}, {"empty", u""}};
    for (auto t : texts) {
        std::string name = std::string("scalar-text-roundtrip-") + t.first;
        auto s = t.second;
        add(name, [=] {
            {
                C_TEXT value; value.setUTF16String(chars(s), key(s).size());
                CUTF8String utf8; value.copyUTF8String(&utf8);
                C_TEXT roundtrip; roundtrip.setUTF8String(&utf8);
                check(roundtrip.getUTF16Length() == key(s).size() &&
                      Text(roundtrip.getUTF16StringPtr()) == key(s), name + " unit length and termination");
            }
            balance(name);
        });
    }
}

// ---- array wrappers
struct ArraySpec {
    const char* name; int kind;
    std::function<void(PA_Variable&, int)> toParam; // fills n values then writes param 1
};
template<class W, class F> std::function<void(PA_Variable&, int)> filler(F append) {
    return [=](PA_Variable& arr, int n) {
        W w;
        for (int i = 0; i < n; ++i) append(w, i);
        void* params[] = {&arr};
        w.toParamAtIndex(reinterpret_cast<PackagePtr>(params), 1);
    };
}
std::vector<ArraySpec> arraySpecs() {
    return {
        {"text", eVK_ArrayUnicode, filler<ARRAY_TEXT>([](ARRAY_TEXT& w, int i) {
            const char16_t* v[] = {u"ASCII", u"\u03a9", u"\U0001f600", u""};
            w.appendUTF16String(chars(v[i % 4]));
        })},
        {"boolean", eVK_ArrayBoolean, filler<ARRAY_BOOLEAN>([](ARRAY_BOOLEAN& w, int i) { w.appendBooleanValue(i & 1); })},
        {"date", eVK_ArrayDate, filler<ARRAY_DATE>([](ARRAY_DATE& w, int i) { w.appendYearMonthDay(2000, 1, 1 + i); })},
        {"integer", eVK_ArrayInteger, filler<ARRAY_INTEGER>([](ARRAY_INTEGER& w, int i) { w.appendIntValue(short(i)); })},
        {"longint", eVK_ArrayLongint, filler<ARRAY_LONGINT>([](ARRAY_LONGINT& w, int i) { w.appendIntValue(i); })},
        {"real", eVK_ArrayReal, filler<ARRAY_REAL>([](ARRAY_REAL& w, int i) { w.appendDoubleValue(i); })},
        {"time", eVK_ArrayTime, filler<ARRAY_TIME>([](ARRAY_TIME& w, int i) { w.appendTimeValue(i); })},
    };
}
void registerArrays() {
    add("array-text-transfer-reuse", [] {
        const char16_t* values[] = {u"zero", u"ASCII", u"\u03a9", u"\U0001f600", u""};
        PA_Variable arr = PA_CreateVariable(eVK_ArrayUnicode);
        void* params[] = {&arr};
        {
            ARRAY_TEXT a;
            for (auto s : values) a.appendUTF16String(chars(s));
            for (int pass = 0; pass < 2; ++pass) {
                a.toParamAtIndex(reinterpret_cast<PackagePtr>(params), 1);
                check(PA_GetArrayNbElements(arr) == 4, "array count includes index zero");
                int i = 0;
                for (auto s : values) {
                    auto borrowed = PA_GetStringInArray(arr, i++);
                    check(Text(borrowed.fString, borrowed.fLength) == key(s), "array transferred content");
                }
            }
            ARRAY_TEXT b; b.fromParamAtIndex(reinterpret_cast<PackagePtr>(params), 1);
            CUTF16String out; b.copyUTF16StringAtIndex(&out, 3);
            check(out == key(u"\U0001f600"), "array borrowed getter copied independently");
        }
        PA_ClearVariable(&arr); balance("array-text-transfer-reuse");
    });
    const std::pair<const char*, int> rejects[] = {{"negative", -1}, {"past-end", 1},
        {"wrong-kind", 0}, {"null-data", 0}};
    for (auto r : rejects) {
        std::string name = std::string("array-reject-") + r.first;
        int index = r.second; std::string what = r.first;
        add(name, [=] {
            PA_Variable dest = PA_CreateVariable(what == "wrong-kind" ? eVK_ArrayLongint : eVK_ArrayUnicode);
            PA_Variable saved = dest;
            if (what == "null-data") dest.uValue.fArray.fData = nullptr;
            auto s = PA_CreateUnistring(const_cast<PA_Unichar*>(chars(u"owned")));
            PA_SetStringInArray(dest, index, &s);
            check(strings.count(s.fString) == 1, name + " caller retains rejected string");
            PA_DisposeUnistring(&s); PA_ClearVariable(&saved);
            balance(name);
        });
    }
    add("array-text-null-data-wrapper", [] {
        PA_Variable arr = {}; arr.fType = eVK_ArrayUnicode; // valid shape the SDK guards
        void* params[] = {&arr};
        {
            ARRAY_TEXT a; a.appendUTF16String(chars(u"zero")); a.appendUTF16String(chars(u"one"));
            a.toParamAtIndex(reinterpret_cast<PackagePtr>(params), 1);
        }
        check(arr.uValue.fArray.fData == nullptr && arr.uValue.fArray.fNbElements == 0,
              "array-text-null-data-wrapper descriptor unchanged");
        balance("array-text-null-data-wrapper");
    });
    for (auto spec : arraySpecs()) {
        std::string transfer = std::string("array-transfer-") + spec.name;
        add(transfer, [=] {
            auto arr = PA_CreateVariable(static_cast<PA_VariableKind>(spec.kind));
            spec.toParam(arr, 10);
            check(arr.fType == spec.kind && PA_GetArrayNbElements(arr) == 9, transfer + " count");
            PA_ClearVariable(&arr); balance(transfer);
        });
        std::string existing = std::string("resize-fail-existing-") + spec.name;
        add(existing, [=] {
            auto arr = PA_CreateVariable(static_cast<PA_VariableKind>(spec.kind));
            auto before = arr;
            auto size = handles.at(before.uValue.fArray.fData);
            failResize = true; spec.toParam(arr, 10); failResize = false;
            check(arr.fType == spec.kind && arr.uValue.fArray.fNbElements == 0 &&
                  arr.uValue.fArray.fData == before.uValue.fArray.fData &&
                  handles.at(before.uValue.fArray.fData) == size,
                  existing + " caller descriptor intact");
            PA_ClearVariable(&arr); balance(existing);
        });
        std::string fresh = std::string("resize-fail-fresh-") + spec.name;
        add(fresh, [=] {
            PA_Variable arr = {}; arr.fType = eVK_Undefined;
            failResize = true; spec.toParam(arr, 10); failResize = false;
            check(arr.fType == eVK_Undefined && arr.uValue.fArray.fData == nullptr,
                  fresh + " caller left undefined");
            balance(fresh);
        });
    }
}

// ---- picture wrapper
void deferredPictures(const std::string& name, size_t expected) {
    check(strings.empty() && references.empty() && handles.empty(), name + " non-picture balance");
    check(pictures.size() == expected, name + " only deferred engine out-param pictures remain");
    std::cout << "DEFERRED " << name << ": " << pictures.size()
              << " picture(s) from engine out-parameter with unknown release contract" << std::endl;
    reset();
}
void registerPictures() {
    add("picture-copy-utf16", [] {
        {
            CPicture p;
            auto r = p.createCopy(".png");
            check(lastConversion == key(u".png"), "picture-copy-utf16 exact UTF-16 units");
            check(pictures.count(r) == 1, "picture-copy-utf16 result owned");
            PA_DisposePicture(r);
        }
        balance("picture-copy-utf16");
    });
    add("picture-set-bytes", [] {
        {
            CPicture p; p.setBytes(reinterpret_cast<const uint8_t*>("TEST"), 4);
            check(pictures.count(p.getPicture()) == 1, "picture-set-bytes retained");
        }
        balance("picture-set-bytes");
    });
    add("picture-grayscale", [] {
        {
            CPicture p; auto r = p.createGrayScale();
            check(pictures.count(r) == 1, "picture-grayscale result owned");
            PA_DisposePicture(r);
        }
        balance("picture-grayscale");
    });
    add("picture-get-size", [] {
        {
            CPicture p; unsigned w = 0, h = 0; p.getSize(&w, &h);
            check(w == 1 && h == 1, "picture-get-size values");
        }
        balance("picture-get-size");
    });
    add("picture-thumbnail", [] {
        {
            CPicture p; auto r = p.createThumbnail(1, 1);
            check(pictures.count(r) == 1, "picture-thumbnail result owned");
            PA_DisposePicture(r);
        }
        deferredPictures("picture-thumbnail", 1);
    });
    add("picture-path", [] {
        {
            CPicture p; CUTF8String path(reinterpret_cast<const uint8_t*>("synthetic-not-a-file"));
            p.fromFileAtPath(&path);
            check(pictures.count(p.getPicture()) == 1, "picture-path result retained");
        }
        deferredPictures("picture-path", 2);
    });
}

// ---- raw-owning wrapper copies
template<class T> void copyProbe(const std::string& name, std::true_type) {
    T first;
    { T second(first); }
    check(true, name + " runtime copy survived"); // ASan reports the shared owner.
}
template<class T> void copyProbe(const std::string& name, std::false_type) {
    check(true, name + " copy rejected at compile time");
}
template<class T> void assignProbe(const std::string& name, std::true_type) {
    T first, second;
    second = first;
    check(true, name + " runtime assignment survived");
}
template<class T> void assignProbe(const std::string& name, std::false_type) {
    check(true, name + " assignment rejected at compile time");
}
template<class T> void copyCase(const std::string& name) {
    add(name, [=] {
        copyProbe<T>(name, std::integral_constant<bool, std::is_copy_constructible<T>::value>());
        assignProbe<T>(name, std::integral_constant<bool, std::is_copy_assignable<T>::value>());
        balance(name);
    });
}
void registerCopies() {
    copyCase<ARRAY_TEXT>("copy-array-text");
    copyCase<ARRAY_BOOLEAN>("copy-array-boolean");
    copyCase<ARRAY_DATE>("copy-array-date");
    copyCase<ARRAY_INTEGER>("copy-array-integer");
    copyCase<ARRAY_LONGINT>("copy-array-longint");
    copyCase<ARRAY_REAL>("copy-array-real");
    copyCase<ARRAY_TIME>("copy-array-time");
    copyCase<C_TEXT>("copy-c-text");
    copyCase<C_BLOB>("copy-c-blob");
    copyCase<CPicture>("copy-cpicture");
    copyCase<C_PICTURE>("copy-c-picture");
}

// ---- exported dispatcher
PA_ObjectRef run(int command, const char* bytes, void* options) {
    auto blob = newHandle(4); memcpy(*blob, bytes, 4);
    PA_ObjectRef result = nullptr;
    void* args[] = {&blob, &options};
    FourDPackex(command, args, nullptr, &result);
    check(locks[blob] == 0, "blob lock balanced");
    handleDispose(blob);
    return result;
}
void* options(const char16_t* format, void* filters) {
    auto o = createRef();
    text(o, u"format", format);
    number(o, u"count", 1);
    PA_Variable tags = {}; PA_SetBooleanVariable(&tags, true); set(o, u"tags", tags);
    if (filters) collection(o, u"filters", filters);
    return o;
}
void* oneFilter(void* filter) { auto c = createRef(); pushRef(c, filter, true); return c; }
void* named(const char16_t* name) { auto f = createRef(); text(f, u"filter", name); return f; }
size_t count(void* result, const char16_t* k) { return ref(get(result, k).uValue.fCollection)->elements.size(); }
void registerDispatcher() {
    const std::pair<const char*, const char16_t*> formats[] = {{"bmp", u".bmp"}, {"png", u".png"},
        {"jpeg", u".jpeg"}, {"gif", u".gif"}, {"tiff", u".tiff"}, {"wbmp", u".wbmp"}, {"webp", u".webp"}};
    for (auto f : formats) {
        auto format = f.second;
        std::string getName = std::string("dispatch-get-images-") + f.first;
        add(getName, [=] {
            auto o = options(format, nullptr);
            auto result = run(1, "TEST", o);
            check(result != nullptr, getName + " returns object");
            if (result) {
                check(count(result, u"images") == 1, getName + " image shape");
                auto image = ref(get(result, u"images").uValue.fCollection)->elements.at(0).uValue.fObject;
                check(textOf(image, u"format") == key(format), getName + " format preserved");
                check(pictures.count(get(image, u"image").uValue.fPicture) == 1, getName + " picture live");
                auto tag = ref(get(result, u"tags").uValue.fCollection)->elements.at(0).uValue.fObject;
                check(textOf(tag, u"value") == key(u"synthetic \u03a9\U0001f600"), getName + " tag UTF-16");
                release(result);
            }
            release(o); balance(getName);
        });
        std::string filterName = std::string("dispatch-apply-filters-") + f.first;
        add(filterName, [=] {
            auto o = options(format, oneFilter(named(u"negate")));
            auto result = run(2, "TEST", o);
            check(result != nullptr, filterName + " returns object");
            if (result) {
                check(count(result, u"images") == 2 && count(result, u"filters") == 1, filterName + " shape");
                auto image = ref(get(result, u"images").uValue.fCollection)->elements.at(1).uValue.fObject;
                check(textOf(image, u"format") == key(format), filterName + " format preserved");
                release(result);
            }
            release(o); balance(filterName);
        });
    }
    add("dispatch-get-images-wrong-format", [] {
        auto o = options(u".bmp", nullptr);
        auto result = run(1, "XXXX", o);
        check(result == nullptr, "dispatch-get-images-wrong-format no result (preserved)");
        if (result) release(result);
        release(o); balance("dispatch-get-images-wrong-format");
    });
    add("dispatch-apply-filters-null-decode", [] {
        auto o = options(u".bmp", oneFilter(named(u"negate")));
        auto result = run(2, "FAIL", o);
        check(result && count(result, u"images") == 0 && count(result, u"filters") == 0,
              "dispatch-apply-filters-null-decode empty result");
        if (result) release(result);
        release(o); balance("dispatch-apply-filters-null-decode");
    });
    add("dispatch-apply-filters-missing", [] {
        auto o = options(u".bmp", nullptr);
        auto result = run(2, "TEST", o);
        check(result && count(result, u"images") == 0 && count(result, u"filters") == 0,
              "dispatch-apply-filters-missing empty result");
        if (result) release(result);
        release(o); balance("dispatch-apply-filters-missing");
    });
    auto scatter = [] {
        auto f = named(u"scatterColor");
        number(f, u"sub", 1); number(f, u"plus", 2);
        auto colors = createRef();
        pushReal(colors, 1); pushReal(colors, 2); pushText(colors, u"x");
        collection(f, u"colors", colors);
        return f;
    };
    auto convolution = [] {
        auto f = named(u"convolution");
        number(f, u"div", 1); number(f, u"offset", 0);
        auto m = createRef();
        for (int y = 0; y < 3; ++y) {
            auto row = createRef();
            for (int x = 0; x < 3; ++x) pushReal(row, 1 + y * 3 + x);
            pushRef(m, row, false);
        }
        collection(f, u"matrix", m);
        return f;
    };
    struct Nested { const char* name; std::function<void*()> make; int failAt; int poisonKind;
                    std::vector<float> expected; size_t applied; };
    const std::vector<Nested> nested = {
        {"scatter-color", scatter, 0, 0, {1, 2}, 1},
        {"convolution", convolution, 0, 0, {1, 2, 3, 4, 5, 6, 7, 8, 9}, 1},
        {"failed-filter-element", scatter, 1, eVK_Object, {}, 0},
        {"failed-scatter-color-element", scatter, 2, eVK_Real, {2}, 1},
        {"failed-convolution-row", convolution, 2, eVK_Collection, {0, 0, 0, 4, 5, 6, 7, 8, 9}, 1},
        {"failed-convolution-cell", convolution, 3, eVK_Real, {0, 2, 3, 4, 5, 6, 7, 8, 9}, 1},
    };
    for (auto n : nested) {
        std::string name = std::string("dispatch-apply-filters-") + n.name;
        add(name, [=] {
            auto o = options(u".bmp", oneFilter(n.make()));
            syntheticScatterColors().clear(); syntheticConvolution().clear();
            if (n.failAt) { arm(n.poisonKind); failElementAt = n.failAt; elementGets = 0; }
            auto result = run(2, "TEST", o);
            failElementAt = 0; poisonActive = false;
            check(result != nullptr, name + " returns object");
            if (result) {
                check(count(result, u"images") == 2 && count(result, u"filters") == n.applied, name + " shape");
                release(result);
            }
            if (n.applied) {
                std::vector<float> seen;
                for (int c : syntheticScatterColors()) seen.push_back(c);
                for (float c : syntheticConvolution()) seen.push_back(c);
                check(seen == n.expected, name + " values crossing codec boundary");
            }
            check(invalidInspections == 0, name + " failed output not inspected or cleared");
            release(o); balance(name);
        });
    }
}
}

int main(int argc, char** argv) {
    @autoreleasepool {
        gCall4D = engine;
        deny([NSURL class], "initWithString:");
        deny([NSImage class], "initWithCGImage:size:");
        deny([NSImage class], "TIFFRepresentation");
        registerGetters(); registerSetters(); registerArrays();
        registerPictures(); registerCopies(); registerDispatcher();
        if (argc != 2) { std::cerr << "usage: ownership --list | CASE\n"; return 2; }
        std::string wanted(argv[1]);
        if (wanted == "--list") {
            for (auto& c : cases) std::cout << c.first << '\n';
            return 0;
        }
        for (auto& c : cases) {
            if (c.first != wanted) continue;
            try { c.second(); }
            catch (const std::exception& e) {
                check(false, wanted + " uncaught exception: " + e.what());
            }
            std::cout << "checks=" << checks << " failures=" << failures << std::endl;
            return failures ? 1 : 0;
        }
        std::cerr << "unknown case " << wanted << '\n';
        return 2;
    }
}
