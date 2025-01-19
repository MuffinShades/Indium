#pragma once
#include "ttf.hpp"
#include "bitmap.hpp"

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

class ttfRender {
public:
    MSFL_EXP static i32 RenderGlyphToBitmap(Glyph tGlyph, Bitmap* bmp, float scale = 1.0f);
};

#ifdef MSFL_DLL
#ifdef __cplusplus
}
#endif
#endif