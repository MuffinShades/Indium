#pragma once
#include <iostream>
#include "msutil.hpp"

template<typename _Ty> struct hash_node {
    hash_node* prev = nullptr;
    bool del = false; //
    size_t sz;
    char* key;
    u64 hash;
    _Ty val;
};

//recommended 15 bits
template<class _storeType, size_t hBits> class linked_map {
private:

    const size_t hashBits = hBits; //number of bits per hash
    size_t hashSz;

    hash_node<_storeType>** roots = nullptr;

    /**
     * linked_map::allocHash
     *
     * allocates hash data for le hash
     * woah :O
     *
     */
    void allocHash() {
        this->free();
        this->hashSz = 1 << this->hashBits;
        this->roots = new hash_node<_storeType> *[this->hashSz];
        ZeroMem(this->roots, this->hashSz);
    }

    /**
     *
     *
     *
     */
    u64 computeHash(char* dat, const size_t len) {
        const u64 mask = (1 << this->hashBits) - 1;

        u64 hash = 0;

        for (size_t i = 0; i < len; i++) {
            hash += dat[i] ^ (i ^ dat[0] + 0xf6) * 7;
            hash ^= ~mask;
        }

        return (hash % mask) & mask;
    }

    /**
     *
     *
     *
     */
    hash_node<_storeType>* _insert(hash_node<_storeType>* n) {
        n->prev = this->roots[n->hash];
        this->roots[n->hash] = n;
        return n->prev;
    }
public:

    /**
     *
     *
     *
     */
    void free() {
        this->clear();
        if (this->roots != nullptr) {
            delete[] this->roots;
            this->roots = nullptr;
        }
    }

    /**
     *
     *
     *
     */
    hash_node<_storeType>* insert(char* key, const size_t key_sz, const _storeType dat) {
        u64 hsh = this->computeHash(key, key_sz);
        return this->_insert(new hash_node<_storeType>{
            .sz = key_sz,
            .key = key,
            .hash = hsh,
            .val = dat
            });
    }

    /**
     *
     *
     *
     */
    hash_node<_storeType>* insert(std::string key, const _storeType dat) {
        return this->insert(
            const_cast<char*>(key.c_str()),
            key.length(),
            dat
        );
    }

    /**
     *
     *
     *
     */
    template<class _KeyTy> hash_node<_storeType>* insert(_KeyTy key, const _storeType dat) {
        const size_t k_sz = sizeof(_KeyTy);
        void* k_mem = (void*)&key;

        return this->insert(
            reinterpret_cast<char*>(k_mem),
            k_sz,
            dat
        );
    }

    /**
     *
     *
     *
     */
    void clear() {
        for (size_t i = 0; i < this->hashSz; i++) {
            hash_node<_storeType>* cur = this->roots[i];
            while (cur) {
                //std::cout << "Deleting:" << cur << std::endl;
                hash_node<_storeType>* prev = cur->prev;
                delete cur;
                cur = prev;
            }

            this->roots[i] = nullptr;
        }
    }

    /**
     *
     * uhh alloc ye ye
     *
     */
    linked_map() {
        this->allocHash();
    }
};