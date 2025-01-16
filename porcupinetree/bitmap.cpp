#include "bitmap.hpp"

#define BMP_SIG 0x4d42
#define BMP_HEADER_SZ 40

int write_bmp_header(Bitmap* bmp, ByteStream* stream) {
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

    ByteStream datStream;

    datStream.mode = bmode_LittleEndian;

    write_bmp_header(bmp, &datStream);

    size_t datPos = datStream.getSize();

    //write bitmap data
    datStream.writeBytes(bmp->data, bmp->header.fSz);

    //create file stream
    ByteStream oStream;

    oStream.mode = bmode_LittleEndian;

    oStream.writeUInt16(BMP_SIG);
    oStream.writeUInt32(datStream.getSize());
    oStream.writeUInt16(0);
    oStream.writeUInt16(0);
    oStream.writeUInt32(oStream.getSize() + sizeof(u32) + datPos);
    oStream.writeBytes(datStream.getBytePtr(), datStream.getSize());

    datStream.free();

    FileWrite::writeToBin(src, oStream.getBytePtr(), oStream.getSize());

    oStream.free();

    return 0;
}