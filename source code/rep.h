#pragma once
#include <windows.h>
#include <string>
#include <gdiplus.h>

namespace rep{

    extern HWND mainWindowProc;

    extern Gdiplus::Image* imgBackGround;
    extern Gdiplus::Image* imgButtonSelected;

    extern HINSTANCE hInst;
    extern int nCmdShow;

    extern int clientWidth;
    extern int clientHeight;

    extern float scaleX;
    extern float scaleY;

    std::wstring GetExeDirectory();

    extern ULONG_PTR g_gdiplusToken;

    void InitGDIPlus();
    void ShutdownGDIPlus();
    Gdiplus::Image* LoadImageGDIPlus(const std::wstring& path);
    bool pointInRect(const RECT& rect, int x, int y);
    
    void drawImageResized(Gdiplus::Graphics&, Gdiplus::Image*,const int&, const int&, const int&, const int&);
    RECT reSizedRect(const RECT& rect);

}