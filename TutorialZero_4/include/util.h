#pragma once

#include <graphics.h>

#include <stdio.h>

#pragma comment(lib, "WINMM.lib")
#pragma comment(lib, "MSIMG32.lib")

struct Rect
{
    int x, y;
    int w, h;
};

// 绘制的图片对象, 目标矩形, 源矩形
inline void putimage_ex(IMAGE* img, const Rect* rect_dst, const Rect* rect_src = nullptr)
{
    static BLENDFUNCTION blend_func = { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };
    AlphaBlend(GetImageHDC(GetWorkingImage()), rect_dst->x, rect_dst->y, rect_dst->w, rect_dst->h,
               GetImageHDC(img), rect_src ? rect_src->x : 0, rect_src ? rect_src->y : 0,
               rect_src ? rect_src->w : img->getwidth(), rect_src ? rect_src->h : img->getheight(), blend_func);
}

inline void load_audio(LPCSTR path, LPCSTR id)
{
    static TCHAR str_cmd[512];
    _stprintf_s(str_cmd, _T("open %s alias %s"), path, id);
    mciSendString(str_cmd, NULL, 0, NULL);
}

inline void play_audio(LPCSTR id, bool is_loop = false)
{
    static TCHAR str_cmd[512];
    _stprintf_s(str_cmd, _T("play %s %s from 0"), id, is_loop ? _T("repeat") : _T(""));
    mciSendString(str_cmd, NULL, 0, NULL);
}

inline void stop_audio(LPCSTR id)
{
    static TCHAR str_cmd[512];
    _stprintf_s(str_cmd, _T("stop %s"), id);
    mciSendString(str_cmd, NULL, 0, NULL);
}

inline int range_random(int min_num, int max_num)
{
    return min_num +  rand() % (max_num - min_num + 1);
}