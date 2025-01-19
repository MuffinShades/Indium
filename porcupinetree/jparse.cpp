#include <pch.h>


/**
 *
 * Simple .json file parser
 *
 * Programmed by muffinshades
 *
 * Copyright muffinshades 2024 all rights reserved
 *
 */
#include "jparse.hpp"
#include <iostream>
#include <vector>
#include <string>

void JStruct::print(std::string tab) {
	if (!this) return;
	switch (this->mode) {
	case _JMODE_OBJ: {
		std::cout << tab << "{" << std::endl;
		tab = tab + "\t";
		//std::cout << "sz: " << this->body.size() << std::endl;
		for (int i = 0; i < this->body.size(); i++) {
			if (this->body[i].body == nullptr) {
				std::cout << tab + this->body[i].label << ": " << this->body[i].rawValue << "," << std::endl;
			}
			else {
				//std::cout << tab + this->body[i].label << "   " << this->body[i].body << "   " << this << "   " << this->body.size() << "   "  << i;
				std::cout << tab + this->body[i].label << ": ";
				this->body[i].body->print(tab);
				//std::cout << tab + this->body[i].label << "   " << this->body[i].body << "   " << this->body[i].body->body[0].label << "  " << this << "   " << this->body.size() << "   " << i;
			}
		}
		std::cout << std::endl << tab.substr(0, tab.length() - 1) << "}" << std::endl;
		break;
	}
	case _JMODE_ARR: {
		std::cout << tab << "[" << std::endl;
		tab = tab + "\t";
		//std::cout << "sz: " << this->body.size() << std::endl;
		for (int i = 0; i < this->body.size(); i++) {
			if (this->body[i].body == nullptr) {
				std::cout << tab + this->body[i].rawValue << "," << std::endl;
			}
			else {
				//std::cout << tab + this->body[i].label << "   " << this->body[i].body << "   " << this << "   " << this->body.size() << "   "  << i;
				this->body[i].body->print(tab);
				//std::cout << tab + this->body[i].label << "   " << this->body[i].body << "   " << this->body[i].body->body[0].label << "  " << this << "   " << this->body.size() << "   " << i;
			}
		}
		std::cout << std::endl << tab.substr(0, tab.length() - 1) << "]" << std::endl;
		break;
	}
	}
}

JValue JStruct::operator[](int idx) {
	if (this->mode == _JMODE_ARR) {
		if (idx > 0 && idx < this->body.size())
			return JValue(this->body[idx]);
		else
			return JValue();
	}
	else {
		return JValue();
	}
}

JValue JStruct::operator[](std::string label) {
	return JValue(this->FindToken(label));
}

void JStruct::free() {
	for (int i = 0; i < this->body.size(); i++) {
		if (this->body[i].body != nullptr) {
			this->body[i].body->free();
		}
		this->body[i].free();
	}
}

JToken JStruct::FindToken(std::string label) {
	for (const JToken tok : body) {
		if (tok.label == label) return tok;
	}

	return JToken();
}

/*template<typename _Ty> _Ty JValue::value() {
	switch ((int)this->type) {
	case JTYPE_STRING: {
		return this->rawValue;
		break;
	}
	case JTYPE_FLOAT: {
		try {
			return std::stof(this->rawValue);
		}
		catch (std::exception e) {
			return 0.0f;
		}
		break;
	}
	case JTYPE_INT: {
		try {
			return std::stoi(this->rawValue);
		}
		catch (std::exception e) {
			return NULL;
		}
		break;
	}
	}

	return NULL;
}*/

JValue::JValue(JToken jt) {
	const char* vRaw = jt.rawValue.c_str();

	auto _gType = [](std::string str) {
		std::string nChars = "0123456789.";
		JType _mode = JType_Int;
		for (const char _C : str) {
			if (nChars.find(_C) == std::string::npos)
				return JType_String;
			if (_C == '.')
				_mode = JType_Float;
		}

		return _mode;
		};

	if (jt.rawValue.length() > 0)
		this->type = _gType(jt.rawValue);
}

