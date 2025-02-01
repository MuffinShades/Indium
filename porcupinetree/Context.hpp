/**
 * 
 * Context.hpp
 * 
 * basic context container for other contexts that will be used in a lot of places
 * 
 */

#pragma once
#include "bitstream.hpp"

class Context {
public:
	BitStream* stream = nullptr;
};