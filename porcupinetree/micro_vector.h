#pragma once
#include "msutil.hpp"
#include <iostream>

//use this instead of vec for memory efficiency
template<size_t i_sz> class micro_vector {
private:
    u64 mask = 0;
public:
    void* dat;
    size_t allocSz, chunkSz;
    size_t sz;

    /*

    Frees the vectors memory

    */
    void free() {
        if (this->dat) {
            delete[] this->dat;
            this->dat = nullptr;
        }
        this->sz = this->allocSz = 0;
    }

    /*

    Allocates n bytes and sets all values to NULL

    */
    void alloc(size_t sz, bool autoCompute = true) {
        if (autoCompute) sz *= sizeof(i_sz);
        if (this->dat != nullptr) this->free();
        this->dat = malloc(sz);
        this->sz = sz;
        this->allocSz = sz;
        ZeroMem(this->dat, this->sz);
    }


    micro_vector(size_t iniSz = 0, size_t chunkSz = 0) {
        if (iniSz > 0) this->alloc(iniSz);
        this->chunkSz = (chunkSz > 0 ? chunkSz : 0xff) * i_sz; //by default alloc 255 * sizeof(_T) bytes

        //set mask
        this->mask = (1 << (i_sz * 8)) - 1;
    }

    //
    void allocNewChunk() {
        this->allocSz += this->chunkSz;
        void* t = malloc(this->allocSz);
        ZeroMem<byte>((byte*)t, this->allocSz);

        if (this->dat) {
            memcpy(t, this->dat, this->allocSz);
            delete[] this->dat;
        }

        this->dat = t;
    }

    //
    void allocCheck() {
        if (this->sz >= this->allocSz)
            this->allocNewChunk();
    }

    //
    void push(u64 val) {
        val &= this->mask;
        this->allocCheck();
        //this->dat[this->sz++] = val;
        memcpy((byte*)this->dat + (this->sz++) * i_sz, &val, i_sz);
    }

    template<class _T> void copyToBuffer(_T* b, size_t bSz) {
        if (!b)
            return;
        memcpy((void*)b, (void*)this->dat, min(this->sz, bSz) * i_sz);
    }
};