void JToken::free() {
	if (this->body != nullptr) delete[] this->body;
}

/*

Not Used so yeah

*/
void copyTokToVec(std::vector<JToken> _v, JToken tok) {
	_v.push_back(tok);
	JToken* vtk = &_v[_v.size() - 1];
	vtk->body = tok.body;
	vtk->label = tok.label;
	vtk->rawValue = tok.rawValue;
}

std::string fetchStr(const char* str, int str_len, int idx, int len) {
	std::string r = "";
	for (int i = idx; i < idx + len; i++) {
		if (i > str_len) return r;
		r += str[i];
	}

	return r;
}

std::string back_clean(std::string str) { //handles backslah parsing
	if (str.length() <= 0) return str;

	const char* cStr = str.c_str();
	const size_t len = str.length();
	char* rStr = new char[len], * cur = rStr;
	size_t eLen = 0;

	for (size_t i = 0; i < len; i++) {
		switch (cStr[i]) {
		case '\\': {
			i++;

			if (i >= len)
				return str;

			switch (cStr[i]) {
			case 'n': {
				(*cur++) = '\n';
				//std::cout << "Got new line!" << std::endl;
				break;
			}
			case 'r': {
				(*cur++) = '\r';
				break;
			}
			case 't': {
				(*cur++) = '\t';
				break;
			}
			case 'a': {
				(*cur++) = '\a';
				break;
			}
			case 'b': {
				(*cur++) = '\b';
				break;
			}
			case 'f': {
				(*cur++) = '\f';
				break;
			}
			case 'v': {
				(*cur++) = '\v';
				break;
			}
			case 'u': {
				//TODO: add unicode
				std::cout << "JSON info: Sorry unicode escape isn't supported yet :/" << std::endl;
				break;
			}
			default: {
				(*cur++) = cStr[i];
				break;
			}
			}

			break;
		}
		default: {
			(*cur++) = cStr[i];
			break;
		}
		}

		eLen++;
	}

	//fix rstring
	char* tmp = rStr;
	rStr = new char[eLen];
	memcpy(rStr, tmp, eLen);
	//std::cout << "tStr: " << rStr << std::endl;
	std::string res = std::string((const char*)rStr).substr(0, eLen);
	delete[] rStr, tmp;
	return res;
}

std::string jp_raw_val(std::string val) {
	const char* _Vstr = val.c_str();

	if (_Vstr[0] == '"') {
		if (_Vstr[val.length() - 1] == '"') {
			return back_clean(val.substr(1, val.length() - 2));
		}
		else
			throw JERR_INVALID_TOK;
	}
	else
		return val;
}

void jparse::genTok(const char* rStr, JToken& tok) {
	if (rStr[0] == '{') {
		//parse as JStruct
		JStruct _r = jparse::parseStr(rStr);
		tok.body = new JStruct();
		tok.body->body = _r.body;
		tok.body->mode = _r.mode;
	}
	else if (rStr[0] == '[') {
		//parse as JStruct
		JStruct _r = jparse::parseStr(rStr, false);
		tok.body = new JStruct();
		tok.body->body = _r.body;
		tok.body->mode = _r.mode;
	}
	else
		tok.rawValue = jp_raw_val(rStr);
}

enum JType _gType(std::string str) {
	std::string nChars = "0123456789.";
	JType _mode = JType_Int;
	for (const char _C : str) {
		if (nChars.find(_C) == std::string::npos)
			return JType_String;
		if (_C == '.')
			_mode = JType_Float;
	}

	return _mode;
};

