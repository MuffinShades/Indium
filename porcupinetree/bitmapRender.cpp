#include "bitmapRender.hpp"

//changes le resolution of a color
_bmpColor setColorResolution(_bmpColor c, u32 r) {
    r >>= 2;
    const size_t channelMask = ((1 << r) - 1);

    c.r &= channelMask;
    c.g &= channelMask;
    c.b &= channelMask;
    c.a &= channelMask;

    return c;
}

void BitmapGraphics::SetColor(byte r, byte g, byte b, byte a) {
    this->tColor = {
        r, g, b, a
    };
}

void BitmapGraphics::Clear() {
    if (this->bmp->header.fSz <= 0)
        return;
    ZeroMem(this->bmp->data, this->bmp->header.fSz);
}

void BitmapGraphics::DrawPixel(u32 x, u32 y) {
    const u32 bpp = this->bmp->header.bitsPerPixel;

    //std::cout << "BPP: " << bpp << std::endl;

    if (bpp < 16) {
        std::cout << "Bitmap Graphics Error: No support for bpp < 16!" << std::endl;
        return;
    }

    const size_t byPP = bpp >> 3;
    size_t p = (x + (y * this->bmp->header.w)) * byPP;
    //std::cout << p << std::endl;

    _bmpColor nColor = setColorResolution(this->tColor, bpp);

    switch (bpp) {
    case 0x10: {
        this->bmp->data[p++] = ((nColor.b & 0xf) << 0x4) +
            (nColor.g & 0xf);
        this->bmp->data[p] = ((nColor.r & 0xf) << 0x4) +
            (nColor.a & 0xf);
        break;
    }
    case 0x20: {
        this->bmp->data[p++] = nColor.b & 0xff;
        this->bmp->data[p++] = nColor.g & 0xff;
        this->bmp->data[p++] = nColor.r & 0xff;
        this->bmp->data[p] = nColor.a & 0xff;
        break;
    }
    default: {
        std::cout << "Bitmap Graphics Error: No support for bpp " << bpp << std::endl;
        return;
    }
    }
}

void BitmapGraphics::ClearColor() {

}