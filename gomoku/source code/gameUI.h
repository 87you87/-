#pragma once
#include <windows.h>
#include <string>

LRESULT CALLBACK GameUIProc(HWND, UINT, WPARAM, LPARAM);
extern HWND gameUIWnd;
bool RegisterGameUIClass();

void openGameUIWnd();

bool HitTestIntersection(float, float, int&, int&);

void updataUpArea();

std::string sendPlacePiaceString(int, int);

void AppendText_GameUIPrintEdit(HWND, const wchar_t*);