std::string jparse::GenerateString(JStruct json, bool min, std::string tab) {
	std::string rStr = "";
	switch (json.mode) {
	case _JMODE_OBJ: {
		rStr += "{";
		for (int i = 0; i < json.body.size(); i++) {
			if (json.body[i].body == nullptr) {
				std::string val = json.body[i].rawValue;
				if (json.body[i].ty == JType_String)
					val = "\"" + val + "\"";
				rStr += "\"" + json.body[i].label + "\":" + val + (i < json.body.size() - 1 ? "," : "");
			}
			else {
				rStr += "\"" + json.body[i].label + "\":";
				rStr += jparse::GenerateString(*json.body[i].body, min, "") + (i < json.body.size() - 1 ? "," : "");
			}
		}
		rStr += "}";
		break;
	}
	case _JMODE_ARR: {
		rStr += "[";
		for (int i = 0; i < json.body.size(); i++) {
			if (json.body[i].body == nullptr) {
				std::string val = json.body[i].rawValue;
				if (json.body[i].ty == JType_String)
					val = "\"" + val + "\"";
				rStr += val + (i < json.body.size() - 1 ? "," : "");
			}
			else {
				rStr += jparse::GenerateString(*json.body[i].body, min, "") + (i < json.body.size() - 1 ? "," : "");
			}
		}
		rStr += "]";
		break;
	}
	}

	return rStr;
}

/*

Function to remove whitespace and unnecesary characters from string

*/

std::string removeJunk(const char* code) {
	int len = strlen(code);

	//create a string cause its easier to work with
	std::string codeStrRes = "", res = "";

	//first remove comments
	bool inComment = false;
	bool inLineComment = false;

	int line = 0;

	std::string lineCollector = "";

	bool lineBegin = true;

	bool inStr = false;

	auto _charEscaped = [](const char* code, int idx) {
		int bCount = 0, cIdx = idx - 1, len = strlen(code);

		while (cIdx >= 0 && code[cIdx] == '\\') {
			bCount++;
			cIdx--;
		}

		return (bCount % 2) == 0 ? false : true;
		};

	for (int i = 0; i < len; i++) {
		if (code[i] == '\t' || code[i] == '\v' || code[i] == '\f' || code[i] == '\r' || (code[i] == ' ' && !inStr)) continue;
		if (code[i] == '\n') {
			line++;
			if (inLineComment && !inComment) inLineComment = false;
			continue;
		}

		if (code[i] == '"') {
			bool isBackSlashed = _charEscaped(code, i);

			if (!isBackSlashed) {
				inStr = !inStr;
			}
		}

		//
		if (fetchStr(code, len, i, 2) == "/*" && !inStr) {
			inComment = true;
			i++;
			continue;
		}

		if (fetchStr(code, len, i, 2) == "//" && !inComment && !inStr) {
			i++;
			inLineComment = true;
			continue;
		}

		if (fetchStr(code, len, i, 2) == "*/" && !inStr) {
			if (!inComment)
				return "";

			inComment = false;
			i++;
			continue;
		}

		if (!inComment && !inLineComment) {
			res += code[i];
		}

		lineBegin = false;
	}
	if (inComment) {
		return "";
	}

	return res;
}

bool _charEscaped(const char* code, int idx) {
	int bCount = 0, cIdx = idx - 1, len = strlen(code);

	while (cIdx >= 0 && code[cIdx] == '\\') {
		bCount++;
		cIdx--;
	}

	return (bCount % 2) == 0 ? false : true;
};

