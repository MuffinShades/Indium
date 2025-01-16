#include "xml.hpp"
#include <cstring>
#include <fstream>
#define MSFL_XML_DEBUG
/**
 *
 * Xml.cpp
 *
 * Code file for msfl xml parser
 *
 * Copyright (c) muffinshades 2024 All rights reserved
 *
 * Written May 2024
 *
 * This is part of the msfl file library. This particular
 * sub lib is responsible for reading and writing to xml
 * files.
 */

bool isWhiteSpace(char c) {
    const char _ws[] = { 0x20, 0x9, 0xa, 0xb, 0xc, 0xd, 0x00 };
    for (auto q : _ws)
        if (c == q)
            return true;
    return false;
};

bool _alpha(char c) {
    const char _nonAlpha[] = "!\"#$%&'()*+,/;<=>?@[\\]^`{|}~";

    if (isWhiteSpace(c)) return false;

    for (auto q : _nonAlpha)
        if (q == c)
            return false;

    return true;
};

char* removeInvalidSpace(char* dat) {
    std::string rStr = "";
    const size_t len = strlen(dat);
    for (int i = 0; i < len; i++) {
        if (!(isWhiteSpace(*(dat + i)) && *(dat + i) != 0x20))
            rStr += dat[i];
    }

    //for scope stuff do this
    char* res = new char[rStr.length()];
    const char* rcs = rStr.c_str();
    memcpy(res, rcs, rStr.length());
    return res;
};

void skipSpace(char* cur) {
    while (*cur == 0x20) { cur++; };
}

struct xml_tag {
public:
    std::string label;
    std::string value;
    xml_tag() {};
};

enum _item_type {
    ity_openTag,
    ity_header,
    ity_closeTag,
    ity_singleTag,
    ity_internalValue
};

struct _xml_item {
public:
    std::string label;
    std::vector<xml_tag> tags;
    _item_type ty = ity_openTag;
    _xml_item() {};
};

