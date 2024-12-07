#include "bitstream.hpp"

//returns current bit
bit BitStream::curBit() {
    return (this->curByte() & (1 << this->subBit)) >> this->subBit;
}

bit BitStream::readBit() {
    return (this->curByte() & (1 << this->subBit)) >> this->subBit++;
}

/**
 *
 * BitStream::readByte
 * 
 * reads and unaligned byte by basically
 * combining the next 8 bits.
 * 
 * Works by taking a split between 2 bytes 
 * were on and combining into byte instead 
 * of having to use slow for loop.
 * 
 * uses a big endian bit order
 *  -> TODO: make it work for both big and little endian
 *  -> add bit endian option
 */
byte BitStream::readByte() {
    //calc how much is left in current byte
    const size_t bitsLeft = 8 - this->subBit;
    byte bMask = (1 << bitsLeft) - 1;
    byte bLeft = this->readByteAligned() & bMask; //get left part

    const size_t rBits = 8 - bitsLeft; //number of bits on right

    //if we're not reading an aligned byte we need to split
    if (rBits > 0) {
        byte rMask = ((1 << rBits) - 1) << bitsLeft; //get right bit mask
        byte bRight = this->curByte() & rMask; //get right bits
        this->subBit += rBits;
        return bLeft | bRight; //combine
    }
    else
        return bLeft;
}

byte BitStream::readByteAligned() {
    this->subBit = 0;
    return this->_readByte();
}

u64 BitStream::readBytesAsVal(size_t nBytes) {
    if (nBytes <= 0)
        return 0;
    unsigned long long res = 0u;
    switch (this->mode)
    {
    case bmode_BigEndian:
    {
        for (int i = nBytes - 1; i >= 0; i--)
            res |= (this->_readByte() << (i * 8));
        break;
    }
    case bmode_LittleEndian:
    {
        for (int i = 0; i < nBytes; i++)
            res |= (this->_readByte() << (i * 8));
        break;
    }
    }
    return res;
}

//byte alignment functions
void BitStream::alignToNextByte() {
    const byte _ = this->readByteAligned();
}

void BitStream::alignToPrevByte() {
    this->subBit = 0;
}

/**
 *
 * BitStream::readNBits
 *
 * reads a number of bits from the
 * stream without going bit by bit
 * 
 * uses a big endian bit order
 *  -> TODO: make it work for both big and little endian
 * 
 * but use big endian bit order 99% of time since it's standard for binary
 * 
 */
u32 BitStream::readNBits(size_t nBits) {
    if (nBits <= 0) return 0;

    //first step is try to align to next byte
    const size_t fBitsLeft = 1 - this->subBit;

    size_t bitsLeft = nBits;

    const byte msk = (1 << nBits) - 1, d = fBitsLeft - nBits;
    u32 fChunk = (this->curByte() & (msk << d)) >> d; //first chunk of bits

    //if we are reading less bits that however many to next byte we just return next couple bits
    if (fBitsLeft >= nBits) {
        this->subBit += nBits;

        //incase we end on the final bit
        if (this->subBit >= 8)
            this->alignToNextByte();


        return fChunk;
    }

    //align to next byte
    bitsLeft -= fBitsLeft;
    this->alignToNextByte();

    u32 res = fChunk;

    //add remaining full bits
    while (bitsLeft > 8) {
        res <<= 8;
        res |= this->readByteAligned();
        bitsLeft -= 8;
    }

    //add last chunk
    const size_t lb = 8 - bitsLeft;
    u32 lChunk = (this->curByte() & (((1 << bitsLeft) - 1) << lb)) >> lb;
    this->subBit = bitsLeft;

    return (res << 8) | lChunk;
}