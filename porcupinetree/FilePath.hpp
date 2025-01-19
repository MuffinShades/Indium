#pragma once
#include <iostream>

/**
 *
 * General utility functions around file paths
 *
 */

#include <sys/types.h>
#include <sys/stat.h>
#ifndef WIN32
#include <unistd.h>
#endif

#ifdef WIN32
#define stat _stat
#endif

#include "Date.hpp"
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

struct FileInfo {
    Date creationDate, modifiedDate;
    std::string name, type;
};

class FilePath_int {
public:
    MSFL_EXP static std::string getFileType(std::string path);
    MSFL_EXP static std::string getFileName(std::string path);
    MSFL_EXP static FileInfo getFileInfo(std::string path);
};

#ifdef MSFL_DLL
#ifdef __cplusplus
}
#endif
#endif