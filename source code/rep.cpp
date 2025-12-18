#include <Windows.h>
#include <gdiplus.h>
#include <string>

#include "rep.h"
//#pragma comment(lib, "gdiplus.lib")

int rep::clientWidth = 800;
int rep::clientHeight = 600;

float rep::scaleX = 1.0f;
float rep::scaleY = 1.0f;

std::wstring rep::GetExeDirectory(){
    wchar_t path[MAX_PATH];
    
    DWORD len = GetModuleFileNameW(NULL, path, MAX_PATH);
    if (len == 0 || len == MAX_PATH)
        return L"";  // 取得失敗

    std::wstring fullPath(path);
    size_t pos = fullPath.find_last_of(L"\\/");
    if (pos == std::wstring::npos)
        return L"";
    
    return fullPath.substr(0, pos);  // 只取目錄，不包含檔名
}

ULONG_PTR rep::g_gdiplusToken;

void rep::InitGDIPlus() {
    Gdiplus::GdiplusStartupInput gdiSI;
    Gdiplus::GdiplusStartup(&g_gdiplusToken, &gdiSI, NULL);
    return;
}

void rep::ShutdownGDIPlus() {
    Gdiplus::GdiplusShutdown(g_gdiplusToken);
    return;
}

Gdiplus::Image* rep::LoadImageGDIPlus(const std::wstring& path) {
    Gdiplus::Image* img = new Gdiplus::Image(path.c_str());
    if (img->GetLastStatus() != Gdiplus::Ok) {
        delete img;
        return nullptr;
    }
    return img;
}

bool rep::pointInRect(const RECT& rect, int x, int y) {
    
    return (x >= rect.left && x <= rect.right &&
            y >= rect.top && y <= rect.bottom);
}

void rep::drawImageResized(Gdiplus::Graphics& graphics, Gdiplus::Image* img, const int& x, const int& y, const int& w, const int& h){
    float drawX = x * scaleX;
    float drawY = y * scaleY;
    float drawW = w * scaleX;
    float drawH = h * scaleY;
    
    graphics.DrawImage(img, drawX, drawY, drawW, drawH);
    return;
}

RECT rep::reSizedRect(const RECT& rect){
    RECT resizedRect;
    resizedRect.left   = static_cast<LONG>(rect.left * scaleX);
    resizedRect.top    = static_cast<LONG>(rect.top  * scaleY);
    resizedRect.right  = static_cast<LONG>(rect.right * scaleX);
    resizedRect.bottom = static_cast<LONG>(rect.bottom * scaleY);
    return resizedRect;
}
