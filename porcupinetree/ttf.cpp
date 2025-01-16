#include "ttf.hpp"

const u32 ttf_magic = 0x5f0f3cf5;

const std::string iTableTags[] = {
    "glyf",
    "head",
    "loca",
    "cmap",
    "hmtx"
};

//old
/*bool _strCompare(std::string a, std::string b) {
    const char *aCstr = a.c_str(), *bCstr = b.c_str();

    const size_t len = a.length();

    if (len != b.length())
        return false;

    for (size_t i = 0; i < len; i++) {
        if (aCstr[i] != bCstr[i])
            return false;
    }

    return true;
}*/

enum iTable getITableEnum(offsetTable tbl) {
    i32 p = 0;
    for (const std::string itm : iTableTags) {
        if (_strCompare(itm, tbl.tag))
            return (enum iTable)p;
        p++;
    }

    return iTable_NA;
}

//reference: https://developer.apple.com/fonts/TrueType-Reference-Manual/

//ttfSream stuff
short ttfStream::readFWord() {
    return this->readInt16();
}

unsigned short ttfStream::readUFWord() {
    return this->readUInt16();
}

float ttfStream::readF2Dot14() {
    return (float)(this->readInt16()) / (float)(1 << 14);
}

float ttfStream::readFixed() {
    return (float)(this->readInt32()) / (float)(1 << 16);
}

ttfLongDateTime ttfStream::readDate() {
    return (ttfLongDateTime)this->readUInt64();
}

std::string ttfStream::readString(size_t sz) {
    char* b = new char[sz], * cur = b;

    for (size_t i = 0; i < sz; i++)
        *cur++ = this->readByte();

    std::string res = std::string((const char*)b);
    delete[] b;
    return res;
}

offsetTable readOffsetTable(ttfStream* stream) {
    offsetTable res;

    char tagBytes[] = {
        (char)stream->readByte(),
        (char)stream->readByte(),
        (char)stream->readByte(),
        (char)stream->readByte()
    };

    res.tag = std::string(tagBytes);
    res.checkSum = stream->readUInt32();
    res.off = stream->readUInt32();
    res.len = stream->readUInt32();

    return res;
};

//loca
//dosent need a struct but heres how it works for reference:
/*
if head.idxToLocFormat == 0 (short) ->
    u16 -> offset to char n
elif header.idxToLocFormat == 1 (long) ->
    u32 -> offset to char n
*/

/**
 *
 * read_header_table
 *
 * just reads and parses a header table from a stream
 *
 */
table_head read_header_table(ttfStream* stream) {
    table_head res;

    res.fVersion = stream->readFixed();
    res.fontRevision = stream->readFixed();
    res.checkSumAdjust = stream->readUInt32();
    res.magic = stream->readUInt32();
    res.flags = stream->readUInt16();
    res.unitsPerEm = stream->readUInt16();
    res.created = stream->readDate();
    res.modified = stream->readDate();
    res.xMin = stream->readFWord();
    res.yMin = stream->readFWord();
    res.xMax = stream->readFWord();
    res.yMax = stream->readFWord();
    res.macStyle = stream->readUInt16();
    res.lowestRecPPEM = stream->readUInt16();
    res.fontDirectionHint = stream->readInt16();
    res.idxToLocFormat = stream->readInt16();
    res.glyphDataFormat = stream->readInt16();

    return res;
}

/**
 *
 * read_offset_tables
 *
 * reads the offset to the tables in
 * the ttf and saves important tables
 * in the file's memory
 *
 */
void read_offset_tables(ttfStream* stream, ttfFile* f) {
    f->scalarType = stream->readUInt32();
    size_t nTables = stream->readUInt16();
    f->searchRange = stream->readUInt16();
    f->entrySelector = stream->readUInt16();
    f->rangeShift = stream->readUInt16();

    std::vector<offsetTable> res;

    //read the offset tables
    //TODO: checksum for le tables
    for (size_t i = 0; i < nTables; i++) {
        offsetTable table = readOffsetTable(stream);

        enum iTable tEnum = getITableEnum(table);
        table.iTag = tEnum;

        //if it's an important table then get it yk
        switch (tEnum) {
            //itable and read in header table thing ma bob
        case iTable_head: {
            f->head_table = table;
            size_t oPos = stream->seek(table.off);
            f->header = read_header_table(stream);
            stream->seek(oPos);
            break;
        }
        case iTable_glyph: {
            f->glyph_table = table;
            break;
        }
        case iTable_loca: {
            f->loca_table = table;
            break;
        }
        case iTable_cmap: {
            f->cmap_table = table;
            break;
        }
        default:
            break;
        }

        res.push_back(
            table
        );
    }

    f->tables = res;
}

