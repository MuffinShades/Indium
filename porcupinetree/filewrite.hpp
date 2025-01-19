#pragma once
#include <iostream>
#include <fstream>
#include "msutil.hpp"

#ifdef MSFL_DLL
#ifdef MSFL_EXPORTS
#define MSFL_EXP __declspec(dllexport)
#else
#define MSFL_EXP __declspec(dllimport)
#endif
#else
#define MSFL_EXP
#endif

#ifdef MSFL_DLL
#ifdef __cplusplus
extern "C" {
#endif
#endif

struct file {
	size_t len;
	byte* dat;
};

class FileWrite {
public:
	MSFL_EXP static bool writeToBin(std::string src, byte* dat, size_t sz);
	MSFL_EXP static file readFromBin(std::string src);
};

#ifdef MSFL_DLL
#ifdef __cplusplus
}
#endif
#endif