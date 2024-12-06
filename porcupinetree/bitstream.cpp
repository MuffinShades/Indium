#include "bitstream.hpp"

//returns current bit
bit BitStream::curBit() {

}

bit BitStream::readBit() {
    
}

byte BitStream::readByte() {
    //calc how much is left in current byte
    const size_t bitsLeft = 8 - this->subBit;
    byte bMask = (1 << bitsLeft) - 1;

    //shift mask if little endian
    if (this->mode == bmode_LittleEndian)
        bMask <<= this->subBit;

    //get other half
    //TODO: finish this
    //works by taking a split between 2 bytes were on and combining into byte instead of having to use slow for loop
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
            res |= (this->readByte() << (i * 8));
        break;
    }
    case bmode_LittleEndian:
    {
        for (int i = 0; i < nBytes; i++)
            res |= (this->readByte() << (i * 8));
        break;
    }
    }
    return res;
}