_xml_item* getItem(int& rPos, char* dat, bool& inComment) {
    _xml_item* rItem = new _xml_item;
    char* cur = removeInvalidSpace(dat + rPos);

    skipSpace(cur);

    //white space removal stuff
    do {
        if (*cur == '<') break;
        //std::cout << *cur << " " << (int)*cur << " " << cur<< std::endl;

        //found value in tag
        if (!isWhiteSpace(*cur) || rItem->ty == ity_internalValue) {
            if (*cur == '>') {
                std::cout << "Error uxpected tag close" << std::endl;
                return nullptr;
            }

            rItem->ty = ity_internalValue;
            rItem->label += *cur;
        }
    } while (*++cur != '<');//go to next <
    if (rItem->ty == ity_internalValue) {
        rPos = (size_t)(cur - dat);
        return rItem;
    }
    cur++; //skip "<"

    //close tag check
    if (*cur == '/') {
        rItem->ty = ity_closeTag;
        cur++;
    }

    auto IsStr = [](char* v, std::string str) {
        const char* cs = str.c_str();
        for (int i = 0; i < str.length(); i++)
            if (*v++ != cs[i]) return false;
        return true;
        };

    //now traverse to tab name
    std::string tgName = "";
    const char _space = 32;
    int mode = 0, tmode = 0; //0 int, 1 string, 2 for xml version specifier

    do {
        //?xml check thingy for stuff like that
        if (*cur == '?') {
            if (mode == 2) {
                mode = 0;
                continue;
            }
            tmode = 2;
            mode = 2;
            rItem->ty = ity_header;
            continue;
        }

        tgName += *cur;

        //something
        switch (tmode) {
        case 2: {
            break;
        }
        default: {
            if (!_alpha(*cur)) {
                std::cout << "Error invalid token: " << *cur << std::endl;
                delete rItem;
                return nullptr;
            }
        }
        }
    } while (*++cur != _space && *cur != '>' && (*cur != '/' || *(cur + 1) != '>'));

    if (IsStr(cur, "/>")) {
        rItem->ty = ity_singleTag;
        cur++;
    }

    rItem->label = tgName;

    if (tmode == 2 && tgName != "xml") {
        std::cout << "Invalid tag name: \"" << tgName << "\"" << std::endl;
    }

    //tmode = 0; //reset tmode its probably no longer used

    //std::cout << "Got label: " << tgName << std::endl;

    //now get the other tags
    std::vector<std::string> tags;
    std::string cTag = "";

    while (*cur != '>' && (*cur != '/' || *(cur + 1) != '>')) {
        if (*cur == _space) cur++;
        else {
            std::cout << "Error invalid token [tgSpace]: " << *cur << std::endl;
            delete rItem;
            return nullptr;
        }

        bool addTag = true;

        do {
            rPos++;

            cTag += *cur;

            if (tmode == 2 && *cur == '?') {
                if (*++cur != '>') {
                    std::cout << "Error invalid token: ?" << std::endl;
                    delete rItem;
                    return nullptr;
                }
                else {
                    addTag = false;
                    break;
                }
            }

            if (!_alpha(*cur)) {
                std::cout << "Error invalid token [tgLbl]: " << *cur << std::endl;
                delete rItem;
                return nullptr;
            }
        } while (*++cur != _space && *cur != '>' && *cur != '=');

        if (!addTag) continue;

        if (mode == 1) {
            std::cout << "Error could not find end quatation!" << std::endl;
            delete rItem;
            return nullptr;
        }

        //now get tag value
        if (*cur == '=') {
            cur++;
            std::string tgVal = "";

            do {
                rPos++;

                //string parsing
                if (*cur == '"') {
                    if (mode == 1) {
                        mode = 0;
                        cur++; //skip end quote
                        break;
                    }
                    mode = 1;
                    continue;
                }

                //xml only supports string mode so error if its not in the proper mode
                if (mode != 1) {
                    std::cout << "Error invalid tag format!" << std::endl;
                    delete rItem;
                    return nullptr;
                }

                tgVal += *cur;

                //add characters to value
                if (!_alpha(*cur)) {
                    std::cout << "Error invalid token [tgVal]: " << *cur << std::endl;
                    delete rItem;
                    return nullptr;
                }
            } while (*++cur != _space && *cur != '>' && *cur != '=');

            //tags.push_back(cTag+" | "+tgVal);
            xml_tag tg;
            tg.label = cTag;
            tg.value = tgVal;
            rItem->tags.push_back(tg);
        }
        else {
            xml_tag tg;
            tg.label = cTag;
            rItem->tags.push_back(tg);
        }

        cTag = "";
    }

    if (IsStr(cur, "/>")) {
        rItem->ty = ity_singleTag;
        cur++;
    }

    cur++; //skip '>' or end token

    rPos = (size_t)(cur - dat);

    return rItem;
};

typedef struct {
public:
    std::string label;
    std::string value;
} _tag;

template<class _Ty> class tg_stack {
private:
    void alloc_new() {
        this->allocSz += this->chunkSz;
        _Ty* tmp = new _Ty[this->allocSz];
        memset(tmp, 0, this->allocSz * sizeof(_Ty));
        memcpy(tmp, this->stack, sizeof(_Ty) * this->sz);
        delete[] this->stack;
        this->stack = tmp;
    }
public:
    _Ty* stack = nullptr;
    size_t sz = 0;
    size_t chunkSz = 0;
    size_t allocSz = 0;
    tg_stack(size_t allocSz) {
        this->allocSz = allocSz & 0xfffff;
        this->stack = new _Ty[this->allocSz];
        this->chunkSz = allocSz;
    }
    ~tg_stack() {
        delete[] this->stack;
        this->sz = 0;
        this->asz = 0;
    }
    void push(_Ty v) {
        if (this->sz >= allocSz - 1) {
            this->alloc_new();
        }
        this->stack[this->sz++] = v;
    }
    _Ty pop() {
        _Ty r = this->stack[--this->sz];
        memset(this->stack + this->sz, 0, sizeof(_Ty));
        return r;
    }
    _Ty getLast() {
        return this->stack[this->sz - 1];
    }
};