JStruct jparse::parseStr(const char* jData, bool _clean) {
	char* jsonData;
	if (_clean) {
		std::string rStr = removeJunk(jData);
		//std::cout << "Parsing: " << rStr << std::endl;
		const char* c_str = rStr.c_str();
		jsonData = new char[strlen(c_str)];
		memcpy(jsonData, c_str, strlen(c_str));
	}
	else
		jsonData = (char*)jData;

	JStruct rStruct;

	int mode; //0 obj, 1 array

	//mode check
	char bChar = jsonData[0];

	switch (bChar) {
	case '[': {
		mode = _JMODE_ARR;
		break;
	}
	case '{': {
		mode = _JMODE_OBJ;
		break;
	}
	default: {
		return JStruct();
	}
	}

	//split
	int cChar = 1, indentsC = 0, indentsA = 0;
	const size_t len = strlen(jsonData);

	std::vector<std::string> chunks;

	std::string charCollect;

	bool cBreak = false, fBreak = false, inStr = false;

	do {

		switch (jsonData[cChar]) {
		case ',': {
			if (indentsC == 0 && indentsA == 0 && !inStr) {
				cBreak = true;

				chunks.push_back(charCollect);
				charCollect = "";
			}
			break;
		}
		case '{': {
			if (inStr)
				break;
			indentsC++;
			break;
		}
		case '[': {
			if (inStr)
				break;
			indentsA++;
			break;
		}
		case '}': {
			if (inStr)
				break;
			indentsC--;
			if (indentsC < 0) fBreak = true;
			break;
		}
		case ']': {
			if (inStr)
				break;
			indentsA--;
			if (indentsA < 0) fBreak = true;
			break;
		}
		case '"': {
			if (!_charEscaped(jsonData, cChar))
				inStr = !inStr;
			break;
		}
		}

		if (fBreak) break;

		if (!cBreak)
			charCollect += jsonData[cChar];
		else
			cBreak = false;
	} while (++cChar < len);

	chunks.push_back(charCollect);

	//do some parsing stuff, ya know, the usual
	auto _split = [](const char* str, char delim) {
		std::vector<std::string> res;
		std::string current;
		bool inStr = false;
		int rb, ab;
		const size_t len = strlen(str);
		rb = ab = 0;
		for (int i = 0; i < len; i++) {
#ifdef MSFL_JSON_DEBUG
			if (i % 5000 == 0)
				std::cout << "split: " << i << " / " << len << std::endl;
#endif
			switch (str[i]) {
			case '{': {
				if (inStr)
					break;
				rb++;
				break;
			}
			case '}': {
				if (inStr)
					break;
				rb--;
				break;
			}
			case '[': {
				if (inStr)
					break;
				ab++;
				break;
			}
			case ']': {
				if (inStr)
					break;
				ab--;
				break;
			}
			case '"': {
				if (!_charEscaped(str, i))
					inStr = !inStr;
			}
			}
			if (str[i] == delim && rb <= 0 && ab <= 0 && !inStr) {
				res.push_back(current);
				current = "";
				continue;
			}

			current += str[i];
		}

		res.push_back(current);
		return res;
		};

	if (mode == _JMODE_OBJ) {
		//std::cout << "GOT: " << chunks.size() << " chunks" << std::endl;
		for (std::string c : chunks) {
			std::vector<std::string> lSplit = _split(c.c_str(), ':');

			//std::cout << "Spl Sz: " << lSplit.size() << " " << c << std::endl;;

			if (lSplit.size() > 2) return JStruct();

			//std::cout << "parsing chunk: " << c << std::endl;

			JToken tok;

			const char* lStr = lSplit[0].c_str();
			const size_t zLen = lSplit[0].length();
			//label str check
			if (lStr[0] != '"' || lStr[zLen - 1] != '"') return JStruct();

			//std::cout << "Test backslash thing: " << back_clean("Test \\n Slash \\\"") << std::endl;
			//std::cout << "--------------------------" << std::endl;

			tok.label = lSplit[0].substr(1, zLen - 2); //left

			//value
			const char* rStr = lSplit[1].c_str();
			genTok(rStr, tok);

			//std::cout << "Token: " << tok.label << " " << tok.rawValue << std::endl;

			rStruct.body.push_back(tok);
		}
	}
	else {
		//parse as an array
		for (std::string c : chunks) {
			JToken _tok;
			genTok(c.c_str(), _tok);
			rStruct.body.push_back(_tok);
		}
	}

	rStruct.mode = mode;

	return rStruct;
}