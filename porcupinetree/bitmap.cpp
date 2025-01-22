#include <pch.h>

#include "bitmap.hpp"

#define BMP_SIG 0x4d42
#define BMP_HEADER_SZ 40

int write_bmp_header(Bitmap* bmp, ByteStream* stream) {
    std::cout << "Bmp Header Write" << std::endl;
    std::cout << "Sz: " << BMP_HEADER_SZ << std::endl;
    std::cout << "Width: " << bmp->header.w << std::endl;
    std::cout << "Height: " << bmp->header.h << std::endl;
    stream->writeUInt32(BMP_HEADER_SZ); //write header size
    stream->writeUInt32(bmp->header.w);
    stream->writeUInt32(bmp->header.h);
    stream->writeUInt16(bmp->header.colorPlanes);
    stream->writeUInt16(bmp->header.bitsPerPixel);
    stream->writeUInt32(bmp->header.compressionMode);
    stream->writeUInt32(0);
    stream->writeInt32(bmp->header.hResolution);
    stream->writeInt32(bmp->header.vResolution);
    stream->writeUInt32(bmp->header.nPalleteColors);
    stream->writeUInt32(bmp->header.importantColors);
    return 0;
}

i32 BitmapParse::WriteToFile(std::string src, Bitmap* bmp) {
    if (
        src.length() <= 0 ||
        !bmp ||
        !bmp->data
        ) return 1;

    std::cout << "Writing bmp stream" << std::endl;

    ByteStream datStream;

    datStream.mode = bmode_LittleEndian;

    write_bmp_header(bmp, &datStream);

    size_t datPos = datStream.getSize();

    //write bitmap data
    datStream.writeBytes(bmp->data, bmp->header.fSz);

    //create file stream
    ByteStream oStream;

    oStream.mode = bmode_LittleEndian;

    oStream.writeUInt16(BMP_SIG); //sig
    oStream.writeUInt32(datStream.getSize()); //how many bytes in file
    oStream.writeUInt32(0); // reserved
    oStream.writeUInt32(oStream.getSize() + sizeof(u32) + datPos); //data offset
    oStream.writeBytes(datStream.getBytePtr(), datStream.getSize());

    datStream.free();

    FileWrite::writeToBin(src, oStream.getBytePtr(), oStream.getSize());

    oStream.free();

    return 0;
}

Bitmap Bitmap::CreateBitmap(size_t w, size_t h) {
    if (w < 0 || h < 0)
        return { 0 };

    const size_t allocSz = w * h;

    BitmapHeader header = {
        .fSz = allocSz,
        .bmpSig = BMP_SIG,
        .w = w, 
        .h = h
    };

    Bitmap res = {
        .header = header,
        .data = new byte[allocSz]
    };

    ZeroMem(res.data, allocSz);

    return res;
}

void Bitmap::Free(Bitmap* bmp)  {
    if (!bmp) return;

    if (bmp->data)
        delete[] bmp->data;

    bmp->data = nullptr;
    bmp->header = {};
}