void PrintXMLStruct(xml_token* tk, std::string tabs = "") {
    if (tk != nullptr) {
        std::cout << tabs << "- [" << tk->ty << "]" << " " << tk->label << std::endl;
    }

    for (auto c : tk->GetChildren()) {
        if (c != nullptr) {
            switch (c->ty) {
            case XMLType_Value: {
                std::cout << tabs << "\t- [" << c->ty << "]" << " " << c->label << std::endl;
                break;
            }
            default: {
                PrintXMLStruct(c, tabs + "\t");
                break;
            }
            }
        }
    }
}

//function to remove comments from xml data
std::string removeXmlComments(std::string dat) {
    char* cdat = (char*)(dat).c_str();

    std::string res = "";

    char commentStart[] = "--!<";
    char commentEnd[] = ">--";
    const size_t csLen = strlen(commentStart);

    //small string end comparator
    auto ils = [](char* str, char* cmp, size_t sLen) {
        size_t cLen = strlen(cmp);
        if (sLen + 1 < cLen) return false;
        char* cEnd = cmp + cLen, * s = str + sLen;
        do {
            if (*s-- != *cmp++)
                return false;
        } while (cmp < cEnd);

        return true;
        };

    bool inComment = false; //comment flag

    //just loop over every character checking for comment begins and ends and set in comment flag accordingly
    int i = 0;
    do {
        if (ils(cdat, commentStart, i)) {
            inComment = true;

            //remove comment start
            res = res.substr(0, res.length() - (csLen - 1));
            continue;
        }
        else if (ils(cdat, commentEnd, i)) {
            //error out if unexpected comment end
            if (!inComment) {
                std::cout << "XMLError: Unexpected comment end!" << std::endl;
                return "";
            }
            inComment = false;
            continue;
        }

        //if not in comment then its good data
        if (!inComment)
            res += cdat[i];
    } while (++i < dat.length());

    //error out if no comment close
    if (inComment) {
        std::cout << "XMLError: Could not find end of comment!" << std::endl;
        return "";
    }

    return res;
};

