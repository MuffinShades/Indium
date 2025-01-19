#pragma once
#include <iostream>
#include <vector>
#include <string>

#define JTOK_VMODE_RAW 0x00
#define JTYPE_STRING 0
#define JTYPE_INT 1
#define JTYPE_FLOAT 2

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

enum JType {
	JType_String,
	JType_Int,
	JType_Float,
	JType_Null
};

class JToken {
public:
	std::string label;
	std::string rawValue;
	JType ty = JType_Null;
	struct JStruct* body = nullptr;
	MSFL_EXP JToken() {};
	MSFL_EXP JToken(std::string label, std::string val, JType ty = JType_String) : label(label), rawValue(val), ty(ty) {
	}
	MSFL_EXP void free();
};

#define JERR_INVALID_TOK 0xfab9

#define _JMODE_OBJ 0
#define _JMODE_ARR 1

struct JValue {
	std::string rawValue;
	JType type = JType_String;
	MSFL_EXP JValue(JToken jt);
	MSFL_EXP JValue() {};
};

class JStruct {
public:
	std::vector<struct JToken> body;
	int mode = 0;
	MSFL_EXP JToken FindToken(std::string label);
	MSFL_EXP JStruct() {};
	MSFL_EXP void free();
	MSFL_EXP JValue operator[](std::string label);
	MSFL_EXP JValue operator[](int idx);
	void print(std::string tab = "");
};

/*
Main parsing class

*/

class jparse {
public:
	MSFL_EXP static JStruct parseStr(const char* jData, bool _clean = true);
	MSFL_EXP static std::string GenerateString(JStruct json, bool min = false, std::string tab = "");
private:
	MSFL_EXP static void genTok(const char* rStr, JToken& tok);
};

#ifdef MSFL_DLL
#ifdef __cplusplus
}
#endif
#endif