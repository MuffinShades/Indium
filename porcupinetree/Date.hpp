#pragma once
#include <iostream>
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

    class Date {
    private:
        MSFL_EXP void _i(std::string date = "", std::string time = "");
    public:
        i16 year = 0, ms = 0;
        byte day = 0, month = 0, hour = 0, minute = 0, second = 0;
        MSFL_EXP Date(std::string date, std::string time = "") {
            this->_i(date, time);
        }
        MSFL_EXP Date(u64 iVal);
        MSFL_EXP Date(time_t t);
        MSFL_EXP Date() {};
        MSFL_EXP u64 getLong();
        MSFL_EXP std::string getString();
    };

#ifdef MSFL_DLL
#ifdef __cplusplus
}
#endif
#endif