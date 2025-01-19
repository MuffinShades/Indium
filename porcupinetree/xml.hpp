#include <iostream>
#include <vector>

/**
 *
 * Small XML parse library written for msfl
 *
 * Copyright muffinshades 2024 All rights reserved
 *
 */

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

enum xml_type {
    XMLType_Tag,
    XMLType_Value
};

class xml_attribute {
public:
    std::string label;
    std::string value;
    int returnCode = 1;
    MSFL_EXP xml_attribute(int code = 1) {
        this->returnCode = code;
    };
    MSFL_EXP int operator==(int g) {
        return g == this->returnCode;
    };
    MSFL_EXP int operator!=(int g) {
        return g != this->returnCode;
    };
};

class xml_token {
private:
    xml_token* parent;
    std::vector<xml_token*> children;
    std::vector<xml_attribute> attributes;
public:
    std::string label;

    xml_type ty = XMLType_Tag;

    bool root = false;
    bool singleTag = false;

    MSFL_EXP xml_token(xml_token* parent = nullptr) {
        this->parent = parent;
    };

    MSFL_EXP xml_token(xml_token* parent, std::vector<xml_attribute> attr) {
        this->parent = parent;

        if (attr.size() > 0) {
            this->attributes = attr;
        }
    };

    //all functions / utility functions
    MSFL_EXP xml_token* GetFirstChild(std::string label);
    MSFL_EXP xml_token* GetLastChild(std::string label);
    MSFL_EXP std::vector<xml_token*> GetAllChildren(std::string label);
    MSFL_EXP std::vector<xml_token*> GetChildren();
    MSFL_EXP xml_token* GetParent();
    MSFL_EXP void SetChildren(std::vector<xml_token*> ch);
    MSFL_EXP void AddChild(xml_token* ch);
    MSFL_EXP std::vector<xml_attribute>* GetAttrPtr();
    MSFL_EXP xml_attribute GetAttr(std::string label);
    MSFL_EXP std::vector<xml_attribute> GetAttrs();
    MSFL_EXP void SetAttribute(std::string label, std::string value);
    MSFL_EXP std::string GetFirstValue();
    MSFL_EXP std::string GetCollectedValue(char delim = ' ');
    MSFL_EXP std::string GetValue();
    MSFL_EXP std::string GetLastValue();
    MSFL_EXP std::vector<std::string> GetValueList();
};

class xml_file {
    std::string fName;
public:
    xml_token* root;
    MSFL_EXP xml_file(std::string src = "") {
        this->root = nullptr;
        this->fName = "";
    }
    MSFL_EXP xml_token* GetRoot();
    MSFL_EXP ~xml_file();
};

struct xml_fileSettings {
public:
    bool format = false;
    std::string src = "";
};

class xml_parse {
public:
    MSFL_EXP static xml_file* parseXml(std::string dat);
    MSFL_EXP static xml_file* parseFile(xml_fileSettings settings);
    MSFL_EXP static bool writeToFile(xml_file* xmlf, xml_fileSettings settings);
    MSFL_EXP static std::string generateXmlString(xml_file* xmlf, xml_fileSettings settings);
};

#ifdef MSFL_DLL
#ifdef __cplusplus
}
#endif
#endif