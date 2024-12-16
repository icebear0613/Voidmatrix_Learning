#pragma once

#include <graphics.h>

#pragma comment(lib, "WINMM.lib")
#pragma comment(lib, "MSIMG32.lib")

struct Rect
{
    int x, y;
    int w, h;
};

inline void putimage_ex(IMAGE* img, const Rect* rect_dst, const Rect* rect_src = nullptr)
{
    static BLENDFUNCTION blend_func = { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };
    AlphaBlend(GetImageHDC(GetWorkingImage()), rect_dst->x, rect_dst->y, rect_dst->w, rect_dst->h,
               GetImageHDC(img), rect_src->x ? rect_src->x : 0, rect_src->y ? rect_src->y : 0,
               rect_src->w ? rect_src->w : img->getwidth(), rect_src->h ? rect_src->h : img->getheight(), blend_func);
}