/**
 *
 * getGlyphOffset
 *
 * function to take value from loca table
 * and get the position in the glyph table
 * of the target glyph (tChar)
 *
 */
u32 getGlyphOffset(ttfStream* stream, ttfFile* f, u32 tChar) {
    if (f->loca_table.iTag != iTable_loca)
        return 0;

    size_t rPos = stream->seek(f->loca_table.off);

    u32 offset = 0;

    switch (f->header.idxToLocFormat) {
        //short
    case 0: {
        size_t pOff = tChar * 2;
        stream->seek(stream->tell() + pOff);
        offset = stream->readUInt16();
        stream->seek(rPos);
        break;
    }
          //long / int
    case 1: {
        size_t pOff = tChar * 4;
        stream->seek(stream->tell() + pOff);
        offset = stream->readUInt32();
        stream->seek(rPos);
        break;
    }
    default:
        return 0;
    }

    return offset;
}

/**
 *
 * getUnicodeOffset
 *
 * function to take value from cmap table
 * and get the position in the loca table
 * of the target character (tChar). Then
 * gets location in glyph table based off
 * of loca table.
 *
 */
u32 getUnicodeOffset(ttfStream* stream, ttfFile* f, u32 tChar) {
    size_t mapOff = f->cmap_table.off;

    size_t rPos = stream->seek(mapOff);

    //read data from the cmap table
    u16 version = stream->readUInt16();
    u16 nSubTabls = stream->readUInt16();

    //mode
    if (f->charMapMode == CMap_null) {
        f->charMapMode = (enum CMapMode)stream->readInt16();
        f->charMapModeId = stream->readInt16();
    }
    else
        //since we've already read it, skip the map type and platform specific ID
        stream->skipBytes(sizeof(i16) * 2);

    return 0;
}

/**
 *
 * read_glyph
 *
 *
 * Reads a glyph from the ttf file.
 * Returns a glyph with the points and flags
 */