//main parser function
xml_file* xml_parse::parseXml(std::string dat) {
    xml_file* rs = new xml_file;
    bool inComment = false;
    std::string _xmDat = removeXmlComments(dat);
    std::cout << "Filtered Dat: " << _xmDat << std::endl;
    if (_xmDat == "") {
        delete rs;
        return nullptr;
    }
    char* dstr = (char*)_xmDat.c_str();
    int rPos = 0;
    xml_token* currentItem = nullptr;
    tg_stack<_xml_item*>* _stack = new tg_stack<_xml_item*>(0xfff); //tag stack
    _xml_item* header = nullptr;

    //convert from xml_item to xml_token
    auto citxt = [](_xml_item* itm, xml_token* parent) {
        xml_token* nTok = new xml_token(parent);
        nTok->label = itm->label;
        std::vector<xml_attribute>* _at = nTok->GetAttrPtr();

        for (auto& attr : itm->tags) {
            //convert from xml_tag to xml_attribute
            xml_attribute nxa;
            nxa.label = attr.label;
            nxa.value = attr.value;
            _at->push_back(nxa);
        }

        return nTok;
        };

    //parse le file
    do {
        _xml_item* itm = getItem(rPos, dstr, inComment);

        switch (itm->ty) {
        case ity_header: {
            if (header != nullptr) {
#ifdef MSFL_XML_DEBUG
                std::cout << "Error cannot have more than 1 header!" << std::endl;
#endif
                delete header, itm;
                return nullptr;
            }
            else
                header = itm;
            break;
        }
        case ity_singleTag: {
#ifdef MSFL_XML_DEBUG
            std::cout << "Got [single]: " << itm->label << std::endl;
#endif
            if (currentItem != nullptr) {
                xml_token* nItem = citxt(itm, currentItem);
                nItem->singleTag = true;
                currentItem->AddChild(nItem);
            }
            else {
                std::cout << "Error cannot put single tag outside of root!" << std::endl;
                return nullptr;
            }
            break;
        }
        case ity_closeTag: {
#ifdef MSFL_XML_DEBUG
            std::cout << "Got [closing]: " << itm->label << std::endl;
#endif
            _xml_item* pItem = _stack->pop();

            if (pItem->label != itm->label) {
                std::cout << "Error invalid closing tag: \"" << pItem->label << "\" expected: \"" << itm->label << "\"" << std::endl;
                delete itm, pItem;
                return nullptr;
            }
            else {
                currentItem = currentItem->GetParent();
                delete pItem;
            }

            break;
        }
        case ity_openTag: {
#ifdef MSFL_XML_DEBUG
            std::cout << "Got [opening]: " << itm->label << std::endl;
#endif
            if (_stack->sz <= 0) {
                if (rs->root != nullptr) {
                    delete rs->root, rs;
                    std::cout << "Error cannot have more than one root node!" << std::endl;
                    return nullptr;
                }
                else {
                    rs->root = citxt(itm, nullptr);
                    currentItem = rs->root;
                }
            }
            else {
                xml_token* nItem = citxt(itm, currentItem);
                currentItem->AddChild(nItem);
                currentItem = nItem;
            }

            _stack->push(itm);
            break;
        }
        case ity_internalValue: {
#ifdef MSFL_XML_DEBUG
            std::cout << "Got [internalValue]: " << itm->label << std::endl;
#endif
            if (currentItem) {
                //add new child
                xml_token* vTok = new xml_token(currentItem);
                vTok->label = itm->label;
                vTok->ty = XMLType_Value;
                currentItem->AddChild(vTok);
            }
            else {

                std::cout << "Error cannot put data outside of node!" << std::endl;
                return nullptr;
            }
            break;
        }
        }
    } while (_stack->sz > 0);


    PrintXMLStruct(rs->root);
    //TODO check for left over data if there is some error out

    return rs;
};

//set children on xml_token
void xml_token::SetChildren(std::vector<xml_token*> ch) {
    this->children = ch;
}

//add child
void xml_token::AddChild(xml_token* ch) {
    if (ch == nullptr) return;

    this->children.push_back(ch);
}

//deconstructor
void _del(xml_token* t) {
    if (t == nullptr) return;

    for (auto& c : t->GetChildren())
        _del(c);

    delete t;
}

xml_file::~xml_file() {
    _del(this->root);
}

//function to compare 2 strings
bool StrCompare(std::string s1, std::string s2) {
    if (s1.length() != s2.length()) return false;

    const char* c1 = s1.c_str(), * c2 = s2.c_str();

    for (size_t i = 0; i < s1.length(); i++) {
        if (*(c1 + i) != *(c2 + i))
            return false;
    }

    return true;
}

//Get first child
xml_token* xml_token::GetFirstChild(std::string label) {
    if (label == "") return nullptr;

    for (auto tk : this->children) {
        if (StrCompare(tk->label, label))
            return tk;
    }

    return nullptr;
}

//Get last child
xml_token* xml_token::GetLastChild(std::string label) {
    xml_token* last = nullptr;

    for (auto tk : this->children) {
        if (StrCompare(tk->label, label))
            last = tk;
    }

    return last;
}

//Get all children
std::vector<xml_token*> xml_token::GetAllChildren(std::string label) {
    std::vector<xml_token*> _children;

    for (auto& tk : this->children) {
        if (StrCompare(tk->label, label))
            _children.push_back(tk);
    }

    return _children;
}

//get all children despite label
std::vector<xml_token*> xml_token::GetChildren() {
    return this->children;
}

