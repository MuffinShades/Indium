#include <pch.h>

#include "FilePath.hpp"

std::vector<std::string> SplitPath(std::string path) {
    std::vector<std::string> sPath = SplitString(path, '/');

    if (sPath.size() <= 1)
        sPath = SplitString(path, '\\');

    return sPath;
}

std::string FilePath_int::getFileName(std::string path) {
    std::vector<std::string> sPath = SplitPath(path);

    if (sPath.size() <= 0)
        return "";

    //now actually get name
    std::string pName = sPath[sPath.size() - 1];

    std::vector<std::string> pParse = SplitString(pName, '.');

    if (pParse.size() <= 1)
        return pParse[0];

    //else get name
    std::string name = "";
    for (size_t i = 0; i < pParse.size() - 1; i++)
        name += pParse[i] + ".";

    if (name.length() > 1)
        return name.substr(0, name.length() - 1);
    else
        return "";
}

std::string FilePath_int::getFileType(std::string path) {
    std::vector<std::string> sPath = SplitPath(path);

    if (sPath.size() <= 0)
        return "";

    //now actually get name
    std::string pName = sPath[sPath.size() - 1];

    std::vector<std::string> pParse = SplitString(pName, '.');

    if (pParse.size() <= 1)
        return "";

    return pParse[pParse.size() - 1];
}

FileInfo FilePath_int::getFileInfo(std::string path) {
    if (path.length() <= 0)
        return {};

    struct stat r;

    FileInfo res;

    if (stat(path.c_str(), &r) == 0) {

        //get creation times and stuff
        auto mDate = r.st_mtime,
            cDate = r.st_ctime;

        //convert times into dates
        res = {
            .creationDate = Date(cDate),
            .modifiedDate = Date(mDate)
        };
    }

    res.name = FilePath_int::getFileName(path);
    res.type = FilePath_int::getFileType(path);

    return res;
}