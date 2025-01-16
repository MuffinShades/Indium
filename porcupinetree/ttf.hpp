#pragma once
#include <iostream>
#include "FileWriter.h"
#include "ByteStream.hpp"

#ifdef __cplusplus
extern "C" {
#endif

#define MSFL_COMPILE_DLL
#define MSFL_EXPORTS

#ifdef MSFL_COMPILE_DLL
#ifdef MSFL_EXPORTS
#define MSFL_EXP __declspec(dllexport)
#else
#define MSFL_EXP __declspec(dllimport)
#endif
#else
#define MSFL_EXP
#endif

    //some flagg stuff for getting a glyph
    enum PointFlag {
        PointFlag_onCurve,
        PointFlag_xSz,
        PointFlag_ySz,
        PointFlag_repeat,
        PointFlag_xMode,
        PointFlag_yMode
    };

#define GetFlagValue(f, p) ((f) & (1 << (p))) //macro this function cause it's so small
    static inline i32 ModifyFlagValue(i32 f, i32 p, i32 v) {
        return (f & (0xff ^ (1 << p))) | ((v & 1) << p); //weird ass bit manipulation thingy
    }

    typedef long long ttfLongDateTime;

    enum iTable {
        iTable_glyph,
        iTable_head,
        iTable_loca,
        iTable_cmap,
        iTable_hmtx,
        iTable_NA
    };

    //ttf file stuff starts here
    struct offsetTable {
        u32 checkSum;
        size_t off;
        size_t len;
        std::string tag;
        enum iTable iTag = iTable_NA;
    };

    /**
     *
     * All the structs for a ttf file
     *
     * yeah it's a lot
     *
     */
     //head
    struct table_head {
        float fVersion;
        float fontRevision;
        u32 checkSumAdjust;
        u32 magic;
        u16 flags;
        u16 unitsPerEm;
        ttfLongDateTime created;
        ttfLongDateTime modified;
        float xMin, yMin, xMax, yMax;
        u16 macStyle;
        u16 lowestRecPPEM;
        i16 fontDirectionHint;
        i16 idxToLocFormat;
        i16 glyphDataFormat;
    };

    class ttfStream : public ByteStream {
    public:
        MSFL_EXP short readFWord();
        MSFL_EXP unsigned short readUFWord();
        MSFL_EXP float readF2Dot14();
        MSFL_EXP float readFixed();
        MSFL_EXP std::string readString(size_t sz);
        MSFL_EXP ttfLongDateTime readDate();
        using ByteStream::ByteStream;
    };

    struct Glyph {
        i16 nContours;
        float xMin, yMin, xMax, yMax;
        Point* points = nullptr;
        byte* flags = nullptr;
        i32* contourEnds = nullptr;
        size_t nPoints;
    };

    enum CMapMode {
        CMap_Unicode,
        CMap_Mac,
        CMap_Reserved,
        CMap_Microsoft,
        CMap_null
    };

    class ttfFile {
    public:
        u32 scalarType;
        u16 searchRange;
        u16 entrySelector;
        u16 rangeShift;
        offsetTable head_table;
        offsetTable loca_table;
        offsetTable glyph_table;
        offsetTable cmap_table;
        CMapMode charMapMode = CMap_null;
        i32 charMapModeId = 0;
        std::vector<offsetTable> tables;
        table_head header;
    };

    class ttfParse {
    public:
        MSFL_EXP static ttfFile ParseTTFFile(std::string src);
        MSFL_EXP static ttfFile ParseBin(byte* dat, size_t sz);
        MSFL_EXP static Glyph ReadTTFGlyph(std::string src, u32 id);
    };

#ifdef __cplusplus
}
#endif