Glyph read_glyph(ttfStream* stream, ttfFile* f, u32 loc) {
    Glyph res;
    if (stream == nullptr || f == nullptr)
        return res;

    size_t rPos = stream->seek(f->glyph_table.off + loc);

    //read some glyph data
    res.nContours = stream->readInt16();
    res.xMin = stream->readFWord();
    res.yMin = stream->readFWord();
    res.xMax = stream->readFWord();
    res.yMax = stream->readFWord();

    if (res.nContours <= 0)
        return res;

    //for now we can only read simple glyphs
    i32* contourEnds = new i32[res.nContours];
    size_t nPoints = 0;

    for (size_t i = 0; i < res.nContours; i++) {
        contourEnds[i] = stream->readUInt16();

        if (contourEnds[i] > nPoints)
            nPoints = contourEnds[i];
    }

    nPoints++; //number of points we need to read

    //skip over byte instructions
    const size_t nInstructions = stream->readUInt16();
    stream->skipBytes(nInstructions);

    //flags
    byte* flags = new byte[nPoints];

    for (size_t i = 0; i < nPoints; i++) {
        byte flag = stream->readByte();
        flags[i] = flag;

        if ((bool)GetFlagValue(flag, PointFlag_repeat)) {
            size_t repeatAmount = stream->readByte();

            while (repeatAmount--)
                flags[++i] = flag;
        }
    }

    //now do point stuff
    Point* glyphPoints = new Point[nPoints];

    //x coordinates
    for (size_t i = 0; i < nPoints; i++) {
        const byte flag = flags[i];
        size_t xSz = ((size_t)!((bool)GetFlagValue(flag, PointFlag_xSz))) + 1;
        bool xMode = (bool)GetFlagValue(flag, PointFlag_xMode);
        i32 xSign = xSz == 1 ? xMode ? 1 : -1 : 1;

        if ((bool)(xSz - 1) && xMode) {
            if (i <= 0) {
                //std::cout << "Invalid glyph x-coordinate!" << std::endl;
                //return res;

                glyphPoints[i].x = 0;
                continue;
            }
            //std::cout << "Coord copy...\n" << std::endl;
            glyphPoints[i].x = glyphPoints[i - 1].x;
            continue;
        }
        else {
            switch (xSz) {
            case 1: {
                glyphPoints[i].x = stream->readByte() * xSign;
                //std::cout << "Posf: " << glyphPoints[i].x << "  8bit" << std::endl;
                break;
            }
            case 2: {
                glyphPoints[i].x = stream->readInt16();
                //std::cout << "Posf: " << glyphPoints[i].x << "  16bit"<< std::endl;
                break;
            }
            default:
                return res;
            }

            if (i > 0)
                glyphPoints[i].x += glyphPoints[i - 1].x;

            //std::cout << "X-Coord: " << glyphPoints[i].x << " Sz: " << xSz << std::endl;

            //std::cout << "\n" << std::endl;
        }
    }

    //y coordinates
    for (size_t i = 0; i < nPoints; i++) {
        const byte flag = flags[i];
        size_t ySz = ((size_t)!((bool)GetFlagValue(flag, PointFlag_ySz))) + 1;
        bool yMode = (bool)GetFlagValue(flag, PointFlag_yMode);
        i32 ySign = ySz == 1 ? yMode ? 1 : -1 : 1;

        if ((bool)(ySz - 1) && yMode) {
            if (i <= 0) {
                //std::cout << "Invalid glyph y-coordinate!" << std::endl;
                //return res;

                glyphPoints[i].y = 0;
                continue;
            }
            //std::cout << "Coord copy...\n" << std::endl;
            glyphPoints[i].y = glyphPoints[i - 1].y;
            continue;
        }
        else {
            switch (ySz) {
            case 1: {
                glyphPoints[i].y = stream->readByte() * ySign;
                break;
            }
            case 2: {
                glyphPoints[i].y = stream->readInt16();
                break;
            }
            default:
                return res;
            }

            if (i > 0)
                glyphPoints[i].y += glyphPoints[i - 1].y;

            //std::cout << "\n" << std::endl;
        }
    }

    //set res stuff
    res.points = glyphPoints;
    res.flags = flags;
    res.contourEnds = contourEnds;
    res.nPoints = nPoints;

    stream->seek(rPos);
    return res;
}

ttfFile create_err_file(int code) {
    ttfFile res;
    //TODO: add error stuff for le ttf file
    return res;
}

ttfFile ttfParse::ParseTTFFile(std::string src) {
    arr_container<byte> fBytes = FileWriter::ReadBytesFromBinFile(src);

    if (fBytes.dat == nullptr || fBytes.sz <= 0)
        return create_err_file(1);

    ttfFile r = ttfParse::ParseBin(fBytes.dat, fBytes.sz);
    delete[] fBytes.dat;

    return r;
}

ttfFile ttfParse::ParseBin(byte* dat, size_t sz) {

    //create file and le byte stream
    ttfFile f;
    ttfStream fStream = ttfStream(dat, sz);

    //read offset tables and other important info
    read_offset_tables(&fStream, &f);

    //test to read a location in le table
    u32 offset = getGlyphOffset(&fStream, &f, 36u);
    Glyph tGlyph = read_glyph(&fStream, &f, offset);

    //std::cout << "Glyph Pos: " << offset << std::endl;

    //print le test glyph data
    /*for (size_t i = 0; i < tGlyph.nPoints; i++) {
        std::cout << "Glyph Point: " << tGlyph.points[i].x << ", " << tGlyph.points[i].y << std::endl;
    }*/

    fStream.free();

    return f;
}


Glyph ttfParse::ReadTTFGlyph(std::string src, u32 id) {
    Glyph tGlyph;
    arr_container<byte> fBytes = FileWriter::ReadBytesFromBinFile(src);

    if (fBytes.dat == nullptr || fBytes.sz <= 0)
        return tGlyph;

    ttfFile f;
    ttfStream fStream = ttfStream(fBytes.dat, fBytes.sz);

    read_offset_tables(&fStream, &f);

    u32 offset = getGlyphOffset(&fStream, &f, id);
    tGlyph = read_glyph(&fStream, &f, offset);

    delete[] fBytes.dat;
    fStream.free();

    return tGlyph;
}