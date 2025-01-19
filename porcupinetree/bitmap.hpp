#pragma once
#include <iostream>
#include "ByteStream.hpp"
#include "filewrite.hpp"


#ifdef MSFL_DLL
#ifdef MSFL_EXPORTS
#define MSFL_EXP __declspec(dllexport)
#else
#define MSFL_EXP __declspec(dllimport)
#endif
#else
#define MSFL_EXP
#endif

#ifdef MSFL_DLL
#ifdef __cplusplus
extern "C" {
#endif
#endif

struct BitmapHeader {
    size_t fSz = 0;
    i16 bmpSig = 0;
    size_t w = 0, h = 0;
    i32 compressionMode = 0;
    u16 colorPlanes = 1;
    u16 bitsPerPixel = 32;
    i32 vResolution = 1, hResolution = 1;
    size_t nPalleteColors = 0;
    size_t importantColors = 0;
};

class Bitmap {
public:
    BitmapHeader header;
    byte* data = nullptr;
    MSFL_EXP static Bitmap CreateBitmap(size_t w, size_t h);
    MSFL_EXP static void Free(Bitmap* bmp);
};

class BitmapParse {
public:
    MSFL_EXP static i32 WriteToFile(std::string src, Bitmap* bmp);
};

#ifdef MSFL_DLL
#ifdef __cplusplus
}
#endif
#endif