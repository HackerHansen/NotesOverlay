#pragma once
#include <iostream>
#include <fstream>
#include <algorithm>
#include <vector>
#include <map>
#include <set>
#include <stack>
#include <queue>
#include <Windows.h>
#include <string>
#include <atlstr.h>
#include <TlHelp32.h>

class Utils {
public:
	static int getTextWidth(HDC hdc, std::string txt);
	static std::string promptUserForFilePath(LPCWSTR types);
	static std::vector<std::vector<std::string>> readFile(std::string path);
};

class RenderUtils {
public:
	static void fillRect(HDC hdc, int x, int y, int w, int h, COLORREF color);
	static void renderText(HDC hdc, std::string txt, int x, int y, COLORREF color);
};