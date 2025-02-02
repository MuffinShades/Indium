#include <pch.h>

#ifdef ________asdf

#include "ByteStream.hpp"

constexpr size_t __glb_sz_short = 2;
constexpr size_t __glb_sz_int = 4;
constexpr size_t __glb_sz_long = 8;

void ByteStream::allocNewChunk()
{
    this->allocBytes(this->chunkSz);
}

void ByteStream::allocBytes(size_t sz) 
{
    if (sz <= 0)
        return;
    this->allocSz += sz;
    byte* tmp = new byte[this->allocSz];
    ZeroMem(tmp, this->allocSz);
    if (this->len > 0 && this->bytes)
        memcpy(tmp, this->bytes, this->len);
    if (this->bytes)
        delete[] this->bytes;
    this->bytes = tmp;
}

ByteStream::ByteStream(byte* dat, size_t sz)
{
    if (dat == nullptr || sz <= 0)
        return;

    this->bytes = new byte[sz];
    this->len = sz;
    this->allocSz = this->len;
    memcpy(this->bytes, dat, this->len);
}

ByteStream::ByteStream()
{
    this->bytes = nullptr;
    this->len = 0;
    this->allocSz = 0;
};

byte ByteStream::readByte()
{
    if (this->readPos >= this->len) //Note to self, this has broken things ;-;
        return 0;

    return this->bytes[this->readPos++];
}

u64 ByteStream::readBytesAsVal(size_t nBytes)
{
    if (nBytes <= 0)
        return 0u;
    u64 res = 0u;
    switch (this->mode)
    {
    case bmode_BigEndian:
    {
        for (i32 i = nBytes - 1; i >= 0; i--)
            res |= (this->_readByte() << ((unsigned)i * 8u));
        break;
    }
    case bmode_LittleEndian:
    {
        for (size_t i = 0; i < nBytes; i++)
            res |= (this->_readByte() << (i * 8));
        break;
    }
    }
    return res;
}

byte* ByteStream::readBytes(size_t nBytes) {
    if (nBytes <= 0) return nullptr;

    byte* res = new byte[nBytes];
    ZeroMem(res, nBytes);

    const size_t byteRead = min(nBytes, this->len - this->readPos);
    memcpy(res, this->bytes, byteRead);
    this->readPos += byteRead;

    return res;
}

i16 ByteStream::readInt16()
{
    return (short)this->readBytesAsVal(__glb_sz_short);
}

u16 ByteStream::readUInt16()
{
    return (unsigned)this->readInt16();
}

i32 ByteStream::readInt32()
{
    return (int)this->readBytesAsVal(__glb_sz_int);
}

u32 ByteStream::readUInt32()
{
    return (unsigned)this->readInt32();
}

i64 ByteStream::readInt64()
{
    return (int64_t)this->readBytesAsVal(__glb_sz_long);
}

u64 ByteStream::readUInt64()
{
    return (uint64_t)this->readInt64();
}

void ByteStream::writeByte(byte b) {
    this->_writeByte(b);
}

void ByteStream::_writeByte(byte b) {
    if (++this->len > this->allocSz)
        this->allocNewChunk();
    this->bytes[this->writePos++] = b;
    //this->byteWriteAdv();
}

void ByteStream::writeNBytesAsVal(u64 v, size_t nBytes)
{
    if (nBytes <= 0)
        return;
    switch (this->mode)
    {
    case bmode_BigEndian:
    {
        for (int i = nBytes - 1; i >= 0; i--)
            this->_writeByte((v >> (i * 8u)) & 0xff);
        break;
    }
    case bmode_LittleEndian:
    {
        for (size_t i = 0; i < nBytes; i++)
            this->_writeByte((v >> (i * 8)) & 0xff);
        break;
    }
    }
}

void ByteStream::writeInt16(i16 v)
{
    this->writeNBytesAsVal(v, __glb_sz_short);
}

void ByteStream::writeUInt16(u16 v)
{
    this->writeNBytesAsVal(v, __glb_sz_short);
}

void ByteStream::writeInt32(i32 v)
{
    this->writeNBytesAsVal(v, __glb_sz_int);
}

void ByteStream::writeUInt32(u32 v)
{
    this->writeNBytesAsVal(v, __glb_sz_int);
}

void ByteStream::writeInt64(i64 v)
{
    this->writeNBytesAsVal(v, __glb_sz_long);
}

void ByteStream::writeUInt64(u64 v)
{
    this->writeNBytesAsVal(v, __glb_sz_long);
}

void ByteStream::clip()
{
    if (this->allocSz == this->len)
        return;
    byte* tmp = new byte[this->len];
    memcpy(tmp, this->bytes, this->len);
    this->allocSz = this->len;
    delete[] this->bytes;
    this->bytes = tmp;
}

void ByteStream::writeBytes(byte* dat, size_t sz)
{
    if (dat == nullptr || sz <= 0)
        return;
    const size_t padding = 0xff; // padding yk
    const size_t pos = this->len;
    if ((this->len + sz) > this->allocSz)
        this->allocBytes((this->len + sz + padding) - this->allocSz);

    // memcpy
    memcpy(this->bytes + pos, dat, sz);
    this->len = pos + sz;

    if (this->writePos == this->readPos)
        this->readPos = this->len - 1;

    this->writePos = this->len - 1;
}

ByteStream::~ByteStream()
{

}

size_t ByteStream::seek(size_t pos) {
    size_t old = this->readPos;
    this->readPos = pos;
    return old;
}

size_t ByteStream::skipBytes(size_t nBytes) {
    const size_t oPos = this->readPos;
    this->readPos += nBytes;
    this->readPos = min(this->readPos, this->len - 1);
    return oPos;
}

char* ByteStream::readCStr(size_t len) {
    if (len <= 0)
        return nullptr;

    char* _by = new char[len], * cur = _by, * e = _by + len;
    ZeroMem(_by, len);

    do {
        *cur++ = (char)this->_readByte();
    } while (cur != e);

    return _by;
}

std::string ByteStream::readStr(size_t len) {
    char* cStr = this->readCStr(len);

    std::string res;

    if (cStr == nullptr)
        res = "";
    else
        res = std::string(
            const_cast<const char*>(cStr)
        );

    delete[] cStr;

    //length check / correction
    if (res.length() > len)
        res = res.substr(0, len);

    return res;
}

byte ByteStream::curByte() {
    return this->bytes[this->tell()];
}

byte ByteStream::_readByte()
{
    if (this->readPos >= this->len) //Note to self, this has broken things ;-;
        return 0;

    return this->bytes[this->readPos++];
}

//advances another byte when writing stuff
void ByteStream::byteWriteAdv() {
    if (this->readPos == this->writePos)
        this->readPos = this->writePos;

    if (this->writePos >= this->len)
        if (++this->len >= this->allocSz)
            this->allocNewChunk();
}

void ByteStream::catchUp() {
    this->readPos = this->writePos;
}

//welp

#endif