//attribute ptr
std::vector<xml_attribute>* xml_token::GetAttrPtr() {
    return &this->attributes;
}

xml_attribute xml_token::GetAttr(std::string label) {
    if (label.length() <= 0) return { 0 };

    for (auto& at : this->attributes) {
        if (StrCompare(at.label, label))
            return at;
    }
}

//get le parent
xml_token* xml_token::GetParent() {
    return this->parent;
}

std::string xtgfxml(xml_token* tk, std::string cDat = "", std::string tabs = "") {
    if (tk == nullptr) return cDat;

    if (tk->ty == XMLType_Value)
        return cDat + tabs + tk->label;

    cDat += tabs + "<" + tk->label + " ";
    //add tags
    for (auto attr : tk->GetAttrs())
        cDat += attr.label + "=\"" + attr.value + "\" ";

    //now add children
    if (tk->singleTag) {
        cDat += "/>";
        return cDat;
    }
    else
        cDat += ">";

    for (auto c : tk->GetChildren()) {
        cDat += "\n";
        cDat = xtgfxml(c, cDat, tabs + "\t");
    }

    //add closin
    cDat += "\n";
    cDat += tabs + "</" + tk->label + ">";

    return cDat;
}

//same thing as formated just without the fomatting
//;-;
std::string xtgnfxml(xml_token* tk, std::string cDat = "") {
    if (tk == nullptr) return cDat;

    if (tk->ty == XMLType_Value)
        return cDat + tk->label;

    cDat += "<" + tk->label + " ";
    //add tags
    for (auto attr : tk->GetAttrs())
        cDat += attr.label + "=\"" + attr.value + "\" ";

    //now add children
    if (tk->singleTag) {
        cDat += "/>";
        return cDat;
    }
    else
        cDat += ">";

    for (auto c : tk->GetChildren())
        cDat = xtgnfxml(c, cDat);

    //add closin
    cDat += "</" + tk->label + ">";

    return cDat;
}

//formatted xml data
std::string generateFormatedXml(xml_file* xmlf) {
    if (xmlf->root == nullptr)
        return "";
    return xtgfxml(xmlf->root);
};

//non formatted xml data
std::string generateDefXml(xml_file* xmlf) {
    if (xmlf->root == nullptr)
        return "";
    return xtgnfxml(xmlf->root);
};

//get generated xml string
std::string xml_parse::generateXmlString(xml_file* xmlf, xml_fileSettings settings) {
    if (xmlf == nullptr || xmlf->root == nullptr) return "";

    if (settings.format)
        return generateFormatedXml(xmlf);
    else
        return generateDefXml(xmlf);
}

//write to file
bool xml_parse::writeToFile(xml_file* xmlf, xml_fileSettings settings) {
    if (xmlf == nullptr || xmlf->root == nullptr) return false;
    std::string fdat = xml_parse::generateXmlString(xmlf, settings);
    std::cout << "FDat: " << fdat << std::endl;
    if (settings.src == "") return false;

    std::ofstream outFile(settings.src);

    if (!outFile.good()) return false;

    //write le data
    outFile << fdat;
    outFile.close();

    return true;
}

//
std::string xml_token::GetFirstValue() {
    for (auto& ch : this->children) {
        if (ch->ty == XMLType_Value)
            return ch->label;
    }

    return "";
}

std::string xml_token::GetValue() {
    return this->GetFirstValue();
}

//get last value
std::string xml_token::GetLastValue() {
    std::string last = "";

    for (auto& ch : this->children) {
        if (ch->ty == XMLType_Value)
            last = ch->label;
    }

    return last;
}

//get collected values
std::string xml_token::GetCollectedValue(char delim) {
    std::string collection = "";

    for (auto& ch : this->children) {
        if (ch->ty == XMLType_Value)
            collection += ch->label + delim;
    }

    return collection;
}

std::vector<xml_attribute> xml_token::GetAttrs() {
    return this->attributes;
}