/**
 * ---------------------------------------
 * Game Asset file format
 *
 * Written by muffinshades for msfl July, 2024
 *
 * Copyright (c) muffinshades, All Rights Reserved.
 *
 * This code is free to use as long as credit is given
 * to the author, muffinshades.
 *
 * Version 1.0.0
 * ---------------------------------------
 */

#include "jparse.hpp"
#include "ByteStream.hpp"
#include "filewrite.hpp"
#include "Date.hpp"
#include "FilePath.hpp"

#define MSFL_COMPILE_DLL
#define MSFL_EXPORTS

#ifdef __cplusplus
extern "C" {
#endif

#ifdef MSFL_COMPILE_DLL
#ifdef MSFL_EXPORTS
#define MSFL_LIB __declspec(dllexport)
#else
#define MSFL_LIB __declspec(dllimport)
#endif
#else
#define MSFL_LIB
#endif

    struct AssetDescriptor {
        size_t sz = 0;
        std::string fileType = "";
        Date created;
        Date modified;
        std::string fname = "";
        int compressionType = 2;
        std::string aId = "";
        size_t dataOffset = 0;
        byte F_INFO = 0;
        size_t compressedSize = 0;
    };

    struct Asset {
        AssetDescriptor inf;
        size_t sz;
        byte* bytes = nullptr;
        bool stream = false, __nb_free = false;
        MSFL_LIB ~Asset();
        MSFL_LIB void free();
    };

    enum _aContainerType {
        _aTypeAsset,
        _aTypeContainer
    };

    class AssetContainer {
    private:
        enum _aContainerType _ty = _aTypeContainer;
        Asset _assetData;
        std::string id = "";
    public:
        std::vector<AssetContainer*> assets;
        MSFL_LIB AssetContainer* GetNode(std::string id);
        MSFL_LIB Asset* GetAsset(std::string id);
        MSFL_LIB Asset* GetAssetData() {
            return &this->_assetData;
        };
        MSFL_LIB void SetAssetData(Asset a);
        MSFL_LIB void AddAsset(std::string id, byte* data, size_t sz, AssetDescriptor desc = {});
        void AddAsset(std::string id, std::string fSrc);
        MSFL_LIB void AddAsset(std::string id, AssetDescriptor desc);
        MSFL_LIB AssetContainer* AddContainer(std::string id);
        MSFL_LIB void SetId(std::string id);
        MSFL_LIB std::string GetId();
        MSFL_LIB AssetContainer(std::string id = "");
        MSFL_LIB ~AssetContainer();
        MSFL_LIB size_t getNAssets() {
            return this->assets.size();
        }
        MSFL_LIB enum _aContainerType getType() {
            return this->_ty;
        }
    };

    class AssetStruct {
    private:
        AssetContainer* map;
    public:
        MSFL_LIB Asset* GetAsset(std::string path);
        MSFL_LIB void AddAsset(std::string path, byte* data, size_t sz, AssetDescriptor desc = {});
        MSFL_LIB void AddAsset(std::string path, std::string fSrc);
        MSFL_LIB void AddAsset(std::string path, AssetDescriptor desc);
        MSFL_LIB AssetContainer* GetRoot() {
            return this->map;
        };
        MSFL_LIB AssetStruct() {
            this->map = new AssetContainer("root");
        };
        MSFL_LIB ~AssetStruct();
    };

    class AssetFile {
    private:
        byte* data = nullptr;
        size_t sz = 0;
        size_t rootOffset = 0;
    public:
        AssetStruct constructStruct();
        Asset getAsset(std::string path);
        AssetContainer constructContainer(std::string path);
        byte* getDataPtr();
    };

    //TODO: v1.1
    class AssetInstance {
        AssetStruct fMap;
        ByteStream fStream;
        MSFL_LIB ~AssetInstance() {
            fStream.free();
        }
        MSFL_LIB AssetInstance() {}
        Asset GetAsset(std::string path);
        MSFL_LIB AssetInstance(ByteStream fStream, AssetStruct fMap) {
            this->fStream = fStream;
            this->fMap = fMap;
        }
    };

    class AssetParse {
    public:
        MSFL_LIB static int WriteToFile(std::string src, AssetStruct* dat); //done
        static byte* WriteToBytes(AssetStruct* dat); //technically done
        MSFL_LIB static int WriteToFile(std::string src, std::string jsonMap, std::string parentDir = ""); //done
        MSFL_LIB static AssetStruct ParseAssetFile(std::string src); //done
        static AssetStruct ParseDat(byte* dat, size_t sz); //technically done
        static AssetFile ReadFile(std::string src);
        MSFL_LIB static JStruct ReadFileMapAsJson(std::string src); //done
        MSFL_LIB static JStruct ReadFileMapAsJson(byte* dat, size_t sz); //done
        MSFL_LIB static Asset ExtractAssetFromFile(std::string src, std::string path, bool streamData = false); //done
        static Asset ExtractAssetFromFile(std::string src, std::string path, JStruct assetMap, bool streamData = false); // mostly done
        static Asset ExtractAssetFromData(byte* dat, size_t sz, bool streamData = false); // technically done
    };

#ifdef __cplusplus
}
#endif